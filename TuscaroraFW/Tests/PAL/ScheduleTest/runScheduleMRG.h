////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 


#ifndef SCHEDULEMRGTEST_H_
#define SCHEDULEMRGTEST_H_


#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>

#include <Sys/Run.h>
#include "Lib/PAL/Schedule/RandomSchedule.h"
#include "Lib/Math/Rand.h"

// extern bool DBG_TEST; // set in Lib/Misc/Debug.cpp
using namespace PAL;


class ScheduleMRGTest{
private:
  Delegate<void, EventInfoU64&> *event_del;
  UniformRandomInt *rand;
  RandomSchedule<UniformRandomInt,UniformRNGState,uint64_t,uint64_t> *randSch;
  uint64_t stream;
  uint32_t eventNumber;
  uint64_t last_event_time;
  uint64_t curr_interval;
  uint64_t sum_intervals;
  uint64_t mean_interval;
  uint64_t startTime;
  UniformRNGState rngState;
  
public:
  ScheduleMRGTest(){
	  //printf("ScheduleTest:: Intializing\n");
	  MY_NODE_ID = atoi(getenv("NODEID"));	
	  eventNumber=0; 
	  printf("ScheduleTest:: Done Intializing\n");
	  
  }
  
  void Execute(){
	  RNStreamID stream(23,37);
	  UniformRNGState rngState(stream);
	  UniformIntDistParameter dist; dist.min = 80000; dist.max = 120000;
	  rngState.SetDistributionParameters(dist);

	  rand = new UniformRandomInt(rngState);
	  event_del = new Delegate<void, EventInfoU64&>(this, &ScheduleMRGTest::Event_Handler);
	  
	  randSch = new RandomSchedule<UniformRandomInt,UniformRNGState,uint64_t,uint64_t>(*rand,0);
	  randSch -> SetNode(MY_NODE_ID);
	  randSch -> RegisterDelegate(*event_del);
	  //gettimeofday(&curtime,NULL);
	  //startTime = (uint64_t)(curtime.tv_sec * 1000000 + curtime.tv_usec);
	  startTime = PAL::SysTime::GetEpochTimeInMicroSec();

	  eventNumber=0; 

	 Debug_Printf(DBG_TEST, "Starting scheduler with starttime %lu \n", startTime);
	  randSch->Start( startTime);
	  //if(MY_NODE_ID==0){
		  //randSch->Set(rngState, startTime);
	  //}
  }


  void Event_Handler(EventInfoU64& eventInfo){

    eventNumber++;
    if(eventNumber % 20 == 0 && MY_NODE_ID == 0) {
      //printf("Recreating the scheduler\n");
	  /*rngState.cmrgState.stream = 2;
	  rngState.cmrgState.run = RngSeedManager::GetRun();
	  //stream=2*MY_NODE_ID; //this will give diff schedules at diff nodes
	  //stream=2; //this will give one fixed schedule for all nodes
	  rngState.mean=100000;
	  rngState.range=25000;
	  randSch->Set(rngState, startTime);
	  */
    }
    Debug::PrintTimeMicro();
    printf("Event number %u ; Inside Random Scheduler Handler; Time is %ld \n",eventNumber, eventInfo.time);
    if (eventNumber==1) {
	last_event_time = eventInfo.time;
	sum_intervals = 0;
    }
    else {
	curr_interval = eventInfo.time-last_event_time;
	last_event_time = eventInfo.time;
	sum_intervals += curr_interval;
	mean_interval = sum_intervals/(eventNumber-1);
	printf("Event number %u ; curr_interval %ld; mean_interval %ld \n",eventNumber, curr_interval, mean_interval);
    }
	
    //eventNumber++;
    //Debug::PrintTimeMicro();
    //printf("Event number after increment: %u \n",eventNumber);
    
  }
};

//int testSchedule(int argc, char* argv[]);

#endif	
