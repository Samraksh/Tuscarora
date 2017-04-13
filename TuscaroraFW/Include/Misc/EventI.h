////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef EVENTI_H_
#define EVENTI_H_

#include <Base/BasicTypes.h>
#include <Base/Delegate.h>

using namespace PAL;
namespace PAL{

	class EventI;
	struct EventDelegateParam {
		EventI *eventPtr;
		void* eventParam;
	};

	typedef Delegate<void, EventDelegateParam> EventDelegate;

	/// Event Interface: Defines an interface for a scheduling an arbitrary event in future with arbitrary parameter
	class EventI{
	public:
		/// Constructor takes 2 parameters a period in microseconds, TimerTypeE is either PERIODIC or ONE_SHOT,  a delegate to callback on events, and a timer name.
		EventI(uint32_t delayInMicroSec, EventDelegate& del, void * eventParam);
		EventI(){}

		virtual bool ReSchedule(uint32_t microsecond_period, void * param)=0;

		///Cancels an event from firing
		virtual void Cancel()=0;

		///Destructor for the virtual interface
		virtual ~EventI() {}
	};
}



#endif /* EVENTI_H_ */
