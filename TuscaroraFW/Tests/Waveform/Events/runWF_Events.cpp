////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#include "runWF_Events.h"
#include <Sys/Run.h>
#include "Tests/FW_Init.h"
#include <string.h>
//#include "Platform/dce/PAL/WF_AsyncEvent_Special.h"

using namespace PAL;

// extern bool DBG_SIGNAL; // set in Lib/Misc/Debug.cpp
// extern bool DBG_PATTERN; // set in Lib/Misc/Debug.cpp
// extern bool DBG_WAVEFORM; // set in Lib/Misc/Debug.cpp
// extern bool DBG_CORE; // set in Lib/Misc/Debug.cpp
// extern bool DBG_TEST; // set in Lib/Misc/Debug.cpp

WF_Event_Test::WF_Event_Test(){
	myWfId=10;
  printf(" Framework Test Constructor , with fake waveform %d....\n", myWfId);
 
//   DBG_WAVEFORM=true; // set in Lib/Misc/Debug.cpp
//   DBG_SIGNAL = true; // set in Lib/Misc/Debug.cpp

  
  fi = static_cast<StandardFI *> (&GetFrameworkInterface());
  
  printf("Done.");fflush(stdout);
}


void WF_Event_Test::Execute(RuntimeOpts *opts){
  controlResponseEvent = new WF_ControlResponseEvent_t(myWfId);
  scheduleUpdateEvent = new WF_ScheduleUpdateEvent_t(myWfId);
  linkEstimateEvent = new Waveform_I<uint64_t>::WF_LinkEstimateEvent_t(myWfId);
  
  timerDelegate = new TimerDelegate(this, &WF_Event_Test::Timer_Handler);
  sigTimer = new Timer(1000000, PERIODIC, *timerDelegate);
  sigTimer->Start();

  printf("WF_Event TEST:: Execute \n");
}

void WF_Event_Test::SendScheduleUpdates(){
  Debug_Printf(DBG_TEST, "Sending Schedule updates signal: \n");
  WF_ScheduleUpdateParam param;
  param.type = LinkEstSchd;
  param.nodeid = 10000;
  scheduleUpdateEvent->Invoke(param);  
}


void WF_Event_Test::SendControlResponse(){
  Debug_Printf(DBG_TEST, "Sending Control Response signal: \n");
  WF_ControlResponseParam param;
  param.id = seqNumber;
  param.type = AttributeResponse;
  controlResponseEvent->Invoke(param);    
}

void WF_Event_Test::SendLinkEstimates(){
  Debug_Printf(DBG_TEST, "Sending Link Estimates signal: \n");
  WF_LinkEstimationParam<uint64_t> param;
  param.changeType = NBR_DEAD;
  param.nodeId = 1001;
  linkEstimateEvent->Invoke(param);    
}

void WF_Event_Test::Timer_Handler(uint32_t event)
{
  seqNumber++;
  Debug_Printf(DBG_TEST, "WF_Event Test:: Timer fired:: id : %d \n", seqNumber);
  
  uint16_t x = seqNumber % 3;
  switch(x){
    case 0:
      SendControlResponse();break;
    case 1:
      SendLinkEstimates(); break;
    case 2:
      SendScheduleUpdates(); break;
  }
}


int main(int argc, char* argv[]) {
	//Make sure RuntimeOpts construction is the very first line of main
	RuntimeOpts opts ( argc-1, argv+1 );
	InitPlatform(&opts);	//This does a platform specific initialization

	FW_Init fwInit;
	//fwInit.InitPI();
	fwInit.Execute(&opts);
	
	WF_Event_Test ctest;
	ctest.Execute ( &opts );

	//RunMasterLoopNeverExit(); //never exists on dce
	RunTimedMasterLoop();
	return 0;
}
