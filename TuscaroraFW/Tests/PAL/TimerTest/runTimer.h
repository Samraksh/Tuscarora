////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include <stdio.h>
#include <unistd.h>

//Samraksh Fixed Stack
#include "Lib/PAL/PAL_Lib.h"
#include "Lib/Misc/Debug.h"

using namespace Lib;

// extern bool DBG_TEST; // set in Lib/Misc/Debug.cpp

class TimerTest{
public:

  TimerDelegate *delegate1;
  TimerDelegate *delegate2;
  Timer *timer1;
  Timer *timer2;
  uint16_t sharedCounter;
  //UniformRandomValue rnd;

  uint16_t t1,t2;
public:
  TimerTest();
	void Execute();
	void Timer_1_Handler(uint32_t event);
	void Timer_2_Handler(uint32_t event);
};

//int testTimer(int argc, char* argv[]){
