////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef MULTIWAVEFORM_TEST_H_
#define MULTIWAVEFORM_TEST_H_

// extern bool DBG_TEST; // set in Lib/Misc/Debug.cpp
//#include <stdlib.h>
#include <iostream>

#include <Interfaces/Core/MessageT.h>
#include "Lib/PAL/PAL_Lib.h"
#include <Interfaces/PWI/Framework_I.h>
#include "Framework/PWI/StandardFI.h"
#include <Sys/Run.h>

#if defined(PLATFORM_LINUX)
#elif defined(PLATFORM_DCE_DC)
#include "Platform/dce/ExternalServices/LocationService/LocationService.h"
#elif defined(PLATFORM_DCE)
#include "Platform/dce/ExternalServices/LocationService/LocationService.h"
#elif defined(PLATFORM_ANDROID)
#endif

#define DEBUG_CORE 1

using namespace PWI;

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
  }
  bool CompareMsg(const char *r1){
    bool rtn=false;
    for(int i = 0 ; i < 256; i++ ) {
      if(msg[i] != r1[i]) return rtn;
    }
    return true;
  }
};


class Core_Test{
  uint16_t seqNumber;
  FMessage_t sendMsg;

  Timer *msgTimer;
  Timer *apiTimer;
  PatternId_t pid;
  MessageId_t msgId;
  uint16_t nonce;
  //Delegates
  TimerDelegate *timerDelegate;
  TimerDelegate *apitimerDelegate;
  RecvMessageDelegate_t *recvDelegate; 
  DataflowDelegate_t *dataNotifierDelegate;
  ControlResponseDelegate_t *controolResponceDelegate;
  NeighborDelegate* nbrDelegate;
  //Delegate<void, FMessageAckParam_t&> *ackDelegate;
  //Delegate<void, Message_n64_t&> *recvDelegate;

  PWI::StandardFI *fi;
  
public:
  
  Core_Test();
  void Execute(RuntimeOpts *opts);
  void StartMessaging();

  void LocalMsgTimer_Handler(uint32_t event);
  void FrameworkApiTimer_Handler(uint32_t event);
  void DataStatus_Handler(DataStatusParam& ack);
  void Recv_Handler(FMessage_t& msg);
  void Control_Handler(ControlResponseParam param);
  void NeighborUpdateEvent(NeighborUpdateParam param);

};

#endif /* MULTIWAVEFORM_TEST_H_ */
