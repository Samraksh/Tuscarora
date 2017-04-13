////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef NEIGHBOR_TEST_H_
#define NEIGHBOR_TEST_H_

#include <iostream>

//#include <Interfaces/Core/MessageT.h>
#include "Lib/PAL/PAL_Lib.h"
//#include <Interfaces/PWI/Framework_I.h>
//#include "Src/Framework/PWI/StandardFI.h"
#include <Interfaces/Pattern/PatternBase.h>
#include "Lib/Pattern/NeighborTable/PatternNeighborIterator.h"
#include "Framework/PWI/StandardFI.h"
#include "DummyEstimator.h"
#include <Sys/Run.h>

using namespace Patterns;
using namespace Core::Naming;
using namespace PAL;
using namespace PWI::Neighborhood;

using namespace Core::Estimation;

class RandMsg{
public:
  uint64_t src;
  uint64_t dst;
  char msg[256];
  uint16_t seqNo;
  bool acked;

public:
  RandMsg(){
   const char *hex_digits = "0123456789ABCDEF";
   for(int i = 0 ; i < 256; i++ ) {
      msg[i] = hex_digits[ ( rand() % 16 ) ];
   }
   printf( "New Random String %s\n", msg );
  }
  bool CompareMsg(const char *r1){
    bool rtn=false;
    for(int i = 0 ; i < 256; i++ ) {
      if(msg[i] != r1[i]) return rtn;
    }
    return true;
  }
};



class NeighborTest : public PatternBase{
  
  PWI::StandardFI *fi;
  uint16_t seqNumber;
  FMessage_t *sendMsg;

  Timer *msgTimer;

  TimerDelegate *timerDelegate;
  
  PatternNeighborTableI *myNeighborTable;
  
  DummyEstimator *dEstimator;
  //PatternNeighborIterator * nbrIterator;
  
  
public:
  
  NeighborTest();
  void Execute(RuntimeOpts *opts);
  bool Start();
  bool Stop();

  void NeighborUpdateEvent (NeighborUpdateParam nbrUpdate);
  void ControlResponseEvent (ControlResponseParam response);
  void DataStatusEvent (DataStatusParam notification);
  void ReceiveMessageEvent (FMessage_t& msg);
  
  void LocalMsgTimer_Handler(uint32_t event);
};

#endif /* NEIGHBOR_TEST_H_ */
