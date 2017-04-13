////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef MOBILITY_CONTROL_TEST_H_
#define MOBILITY_CONTROL_TEST_H_

#include "Lib/PAL/PAL_Lib.h"
#include "Platform/dce/ExternalServices/MobilityControl/MobilityControl.h"
//#include "Rand15.h"

using namespace ExternalServices;

extern uint16_t MY_NODE_ID;


class MobilityControlTest {
    
   
public:
  MobilityControlTest();
  void Execute(int argc, char *argv[]);  
};


#endif /* HEAPTEST_H_ */
