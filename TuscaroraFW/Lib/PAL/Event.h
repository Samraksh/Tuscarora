////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef EVENT_H_
#define EVENT_H_

#include <Misc/EventI.h>
#include "Lib/PAL/PAL_Lib.h"

namespace PAL {

class Event: public EventI {
	bool alive;
	EventDelegateParam param;
	EventDelegate *del;

	TimerDelegate *timerDel;
	Timer *oneshotTimer;

private:

	void TimerCallback(uint32_t timercount);


public:
	/// Constructor takes 2 parameters a period in microseconds, TimerTypeE is either PERIODIC or ONE_SHOT,  a delegate to callback on events, and a timer name.
	Event(uint32_t delayInMicroSec, EventDelegate& del, void * eventParam);
	bool ReSchedule(uint32_t microsecond_period, void * param);
	///Cancels an event from firing
	void Cancel();
	~Event(){}

};


}



#endif /* EVENT_H_ */
