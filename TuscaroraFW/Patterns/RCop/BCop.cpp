////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include "BCop.h"   // gets Cop.h
#include <cstdio> // printf
#include "Lib/Misc/datastructs.h" // map_t, vector_t
#include "Lib/Misc/logString.h"
#include "Lib/Misc/ElapsedTime.h"
#include <cassert> // assert
#include <cstring> // std::memcpy
using std::memcpy;

#include "Framework/Core/Naming/StaticNaming.h"
using namespace Core::Naming;

// extern bool DBG_PATTERN;  // XXX? // set in Lib/Misc/Debug.cpp
extern NodeId_t MY_NODE_ID;

namespace Patterns {
/***** Local utilities ******/
/*  We ignore essentially all of this.
    Note: the "float"s below are actually UFixed_2_8_t
  struct WF_LinkMetrics {
    ///A quality metric expressed as a real number between [0,1]
    float quality;
    ///Datarate expressed as log_2(bps)
    float dataRate; 
    ///Average latency expressed as log_2(seconds)
    float avgLatency;
    ///Average energy to transmit a packet expressed as log_2(pica-joules)
    float energy;
  };

  struct LinkMetrics: public WF_LinkMetrics {
    ///Average cost of sending a packet
    UFixed_7_8_t (i.e., float) cost;
    ///Tuscarora's estimate of the latency for the next transmit
    U64NanoTime expLatencyToXmit;
  };
*/
/*static void dumpNeighborUpdate(NeighborUpdateParam& u) {
  Debug_Printf_Cont(DBG_PATTERN,
		     "ID=wf%u:node%u, LinkType=%u, NodeId=%u, ChangeType=%s",
	  u.link.linkId.waveformId, u.link.linkId.nodeId, u.link.type, u.nodeId,
	  (u.changeType==NBR_NEW ? "NEW" :
	   (u.changeType==NBR_DEAD ? "DEAD" :
	    (u.changeType==NBR_UPDATE ? "UPDATE" : "?????"))));
  if (u.changeType!=NBR_DEAD) { // print link metrics
    Debug_Printf_Cont(DBG_PATTERN, " Quality=%f\n", u.link.metrics.quality);
  } else
    Debug_Printf_Cont(DBG_PATTERN, "\n");
}*/

/* The PatternBase class handles *some* details of getting the pattern ID,
 * registering the pattern/delegates, etc.
 */
BCop::BCop (char* name):
	Cop(name),rowSelectionMethod(Seen) 
{
	// derived from Gossip...
	Debug_Printf(DBG_PATTERN, "BCop::Constructor.")

};
  
BCop::~BCop ()
{
	if ((this->patternState >= GOT_PID) && !this->stopped)  // started...
	this->Stop();

	// free the items in the cop table
	for ( CopTable_t::Iterator it = this->copTable.Begin();
		it != this->copTable.End(); ++it ) {
		Row *r = it->Second();
		if (r) {  // test is redundant TimeStamp, but we might someday remove an entry.
			delete r;
		}
	}
	Debug_Printf(DBG_PATTERN,  "BCop::~BCop: Done\n" );
};


bool BCop::SelectBySeen ( Row *r ) {
	double dist = r->item.distance * this->distanceSensitivity;
	bool wellKTimeStampn = r->seen > 3;
	return !wellKTimeStampn && rng.GetNext() * dist <= 1;
};


//The staleness we have for a node should be propotional to its distance. Select nodes where the staleness is higher than distance
bool BCop::SelectByPropotionalStaleness ( Row *r ) {
	double dist = r->item.distance * this->distanceSensitivity;
	double timeScale = 1.0/1000; //Our timestamps are in milliseconds
	if ( dist == 0 ) return true;  // XXX this can't be true, should be at least 1!
	// If hop count is too low, using rebroadcast times is too aggressive.
	double delay; // delay of 1 == 1 hop == distance of one
	if ( r->item.distance <= 4 ) {
		delay = timeScale * (PTN_Utils::TimeStampMilliSec() - r->received ) / this->copPeriod;
		while ( delay > dist ) delay -= dist; // floating point modulo
		return delay <= 1;
	}
	else { 
		delay = timeScale * ( PTN_Utils::TimeStampMilliSec() - r->rebroadcast ) / this->copPeriod;
		return dist <= delay;
	}
};

//Randomly select nodes where 
bool BCop::SelectByStaleness ( Row *r ) {
	double dist = r->item.distance * this->distanceSensitivity;
	double timeScale = 1.0/1000; //Our timestamps are in milliseconds
	if ( dist == 0 ) return true;
	double delay; // delay of 1 == 1 hop == distance of one
	// If hop count is too low, using rebroadcast times is too aggressive.
	if ( r->item.distance <= 4 ) {
		return rng.GetNext() * dist <= 1;
	}
	delay = timeScale * ( PTN_Utils::TimeStampMilliSec() - r->rebroadcast ) / this->copPeriod;
	double randomizer = .5 + rng.GetNext(); // average value == 1 
	return randomizer * dist <= delay; // randomized version of selectByPropotionalStaleness 
};

void BCop::SetRowSelectionMethod ( BCopRowSelectionMethod method ) {
	rowSelectionMethod = method;
};

bool BCop::SelectRow ( Row *r ) {
	switch (rowSelectionMethod ) {
		case Seen: return SelectBySeen ( r );
		case Staleness: return SelectByStaleness ( r );
		case PropotionalStaleness: return SelectByPropotionalStaleness ( r );
		default:
			Debug_Printf(DBG_ERROR,  "Invalid row selection method\n" );
			exit(1);
	}
};

bool BCop::BroadcastMessage(CopMessage* m, uint16_t n) {
	// create a buffer
	uint16_t bufsize=m->GetWireLength();
	Debug_Printf(DBG_PATTERN,"BCop::BroadMessage: message->wireLength: %u, MaxPyaload: %u\n", bufsize, this->maxBytesInPayload);

	uint16_t itemSize = sizeof(CopNodeInfo);
	if( bufsize > this->maxBytesInPayload){
		bufsize = this->maxBytesInPayload - (this->maxBytesInPayload % itemSize);
	}
	
	uint8_t *data = new uint8_t[bufsize];
	uint8_t *endPtr = m->ToWire(data,bufsize);
	
	unsigned wireSize = endPtr - data;
	Debug_Printf(DBG_PATTERN,"BCop::BroadMessage: dataptr=%p, endPtr=%p,  wireLength=%u, bufSize=%u.\n",data, endPtr, wireSize, bufsize);
	FMessage_t *sendMsg = new FMessage_t;   // check return?!
	sendMsg->SetSource(MY_NODE_ID);
	sendMsg->SetType(PATTERN_MSG);
	
	#if ENABLE_BROADCAST==1
		WaveformId_t radio = fwAttrs->waveformIds[0];
		sendMsg->SetWaveform(radio);
	#endif
		
	sendMsg->SetPayloadSize(wireSize);
	sendMsg->SetPayload(data);
	Debug_Printf(DBG_PATTERN,"BCop::Broadcasting: no of elements=%u, wireSize=%u.\n",m->GetNumberOfItems()+1,wireSize);

	if(wireSize < m->GetWireLength()) {
		Debug_Printf(DBG_PATTERN,"BCop::Broadcasting: Not all items are sent. Need to handle this case.\n");
	}
	
	if (n==0) { n = this->nonce++;}
	if (n==0) { n = this->nonce++;} //Second one handles roleover.
	
	this->pktHelper.PreSendingNotice(n, sendMsg);
	
	
	/*PendingMessageRecord *saved;
	if (n==0) {  // n==0 means it hasn't been sent before.
		// before sending, record it in the set of pending messages.
		saved = new PendingMessageRecord();
		saved->msg = sendMsg; // XXX why do we need this?
		saved->lastSubmitted = PTN_Utils::TimeStampMilliSec();
		n = this->nonce++;
		if (n == 0) { // we sent 64K messages already, how quaint
			n = this->nonce++; // need nonce nonzero in case of retrans.
		}
		bufferedMsgs[n] = saved; // remember state for later
		Debug_Printf(DBG_PATTERN,
			"BCop::BroadcastMessage: buffering nonce=%u,wiresize=%u.\n", n, wireSize);
		} else {
		saved = bufferedMsgs[n];
	}
	*/
	
	
	if(this->pktHelper.OkToSend()){
		Debug_Printf_Cont(DBG_PATTERN, "    ... passing to framework to broadcast.\n");
	#if ENABLE_BROADCAST==1
		this->FRAMEWORK->BroadcastData(this->PID, *sendMsg, sendMsg->GetWaveform(),n);
	#else
		this->RandomLocalSpray(this->PID, *sendMsg, SPRAY_FOUR_NBR, 
			true, *this->myNbrTable, n);
	#endif
		 this->pktHelper.PostSendingNotice(n);
		return true;
	} else {
		Debug_Printf_Cont(DBG_PATTERN, "    ... FW not ready, queueing message.\n");
		this->pktHelper.UpdateStatus(n, WAITING);
		//saved->status = WAITING;
		return false;
	}
	
	
} // BroadcastMessage

void BCop::BroadcastFromCopTable () {
	unsigned rowsSelected = 0;
	uint8_t seq = 0;

	Debug_Printf(DBG_PATTERN, "BCop::BroadcastFromCopTable, CopTable has %u items.\n",
		this->copTable.Size());
	

	CopMessage* msg = new CopMessage(this->myInfo,seq,0);  
	rowsSelected++; // I have added my own info to packet, hence increament

	unsigned bytesSoFar = msg->GetWireLength(); // validates wireLength invariant
	Debug_Printf(DBG_PATTERN, "BCop::BroadcastFromCopTable, Msg length as of now is: %u.\n", bytesSoFar);
	
	for ( CopTable_t::Iterator it = this->copTable.Begin();
		it != this->copTable.End(); ++it ) {
		Row *r = it->Second();
		ASSERT_MSG(r!=0,  "BCop::BroadcastFromCopTable: Something wrong with Cop Table, returnned null pointer\n");
	
		// skip rows with no data (are there any? why?) or which are not selected.
		if ( ! this->SelectRow ( r )) {
			Debug_Printf(DBG_PATTERN,  "BCop::BroadcastFromCopTable: skipping node info %u\n", r->item.GetSource() );
			continue;
		}
		Debug_Printf(DBG_PATTERN,  "BCop::BroadcastFromCopTable: Parsing node data %u\n", r->item.GetSource());
		/* r was selected */ 
		unsigned additionalBytes = r->item.GetWireLength();
		if ( bytesSoFar + additionalBytes > this->maxBytesInPayload ) {
			/* no more room in this packet... send it and start another. */
			ASSERT_MSG(rowsSelected > 0, "No rows are selected");  // make sure we got SOMEthing in the message!
			// XXX DEBUG
			//Debug_Printf(DBG_PATTERN,  "BCop::BroadcastFromCopTable: sending message, time=%lu\n",
			//	PTN_Utils::TimeStampMilliSec() );
			msg->SetSentTime(PTN_Utils::TimeStampMilliSec());
			if (!this->BroadcastMessage(msg,0)) return;
			if (++seq > this->maxPacketsPerCycle) return;
			msg->Reset();   // clear have, items; everything else stays same.
			msg->IncrementSequence();
			msg->SetSentTime(PTN_Utils::TimeStampMilliSec());
			// XX should we update our item? nah...
			//rowsSelected = 0;
			bytesSoFar = msg->GetWireLength();
		}
		bytesSoFar += additionalBytes;
		msg->AddItem(r->item);
		rowsSelected++;
		r->rebroadcast = PTN_Utils::TimeStampMilliSec();
	}

	/* we need to send our own item, even if there's nothing in our table! */
	msg->SetSentTime(PTN_Utils::TimeStampMilliSec());
	Debug_Printf(DBG_PATTERN,  "BCop::BroadcastFromCopTable: sending message of size %u with %d items\n", msg->GetWireLength(), rowsSelected);
	BroadcastMessage(msg,0);
};
  
    
void BCop::CustomActivityTimerHandler(uint32_t event) {
  BroadcastFromCopTable();
  //this->activityTimer->Change ( this->GetBroadcastDelay(), PERIODIC );
};

/*void BCop::CustomNeighborUpdateHandler(NeighborUpdateParam update) {
  Debug_Printf_Start(DBG_PATTERN,"BCop::NeighborUpdateEvent: ");
  dumpNeighborUpdate(update);
}*/

void BCop::CustomDataStatusHandler(DataStatusParam notice) {
	// The protocol says that status updates for broadcast messages
	// have 0 destinations, but actual status is in the 0 element of the array.
	// We only send Broadcasts, so if this #dests is nonzero it's an error.
	//Debug_Printf_Start(DBG_PATTERN,"BCop::DataStatusEvent called; #dests=%d.\n",
	//		notice.noOfDest);
	// we are dealing with a TimeStamp message
	//PendingMessageRecord* msgRec = bufferedMsgs[notice.nonce];
	
	switch (notice.statusType[0]) {
		case PDN_DST_RECV:
			Debug_Printf_Cont(DBG_PATTERN,"  Unexpected status type DST_RECV(!)\n");
			// not supposed to get those for broadcast messages!
			break;

		case PDN_ERROR:
		// XXX what to do?  LOG IT
			Debug_Printf_Cont(DBG_PATTERN,"  Status type ERROR: ");
			Debug_Printf_Cont(DBG_PATTERN,"Ready=%s, nonce=%u, messageId=%u\n",
				(notice.readyToReceive ? "true" : "false"),
				notice.nonce, notice.messageId);
			break;
			
		case PDN_ERROR_PKT_TOOBIG:
			Debug_Printf(DBG_PATTERN,"BCop:: Framework rejected message because its too big\n");
			break;	
			

		case PDN_FW_RECV:
			
			if (notice.statusValue[0]) {
				this->pktHelper.FWAckNotice(notice.nonce, notice.messageId);
				//msgRec->status = FW_ACKED;
				//msgRec->id = notice.messageId;
			} else { // message was rejected
				Debug_Printf_Cont(DBG_PATTERN, "Message rejected: nonce=%u.\n",
					notice.nonce);
				//msgRec->status = WAITING;
				this->pktHelper.UpdateStatus(notice.nonce, WAITING);
			}
			break;

		case PDN_WF_RECV:
			Debug_Printf_Cont(DBG_PATTERN, "  Unexpected status type WF_RECV.\n");
			Debug_Printf_Cont(DBG_PATTERN, "  Ready=%s, nonce=%u, messageId=%u\n",
					(notice.readyToReceive ? "true" : "false"),
					notice.nonce, notice.messageId);
			break;

		case PDN_WF_SENT:
			Debug_Printf_Cont(DBG_PATTERN, "  status type WF_SENT.\n");
			Debug_Printf_Cont(DBG_PATTERN, "  Ready=%s, nonce=%u, messageId=%u\n",
				(notice.readyToReceive ? "true" : "false"),
				notice.nonce, notice.messageId);
			if (!notice.statusValue[0]) { // XXX paranoia
				Debug_Printf_Cont(DBG_PATTERN,"  Sending has failed in the waveform layer. Nothing much we can do, ignoring failure \n.");
			} 
			this->pktHelper.DeleteMessage(notice.nonce);
			break;

		default:
			Debug_Printf_Cont(DBG_PATTERN,"  UnkTimeStampn status type!\n");
	} // end switch

	// Check if there are more packets that can be sent.
	if (notice.readyToReceive && !this->pktHelper.OkToSend()) {
		AckPendingMsgRecord *rcd = this->pktHelper.GetWaitingMessage();
		if(rcd){
			#if ENABLE_FW_BROADCAST==1
				FRAMEWORK->BroadcastData(this->PID,
					*rcd->msg,
					rcd->msg->GetWaveform(),
					rcd->nonce);
			#else
				this->RandomLocalSpray(this->PID, *rcd->msg,    
					Types::SPRAY_FIFTYPERCENT_NBR, true, *this->myNbrTable,        
					rcd->nonce);
			#endif	
			this->pktHelper.PostSendingNotice(rcd->nonce);	
		}
	}
	
	/*if (notice.readyToReceive) {
		std::map<uint16_t,PendingMessageRecord*>::const_iterator iter;
		iter = this->bufferedMsgs.begin();
		// OKToSend may change in BroadcastMessage so we have to keep checking it...
		while (iter != this->bufferedMsgs.end() && this->OKToSend) {
			msgRec = iter->second;
			if (msgRec == 0) {
				Debug_Printf_Cont(DBG_PATTERN,"XXX bufferedMsgs has null entry @ %u.\n",
					iter->first);
				continue;
			}
			if (msgRec->status == WAITING) {
				Debug_Printf_Cont(DBG_PATTERN," ...found waiting msg w/id=%u.\n",
					msgRec->id);
				msgRec->status = SUBMITTED;
				msgRec->lastSubmitted = PTN_Utils::TimeStampMilliSec();
				#if ENABLE_FW_BROADCAST==1
				FRAMEWORK->BroadcastData(this->PID,
					*msgRec->msg,
					msgRec->msg->GetWaveform(),
					msgRec->nonce);
				#else
				this->RandomLocalSpray(this->PID, *msgRec->msg,    
					Types::SPRAY_FIFTYPERCENT_NBR, true, *this->myNbrTable,        
					msgRec->nonce);
				#endif
			}
			iter++;
		} // while
	} // notice.readyToReceive*/
}

void BCop::ProcessCopMessage (CopMessage& inMsg) 
{
	CopTimestamp_t currentTime = PTN_Utils::TimeStampMilliSec();
	unsigned stale=0, current=0, fresh=0, sawmine=0;

	// process items, starting with sender
	CopNodeInfo rcdSourceItem = inMsg.GetSourceInfo();
	Debug_Printf(DBG_PATTERN,"BCop::ProcessCopMessage: source info: %s \n",
		rcdSourceItem.ToStringVerbose().c_str());
	CopTable_t::Iterator x = this->copTable.Find (rcdSourceItem.source);
	Row *r; 
	if (x==copTable.End()) {
	// source was previously unkTimeStampn -- how often will this happen?
	// pretty often, if we use a high threshold.
		Debug_Printf(DBG_PATTERN,"BCop::ProcessCopMessage: Message from a new source %u , adding a entry to the CopTable \n",
			rcdSourceItem.source);
		r = new Row();
		r->item = rcdSourceItem;
		r->seen = 0;
		r->rebroadcast=0;
		r->received = currentTime;
		r->item.distance=1;
		this->copTable.Insert(rcdSourceItem.source, r);
		Debug_Printf(DBG_PATTERN,"BCop::ProcessCopMessage: Coptable has %u entries\n", this->copTable.Size());
		fresh++;
	}
	else {
		r = x->Second();
		// r == copTable[sourceItem.node] && r != NULL 
		if (rcdSourceItem.sourceTime > r->item.sourceTime) { // incoming is newer
			fresh++;
			r->item = rcdSourceItem;
			r->item.distance++;
			r->received = currentTime;
			r->seen = 1;
			if ( this->Notify ) { // callback is set, send data to the app
				(*(this->Notify))(SizedBuffer(rcdSourceItem.dataSize, rcdSourceItem.data));
			}
		} else if (rcdSourceItem.sourceTime < r->item.sourceTime) {
			Debug_Printf(DBG_PATTERN, "BCop::ProcessCopMsg: src info older than table entry. \n");
			stale++;
		} else { /* sourceItem.sourceTime == r->item.sourceTime */
			Debug_Printf(DBG_PATTERN, "BCop::ProcessCopMessage: msg==table sourceTime=%lu. \n",
				rcdSourceItem.sourceTime);
			current++;
			r->seen++;
		}

	}
	// TimeStamp process the other items in the message.
	Debug_Printf(DBG_PATTERN,"BCop::ProcessCopMsg: msg has %d (add'l) items.\n",
		inMsg.GetNumberOfItems());
	
	
	CopItemList_t::Iterator i = inMsg.ItemsBegin();
	for ( ; i != inMsg.ItemsEnd() ; i++ ) {
		NodeId_t n = i->source;
		//Debug_Printf(DBG_PATTERN,"BCop::Processing addl items in the CopMsg of node: %u.\n", n);
		
		if (n==MY_NODE_ID) { // no need to keep our own info!
			sawmine++;
			continue;
		}
		CopTable_t::Iterator y = this->copTable.Find(n);
		//r = this->copTable[n];
		if (y==this->copTable.End()) {
			// first time seeing it
			r = new Row();
			/* establish invariant: copTable[n]->item.source == n */
			r->item = i.operator*() ;
			r->item.distance++;
			r->received = currentTime;
			r->seen = 1;
			
			this->copTable.Insert(n, r);
			fresh++;
		} else {
			r = y->Second();
			if (i->sourceTime > r->item.sourceTime) { // incoming is newer
				fresh++;
				// copy the data in...
				r->item.dataSize = i->dataSize;
				memcpy(r->item.data,i->data,i->dataSize);
				// update metadata
				r->received = currentTime;
				r->item.sourceTime = i->sourceTime;
				r->item.distance = i->distance+1;
				// Note that newer incoming should always define the
				// path length, even if it's longer than we had before - the node may
				// have moved out of our neighborhood, for example.  If we see it
				// again via a shorter path later, we'll save the shorter path below.
				r->seen = 1;
				// r->rebroadcast = ts; // Don't update rebroadcast the first
				// time, otherwise we risk never rebroadcasting this row if it's
				// updated frequently.
			} else if (i->sourceTime < r->item.sourceTime) {
				stale++;
			} else { // we've seen this same item before
				r->rebroadcast = inMsg.GetSentTime();
				// If we get a *current* incoming (something we've seen before) with a
				// *shorter* path, that's useful and we should update it.
				// OTOH if we see something we've already seen with a longer path,
				// it is probably one of our neighbors rebroadcasting our own message.
				if (i->distance+1 < r->item.distance) // found a shorter path
					r->item.distance = i->distance+1;
				current++;
				r->seen++;
			}
		}
	} // end for

	/* XXX log stale, current, fresh - for debugging purposes (at least) */
	//  packetLogger.LogRecv ( payloadSize );
	//  LogReceivedPacket ( pid, stale, current, fresh );
	
	Debug_Printf(DBG_PATTERN,
		"BCop: Processed incoming message. sawmine=%d, stale=%d, current=%d, fresh=%d.\n\n",
		sawmine, stale, current, fresh);
	
	logPrintf ( "packetEffectiveness.log",
		"seconds stale current fresh",
		"%.06lf %u %u %u\n",
		ElapsedTime::seconds(), stale, current, fresh );
	
	//assert logic
	// "sawmine" accounts for seeing our own info, reflected back to me
	//Each message as source + number of items as list. They are put into four bins. All four added should equal total
	//assert(stale + current + fresh + sawmine == inMsg.GetNumberOfItems() + 1);
	ASSERT_MSG (stale + current + fresh + sawmine == inMsg.GetNumberOfItems() + 1, "BCop:: Assert failed: Something wrong in processing incoming msg\n\n");
	
	
	
} // end of ProcessCopMessage()

}; // end Pattern namespace
