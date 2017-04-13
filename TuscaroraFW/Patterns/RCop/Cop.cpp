////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include "Cop.h"
#include <cstdio> // sscanf
#include "Lib/Misc/datastructs.h" // map_t, vector_t
#include "Lib/Logs/logString.h"
#include <cassert> // assert
#include <string>
#include <cstring> // std::memcpy
using std::memcpy;

#include "Framework/Core/Naming/StaticNaming.h"
#include "CopTypes.h"

using namespace Core::Naming;
using namespace Patterns::Cop;

extern NodeId_t MY_NODE_ID;


#define DEBUG_COP 1

namespace Patterns {
namespace Cop {

// local utility 
void Cop::dumpBytes ( uint16_t size, uint8_t *payload ) {
	while ( size > 0 ) {
		Debug_Printf_Cont(DBG_PATTERN,  "%2.2x ",*payload++);
		size--;
	}
	Debug_Printf_Cont(DBG_PATTERN, "\n");
};


void Cop::Cop::PrintCopTable()
{
	Debug_Printf(DBG_PATTERN,  "Cop::CopTable: Report for Node %u, has %u items \n", MY_NODE_ID, copTable.Size());
	Debug_Printf(DBG_PATTERN,  "\t|Node ID|\t |Distance|\t |ReceviedTime|\t |SourceTime|\t |Staleness|\t |Seen|\n");
	
	RowList_t printingList;
	
	for ( CopTable_t::Iterator it = this->copTable.Begin();
		it != this->copTable.End(); ++it ) {
		Row *r = it->Second();
		printingList.Insert(r);
		//Debug_Printf(DBG_PATTERN,  "PrintCopTable::InsertIntoPrintList: %u\t %f\t %lu\t %lu\t %2.3f\t %u\n", r->item.GetSource(), 
		//	r->item.distance,r->received , r->item.sourceTime, double(r->received - r->item.sourceTime)/1000, r->seen ) ;	
	}
	//if(copTable.Size() <=0) return; 
	 uint16_t i=0;
	for (; i < printingList.Size(); i++) {
		Row *r = printingList.GetItem(i);
		Debug_Printf(DBG_PATTERN,  "\t%u\t %f\t %lu\t %lu\t %2.3f\t %u\n", r->item.GetSource(), 
			r->item.distance,r->received , r->item.sourceTime, double(r->received - r->item.sourceTime)/1000, r->seen ) ;
	}
	
	Debug_Printf (DBG_PATTERN ,  "Cop::CopTable: ========== End Report \n\n");
}

Cop::Cop(char* name)
: PatternBase(Core::Naming::COP_PTN, (char *) name)
{
	PTN_Utils::InitializeStateTime();
	patternName=string(name);
	myNodeId=MY_NODE_ID;
	Notify=0;
	copPeriod=COP_DEFAULT_PERIOD; // average period in seconds
	distanceSensitivity=COP_DEFAULT_SENSITIVITY; // denominator, in hops
	maxBytesInPayload=COP_DEFAULT_PAYLOAD;
	maxPacketsPerCycle=COP_DEFAULT_PPC;
	startTime=SysTime::GetEpochTimeInMilliSec();
	activityTimerDel=NULL; activityTimer=NULL;
	stopped=false; nonce=0; 
	randPublishDelay = new UniformRandomInt(copPeriod*1000000*0.5, 1.5*1000000*copPeriod);
	
	myNbrTable = new PatternNeighborTable(QUAL_XMIT_DELAY_LC);  
	Debug_Printf(DBG_PATTERN, "Cop:: constructor for node %u", this->myNodeId );
};
  
  
Cop::~Cop () {  // XXX what should be done here vs. subclass?
  Debug_Printf(DBG_PATTERN,  "Cop::~Cop: Done\n" ); fflush(stdout);
};

void Cop::ActivityTimerHandler(uint32_t event) {
	Debug_Printf(DBG_PATTERN,"Cop::ActivityTimerHandler called.\n");
	activityTimer->Change(this->GetRandomPublishDelay(), ONE_SHOT);
	this->CustomActivityTimerHandler(event);

	#if DEBUG_COP==1
		PrintCopTable();
	#endif
}

/* Called from ControlResponse handler after attributes are obtained */
void Cop::StartTimers() {
	Debug_Printf(DBG_PATTERN, "Cop:: Starting activityTimer; startTime = %lu \n", this->startTime);
	activityTimerDel = new TimerDelegate ( this, &Cop::ActivityTimerHandler );
	//assert ( this->activityTimerDel && "Non null activity timer delegate");
	this->activityTimer =  new Timer( this->GetRandomPublishDelay(), ONE_SHOT, *activityTimerDel);
	

	if ( this->startTime == 0 ) this->startTime = PTN_Utils::PatternAliveTimeMilliSec();

	Debug_Printf(DBG_PATTERN, "Cop:: Starting activityTimer; startTime = %lu \n", this->startTime);
	this->activityTimer->Start();
}

/*  This starts a sequence of Control events, as follows:
 *  RegisterPatternRequest
 *  RegisterpatternResponse -> FrameworkAttributesRequest
 *             (from ControlResponseEvent)
 *  FrameworkAttributesResponse -> FinishStartup(),
 *       which issues a bunch of other control requests, including StartTimers
 */

bool Cop::Start() {
    Debug_Printf(DBG_PATTERN,"Cop::Start() called ...\n");

	assert(this->patternState == NO_PID);  // XXX
	this->nonce = COP_INITIAL_NONCE;
	//this->OKToSend = true;
	this->myInfo.source = MY_NODE_ID;
	this->StartTimers();

	Debug_Printf(DBG_PATTERN, "Cop::Start: sending RegisterPatternRequest...\n");
	// N.B.: record request BEFORE issuing, because some responses are returned
	// synchronously (i.e., callback invoked before request method returns)
	// and thus will be unexpected.
	this->pendingControlResponses.insert(PTN_RegisterResponse);
	this->requestState = WAITING_FOR_CONTROL_RESPONSE; // XXX notused...
	this->FRAMEWORK->RegisterPatternRequest (PID, patternName.c_str(), 
		Core::Naming::COP_PTN);
	return true;
};

bool Cop::Stop() {
  if (this->stopped) {
    Debug_Printf(DBG_PATTERN,"Cop::Stop() called again. \n");
  } else {
    Debug_Printf(DBG_PATTERN,"Cop::Stop() called. Next nonce=%u \n",this->nonce);
    if ( activityTimer ) {
      this->activityTimer->Suspend();
      delete this->activityTimerDel; this->activityTimerDel = 0;
      delete this->activityTimer;    this->activityTimer = 0;
    }
    this->stopped = true;
  }
  return true;
};

/*  
CopTimestamp_t Cop::TimeStamp() {
  return (CopTimestamp_t)SysTime::GetEpochTimeInMilliSec();
};

CopTimestamp_t Cop::PatternAliveTimeStamp(){
	return SysTime::GetEpochTimeInMilliSec() - startTime;
}


/// Return elapsed time in seconds
double Cop::PatternAliveTimeInSecs() {
	double now = PatternAliveTimeStamp()/1000;
	//Debug_Printf(DBG_PATTERN,"Cop::PatternAliveTimeInSecs() Now: %f\n", now);
	return now;
};
*/


bool Cop::Publish ( uint16_t size, uint8_t *data ) {
  assert(size <= COP_DATA_SIZE);  // XXX
  Debug_Printf(DBG_PATTERN,  "Cop::Publish: size = %u, data = %s\n", size, (char*)data );
  //  dumpBytes(size,data);
  this->myInfo.SetData(size, data);
  this->myInfo.sourceTime = PTN_Utils::TimeStampMilliSec();
  return true;
};

/// Publish a C-string, writing it into a new'd buffer.
/// Silently truncates if data is too long.
bool Cop::Publish (string s) {
  unsigned length = COP_DATA_SIZE;
  char* data = new char[length];
  snprintf(data, length, "%s", s.c_str());
  this->Publish(length,(uint8_t *) data);
  return true;
};

// XXX FIXME: look this over, check for subclass-specific stuff.
void Cop::ControlResponseEvent(ControlResponseParam response) {

	std::set<ControlResponseTypeE>::iterator elt =
	this->pendingControlResponses.find(response.type);
	if (elt == this->pendingControlResponses.end()) {  // not found
		Debug_Printf(DBG_PATTERN,"Cop::ControlResponseEvent - unexpected type %d",
				response.type);
		return;
	}
	/* elt points to the expected response */
	this->pendingControlResponses.erase(elt);

	Debug_Printf_Start(DBG_PATTERN,"Cop::ControlResponseEvent invoked of type ");
	switch (response.type) {

		case PTN_RegisterResponse:  // we sent this.
			Debug_Printf_Cont(DBG_PATTERN,"Register\n");
			// Note: Handle_RegisterResponse is defined in PatternBase. (I think.)
			this->Handle_RegisterResponse(response);
			// we are registered. get FW attributes, then finish setup.
			assert(this->patternState == REGISTERED);
			this->pendingControlResponses.insert(PTN_AttributeResponse);
			this->FRAMEWORK->FrameworkAttributesRequest(PID);
			break;

		case PTN_AttributeResponse:
			Debug_Printf_Cont(DBG_PATTERN,"Attribute:\n");
			{
				FrameworkAttributes *atr = (FrameworkAttributes*) response.data;
				fwAttrs = *atr;
				Debug_Printf_Cont(DBG_PATTERN,
					"   ====Framework has %d wfs, max pkt size is %d.\n",
					atr->numberOfWaveforms, atr->maxFrameworkPacketSize);
				for(int i=0; i < atr->numberOfWaveforms; i++) {
					Debug_Printf_Cont(DBG_PATTERN,"   ==== Waveform[%d] Id is %d\n",
						i, atr->waveformIds[i]);
				}
				if (atr->maxFrameworkPacketSize < this->maxBytesInPayload){
					this->maxBytesInPayload = atr->maxFrameworkPacketSize;
				}
				this->FinishStartup();
			}
			break;

		case PTN_SetLinkThresholdResponse:
			Debug_Printf_Cont(DBG_PATTERN,"Cop:: SetLinkThreshold\n");
			break;

		case PTN_SelectDataNotificationResponse:
			Debug_Printf_Cont(DBG_PATTERN,"Cop::  SelectDataNotification...");
			{
				SelectDataNotificationResponse_Data *data =
			(SelectDataNotificationResponse_Data*) response.data;
				if (data->status) {
			Debug_Printf_Cont(DBG_PATTERN,"success.\n");
				} else {
			Debug_Printf_Cont(DBG_PATTERN,"Failed???, shouldnt! \n");
			// XXXX What to do?? Try again?  Why would that work?!?
				}
			}
			break;

		/* everything else is going to depend on the specific strategy.
			* So we callout to it...
			*/
		default:
			Debug_Printf_Cont(DBG_PATTERN,"event to be handled in subclass...");
			this->CustomControlRspHandler(response);
			break;
	} // switch

	// XXX Does this actually work?  We may have handled several responses
	// before we actually get here...
	size_t numPending = this->pendingControlResponses.size();
	if (numPending==0)
		this->requestState = NONE_PENDING;
	else
		this->requestState = WAITING_FOR_CONTROL_RESPONSE;
} // end of ControlResponseEvent()


/* Submit the final control requests:
 * select Link Comparator
 * set Link metric threshold
 * select data notifications
 * after this we should be getting messages.
 */ 
void Cop::FinishStartup() {
  this->patternState = EXECUTING;
  Debug_Printf_Cont(DBG_PATTERN, "   Cop::FinishStartup: EXECUTING\n");

  LinkMetrics myThreshold;
  myThreshold.quality = COP_DEFAULT_QUALITY_THRESHOLD;
  Debug_Printf_Cont(DBG_PATTERN, "Cop::FinishStartup:   ...Setting link threshold to %f.\n",     myThreshold.quality);
  this->pendingControlResponses.insert(PTN_SetLinkThresholdResponse);
  this->FRAMEWORK->SetLinkThresholdRequest(PID,myThreshold);

  // XXX there is no response type for this!
  this->FRAMEWORK->SelectLinkComparatorRequest(PID, QUALITY_LC);

  this->pendingControlResponses.insert(PTN_SelectDataNotificationResponse);
  this->FRAMEWORK->SelectDataNotificationRequest(this->PID,PDN_WF_RECV_MASK);

  // some of the above may have been handled inline, i.e., responses may
  // already have been received.
  if (!this->pendingControlResponses.empty()) {
    requestState = WAITING_FOR_CONTROL_RESPONSE;
  } else {
    requestState = NONE_PENDING;
  }
}

void Cop::NeighborUpdateEvent (NeighborUpdateParam nbrUpdate) {
  //Debug_Printf(DBG_PATTERN,"Cop::NeighborUpdateEvent.\n");
  if(myNbrTable != NULL) myNbrTable->UpdateTable(nbrUpdate);
  this->CustomNeighborUpdateHandler(nbrUpdate);
}

/* incoming message.  Recipient is responsible for deleting. */
void Cop::ReceiveMessageEvent (FMessage_t& msg) {
	uint16_t payloadSize = msg.GetPayloadSize();
	uint8_t* payload = msg.GetPayload();
	Debug_Printf(DBG_PATTERN,  "Cop::ReceiveMessageEvent: msg length=%u\n",
			msg.GetPayloadSize() );
	uint8_t* endOfPayload = payload + payloadSize;
	CopMessage copMsg;
	// record the time
	if (CopMessage::FromWire(payload,endOfPayload,copMsg)) {
		Debug_Printf_Cont(DBG_PATTERN,"Cop: input msg parsed successfully.\n");
		const unsigned clockWobble = 10000;
		// XXX DEBUG
		CopTimestamp_t sentt = copMsg.GetSentTime();
		CopTimestamp_t jetzt = PTN_Utils::PatternAliveTimeMilliSec();
		if ((sentt > jetzt + clockWobble) ||
		(sentt + clockWobble < jetzt)) {
			Debug_Printf_Cont(DBG_PATTERN, 	"Cop: received packet %lu usec out of sync.",
				(sentt > jetzt ? sentt - jetzt : jetzt - sentt));
		}

		this->ProcessCopMessage(copMsg);

	} else { // parse failure
		Debug_Printf_Cont(DBG_ERROR, "Cop:: failed to parse incoming message.\n");
	}
	
	//Debug_Printf(DBG_PATTERN,  "Cop::ReceiveMessageEvent: Done Processing.\n");
	delete &msg;  // grrr. (Just pass the pointer!)
}

void Cop::DataStatusEvent (DataStatusParam notification) {
  //Debug_Printf(DBG_PATTERN,"Cop::DataStatusNotification.\n");
  CustomDataStatusHandler(notification);
}


#if 0
// XXX fix where this is called in BCop!
void Cop::LogStaleness ( ExportRow &erow, int improved, int hops)  {
  stringstream ss;
  ss << this->seconds() << " "
     << (this->PatternAliveTimeStamp() - erow.ts) / 1000000. << " "
     << improved / 1000000. << " "
     << hops << " "
     << erow.hops + 1 << " "
     << erow.nodeId;
  logString ( "cop.staleness.log",
	      "seconds staleness improvement hops_old hops_new fromNode",
	      ss.str() );
};

void Cop::LogBroadcastSize ( unsigned bytes ) {
  stringstream ss;
  ss << this->seconds() << " "
     << bytes << " ";
    //     << this->copTable.size() << " "
  logString ( "cop.broadcastSize.log",
	      "seconds bytesSent ", //copTableEntries,
	      ss.str() );
};

void Cop::LogSentPacket ( PacketId pid, unsigned bytes, unsigned items ) {
  stringstream ss;
  ss << this->seconds() << " "
     << pid.sender << " "
     << pid.sent << " " 
     << (unsigned) pid.seq << " " 
     << bytes;
  //     << snapshots;
  
  logString ( "cop.sentPacket.log",
	      "secondsAtSender sender sentTime seq size",// snapshots",
	      ss.str() );
};
  
void Cop::LogReceivedPacket ( PacketId pid, unsigned stale, unsigned current, unsigned fresh ) {
  // log how many snopshots in a received packet are stale, current, or fresh (contain new info)
  stringstream ss;
  ss << this->seconds() << " "
     << pid.sender << " "
     << pid.sent << " " 
     << (unsigned) pid.seq << " " 
     << stale << " "
     << current << " "
     << fresh;
  logString ( "cop.receivedPacket.log",
	      "secondsAtReceiver sender sentTime seq stale current fresh",
	      ss.str() );
};
#endif

}; // end Cop namespace
}; // end Pattern namespace

