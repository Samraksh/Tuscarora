////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef CONFLICTAWARE_H_
#define CONFLICTAWARE_H_

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
#include "Scheduled.h"

using namespace PAL;
using namespace PWI;

namespace Core {
namespace Estimation {

	class ConflictAware: public Scheduled{
	private:
		/*
		 * NeighborDelegate * leDel;
		 * Delegate<void, EventInfoU64& > *sendHb;
		 * Delegate<void, EventInfoU64& > *scheduleNextClean;
		 * TimerDelegate *state_;
		 * RandomSchedule<UniformRandomInt, UniformRNGState,uint64_t,  uint64_t> *beaconSchedule;
		 * RandomSchedule<UniformRandomInt, UniformRNGState,uint64_t,  uint64_t> *shouldReceive;
		 * NodeSchedule* nodeSchedule[Core::Discovery::MAX_NBR];
		 * Framework_I *fi;
		 * WaveformId_t wid;
		 * uint32_t id;
		 * uint32_t leSeqno;
		 * WF_LinkChangeDelegate_n64_t *fworkNbrDel;
		 * EstimationLogging logger;
		 * Waveform::WF_Message_n64_t LeMsg;
		 * int period;
		 * int idx;
		 * struct timeval cur;
		 * uint numNodes;
		 * Timer *clean[10];
		 * void ScheduleNextClean(EventInfoU64& event);
		 * void SendHB(EventInfoU64& event);
		 * NeighborTable *coreNbrTable;
		 * LinkEstimationTable estimationTable;
		 * void NodeExpired(WF_LinkEstimationParam_n64_t _param);
		 * std::map<NodeId_t, std::vector<timeval> > events;
		 * double GetQuality(NodeId_t link);
		 * void LogQualityEvent(NodeId_t link, timeval time);*/
		
		public:
			using Scheduled::Scheduled;
			//ConflictAware(EstimatorCallback_I<uint64_t> &callback);
			//ConflictAware(EstimatorCallback_I<uint64_t> &callback, WaveformId_t _wid);
			//ConflictAware(EstimatorCallback_I<uint64_t> &callbackt, WaveformId_t _wid, NeighborTable *_coreNbrTable);///< Creates the link estimator, assigning the parent pattern interface, and specifying the neighbor table to use
			
			///< Sets the beaconing period for the link estimation messages. This also starts the link estimation module.
			//void SetParam(uint32_t beaconPeriod, uint32_t inactivePeriod);
			///< This checks the neighbor table for invalid links, changing them from a Neighbor to a PotentialNeighbor
			///if the link's expiry time has been reached without overhearing a beacon.
			void CleanUp(uint32_t event);
			//void PotentialNeighborUpdate (Discovery::PotentialNeighbor &pnbr, Discovery::PNbrUpdateTypeE type);
			
			//Inherited from interface
			/*
			 * LinkMetrics* OnPacketReceive(Waveform::WF_MessageBase *rcvMsg); 
			 * bool RegisterDelegate (WF_LinkChangeDelegate_n64_t & del);///< Registers a delegate to inform patterns of changed links. This goes throught he pattern interface, as the patterns register with the pattern interface directly, not with the link estimation module.
			 * 
			 * bool EvaluateAll() {return false;}
			 * bool EvaluateNbr(NodeId_t node) {return false;}
			 */
	};

    
} //End of namespace
}
#endif
