////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#include <Include/Base/TimeI.h>
//#include <stdio.h>
namespace PAL{

U64NanoTime::U64NanoTime(){

}

U64NanoTime::~U64NanoTime(){
	//printf("U64NanoTimeT:: Destroying a time object\n");
	//fflush(stdout);
}


U64NanoTime::U64NanoTime(uint64_t time) {state = time;}

void U64NanoTime::SetTime(uint64_t time) {state = time;}

uint64_t* U64NanoTime::GetTimeRange(){
  return 0;
}
uint32_t U64NanoTime::GetSeconds(){
  return (state/1000000000);
}
uint64_t U64NanoTime::GetMilliseconds(){
  return (state/1000000);
}
uint64_t U64NanoTime::GetMicroseconds(){
  return (state/1000);
}
uint64_t U64NanoTime::GetTime(){
	return state;
}
Fixed_15_16_t U64NanoTime::GetConfidence(){
  Fixed_15_16_t confidence;
  confidence = 1.0;
  return confidence;
}



/*void U64NanoTimeT::SetConfidence(){
}*/

}	//end namespace

