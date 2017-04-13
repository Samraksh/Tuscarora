////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include "PAL/SysTime.h"
#include <stdio.h>
//#include <iostream>
#include <time.h>
#include <sys/time.h>
#include <cstdlib>

using namespace PAL;

namespace PAL {
	
///Returns the number of Microseconds since the Unix Epoch	
uint64_t SysTime::GetEpochTimeInMicroSec()
{
	struct timeval now;
	gettimeofday(&now, NULL);
	return ((now.tv_sec *1000000)+ now.tv_usec);
}


///Returns the number of Milliseconds since the Unix Epoch	
uint64_t SysTime::GetEpochTimeInMilliSec()
{
	struct timeval now;
	gettimeofday(&now, NULL);
	return ((now.tv_sec *1000)+ (now.tv_usec/1000));
}



uint32_t SysTime::GetEpochTimeInMilliSec_u32()
{
	struct timeval now;
	gettimeofday(&now, NULL);
	return ((now.tv_sec *1000)+ (now.tv_usec/1000));
}


///Returns the number of Microseconds since the Unix Epoch	
uint64_t SysTime::GetEpochTimeInNanoSec()
{
uint64_t ret=0;	
#if PLATFORM_LINUX==1 //might work only on linux	
	struct timespec now;
	if(clock_gettime( CLOCK_REALTIME, &now) == -1 ) {
      perror( "clock gettime" );
      exit( EXIT_FAILURE );
    }
    ret=(now.tv_sec *1000000000)+ now.tv_nsec;
#else
	struct timeval now;
	gettimeofday(&now, NULL);
	ret=(now.tv_sec *1000000000)+ (now.tv_usec*1000);
#endif
	return ret;
}
	

double SysTime::GetEpochTimeInSec()
{
	struct timeval now;
	gettimeofday(&now, NULL);
	double ret = double(now.tv_sec) + double(now.tv_usec/1000000);
	return ret;
}

	
	
}; //End of namespace
