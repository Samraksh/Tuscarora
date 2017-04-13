////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef LIB_MATH_CMRG_STATE_H_
#define LIB_MATH_CMRG_STATE_H_

#include <stdint.h>

namespace PAL {

struct RNStreamID{
	uint32_t run;
	uint32_t stream;
public:
	RNStreamID() {
		run=0; stream=0;
	}
	RNStreamID(const uint32_t _run, const uint32_t _stream) {
		run=_run; stream=_stream;
	}
};

}

#endif /* LIB_MATH_RNG_STREAM_PUBLIC_H_ */
