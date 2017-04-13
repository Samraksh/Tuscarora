////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef DUMMY_NEIGHBOR_TEST_H_
#define DUMMY_NEIGHBOR_TEST_H_

#include <iostream>

//#include <Interfaces/Core/MessageT.h>
#include "Lib/PAL/PAL_Lib.h"
//#include <Interfaces/PWI/Framework_I.h>
//#include "Src/Framework/PWI/StandardFI.h"
#include <Interfaces/Pattern/PatternBase.h>
#include "Lib/Pattern/NeighborTable/PatternNeighborIterator.h"
#include "Framework/PWI/StandardFI.h"
#include "DummyEstimator2.h"
#include <Sys/Run.h>

using namespace Patterns;
using namespace Core::Naming;
using namespace PAL;
using namespace PWI::Neighborhood;

using namespace Core::Estimation;




class DummyNeighborTest{
  
  PWI::StandardFI *fi;
  uint16_t seqNumber;

  Timer *msgTimer;

  TimerDelegate *timerDelegate;

  
  DummyEstimator2 *dEstimator;
  //PatternNeighborIterator * nbrIterator;
  
  
public:
  
  DummyNeighborTest();
  void Execute(RuntimeOpts *opts);
  bool Start();
  bool Stop();

  
  void LocalMsgTimer_Handler(uint32_t event);
};

#endif /* DUMMY_NEIGHBOR_TEST_H_ */
