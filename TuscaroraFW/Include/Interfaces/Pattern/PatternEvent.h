////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef _PATTERN_EVENTS_H_
#define _PATTERN_EVENTS_H_
#include <Base/Delegate.h>
#include <Base/FrameworkTypes.h>

namespace PWI {

  enum PTN_EventNameE {
    PTN_NBR_CHG_EVT,
    PTN_RECV_MSG_EVT,
    PTN_DATA_NTY_EVT,
    PTN_CONT_RES_EVT
    //PTN_SCHD_UPD_SIG
  };
 
  template <PTN_EventNameE eventName, typename Param0>
  class PatternEvent{
    
  private:
    //int SignalCount;
    //Delegate<void, Param0> *delegate;

  public:
    PatternEvent();
    ~PatternEvent(void);

   bool Invoke(PatternId_t pid, Param0 param0);
    
  };
  
}

#endif // _PATTERN_EVENTS_H_