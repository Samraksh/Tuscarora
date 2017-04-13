////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 


#ifndef SCHEDULED_H_
#define SCHEDULED_H_

/*#include "Framework/Core/Neighborhood/NeighborTable.h"
#include "Framework/Core/Discovery/NetworkDiscovery.h"
#include <Interfaces/PWI/Framework_I.h>
#include "Lib/PAL/PAL_Lib.h"
#include "EstimationLogging.h"
#include "LinkEstimationTable.h"
#include <map>
#include <vector>
#include <cmath>

*/
#include "EstBase.h"
//#include <assert.h>

using namespace PAL;
using namespace PWI;

namespace Core {
namespace Estimation {

	class Scheduled : public EstBase{
	protected:
		RandomSchedule<UniformRandomInt, UniformRNGState,uint64_t,  uint64_t> *shouldReceive;
		Delegate<void, EventInfoU64& > *scheduleNextClean;
		uint numNodes;
		NodeSchedule* nodeSchedule[Core::Discovery::MAX_NBR];
		Timer *clean[10];
		int idx;
		struct timeval cur;
		
		/* NeighborDelegate * leDel;
		 * Delegate<void, EventInfoU64& > *sendHb;
		 * 
		 * TimerDelegate *state_;
		 * RandomSchedule<UniformRandomInt, UniformRNGState,uint64_t,  uint64_t> *beaconSchedule;
		 * 
		 * 
		 * //Framework_I *fi;
		 * WaveformId_t wid;
		 * uint32_t id;
		 * uint32_t leSeqno;
		 * NeighborDelegate *fworkNbrDel;
		 * EstimationLogging logger;
		 * Waveform::WF_Message_n64_t LeMsg;
		 * int period;
		 * 
		 *
		 
		 * 
		 * 
		 * void SendHB(EventInfoU64& event);
		 * NeighborTable *coreNbrTable;
		 * LinkEstimationTable estimationTable;
		 * void NodeExpired(WF_LinkEstimationParam_n64_t _param);
		 * std::map<NodeId_t, std::vector<timeval> > events;
		 * double GetQuality(NodeId_t link);
		 * void LogQualityEvent(NodeId_t link, timeval time);
		 */
		
		void ScheduleNextClean(EventInfoU64& event);
		void CreateUpdateNbrSchedule(uint64_t wfAddress, NodeId_t nodeid, WaveformId_t wid);
	public:
			
		//Inherited from interface
		void PotentialNeighborUpdate (Discovery::PotentialNeighbor &pnbr, Discovery::PNbrUpdateTypeE type);
		LinkMetrics* OnPacketReceive(Waveform::WF_MessageBase *rcvMsg); 
		
		~Scheduled();
		Scheduled(EstimatorCallback_I<uint64_t> &callback);
		Scheduled(EstimatorCallback_I<uint64_t> &callback, WaveformId_t _wid);
		Scheduled(EstimatorCallback_I<uint64_t> &callback, WaveformId_t _wid, NeighborTable *_coreNbrTable);///< Creates the link estimator, assigning the parent pattern interface, and specifying the neighbor table to use
		
		//bool RegisterDelegate (Core::WF_LinkChangeDelegate_n64_t& del);///< Registers a delegate to inform patterns of changed links. This goes throught he pattern interface, as the patterns register with the pattern interface directly, not with the link estimation module.
		void SetParam(uint32_t beaconPeriod, uint32_t inactivePeriod);
		
		
		
		///< Sets the beaconing period for the link estimation messages. This also starts the link estimation module.
		//void CleanUp(uint32_t event);///< This checks the neighbor table for invalid links, changing them from a Neighbor to a PotentialNeighbor if the link's expiry time has been reached without overhearing a beacon.
		
	};

}
}
#endif
