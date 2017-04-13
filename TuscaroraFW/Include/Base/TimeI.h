////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef TimeBaseT_h
#define TimeBaseT_h



#include <Base/BasicTypes.h>
#include <Base/FixedPoint.h>

using namespace Types;

namespace PAL {
  template <class TimeType>
  class Time_I {
  public:
    virtual TimeType* GetTimeRange()=0;
    virtual uint64_t GetMilliseconds()=0;
    virtual uint64_t GetMicroseconds()=0;
    virtual TimeType GetTime()=0;
    //virtual float GetFloatTime()=0;
    virtual uint32_t GetSeconds()=0;
    virtual Fixed_15_16_t GetConfidence()=0;
    //virtual void SetConfidence();
    virtual ~Time_I(){}
  };
	
	class U64NanoTime: public Time_I<uint64_t>{
	  uint64_t state;
	public:
	  /* Constructors: Creates the U64NanoTime object. The operand [if provided] sets the initial state to the provided value. */
	  U64NanoTime();
	  U64NanoTime(uint64_t time);

	  /* Resets the time to the value provided as the operand.
	   * The operand should be defined in nanoseconds.
	   * */
	  void SetTime(uint64_t time);

	  /* Returns a pointer pointing to an uint64_t[2] array defining the oldest and the newest time that can be represented by this object.
	   * */
	  uint64_t* GetTimeRange();

	  /* Returns the absolute current time in units of seconds.
	   * */
	  uint32_t GetSeconds();
	  /* Returns the absolute current time in units of milli seconds.
	   * */
	  uint64_t GetMilliseconds();

	  /* Returns the absolute current time in units of micro seconds.
	   * */
	  uint64_t GetMicroseconds();

	  /* Returns the absolute current time in units of nano seconds.
	   * */
	  uint64_t GetTime();
	  
	  /* Returns the confidence interval for the timing quaries.
	   * */
	  Fixed_15_16_t GetConfidence();
	  ~U64NanoTime();
	  
	  //void SetConfidence();
	  //overloadded operators for comparison with the
	  inline void operator = (const uint64_t& val){ state =val; }
	  inline void operator = (const U64NanoTime& val){ state =val.state; }
	  inline bool operator< (const U64NanoTime& rhs){ return (state < rhs.state); }
	  inline bool operator> (const U64NanoTime& rhs){ return (state > rhs.state); }
	  inline bool operator == (const U64NanoTime& rhs){ return (state == rhs.state); }
	  inline bool operator != (const U64NanoTime& rhs){ return (state != rhs.state); }
	  inline uint64_t operator - (const U64NanoTime& rhs){ return (state - rhs.state); }
	  inline uint64_t operator + (const U64NanoTime& rhs){ return (state + rhs.state); }
	  inline uint64_t operator - (const uint64_t& rhs){  return state - rhs; }
	  inline uint64_t operator + (const uint64_t& rhs){  return state + rhs; }
	  
	}__attribute__((packed, aligned(1)));

	/*
	template<class FixedTimeT>
	class TimeBaseT {

	private:
		uint8_t* time;
		Fixed_16_15_t confidence;

	public:
		virtual FixedTimeT* GetTimeRange()=0;
		virtual uint64__t GetMilliseconds()=0;
		virtual uint64__t GetMicroseconds()=0;
		virtual FixedTimeT GetTime();
		virtual float GetFloatTime()=0;
		virtual uint32_t GetSeconds()=0;
		virtual Fixed_16_15_t GetConfidence()=0;
		virtual void SetConfidence();
		virtual ~TimeBaseT() {}
	};
	*/
	//typedef TimeBaseT<Fixed_32_20_t> ProbTime_32_20_t;
	//typedef TimeBaseT<uint64__t> LongTime_t;

}

#endif // TimeBaseT_h
