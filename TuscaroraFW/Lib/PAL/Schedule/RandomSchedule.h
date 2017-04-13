////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef RandomSchedule_H_
#define RandomSchedule_H_

#include <Base/BasicTypes.h>
#include <Base/FrameworkTypes.h>
#include <Misc/ScheduleI.h>
#include "Lib/PAL/PAL_Lib.h"


///TODO::
/// - History and future events need to be stored, to enable the user to move along a the event timeline

#include <sys/time.h>


namespace PAL{
	///Time format: Uses time as a single 64-bit number in microseconds.
	typedef  EventInfoTemplate<uint64_t,uint64_t> EventInfoU64;
	#define MAX_SCH_EVENTS 64

	//template <class PRNG, class StateT>
	template<class PRNG, class PrngStateT, class TimeT,  class NodeIDType>
	class RandomSchedule : public ScheduleI<PRNG,  PrngStateT, TimeT, NodeIDType> {
		PRNG *rand;
		PrngStateT state;
		TimeT events[MAX_SCH_EVENTS];
		uint16_t objectID;
		uint64_t nodeID;
		uint64_t period;
		//uint64_t rangeMin;
		//uint64_t rangeMax;
		Delegate<void, EventInfoU64&> *Event;

		///Implementation structures
		TimerDelegate *timDel;
		Timer *eventGenTimer;

		bool alive;

		uint64_t currentScheduledEventTime;
		uint64_t lastScheduledEventTime;
		uint64_t startTime;

	private:
		 void EventTimer_Handler(uint64_t event);
		 uint64_t GetTimeInLong();
		 void MoveForward();
		 void ScheduleEvent();

	 public:
		RandomSchedule<PRNG,  PrngStateT, TimeT, NodeIDType>(PRNG& rand, uint16_t objectId);
		RandomSchedule<PRNG,  PrngStateT, TimeT, NodeIDType>(PRNG& rand, uint16_t objectId, Delegate<void, EventInfoU64&>& delegate);
		void RegisterDelegate(Delegate<void, EventInfoU64&>& delegate);
		TimeT& TimeToNextEvent();
		TimeT TimeOfLastEvent();
		void Suspend();
		bool IsSuspended() { return alive; }
		TimeT TimeOfNextEvent();
		
		bool Start( uint64_t startTime);
		bool SetState(PrngStateT state, uint64_t startTime);
		bool SetEventsRange(int16_t minIndex, uint16_t maxIndex);
		TimeT operator [](int16_t index);

		void SetNode(uint64_t node) {
		  //printf("Setting node id: %ld \n",node);
			fflush (stdout);
			nodeID = node;
		}
		PrngStateT GetState();
		TimeT GetPeriod();
		TimeT GetStartTime();
		TimeT& operator [](uint16_t index) ;
		uint16_t IndexIncremeant(uint16_t index) ;
		uint16_t IndexMod(uint16_t index);

		///Mukundan is not sure if the locking/unlocking is needed. But Ken thinks they are important
		bool Lock(uint16_t lowIndex, uint16_t highIndex);
		bool UnLock(uint16_t lowIndex);

		~RandomSchedule<PRNG,  PrngStateT, TimeT, NodeIDType>(){
			delete rand;
		}
	};

	template <class PRNG, class PrngStateT, class TimeT,  class NodeIDType>
	TimeT RandomSchedule<PRNG, PrngStateT, TimeT, NodeIDType>::TimeOfNextEvent(){
	  return currentScheduledEventTime;
	}

	template <class PRNG, class PrngStateT, class TimeT,  class NodeIDType>
	TimeT RandomSchedule<PRNG,  PrngStateT, TimeT, NodeIDType>::TimeOfLastEvent(){
	  return lastScheduledEventTime;
	}

	template <class PRNG, class PrngStateT, class TimeT,  class NodeIDType>
	PrngStateT RandomSchedule<PRNG,  PrngStateT, TimeT, NodeIDType>::GetState(){
		return state;
	}

	template <class PRNG, class PrngStateT, class TimeT,  class NodeIDType>
	TimeT RandomSchedule<PRNG,  PrngStateT, TimeT, NodeIDType>::GetPeriod(){
		return period;
	}

	template <class PRNG, class PrngStateT, class TimeT,  class NodeIDType>
	TimeT RandomSchedule<PRNG,  PrngStateT, TimeT, NodeIDType>::GetStartTime(){
	  return startTime;
	}

	template <class PRNG, class PrngStateT, class TimeT,  class NodeIDType>
	TimeT& RandomSchedule<PRNG,  PrngStateT, TimeT, NodeIDType>::operator [](uint16_t index){
	  return 0;//To remove warning.
	}

	template <class PRNG, class PrngStateT, class TimeT,  class NodeIDType>
	uint16_t RandomSchedule<PRNG,  PrngStateT, TimeT, NodeIDType>::IndexIncremeant(uint16_t index){
	  return 0;
	}

	template <class PRNG, class PrngStateT, class TimeT,  class NodeIDType>
	uint16_t RandomSchedule<PRNG,  PrngStateT, TimeT, NodeIDType>::IndexMod(uint16_t index){
	  return 0;
	}

