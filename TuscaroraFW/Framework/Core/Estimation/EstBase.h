////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 


#ifndef ESTBASE_H_
#define ESTBASE_H_

#include "Framework/Core/Neighborhood/NeighborTable.h"
#include <Interfaces/PWI/Framework_I.h>
#include <Interfaces/Waveform/WF_Types.h>
#include "Lib/PAL/PAL_Lib.h"
#include "EstimationLogging.h"
#include "LinkEstimationTable.h"
//#include <map>
//#include <vector>
#include <cmath>
#include <Lib/DS/ListT.h>

using namespace PAL;
using namespace PWI;
using namespace Core::Neighborhood;

namespace Core {
namespace Estimation {
	
	
typedef uint64_t LinkAddress_t;	
typedef uint64_t TimeStamp_t;


struct EstMsg {
	uint32_t packetSeqNo;
	uint8_t dummyPayload[508];
};


class EstBase : public LinkEstimatorI<LinkAddress_t> {
 protected:
	UniformRandomInt *randInt;
	RandomSchedule<UniformRandomInt, UniformRNGState,uint64_t,  TimeStamp_t> *beaconSchedule;
	EstimatorCallback_I<LinkAddress_t> *callbackI;
	Waveform::WF_Attributes wfAttributes;
	WaveformId_t wid;
	uint32_t id;
	uint32_t leSeqno;
	WF_LinkChangeDelegate_n64_t *fworkNbrDel;
	LinkEstimationTable_n64_t *estimationTable;
	//LinkEstimationTable_NodeId_t *estimationTable;
	EstimationLogging logger;
	//Waveform::WF_Message_n64_t LeMsg;
	//FMessage_t LeMsg;
	int period;
	int dead;
	Timer *cleaner;
	TimerDelegate *state_;
	Delegate<void, EventInfoU64& > *sendHb;
	NeighborTable *coreNbrTable;
	//std::map<NodeId_t, std::vector<timeval> > events;
	BSTMapT<LinkAddress_t,ListT<TimeStamp_t, false, EqualTo<TimeStamp_t> > > events;
	
	WF_LinkChangeDelegate_n64_t *leDel;

	//methods
	void SendHB(EventInfoU64& event);
	double GetQuality(LinkAddress_t linkAddress);
	void LogQualityEvent(LinkAddress_t linkAddress, TimeStamp_t time);
 public:
	//Inherited from interface
	void PotentialNeighborUpdate (Discovery::PotentialNeighbor &pnbr, Discovery::PNbrUpdateTypeE type);
	LinkMetrics* OnPacketReceive(Waveform::WF_MessageBase *rcvMsg); 
	
	~EstBase();
	EstBase(EstimatorCallback_I<LinkAddress_t> &callback);
	EstBase(EstimatorCallback_I<LinkAddress_t> &callback, WaveformId_t _wid);
	EstBase(EstimatorCallback_I<LinkAddress_t> &callback, WaveformId_t _wid, NeighborTable *_coreNbrTable);///< Creates the link estimator, assigning the parent pattern interface, and specifying the neighbor table to use

	LinkMetrics* ProcessMessage(Waveform::WF_MessageBase *rcvMsg, uint64_t _expiryTime);
	bool RegisterDelegate (Core::WF_LinkChangeDelegate_n64_t &del);///< Registers a delegate to inform the framework
	void NodeExpired(WF_LinkEstimationParam_n64_t _param);
	void SetParam(uint32_t beaconPeriod, uint32_t inactivePeriod);///< Sets the beaconing period and the number of beacons that can be missed before invalidating the link. This also starts the link estimation module
	void CleanUp(uint32_t event);///< This checks the neighbor table for invalid links, changing them from a Neighbor to a PotentialNeighbor if the link's expiry time has been reached without overhearing a beacon
	//void ProcessMessage(NodeId_t nbr);
	///< This is called when the framework receives a link estimation message with the sender as its parameter.
	
//	bool EvaluateAll() {return false;}
//bool EvaluateNbr(NodeId_t node) {return false;}
 private:
};

}
}

#endif //ESTBASE
