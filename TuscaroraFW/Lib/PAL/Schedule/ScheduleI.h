////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef ScheduleI_h
#define ScheduleI_h

#include "Types/BasicTypes.h"
#include <Interfaces/Types/Delegate.h>

namespace PAL{
  
  
enum ScheduleTypeE {RecvSchd, SendSchd, LinkEstSchd};

template<class TimeT, class StateT, class PeriodT>
struct ScheduleInfo {
	TimeT startTime;
	StateT state;
	PeriodT period;
	ScheduleTypeE type;
};

template<class TimeT, class NodeType>
struct EventInfoTemplate {
	TimeT time;
	uint16_t objectID;
	NodeType nodeID;
};

//A base class for all Schedule objects
class ScheduleBaseI {
public:
	virtual ~ScheduleBaseI() {}
};

///This is interface definition to a generic schedule object. The schedule internally maintains a list of events in time,
///a user can either poll the object to get the time to next event or subcribe to a callback using the 'Event' delegate
///The current index or index 0 always points to the time of last event that happened. Future event time and past event times can be
template<class PRNG,  class PrngStateT, class TimeT, class NodeIDType>
class ScheduleI : public ScheduleBaseI {
public:
  
  ///User of this scheduler can bind a function to this delegate to get callbacks when events are generated
  Delegate<void, EventInfoTemplate<TimeT,NodeIDType>& > Event;
  ///Register a delegate to get a callback when an event happens
  virtual void RegisterDelegate(Delegate<void, EventInfoTemplate<TimeT, NodeIDType>& >& delegate) =0;
  ///The time of the next event in future
  virtual TimeT TimeOfNextEvent() = 0;
  ///The time of the last event
  virtual TimeT TimeOfLastEvent() = 0;
  ///Sets the parameters of the schedule. The state sets the parameters for the underlying event generator like a random number generator.
  ///The startTime indicates the start of reference time frame, in the owner nodes reference time
  virtual bool Set(PrngStateT state, TimeT startTime) =0;

  ///Sets the node id of the owner of this schedule
  virtual void SetNode(NodeIDType node)=0;

  ///Returns the current state of the underlying event generator
  virtual PrngStateT GetState() = 0;

  ///Returns starting time of the reference frame
  virtual TimeT GetStartTime() =0;

  ///Returns the time of the indexed event.
  virtual TimeT operator [](int16_t index) =0;

  ///Sets the Min and Max index of events to be maintained.
  virtual bool SetEventsRange(int16_t minIndex, uint16_t maxIndex)=0;

  virtual inline ~ScheduleI(){}
};

}//End of namespace

#endif // ScheduleI_h
