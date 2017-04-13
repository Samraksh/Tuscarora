////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#include "Event.h"

using namespace PAL;
namespace PAL {

Event::Event(uint32_t delayInMicroSec, EventDelegate& _del, void * _eventParam){
	alive=true;
	param.eventPtr=this;
	param.eventParam = _eventParam;
	timerDel = new TimerDelegate(this, &Event::TimerCallback);
	//Initialize timer, but dont start, the period of timer doesnot matter, we just need some value
	oneshotTimer = new Timer(delayInMicroSec, ONE_SHOT, *timerDel);
	del= &_del;
	oneshotTimer->Start();
}

bool Event::ReSchedule(uint32_t delayInMicroSec, void * _param){
	alive=true;
	param.eventParam = _param;
	oneshotTimer->Change(delayInMicroSec,ONE_SHOT);
	return true;
}

///Cancels an event from firing
void Event::Cancel(){
	alive=false;
}

void Event::TimerCallback(uint32_t timercount){
	if(alive){
		alive=false;
		del->operator ()(param);
	}
	if(!alive) {
		this->~Event();
	}
}


}

