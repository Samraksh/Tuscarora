////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef FI_BASIC_TEST_H_
#define FI_BASIC_TEST_H_


//#include <stdlib.h>
#include <iostream>

#include <Interfaces/Core/MessageT.h>
#include "Lib/PAL/PAL_Lib.h"
#include <Interfaces/PWI/Framework_I.h>
#include "Framework/PWI/StandardFI.h"
#include <Sys/Run.h>

#define DEBUG_CORE 1

using namespace PWI;

class Core_Test{
public:
  
  Core_Test();
  void Execute(RuntimeOpts *opts);
};

#endif /* FI_BASIC_TEST_H_ */
