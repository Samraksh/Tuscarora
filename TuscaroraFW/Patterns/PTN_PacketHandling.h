////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef PTN_PACKET_HANDLER_H_
#define PTN_PACKET_HANDLER_H_


#include <PAL/PAL.h>
#include <Interfaces/Pattern/PatternBase.h>
#include <Lib/DS/BSTMapT.h>

#include "PTN_Utils.h"

using namespace PAL;

namespace Patterns {

enum PendingMessageStatusE {
	WAITING=0,    // queued, waiting for framework to accept
	SUBMITTED,    // waiting for framework response
	REJECTED,     // framework rejected, awaiting retry
	FW_ACKED,     // framework accepted, waiting for last ack...
	WF_SENT,      // waveform transmitted (awaiting dest ack).
	DST_ACKED     // waveform received
	//ACKED,        // can be removed (should not see this)
	// we should not need anything beyond this - if dest acks it can be removed.
};
   
/* structure to hold messages while waiting for acks from FW/WF/Dest. */
struct AckPendingMsgRecord {
	FMessage_t *msg;    // Copy of the message that was passed.
	MessageId_t id;    // global ID assigned by FW  (Why??)
	mutable PatternTimestamp_t lastSubmitted;
	uint16_t nonce;     // key for map; redundant but helpful.
	/* All of our messages are sent to either broadcast or a single destination.
	* If we have to retransmit a unicast message we'll need to
	* remember the destination.  But do we really want to unicast?
	*/
	NodeId_t dest;   // ignored in BCop, used in RCop.
	mutable PendingMessageStatusE status;  // 0 == submitted
	// XXX it is possible for this to do the wrong thing if messages are pending
	// when the nonce rolls over.
	bool operator<(AckPendingMsgRecord const& r) const {
	return nonce < r.nonce;
	}
}; // struct PendingMessageRecord

typedef BSTMapT<Nonce_t, AckPendingMsgRecord*> PendingMsgBuffer_t;

class PacketBufferHelper {
	//bool OKToSend;
	int16_t waitingToBeSent;
	PendingMsgBuffer_t ackPendingBuffer;
	uint16_t outStandingPkts=1;
public:
	PacketBufferHelper(uint16_t _outStandingPktsAllowed=1){
		waitingToBeSent=0;
		outStandingPkts=_outStandingPktsAllowed;	
	}
	inline bool OkToSend(){
		return (waitingToBeSent <= outStandingPkts-1);
	}
	
	inline int16_t GetPendingPackets(){
		return waitingToBeSent;
	}
	
	void PreSendingNotice (Nonce_t nonce, FMessage_t* msg){
		AckPendingMsgRecord *saved = new AckPendingMsgRecord();
		if (nonce==0) { 
			Debug_Printf(DBG_PATTERN, "PacketHandlingHelper::PreSendingNotice: Nonce is ZERO check your logic\n");
		}
		
		saved->msg = msg; // XXX why do we need this?
		saved->lastSubmitted = PTN_Utils::PatternAliveTimeMilliSec();
		ackPendingBuffer[nonce] = saved; // remember state for later
		Debug_Printf(DBG_PATTERN, "PacketHandlingHelper::PreSendingNotice: buffering nonce=%u,payload size=%u.\n", nonce, msg->GetPayloadSize());
		//saved = bufferedMsgs[nonce];
	}
	
	bool UpdateStatus(Nonce_t nonce, PendingMessageStatusE _status){
		if(ackPendingBuffer.Find(nonce)!= ackPendingBuffer.End()){
			ackPendingBuffer[nonce]->status = _status;
			if(_status == WAITING) waitingToBeSent++;
			return true;
		}
		return false;
	}
	
	void PostSendingNotice (Nonce_t nonce){
		if(ackPendingBuffer.Find(nonce)!= ackPendingBuffer.End()){
			waitingToBeSent--;
			ackPendingBuffer[nonce]->status = SUBMITTED;
		}
	}
	
	void FWAckNotice (Nonce_t nonce, MessageId_t mid) {
		if(ackPendingBuffer.Find(nonce)!= ackPendingBuffer.End()){
			ackPendingBuffer[nonce]->id = mid;
			ackPendingBuffer[nonce]->status = FW_ACKED;
		}
	}
	void WF_SentNotice (Nonce_t nonce, MessageId_t mid) {
		if(ackPendingBuffer.Find(nonce)!= ackPendingBuffer.End()){
			ackPendingBuffer[nonce]->status = WF_SENT;
		}
	}
	void DeleteMessage (Nonce_t nonce) {
		if(ackPendingBuffer.Find(nonce)!= ackPendingBuffer.End()){
			ackPendingBuffer[nonce]->status = DST_ACKED;
			ackPendingBuffer.Erase(nonce);
		}
	}
	
	AckPendingMsgRecord* GetWaitingMessage(){
		if (waitingToBeSent==0) return NULL;
		PendingMsgBuffer_t::Iterator i=  ackPendingBuffer.Begin();
		for (; i !=ackPendingBuffer.End(); ++i){
			if(i->Second()->status==WAITING){
				return i->Second();
			}	
		}
		return NULL;
	}
};
	
	
	
}

#endif