////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef RCOP_H_
#define RCOP_H_

#include <PAL/PseudoRandom/UniformRandomValue.h>
#include <PAL/PseudoRandom/UniformRandomInt.h>
#include <Interfaces/Pattern/PatternBase.h>

#include "Cop.h"   // incl. namespaces Patterns, Patterns::CopAux

#include <cassert> // assert

#include "Lib/Logs/logPacket.h" // PacketLogger  (XXXXX)

using namespace PWI;
using namespace PAL;
using namespace Patterns::Cop;



namespace Patterns {
namespace Cop {

class RCop : public Cop {
public:
	RCop(char* name = (char*)"RCop_1"){};
	~RCop();

private:
	// info we keep about all other nodes in the network
	struct RCopTableEntry {
		CopNodeInfo item;                  // exportable info
		CopTimestamp_t lastReceived;   // when we last got this info
		uint16_t seen;                 // how many times we rcvd it
		// XXX consider keeping old NeighborInfo here as lower bound on what it knows
	};

	std::map<NodeId_t, RCopTableEntry *> copTable;
	//std::map<uint16_t,PendingMessageRecord *> bufferedMsgs;

	/* Prioritize nodes; one list of these per neighbor. */
	struct NodeSelector {
		NodeId_t id;
		int32_t priority;
		bool operator<(NodeSelector const& r) const {
	return (priority < r.priority) || (priority==r.priority && (id<r.id));
		};
	NodeSelector(NodeId_t n,int32_t i):id(n),priority(i) { };
	};

	struct NeighborInfo {
		Link waveform;               // how it is known to lower layer(?)
		// XXX waveform.metrics.quality is the only thing used.
		CopTimestamp_t lastSentTo;
		CopTimestamp_t lastHeardFrom;  // received a msg from
		CopTimestamp_t lastAcked;      // received an ack from
		std::map<NodeId_t,CopTimestamp_t> has;
		std::set<NodeSelector> needs;  // which items to send (currently unused)
		std::set<NodeState> pending;   // in transit to it; xfer to has on ack
		NodeId_t id;                   // redundant/paranoia
		/* invariant:  !owesAck => (pendingMsgId == 0 && sentMsgNonce == 0) */
		bool owesAck;        // owes us an ack
		MessageId_t pendingMsgId;
		uint16_t sentMsgNonce;
	NeighborInfo(): lastSentTo(0), lastHeardFrom(0),
	lastAcked(0), id(0), owesAck(false), pendingMsgId(0),
	sentMsgNonce(0) {}
	};

	std::map<NodeId_t, NeighborInfo *> nbrTable;

	void ShareInfo();

	void ProcessCopMessage(CopMessage& input); // overrides base
	void CustomActivityTimerHandler(uint32_t event); // overrides base
	void CustomControlRspHandler(ControlResponseParam response); // overrides
	void CustomNeighborUpdateHandler(NeighborUpdateParam update); // overrides
	void CustomDataStatusHandler(DataStatusParam status);  // overrides base

	int32_t CalcImprovement(NeighborInfo *nbr, RCopTableEntry *e);

	bool SendToNeighbor(NodeId_t n, CopMessage& m);
	void SendFMsg(NodeId_t dest, FMessage_t *m, uint16_t n);

};  // class RCop
    
}; // Cop namespace
}; // pattern namespace

#endif // RCOP_H_
