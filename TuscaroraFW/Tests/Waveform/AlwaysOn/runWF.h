////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef WF_TEST_H_
#define WF_TEST_H_

#include "Lib/Math/Rand.h"

#include "Lib/Waveform/AlwaysOn/WF_AlwaysOn_Ack.h"
#include "Lib/Waveform/AlwaysOn/WF_AlwaysOn_NoAck.h"

#include <Sys/RuntimeOpts.h>

using namespace PAL;
using namespace Waveform;



#define PayloadSize 948 //+16
struct MyMsg {
  uint16_t seqNo;
  uint64_t senderID;
  uint8_t data[PayloadSize];
};

class WF_Test{

  Waveform::WF_Attributes wfAttrib;
  WF_Message_n64_t myBuffer;
  uint16_t maxPayloadSize;
	uint32_t messagePeriod;
  //Message_n64_t& sendMsg = myBuffer;
  WF_Message_n64_t *sendMsg;
  TimerDelegate *timerDelegate;
  Timer *timer;
  uint16_t seqNumber;
  UniformRandomValue *rndDouble;

private:
	void ResetTimer(uint32_t _period);
	void Start();
	
public:
	//Handlers *handlers;
	Delegate<void, WF_DataStatusParam_n64_t> *ackDelegate;
	Delegate<void, WF_Message_n64_t&> *recvDelegate;

	Waveform_I<uint64_t> *wf;

	WF_Test();
	void Execute(RuntimeOpts *opts);
	void Timer_Handler(uint32_t event);
	void SendAck_Handler(WF_DataStatusParam_n64_t& ack);
	void Recv_Handler(WF_Message_n64_t& msg);
};

//int testWF(int argc, char* argv[]);

#endif /* WF_TEST_H_ */
