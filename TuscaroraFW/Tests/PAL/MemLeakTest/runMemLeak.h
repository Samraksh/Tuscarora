////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <sys/time.h>


//Samraksh Fixed Stack
#include "Base/BasicTypes.h"
#include "Lib/Misc/Debug.h"
#include "Tests/test.h"


class T1 {
public:
 int intArray[1024];
 char charArray[512]; 
};

struct itimerval currTimer,oldTimer;
int timer;




bool SetTimer(){
   //printf("Setting timer %d for %d micosec\n", timer->myTimerNumber, timer->countdown);
  currTimer.it_interval.tv_sec = timer / 1000000;
  currTimer.it_interval.tv_usec = (timer % 1000000);
  currTimer.it_value.tv_sec = timer / 1000000;;
  currTimer.it_value.tv_usec = (timer % 1000000);
   
  oldTimer.it_interval.tv_sec = 0;
  oldTimer.it_interval.tv_usec = 0;
  oldTimer.it_value.tv_sec = 0;
  oldTimer.it_value.tv_usec = 0;
  
  
  int timer_id = setitimer (ITIMER_REAL, &currTimer, &oldTimer);
  if (timer_id  < 0){
      printf("Timer:: timer init failed\n");
      return false;
  }
  else {
      //printf("timer init succeeded: %d\n", timer_id);  
      return true;
  }
 }

void timerHandler(int sig){
    //Debug::PrintTimeMilli();
    //curTimer=nextTimer;
    printf("Received signal from Kernel: %d ",sig);
    SetTimer();
    printf("done handling\n");
}
 
class MemLeakTest{
public:
  
  MemLeakTest(){
    printf("MemLeakTest:: Intializing\n");
    printf("MemLeakTest:: Done Intializing\n");
    timer=1000000;
    
  }
  
  void Execute(){
    Test_Leak();
    //Test_NoLeak();
  }
  
  void Test_Leak(){  
    signal (SIGALRM, timerHandler);
    SetTimer();
  }
  
  void Test_NoLeak() {
    T1 *t1_ptr;
    int iter=0;
                
    while(1){      
      t1_ptr = new T1();
      t1_ptr->charArray[0]='h';
      t1_ptr->intArray[512]=iter;
      Debug::PrintTimeMilli();
      printf("MemLeakTest:: Iteration %d\n", t1_ptr->intArray[512]);
      sleep(1);
      iter++;
      delete t1_ptr;
    }
  }

};
