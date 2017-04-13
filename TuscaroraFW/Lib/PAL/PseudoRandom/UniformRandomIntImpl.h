////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef LIB_PAL_PSEUDORANDOM_UNIFORMRANDOMINTIMPL_H_
#define LIB_PAL_PSEUDORANDOM_UNIFORMRANDOMINTIMPL_H_

#include <PAL/PseudoRandom/UniformRandomInt.h>
#include <PAL/PseudoRandom/rng-state.h>
#include <Lib/PAL/PseudoRandom/RNStreamID.h>

#include "RandomCommon.h"


namespace PAL {

class UniformRandomIntImpl {
public:
	UniformRandomInt::State* m_internal_state_ptr;
    Tuscarora_RNG2* m_engine_ptr;
    std::uniform_int_distribution<uint64_t>* m_distribution_ptr;

	UniformRandomIntImpl();
	virtual ~UniformRandomIntImpl();
};

} /* namespace PAL */

#endif /* LIB_PAL_PSEUDORANDOM_UNIFORMRANDOMVALUEIMPL_H_ */
