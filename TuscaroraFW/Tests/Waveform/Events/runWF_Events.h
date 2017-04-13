////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef WF_EVENTS_TEST_H_
#define WF_EVENTS_TEST_H_

#include "Framework/PWI/StandardFI.h"
#include <Sys/Run.h>
#include "Interfaces/Waveform/WF_I.h"

#define DEBUG_CORE 1

using namespace Waveform;

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

class WF_Event_Test{
	WaveformId_t myWfId;
  uint16_t seqNumber;
  FMessage_t sendMsg;

  Timer *sigTimer;
  TimerDelegate *timerDelegate;
  
  WF_ControlResponseEvent_t *controlResponseEvent;
  WF_ScheduleUpdateEvent_t *scheduleUpdateEvent;
  Waveform_I<uint64_t>::WF_LinkEstimateEvent_t *linkEstimateEvent;
  
  //Delegate<void, FMessageAckParam_t&> *ackDelegate;
  //Delegate<void, Message_n64_t&> *recvDelegate;

  PWI::StandardFI *fi;
  
public:
  
  WF_Event_Test();
  void Execute(RuntimeOpts *opts);
  void StartEventing();

  void Timer_Handler(uint32_t event);
  void SendScheduleUpdates();
  void SendControlResponse();
  void SendLinkEstimates();
};

#endif /* WF_EVENTS_TEST_H_ */