	///Mukundan is not sure if the locking/unlocking is needed. But Ken thinks they are important
	template <class PRNG, class PrngStateT, class TimeT,  class NodeIDType>
	bool RandomSchedule<PRNG,  PrngStateT, TimeT, NodeIDType>::Lock(uint16_t lowIndex, uint16_t highIndex){
	  return true;
	}

	template <class PRNG, class PrngStateT, class TimeT,  class NodeIDType>
	bool RandomSchedule<PRNG,  PrngStateT, TimeT, NodeIDType>::UnLock(uint16_t lowIndex){
	  return true;
	}

	template <class PRNG, class PrngStateT, class TimeT,  class NodeIDType>
	inline uint64_t RandomSchedule<PRNG,  PrngStateT, TimeT, NodeIDType>::GetTimeInLong(){
		struct timeval curtime;
		gettimeofday(&curtime,NULL);
		return (uint64_t)(curtime.tv_sec) * 1000000 + curtime.tv_usec;
	}

	///TODO:: Not implemented yet
	template <class PRNG, class PrngStateT, class TimeT,  class NodeIDType>
	TimeT RandomSchedule<PRNG,  PrngStateT, TimeT, NodeIDType>::operator [](int16_t index){
		return events[index];
	}

	///TODO:: Not implemented yet
	template <class PRNG, class PrngStateT, class TimeT,  class NodeIDType>
	bool RandomSchedule<PRNG,  PrngStateT, TimeT, NodeIDType>::SetEventsRange(int16_t minIndex, uint16_t maxIndex) {
		return false;
	}

	template <class PRNG, class PrngStateT, class TimeT,  class NodeIDType>
	inline void RandomSchedule<PRNG,  PrngStateT, TimeT, NodeIDType>::Suspend(){
		alive = false;
	}

	template <class PRNG, class PrngStateT, class TimeT,  class NodeIDType>
	void RandomSchedule<PRNG,  PrngStateT, TimeT, NodeIDType>::RegisterDelegate(Delegate<void, EventInfoU64&>& delegate){
		Event= &delegate;
	}

	template <class PRNG, class PrngStateT, class TimeT,  class NodeIDType>
	RandomSchedule<PRNG,  PrngStateT, TimeT, NodeIDType>::RandomSchedule(PRNG& _rnd, uint16_t id){
		rand = &_rnd;
		objectID = id;
		alive = false;
		timDel = new TimerDelegate(this, &RandomSchedule<PRNG,  PrngStateT, TimeT, NodeIDType>::EventTimer_Handler);
		//Initialize timer, but dont start, the period of timer doesnot matter, we just need some value
		eventGenTimer = new Timer(100000, ONE_SHOT, *timDel);
	}

	template <class PRNG, class PrngStateT, class TimeT,  class NodeIDType>
	RandomSchedule<PRNG,  PrngStateT, TimeT, NodeIDType>::RandomSchedule(PRNG& _rnd, uint16_t id, Delegate<void, EventInfoU64&>& delegate)
	:RandomSchedule(_rnd, id)
	{
		Event= &delegate;
	}

	template <class PRNG, class PrngStateT, class TimeT,  class NodeIDType>
	bool RandomSchedule<PRNG,  PrngStateT, TimeT, NodeIDType>::Start(uint64_t _startTime){
	  if(alive){
	    eventGenTimer->Suspend();
	    alive = false;
	  }
	  startTime = _startTime;
	  currentScheduledEventTime = startTime;

	  while(currentScheduledEventTime <= GetTimeInLong()) {
	    MoveForward();
	  }
	  ScheduleEvent();

	  eventGenTimer->Start();
	  alive =true;
	  return false; // todo, to avoid compiler warning. What should this be?
	}


	template <class PRNG, class PrngStateT, class TimeT,  class NodeIDType>
	bool RandomSchedule<PRNG,  PrngStateT, TimeT, NodeIDType>::SetState(PrngStateT _state, uint64_t _startTime){
	  if(alive){
	    eventGenTimer->Suspend();
	    alive = false;
	  }

	  state = _state;
	  rand->SetState(state);

		return Start(_startTime);
	}
	
	
	
	
	template <class PRNG, class PrngStateT, class TimeT,  class NodeIDType>
	void RandomSchedule<PRNG,  PrngStateT, TimeT, NodeIDType>::EventTimer_Handler(uint64_t event){
	  //if the scheduler is alive do stuff, otherwise ignore events
	  if(alive){
	    MoveForward();
	    EventInfoU64 eventInfo;
	    eventInfo.time = currentScheduledEventTime;
	    eventInfo.nodeID = nodeID;
	    eventInfo.objectID = objectID;
	    Event->operator ()(eventInfo);
	    ScheduleEvent();
	  }
	}
	
	template <class PRNG, class PrngStateT, class TimeT,  class NodeIDType>
	void RandomSchedule<PRNG,  PrngStateT, TimeT, NodeIDType>::MoveForward(){
	  lastScheduledEventTime = currentScheduledEventTime;
	  currentScheduledEventTime += this-> template rand->GetNext();
	}
	
	
	template <class PRNG, class PrngStateT, class TimeT,  class NodeIDType>
	  inline void RandomSchedule<PRNG,  PrngStateT, TimeT, NodeIDType>::ScheduleEvent(){
	  eventGenTimer->Change((uint32_t)(currentScheduledEventTime - GetTimeInLong()), ONE_SHOT);
	}
}
#endif // RandomSchedule_H_
