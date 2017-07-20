////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef APP2FWP_SHIM_H_
#define APP2FWP_SHIM_H_

#include <Interfaces/AWI/Fwp_I.h>

using namespace AWI;

namespace AWI{
Fwp_I& GetApp2FWPShim();
}



#endif //APP2FWP_SHIM_H_
