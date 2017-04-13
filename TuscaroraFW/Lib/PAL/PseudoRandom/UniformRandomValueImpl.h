////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef LIB_PAL_PSEUDORANDOM_UNIFORMRANDOMVALUEIMPL_H_
#define LIB_PAL_PSEUDORANDOM_UNIFORMRANDOMVALUEIMPL_H_

#include <PAL/PseudoRandom/UniformRandomValue.h>
#include <PAL/PseudoRandom/rng-state.h>
#include "RandomCommon.h"
#include <Lib/PAL/PseudoRandom/RNStreamID.h>

namespace PAL {

class UniformRandomValueImpl {
public:
	UniformRandomValue::State* m_internal_state_ptr;
	RNStreamID cmrgState;
    Tuscarora_RNG2* m_engine_ptr;
    std::uniform_real_distribution<double>* m_distribution_ptr;

	UniformRandomValueImpl();
	virtual ~UniformRandomValueImpl();
};

} /* namespace PAL */

#endif /* LIB_PAL_PSEUDORANDOM_UNIFORMRANDOMVALUEIMPL_H_ */
