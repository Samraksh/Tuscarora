////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef TIMERI_H_
#define TIMERI_H_

#include <Base/BasicTypes.h>
#include <Base/Delegate.h>
#include <string>

using std::string;

namespace PAL{
	///Delegate for timer callbacks
	typedef Delegate<void, uint32_t> TimerDelegate;

	///Timer type enum
	enum TimerTypeE {
		ONE_SHOT,
		PERIODIC
	};

	///This is a way of exposing member variables of the implementation to derived classes. Use if needed
	//struct TimerImplProtected;
	
	class TimerImpl; 
	
	/// Timer: Defines the timer class
	class Timer{
		
	protected:
		//TimerImplProtected *protectedPtr;
		
	private:
		//class TimerImpl;
		TimerImpl *implPtr;
		
		
	public:
		/// Constructor takes 4 parameters a period in microseconds, TimerTypeE is either PERIODIC or ONE_SHOT,  a delegate to callback on events, and a timer name.
		Timer(uint32_t microsecond_period, TimerTypeE type, TimerDelegate& del, string name);

		/// Constructor takes 3 parameters a period in microseconds, TimerTypeE is either PERIODIC or ONE_SHOT and  a delegate to callback on events.
		Timer(uint32_t microsecond_period, TimerTypeE type, TimerDelegate& del);
		//Timer(){}

		///Starts  timer
		virtual bool Start();

		///Suspends timer
		virtual bool Suspend();

		///Changes the period or type of a timer, does not need a start after changing the value, timer is activated by default
		virtual bool Change(uint32_t period, uint8_t type);

		///Returns the string name of the timer
		virtual const char * GetName();

		///Destructor for the virtual interface
		virtual ~Timer(); 
	};
}
#endif // TIMERI_H_
