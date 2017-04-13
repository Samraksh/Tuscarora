////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef _PTN_UTILS_H_
#define _PTN_UTILS_H_

#include <PAL/PAL.h>


using namespace PAL;

namespace Patterns {

	
typedef uint64_t PatternTimestamp_t;	

class PTN_Utils {
static PatternTimestamp_t startTime;

public:
	
	inline static void InitializeStateTime(){
		startTime = (PatternTimestamp_t)SysTime::GetEpochTimeInMilliSec();
	}
	//System time in millisecs
	inline static PatternTimestamp_t TimeStampMilliSec(){
		 return (PatternTimestamp_t)SysTime::GetEpochTimeInMilliSec();
	}
	
	// time since start of simulation
	inline static PatternTimestamp_t PatternAliveTimeMilliSec(){
		return TimeStampMilliSec() - startTime;	
	}
	
	// time since start of simulation
	inline static double PatternAliveTimeInSecs()
	{
		double now = PatternAliveTimeMilliSec()/1000;
		//Debug_Printf(DBG_PATTERN,"Cop::PatternAliveTimeInSecs() Now: %f\n", now);
		return now;
	}
};

} //End namespace

#endif //_PTN_UTILS_H_