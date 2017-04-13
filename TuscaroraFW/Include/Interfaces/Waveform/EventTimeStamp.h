////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef EventTimeStamp_H_
#define EventTimeStamp_H_

#include <Base/FrameworkTypes.h>
#include <PAL/SysTime.h>

///Structure for implementing event timestamping between two neighbors 
struct EventTimeStamp{
  U64NanoTime senderEventTime;	///Receive time stamp of the packet in local clock time
  uint64_t senderDelay;
  
  EventTimeStamp(){
	senderEventTime = PAL::SysTime::GetEpochTimeInNanoSec();
	senderDelay=0;
  }
};


#endif //EventTimeStamp_H_