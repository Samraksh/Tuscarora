////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 




#ifndef TIMER_H
#define TIMER_H


#include <ostream>

//#include <stdlib.h>
//#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <sys/time.h>

#include  <Base/BasicTypes.h>
#include <PAL/Timer.h>
//#include Delegate.h"

namespace PAL {

	class TimerImpl {
	  TimerDelegate *delegate;
	  int32_t GetFreeTimerID();

	public:
	  uint32_t period;
	  uint32_t countdown;
	  uint32_t eventNumber;
	  //bool alive;
	  bool oneshot;
	  struct timeval lastEvalTime;
	  int32_t myTimerNumber;
	  string name; // label for the timer

	private:
		void Init(uint32_t microsecond_period, TimerTypeE type, TimerDelegate& del);
	public:
		TimerImpl(uint32_t microsecond_period, TimerTypeE type, TimerDelegate& del);
		TimerImpl(uint32_t microsecond_period, TimerTypeE type, TimerDelegate& del, string name);
		~TimerImpl();
		bool Start();
		bool Suspend();
		bool Change(uint32_t period, uint8_t type);
		const char * GetName();
		
		void Execute();
		TimerImpl* EvaluateNext();
		bool SetNext(TimerImpl* timer);
		void ExecuteExpiredTimers(TimerImpl* exp);
		string str();
		
	};

	std::ostream& operator<<(std::ostream &strm, const Timer &t);
}
#endif //TIMER_H
