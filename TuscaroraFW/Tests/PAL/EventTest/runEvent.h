////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include <stdio.h>
#include <unistd.h>

#include "Lib/PAL/PAL_Lib.h"

using namespace PAL;

class EventTest{
  struct DummyStruct {
	  uint8_t data[10];
  } testParam;


public:
  Event *event;
  EventDelegate *del;

  uint16_t t1,t2;

  EventTest(){
    printf("EventTest:: Intializing\n");
    t1=0; t2=0;

    Debug::PrintTimeMicro();
    del = new EventDelegate(this, &EventTest::Event_Handler);
    uint64_t curTime = PAL::SysTime::GetEpochTimeInMicroSec();
    uint64_t eventDelay = 123159;
    event = new Event(eventDelay, *del, (void *) (&testParam));
    printf("EventTest:: Started a event to fire at %lu with parameter %lu\n",curTime+eventDelay, (uint64_t)&testParam);
  }
  
  void Execute(){    
  }

public:
  void Event_Handler(EventDelegateParam param){
    Debug::PrintTimeMicro();
    //uint32_t delay = ((int)(avgDelay * 0.9)) + rnd.GetNext() % ((int)(beacon * 0.1));
    printf("Inside Event 1 Handler: event ptr %lu, param ptr: %lu\n", (uint64_t) param.eventPtr, (uint64_t)param.eventParam);
    uint64_t eventDelay = 123159;
    uint64_t curTime = PAL::SysTime::GetEpochTimeInMicroSec();
    param.eventPtr->ReSchedule(eventDelay,(void *) ((&testParam)+1000));
    printf("EventTest:: Rescheduled event to fire at %lu with parameter %lu\n",curTime+eventDelay, (uint64_t)(&testParam)+1000);

  }

};

//int testEvent(int argc, char* argv[]){
