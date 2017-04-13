////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include "RCop.h"   // gets Cop.h
#include <cstdio> // printf
#include "Lib/Misc/datastructs.h" // map_t, vector_t
#include "Lib/Misc/logString.h"
#include <cassert> // assert
#include <cstring> // std::memcpy
using std::memcpy;

#include "Framework/Core/Naming/StaticNaming.h"

using namespace Core::Naming;
using namespace Patterns::Cop;

// extern bool DBG_PATTERN;  // XXX? // set in Lib/Misc/Debug.cpp
extern NodeId_t MY_NODE_ID;

namespace Patterns {
namespace Cop {


  /* Note: compiler-supplied default constructor */  
RCop::~RCop () {
	if ((this->patternState >= GOT_PID) && !this->stopped)  // started...
	this->Stop();

	// free the items in the cop table
	for ( std::map<NodeId_t,RCopTableEntry*>::const_iterator it
			= this->copTable.begin(); it != this->copTable.end(); it++ ) 
	{
		RCopTableEntry *r = it->second;
		if (r) {  // redundant TimeStamp, but we might someday remove (age out) an entry.
			delete r;
		}
	}

	// TimeStamp the neighbor table
	for ( std::map<NodeId_t,NeighborInfo *>::const_iterator ni
		= this->nbrTable.begin(); ni != this->nbrTable.end(); ni++ ) 
	{
		NeighborInfo *i = ni->second;
		if (i) {
			delete i; // ???
		}
	}

	Debug_Printf(DBG_PATTERN,  "RCop::~RCop: Done\n" ); fflush(stdout);
};


  
/** Strategy Section - which Items to send ... to whom. */

/// Compute amount of improvement by sending node's info to neighbor
int32_t RCop::CalcImprovement(NeighborInfo *nbr, RCopTableEntry *e) {
  CopTimestamp_t saw = nbr->has[e->item.source];
  if (saw > e->item.sourceTime) // neighbor has newer
    return 0;
  return (saw - e->item.sourceTime);   // negative - lower is better
}

// send anew or retransmit
void RCop::SendFMsg(NodeId_t destination, FMessage_t *m, uint16_t nonce) {
	ASSERT_MSG(this->pktHelper.OkToSend(), "There are pending packets in the buffer, wait for it.");
	
	NodeId_t destArray[MAX_DEST];
	destArray[0] = destination;
	this->FRAMEWORK->SendData(this->PID, // pattern ID
		destArray, // destinations
		1,         // # dests
		*m,
		nonce         // nonce
	);
}

bool RCop::SendToNeighbor(NodeId_t destination, CopMessage& m) {
	// NeighborInfo *ni = nbrTable[destination];
	if (!nbrTable.count(destination) ||    // XXX can't send until we have nbrs.
		(m.GetWireLength() > this->maxBytesInPayload)) {
		Debug_Printf(DBG_PATTERN,"RCop::SendToNeighbor: no neighbor to send to.");
		return false;
	}
	// create a buffer
	uint8_t *data = new uint8_t[this->maxBytesInPayload];
	uint8_t *endPtr = m.ToWire(data,this->maxBytesInPayload);
	unsigned wireSize = endPtr - data;
	FMessage_t *msgToSend = new FMessage_t;
	msgToSend->SetPayloadSize(wireSize);
	msgToSend->SetPayload(data);
	msgToSend->SetSource(MY_NODE_ID);

	uint16_t n = this->nonce++;
	if (n==0) { n = this->nonce++;}
	if (n==0) { n = this->nonce++;} //Second one handles roleover.
	
	this->pktHelper.PreSendingNotice(n, msgToSend);
	/*
	PendingMessageRecord *saved = new PendingMessageRecord();
	saved->msg = msgToSend;
	saved->lastSubmitted = SysTime::GetEpochTimeInMicroSec();
	saved->dest = destination;
	uint16_t n = this->nonce++;
	if (n == 0) { // we sent 64K messages already, how quaint
		n = this->nonce++; // need nonce nonzero in case of retrans.
	}
	*/
	
	/*if (this->bufferedMsgs.count(n) > 0) { // uh-oh
		Debug_Printf(DBG_PATTERN,"RCop::SendToNeighbor: message already buffered!");
		delete this->bufferedMsgs[n];
	}
	this->bufferedMsgs[n] = saved; // remember state for later
	*/
	
	/*  Debug_Printf_Cont(DBG_PATTERN,
				"RCop: buffering message w/ nonce=%u,wiresize=%u.\n",
				n,wireSize);
	*/
	
	if(this->pktHelper.OkToSend()){
		Debug_Printf_Cont(DBG_PATTERN, "    ... passing to framework to \n");
		this->SendFMsg(destination,msgToSend,n);
		this->pktHelper.PostSendingNotice(n);
	}else {
		Debug_Printf_Cont(DBG_PATTERN, "    ... FW not ready, queueing message.\n");
		this->pktHelper.UpdateStatus(n, WAITING);
		return false;
	}
	/*
	if (this->OKToSend) {
		this->SendFMsg(destination,msgToSend,n);
	} else {
		Debug_Printf_Cont(DBG_PATTERN, "    ... FW not ready, queueing message.\n");
		saved->status = WAITING;
		return false;
	}*/
	return true;
} // SendToNeighbor



/// Select some nodes to transmit to, and some items to send to them.
/// Construct and send messages.
void RCop::ShareInfo () {
  std::map<NodeId_t,NeighborInfo *>::const_iterator nbrit;  // neighbor
  std::map<NodeId_t,RCopTableEntry *>::const_iterator nodeit;   // global table entry
  std::set<NodeSelector> nbrs;       // prioritized neighbors

  /* for each neighbor:
   *   for each node in the global table:
   *     figure out how much is gained by sending this neighbor this node's info
   *     keep the (node,amount) pair in a sorted list with the neighbor.
   * Then sort neighbors by total amount gained
   */
  for (nbrit = nbrTable.begin(); nbrit != nbrTable.end(); nbrit++) { // for each nbr
    NeighborInfo *ni = nbrit->second;
    if (ni==0)  // XXX ?? Can this happen?  Don't see how...if we're single-threaded
      continue;
    NodeSelector selection(nbrit->first,0); // priority accumulator for this nbr
    for (nodeit = copTable.begin(); nodeit != copTable.end(); nodeit++) {
      RCopTableEntry *tabent = nodeit->second;
      assert(tabent->item.source==nodeit->first);  // invariant!
      if (tabent->item.source==nbrit->first)  // don't send it info about itself
	continue;
      int32_t improvement = CalcImprovement(ni,tabent);
      selection.priority += improvement;
      ni->needs.insert(NodeSelector(tabent->item.source,improvement));
    }
    nbrs.insert(selection);  // add this neighbor to the priority list
  }

  unsigned int msgsSent = 0;

  // iterate over neighbors in order of decreasing improvement
  std::set<NodeSelector>::const_iterator nextNeighbor = nbrs.begin();
  CopMessage msg(this->myInfo,0,PTN_Utils::PatternAliveTimeMilliSec());
  while (nextNeighbor != nbrs.end() && msgsSent < this->maxPacketsPerCycle) {
    /* add items in order of improvement to this node */
    NeighborInfo *ni = nbrTable[nextNeighbor->id];
    assert(ni != 0); // XXX What is this checking for?
    /* invariant: ni points to NeighborInfo in the pair NextNeighbor */
    if (!ni->pending.empty()) { // XXX DEBUG
      Debug_Printf(DBG_PATTERN, "RCop: ShareInfo found pending set nonempty for %u",nextNeighbor->id);
      ni->pending.clear();
    }
    std::set<NodeSelector>::const_iterator nodeselect = ni->needs.begin();
    while (nodeselect != ni->needs.end() && nodeselect->priority < 0) {
		RCopTableEntry *entry = copTable[nodeselect->id];
		assert(entry != 0);
		CopNodeInfo& item = entry->item;
		if (msg.GetWireLength() + item.GetWireLength() > this->maxBytesInPayload)
			break;
		msg.AddItem(item);
		NodeState state(item.source,item.sourceTime);  // keep track of items in transit
		ni->pending.insert(state);
		nodeselect++;    // next node to send
    }
    /* nodeselect == ni->needs.end() OR the item wouldn't fit.
     * While there's room, add some (node,time) pairs
     */
    while (nodeselect != ni->needs.end() &&
	   msg.GetWireLength()+sizeof(NodeState)<=this->maxBytesInPayload) {
      RCopTableEntry *ent = copTable[nodeselect->id];
      assert(ent != 0); // paranoia
      NodeState ns(ent->item.source,ent->item.sourceTime);
      //msg.AddHave(ns);
      nodeselect++;
    }
    // XXX Assumption: ack handler cannot be invoked before we finish here.
    // When we go multi-threaded, the order of these ops will have to change.
    if (this->SendToNeighbor(nextNeighbor->id,msg)) {
      ni->owesAck = true;
      msgsSent += 1;
    } // XXX else??
    msg.Reset();   // clear everything in the message; go again
    nextNeighbor++;
  }
}  // ShareInfo 
    
void RCop::CustomActivityTimerHandler(uint32_t event) {
	this->ShareInfo();
	activityTimer->Change ( this->GetRandomPublishDelay(), PERIODIC );
};

void RCop::ProcessCopMessage (CopMessage& inMsg) {
	CopNodeInfo sourceItem = inMsg.GetSourceInfo();
	NodeId_t src = sourceItem.source;
	CopTimestamp_t currentTime = PTN_Utils::PatternAliveTimeMilliSec();
	NeighborInfo *nbrInf;
	if (nbrTable.count(src)==0) {  // source was not a known neighbor
		// add it to the neighbor table
		nbrInf = new NeighborInfo();
		nbrTable[src] = nbrInf;
		Debug_Printf(DBG_ERROR, "Rcop::learning of neighbor %u from incoming msg.",
				src);
		// fill in the neighbor
		nbrInf->id = src;
		nbrInf->lastHeardFrom = PTN_Utils::PatternAliveTimeMilliSec();
	}
	unsigned stale=0, current=0, fresh=0, sawmine=0;

	// process the sender's item
	RCopTableEntry *r = copTable[src];
	if (r==0) { // source was previously unknown -- see above
		r = new RCopTableEntry();
		r->item.source = src;     // establish invariant
		// XXX are these necessary?
		r->item.distance = 0;
		r->item.dataSize = 0;
		r->item.sourceTime = 0;
		this->copTable[src] = r;
		r->seen = 0;  // should this be 1?
	}
	assert(r->item.source == sourceItem.source);
	/* r == copTable[src] AND 
	* r->item.source==sourceItem.source */
	if (sourceItem.sourceTime > r->item.sourceTime) {
		r->item.dataSize = sourceItem.dataSize;
		memcpy(r->item.data,sourceItem.data,sourceItem.dataSize);
		r->item.distance++;    // XXX old path may be shorter...
		r->item.sourceTime = sourceItem.sourceTime;
		r->seen = 1;
		r->lastReceived = currentTime;
		fresh++;
	} else if (sourceItem.sourceTime < r->item.sourceTime) {
		// ignore the older information
		Debug_Printf(DBG_PATTERN, "RCop: TCH: msg src info older than table info?");
		stale++;
	} else { /* sourceItem.sourceTime == r->item.sourceTime */
	current++;
		if (r->item.distance > sourceItem.distance+1)  // found a shorter path
			r->item.distance = sourceItem.distance+1;    
		r->seen++;
		r->lastReceived = currentTime;
	}

	// TimeStamp process the other items in the message.
	//for (int i=0; i< inMsg.GetNumberOfItems() ; i++ ) {
	for (CopItemList_t::Iterator i = inMsg.ItemsBegin(); i != inMsg.ItemsEnd() ; ++i ) {
		NodeId_t n = i->source;
		if (n==MY_NODE_ID) { // skip - it came from me
			sawmine++;
			continue;
		}
			
		r = this->copTable[n];
		if (r==0) { 	// first time seeing it
			r = new RCopTableEntry();
			r->item = *i;
			r->item.distance++;
			r->lastReceived = currentTime;
			this->copTable[n] = r;
			fresh++;
		} else {  // there is an existing entry
			if (i->sourceTime > r->item.sourceTime) { // incoming is newer
				fresh++;
				r->lastReceived = currentTime;
				r->item.sourceTime = i->sourceTime;
				// Note that newer incoming info should always define the
				// path length, even if it's longer than we had before - the node may
				// have moved out of our neighborhood, for example.  If we see it
				// again via a shorter path later, we'll save the shorter path below.
				r->item.distance = i->distance+1;
				// get the latest info
				r->item.dataSize = i->dataSize;
				memcpy(r->item.data,i->data,i->dataSize);
				r->lastReceived = currentTime;
			} else if (i->sourceTime < r->item.sourceTime) { // stale info
				stale++;
			} else { // we've seen this very item before.
				current++;
				if (r->item.distance > i->distance+1)  // found a shorter path
				{	r->item.distance = i->distance+1;}
				r->seen++;
				r->lastReceived = currentTime;
			}
		} // existing entry
	} // for
	/*** LOG IT ***/
	Debug_Printf(DBG_PATTERN, "RCop:: node %u parsed incoming msg with %d/%d/%d.",
		MY_NODE_ID,fresh,current,stale);
};

/// Maintain per-neighbor information
void RCop::CustomNeighborUpdateHandler(NeighborUpdateParam nbrUpdate) {
	if ((nbrTable.count(nbrUpdate.nodeId) == 0) &&
		(nbrUpdate.changeType != NBR_NEW)) {
		Debug_Printf(DBG_PATTERN, "RCop: non-new update for unkTimeStampn neighbor %u.",
			nbrUpdate.nodeId);
		return;
	}
	NeighborInfo *ni = nbrTable[nbrUpdate.nodeId];
	/* ni == nbrTable[nbrUpdate.nodeId] */
	switch (nbrUpdate.changeType) {
		case NBR_NEW:
			Debug_Printf(DBG_PATTERN, "RCop::NeighborUpdate: new Neighbor.");
			if (ni == 0) { // this might not be true b/c we create the entry on 1st msg.
				ni = new NeighborInfo();
				ni->id = nbrUpdate.nodeId;
				ni->waveform = nbrUpdate.link; // note: struct Link
				nbrTable[ni->id] = ni;
			}
			assert ( nbrTable[nbrUpdate.nodeId] == ni ); 
			break;

		case NBR_DEAD:
			Debug_Printf(DBG_PATTERN, "RCop: Dead Neighbor.");
			/* XXX check for pending acks, clear them */
			if (ni!=0) {
				delete ni;
				nbrTable[nbrUpdate.nodeId] = 0;
			}
			break;

		case NBR_UPDATE:
			assert (ni != 0);
			// XXX
			// float diff = ni->waveform.metrics - nbrUpdate.link.metrics;
			//Debug_Printf(DBG_PATTERN, "RCop::NeighborUpdate: change %f.",diff);
			ni->waveform.metrics = nbrUpdate.link.metrics;
			// XXX anything else?
			break;

		default:
			Debug_Printf(DBG_PATTERN, "RCop::NeighborUpdate: unkTimeStampn type.");
	}
}; // NeighborUpdateEvent


void RCop::CustomDataStatusHandler(DataStatusParam notice) 
{
/*	Debug_Printf_Start(DBG_PATTERN,"RCop::DataStatusHandler - #dests=%d.\n",
		notice.noOfDest);
	PendingMessageRecord *msgRec;
	if (bufferedMsgs.count(notice.nonce)==0) {
		Debug_Printf_Cont(DBG_PATTERN, "  no pending msg w/nonce=%d\n",
			notice.nonce);
	} else {
		//assert(msgRec);
		msgRec = bufferedMsgs[notice.nonce];

		// Even if the # of destinations is 0, we need to look at first entry.
		if (notice.noOfDest == 0)
			notice.noOfDest = 1;

		for (int i=0; i<notice.noOfDest; i++) {
			switch (notice.statusType[i]) {
			case PDN_DST_RECV:
		Debug_Printf_Cont(DBG_PATTERN,
					"  DST_RECV for msgid=%u, n=%u. Deleting.\n",
					msgRec->id,msgRec->nonce);
			msgRec->status = ACKED;
		// OK, we can delete it
		delete msgRec->msg;
		delete msgRec;
		bufferedMsgs.erase(notice.nonce);
		break;

			case PDN_ERROR:
		// XXX what to do?  LOG IT
		Debug_Printf_Cont(DBG_PATTERN,"  ERROR: dest %d, statusValue=%s.\n",
					i,(notice.statusValue[i] ? "true" : "false"));
		break;

			case PDN_FW_RECV:
		// Framework response to our submission
		if (notice.statusValue[i]) {
			if (msgRec->status < FW_ACKED) { // not acked yet.
			msgRec->status = FW_ACKED;
			msgRec->id = notice.messageId;
			} else {
			Debug_Printf_Cont(DBG_PATTERN,"  late/dup FW ACK for nonce=%u.\n",
						msgRec->nonce);
			}
		} else { // message was rejected
			Debug_Printf_Cont(DBG_PATTERN,
					"Message rejected: nonce=%u.\n",notice.nonce);
			msgRec->status = WAITING;
		}
		break;

			case PDN_WF_RECV:
		Debug_Printf_Cont(DBG_PATTERN,"  Unexpected status type WF_RECV.\n");
		Debug_Printf_Cont(DBG_PATTERN,"  Ready=%s, nonce=%u, messageId=%u\n",
					(notice.readyToReceive ? "true" : "false"),
					notice.nonce, notice.messageId);
		msgRec->status = WF_ACKED;
		break;

			case PDN_WF_SENT:
		// it's been transmitted (presumably), so we can TimeStamp forget about it.
		Debug_Printf_Cont(DBG_PATTERN,"  WF transmitted.\n");
		msgRec->status = WF_SENT;
		break;

			default:
		Debug_Printf_Cont(DBG_PATTERN,"  UnkTimeStampn status type!\n");
			} // switch
		} // for each destination...
	}

	// TimeStamp check if we have any messages waiting to be accepted by framework.
	this->OKToSend = notice.readyToReceive;
	if (notice.readyToReceive) {
		std::map<uint16_t,PendingMessageRecord*>::const_iterator iter;
		iter = this->bufferedMsgs.begin();
		// OKToSend may change in SendData, so we have to keep checking it...
		while (iter != this->bufferedMsgs.end() && this->OKToSend) {
			msgRec = iter->second;
			if (msgRec == 0) {
				Debug_Printf_Cont(DBG_PATTERN,"XXX bufferedMsgs has null entry @ %u.\n",
					iter->first);
				continue;
			}
			if (msgRec->status == WAITING) {
				Debug_Printf_Cont(DBG_PATTERN," ...found waiting msg w/id=%u.\n",msgRec->id);
				msgRec->status = SUBMITTED;
				msgRec->lastSubmitted = this->PatternAliveTimeStamp();
				this->SendFMsg(msgRec->dest,msgRec->msg,msgRec->nonce);
			}
			iter++;
		} // while
	} // notice.readyToReceive
	
*/	
}; // customDataStatusHandler()


void RCop::CustomControlRspHandler(ControlResponseParam response) {
	switch( response.type ) {
		case PTN_AddDestinationResponse:
			Debug_Printf_Cont(DBG_PATTERN,"AddDestination.\n");
			break;

		case PTN_ReplacePayloadResponse:
			Debug_Printf_Cont(DBG_PATTERN,"ReplacePayload\n");
			break;

		case PTN_CancelDataResponse:
			Debug_Printf_Cont(DBG_PATTERN,"CancelData\n");
			break;

		default:
			Debug_Printf_Cont(DBG_PATTERN,"UnkTimeStampn type!\n");
	} // switch

}; // customControlRspHandler


}; // end  Cop namespace
}; // end Pattern namespace


