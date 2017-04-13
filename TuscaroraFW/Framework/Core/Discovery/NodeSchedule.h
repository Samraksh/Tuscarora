////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 


#ifndef NODESCHEDULE_H_
#define NODESCHEDULE_H_

#include "Lib/PAL/PAL_Lib.h"
//#include "Lib/Math/Rand.h"
#include <PAL/PseudoRandom/UniformRandomInt.h>


namespace Core {
namespace Discovery {
	const uint16_t MAX_NBR=1000;

	class NodeSchedule {
	public:
	  uint64_t nodeid;///< The NodeID assigned to the schedules

	  RandomSchedule<UniformRandomInt, UniformRNGState, uint64_t, uint64_t> *rcvSchd;///< Scheduler that defines when the referenced node will be awake and ready to receive messages. This is only used when utilizing a duty cycled radio.
	  RandomSchedule<UniformRandomInt, UniformRNGState, uint64_t, uint64_t> *linkEstSchd;///< Scheduler that defines when the referenced node will broadcast or should have broadcasted a link estimation beacon.
	};

} //end namespace Discovery
} //end namespace Core
#endif /* NODESCHEDULE_H_ */
