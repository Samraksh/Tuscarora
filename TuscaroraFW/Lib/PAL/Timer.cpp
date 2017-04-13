////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include <PAL/Timer.h>
#include <Lib/PAL/TimerImpl.h>

namespace PAL {
	
	
Timer::Timer(uint32_t microsecond_period, TimerTypeE type, TimerDelegate& del, string _name){
	implPtr = new TimerImpl(microsecond_period, type, del, _name);
}

Timer::Timer(uint32_t microsecond_period, TimerTypeE type, TimerDelegate& del){
	implPtr = new TimerImpl(microsecond_period, type, del);
}	
	
bool Timer::Start(){
	return implPtr->Start();
}


bool Timer::Suspend(){
	return implPtr->Suspend();
}


bool Timer::Change(uint32_t period, uint8_t type){
	return implPtr->Change(period,type);
}


const char* Timer::GetName(){
	return implPtr->GetName();
}


Timer::~Timer(){
	delete implPtr;
}

} //End namespace