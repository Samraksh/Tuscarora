////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef DEPLOYMENT_H_
#define DEPLOYMENT_H_

#include <Base/FrameworkTypes.h>
#include <Sys/Run.h>
#include <Lib/Misc/Debug.h>

NodeId_t MY_NODE_ID;
RuntimeOpts* RUNTIME_OPTS;
uint16_t NETWORK_SIZE;

uint32_t simtime;
uint64_t StartTime, CurTime;

uint64_t ExpiredTime(){ 
  CurTime = Lib::Debug::GetTimeMicro();
  return (CurTime - StartTime);
}

bool RunDone(double runTimeInSecs){
  if(ExpiredTime() >= runTimeInSecs * 1000000) return true;
  else return false; 
}




#endif //DEPLOYMENT_H_