////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 


#include <sstream>
using std::ostringstream;

#include "TimerImpl.h"
#include "Lib/Misc/Debug.h"
#include <string.h>

namespace PAL {

#define MAX_TIMERS 256
#if defined(PLATFORM_LINUX)
#define TIMER_OVERHEAD 10 //overhead of timer on a actual platform in microseconds
#else
#define TIMER_OVERHEAD 0 //DCE has no timer overhead
#endif

/*#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); \
	} while (0)
*/


////////////////////Global Book Keeping for all TimerImpls///////////////////
//long timerIdValueArray[MAX_TIMERS];
TimerImpl* timerPtrArray[MAX_TIMERS] = { NULL };
TimerImpl *nextTimer, *curTimer;

//struct timeval lastTimeStamp;
//int signalArray[MAX_TIMERS]={0};
//timer_t timerIdArray[MAX_TIMERS];

static bool signalRegistered=false;
static bool timerAlive[MAX_TIMERS] = { false };
struct itimerval curr_timer, old_timer;
static uint16_t timerIDGenerator=0;
bool timerPendingExecution = false;

static void timerHandler(int sig);

struct sigaction timer_action;

void Register(){
	//printf("\nTimerImpl::Register: Registering the signal handler\n\n");
	memset (&timer_action, 0, sizeof (timer_action));
	timer_action.sa_handler= timerHandler;
	//sigemptyset (&timer_action.sa_mask);
	//timer_action.sa_flags = 0;
	//timer_action.sa_flags = SA_RESTART;

	if(sigaction (SIGALRM, &timer_action, NULL) < 0){
		perror("Sigaction: SIGALRM\n");	
		//exit(1);
	}
	//signal(SIGALRM, timerHandler);
	signalRegistered = true;
}

void Deregister(){
 sigaction (SIGALRM, NULL, &timer_action);
 printf("No timer seems to be running.. Deregistering the alarm signal handler\n"); fflush(stdout);
 signalRegistered=false;
}



void TimerImpl::Init(uint32_t microsecond_period, TimerTypeE type, TimerDelegate& del)
{
	delegate = &del;
	myTimerNumber = GetFreeTimerID();
	fflush(stdout);
	if (myTimerNumber == -1) {
		perror( "TimerImpl::TimerImpl: Cons:Ran out of timers, check stack for number of timers used\n");
	} else {
		//printf("TimerImpl::TimerImpl: Cons: Initialized new %s\n", this->GetName()); fflush(stdout);
	}

	//This is the first time a timer is being created
	///Initialize call back
	if (myTimerNumber == 0 || !signalRegistered) {
		//printf("TimerImpl::TimerImpl: Cons: This is the first time any timer is created\n");fflush(stdout);
		Register();
		nextTimer = this;
		//initialize the rest as zero, might help speedup
		for (int i = 1; i < MAX_TIMERS; i++) {
			timerPtrArray[i] = NULL;
		}
	}
	eventNumber = 0;

	// Set timer values
	this->period = microsecond_period;
	this->countdown = microsecond_period;
	if (type == PERIODIC) {
		this->oneshot = false;
	} else {
		this->oneshot = true;
	//printf("%s is a oneshot timer\n", this->GetName()); fflush(stdout);
	}

	//Add timer info to global structure
	timerPtrArray[myTimerNumber] = this;
	timerAlive[myTimerNumber] = false;
	
	//printf("TimerImpl::TimerImpl: Created timer %d, ptr at (%p)\n", myTimerNumber, &myTimerNumber);
}

TimerImpl::TimerImpl(uint32_t microsecond_period, TimerTypeE type, TimerDelegate& del, string _name)
{
	//strcpy(name, _name);
	name.append(_name);
	Init(microsecond_period,type, del);
}

TimerImpl::TimerImpl(uint32_t microsecond_period, TimerTypeE type, TimerDelegate& del) 
{
	Init(microsecond_period,type, del);
	name.append("TimerImpl_");
	name.append(std::to_string(myTimerNumber));
}

bool CheckForAliveTimerImpls(){
  for (int i=0; i < MAX_TIMERS; i++){
    if(timerAlive[i]){
      return true;
    }
  }
  return false;
}

bool TimerImpl::Start() {
	//if(oneshot){} 
	
	if(!signalRegistered) {
		Register();
	}
	//printf("TimerImpl:: Starting timer %s with period %u...\n", this->str().c_str(), period ); fflush(stdout); 	

	//alive = true;
	timerAlive[myTimerNumber]=true;
	gettimeofday(&lastEvalTime, NULL);
	countdown = period;
	TimerImpl *next = nextTimer->EvaluateNext();
	if(next){
		SetNext(next);
	}
	//ExecuteExpiredTimerImpls(next);
	if(timerAlive[myTimerNumber]){
		//printf("TimerImpl:: Started timer %s with period %u\n", this->str().c_str(), period ); fflush(stdout); 
	}else {
		printf("TimerImpl:: Problem Starting timer %s with period %u\n", this->str().c_str(), period ); fflush(stdout); 	
	}
	
	return true;
}

bool TimerImpl::Suspend() {
  period = 4294967295;
  countdown = 4294967295;
  //alive = false;
  timerAlive[myTimerNumber]=false;
  return true;
}

bool TimerImpl::Change(uint32_t microsecond_period, uint8_t type) {
  period = microsecond_period;
  countdown = microsecond_period;
  timerAlive[myTimerNumber]=true;
  //alive = true;
  if (type == PERIODIC) {
	  oneshot = false;
  } else {
	  oneshot = true;
  }
  Start();
  return true;
}

void TimerImpl::Execute() {
  //printf("TimerImpl::Execute: Fired %s\n", this->GetName());
  if (oneshot) {
    //printf("timerHandler: Finished executing a oneshot %s\n", curTimer->c_str()); fflush(stdout);
    //alive = false;
    timerAlive[myTimerNumber]=false;
  } else {
	  //printf("timerHandler: %s is a periodic timer. \n", curTimer->c_str()); fflush(stdout);
  }
  if(delegate) {
	  delegate->operator()(eventNumber);
	  eventNumber++;
  }else {
	  printf("TimerImpl:: Something wrong: TimerImpl %s fired, but NO delegate is registered\n", this->GetName());
  }
}

int32_t TimerImpl::GetFreeTimerID() {
  if(timerIDGenerator < MAX_TIMERS) {
    int32_t ret = timerIDGenerator;
    timerIDGenerator++;
    return ret;
  }
  return -1;
}



void timerHandler(int sig) {
  //Debug::PrintTimeMilli();
  //curTimer=nextTimer;
  //printf("timerHandler: Received signal from Kernel: %d, currentTimer is %d \n",sig, curTimer->myTimerNumber);fflush(stdout);
  if (timerAlive[curTimer->myTimerNumber]) {
	  gettimeofday(&curTimer->lastEvalTime, NULL);
	  curTimer->countdown = curTimer->period;
	  curTimer->Execute();
  }

  //Check what is the next timer to be scheduled to fire.
  //But if meanwhile some timer has elapsed execute it.
  TimerImpl *next = nextTimer->EvaluateNext();
  //Important Comment: Exexecuting expired timers without going through ns3-dce scheduler screws up the non-preemption logic of timer callbacks
  //so simply set the next timer and get out, dont execute multiple callbacks
  //curTimer->ExecuteExpiredTimerImpls(next);
  if(next!=NULL) { nextTimer->SetNext(next);}
  else {
    /*if(!CheckForAliveTimerImpls()){
      Deregister();
    }*/
  }
  
}

void TimerImpl::ExecuteExpiredTimers(TimerImpl* next) {
  while (timerPendingExecution && next==NULL){
	  curTimer->Execute();
	  timerPendingExecution=false;
	  next=nextTimer->EvaluateNext();
  }
  if(next!=NULL){
	  nextTimer->SetNext(next);
  }
  //else {
    //printf("No timer needs to run at this time\n");
  //}
}

bool TimerImpl::SetNext(TimerImpl *timer) {

  //printf("TimerImpl::SetNext: Setting %s for %d microsec\n", timer->c_str(), timer->countdown);
  if(timer!=NULL){
    if(timer->countdown==0) {timer->countdown=1;}
    curr_timer.it_interval.tv_sec = timer->countdown / 1000000;
    curr_timer.it_interval.tv_usec = (timer->countdown % 1000000);
    curr_timer.it_value.tv_sec = timer->countdown / 1000000;
    ;
    curr_timer.it_value.tv_usec = (timer->countdown % 1000000);

    old_timer.it_interval.tv_sec = 0;
    old_timer.it_interval.tv_usec = 0;
    old_timer.it_value.tv_sec = 0;
    old_timer.it_value.tv_usec = 0;

    curTimer = timer;
		//printf("TimerImpl::SetNet:: Setting itimer to fire in %d: %d sec\n", timer->countdown/1000000, timer->countdown%1000000);
    //int timer_id = setitimer(ITIMER_REAL, &curr_timer, &old_timer);
		int timer_id = setitimer(ITIMER_REAL, &curr_timer, NULL);
    if (timer_id < 0) {
	    printf("TimerImpl::SetNext: timer init failed\n"); fflush(stdout);
	    return false;
    } else {
	   // printf("TimerImpl::SetNext: %s with timer id %d\n", curTimer->str().c_str(), myTimerNumber);fflush(stdout);
	    return true;
    }
  }
  else {return false;}
}

TimerImpl* TimerImpl::EvaluateNext() {
	TimerImpl* next = NULL;
	uint32_t usecsElapsed;
	uint32_t min = 4294967295;
	//printf("TimerImpl::EvaluateNext: Intializing %s\n", this->c_str());
	//adjust others values
	struct timeval curtime;
	gettimeofday(&curtime, NULL);
	
	for (int i = 0; i < MAX_TIMERS; i++) {
		
		if (timerPtrArray[i] != NULL) {
			//the guy who just ran
			/*
			 i f(timer*PtrArray[i]==curTimer){
			 //printf("I just ran this guy\n");
			 timerPtrArray[i]->countdown = timerPtrArray[i]->period;
			 timerPtrArray[i]->lastEvalTime=curtime;
		}
		//others
		else{
			*/
			if (timerAlive[timerPtrArray[i]->myTimerNumber]) {
				usecsElapsed = (curtime.tv_sec - timerPtrArray[i]->lastEvalTime.tv_sec) * 1000000 + (curtime.tv_usec - timerPtrArray[i]->lastEvalTime.tv_usec);
				if (usecsElapsed + TIMER_OVERHEAD > timerPtrArray[i]->countdown) {
					curTimer = timerPtrArray[i];
					//This timer has already elapsed execute this.
					//printf("TimerImpl::EvaluateNext: Oops.. %s has expired\n", curTimer->c_str());
					fflush(stdout);
					timerPtrArray[i]->countdown = timerPtrArray[i]->period;
					timerPtrArray[i]->lastEvalTime = curtime;
					timerPendingExecution = true;
					//nextTimer= timerPtrArray[i];
					next = NULL;
					return next;
					//timerPtrArray[i]->Execute();
				} else {
					timerPtrArray[i]->countdown = timerPtrArray[i]->countdown - usecsElapsed;
					timerPtrArray[i]->lastEvalTime = curtime;
					//printf("TimerImpl::EvaluateNext: Updated countdown of %s at place %d to %d\n", timerPtrArray[i]->GetName(), i, timerPtrArray[i]->countdown);
					fflush(stdout);
				}
			}
		}
		//else {
		//    printf("TimerImpl %d not alive\n",i);
		//}
		//}
	}
	//curTimer=NULL;
	//printf("Let me find min\n");
	//find who needs to run next
	for (int i = 0; i < MAX_TIMERS; i++) 
	{
		if (timerPtrArray[i] != NULL && timerAlive[timerPtrArray[i]->myTimerNumber]
			&& timerPtrArray[i]->countdown < min) 
		{
			min = timerPtrArray[i]->countdown;
			next = timerPtrArray[i];
			//printf("Found min: %d, count: %d\n", i, min);
		}
	}
	
	/*if(nextTimer!=next){
		printf("Ch*anging timer from %d to %d\n",nextTimer,next);
	}*/
	nextTimer = next;
	//printf("TimerImpl::EvaluateNext: Found new timer to run..\n");
	return next;
}

std::ostream& operator<<(std::ostream &strm, const TimerImpl &t) {
  return strm << "TimerImpl " << t.myTimerNumber << (t.name.empty() ? "" : " ") << t.name;
}

string TimerImpl::str() {
  ostringstream s;
	if(name.empty()){
		s << "TimerImpl " << myTimerNumber << (name.empty() ? "" : " ") << name;
		return s.str();
	}
	return name; 
}

const char* TimerImpl::GetName() {
  return this->str().c_str();
}


TimerImpl::~TimerImpl()
{
	//printf("TimerImpl::TimerImpl: Destroying timer %d\n", myTimerNumber);fflush(stdout);
	timerPtrArray[myTimerNumber]=NULL;
	timerAlive[myTimerNumber]=false;
	if(nextTimer==this) {nextTimer = NULL;}
	if(curTimer==this) {curTimer= NULL;}
}


} //End of namespace

