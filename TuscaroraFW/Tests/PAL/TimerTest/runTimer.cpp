////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include "runTimer.h"
#include <Sys/Run.h>

TimerTest::TimerTest()
{
	//DBG_TEST = true; // set in Lib/Misc/Debug.cpp
	Debug_Printf (DBG_TEST, "TimerTest:: Intializing\n");
	t1=0; t2=0;
	delegate1 = new TimerDelegate(this, &TimerTest::Timer_1_Handler);
	delegate2 = new TimerDelegate(this, &TimerTest::Timer_2_Handler);
	timer1 = new Timer(500000, ONE_SHOT, *delegate1); //100hz
	timer2 = new Timer(100000, PERIODIC, *delegate2); //50hz
	sharedCounter=0;
	Debug_Printf(DBG_TEST, "TimerTest:: Timer1 initialized for 500ms, Timer 2 intialized for 100ms\n");   
}

void TimerTest::Execute()
{
	timer1->Start(); 
	timer2->Start();
}


void TimerTest::Timer_1_Handler(uint32_t event)
{
	//Debug::PrintTimeMilli();
	//uint32_t delay = ((int)(avgDelay * 0.9)) + rnd.GetNext() % ((int)(beacon * 0.1));
	sharedCounter++;
	Debug_Printf(DBG_TEST, "Inside Timer 1 Handler Entry: %d event, count: %d, shared count: %d\n",event,t1, sharedCounter);
	//Debug_Printf(DBG_TEST,"Inside Timer 1 Handler: %d event, count: %d\n",event,t1);
	t1++;
	//uint32_t avgDelay = 200000;
	/* if (t1 % 2 ){
		timer1->Change(500000, ONE_SHOT);
	}else {
		timer1->Change(1000000, ONE_SHOT);
	}*/
	timer1->Start();
	//if(event==1000){
	//	timer1->Suspend();
	//}
	sharedCounter--;
	Debug_Printf(DBG_TEST, "Inside Timer 1 Handler Exit: %d event, shared counter: %d\n",event, sharedCounter);
}

void TimerTest::Timer_2_Handler(uint32_t event)
{
	//Debug::PrintTimeMilli();
	sharedCounter++;
	Debug_Printf(DBG_TEST,"Inside Timer 2 Handler Entry: %d event, count: %d, shared counter: %d \n",event, t2, sharedCounter);
	t2++;
	/*timer2->Change(20000,ONE_SHOT);
	timer1->Change(0,ONE_SHOT);
	*/
	//if(event==1000){
	//	timer2->Suspend();
	//}
	sharedCounter--;
	Debug_Printf(DBG_TEST,"Inside Timer 2 Handler Exit: %d event, shared counter: %d \n",event, sharedCounter);
}

int main(int argc, char* argv[]){
	//Make sure RuntimeOpts construction is the very first line of main
	RuntimeOpts opts ( argc-1, argv+1 );

	InitPlatform(&opts);	//This does a platform specific initialization
	TimerTest timerTest;
	timerTest.Execute();

	//RunMasterLoopNeverExit(); //never exists on dce
	RunTimedMasterLoop();

	return 0;
}
