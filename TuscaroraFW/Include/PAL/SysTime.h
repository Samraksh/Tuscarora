////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef SYSTIME_H
#define SYSTIME_H

#include <Base/BasicTypes.h>


namespace PAL{
  struct SysTime{
    static uint64_t GetEpochTimeInMicroSec();
	static uint64_t GetEpochTimeInMilliSec();
	static uint64_t GetEpochTimeInNanoSec();
	static double GetEpochTimeInSec();
	static uint32_t GetEpochTimeInMilliSec_u32();
  };
    

}

#endif //SYSTIME_H
