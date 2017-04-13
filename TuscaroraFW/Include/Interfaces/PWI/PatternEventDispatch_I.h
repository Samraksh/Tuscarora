////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef PATTERN_EVENT_DISPATCH_I_H_
#define PATTERN_EVENT_DISPATCH_I_H_

#include <Interfaces/Pattern/PatternEvent.h>
#include <Interfaces/PWI/Framework_I.h>
//#include <Interfaces/Pattern/PatternBase.h>

namespace PWI {
  
 class PatternEventDispatch_I {
    
  public:
    virtual bool InvokeControlEvent (PatternId_t pid, ControlResponseParam _param0) = 0;
    virtual bool InvokeRecvMsgEvent (PatternId_t pid, FMessage_t& _param0) = 0;
    virtual bool InvokeDataStatusEvent (PatternId_t pid, DataStatusParam _param0) = 0;
    virtual bool InvokeNeighborUpdateEvent (PatternId_t pid, NeighborUpdateParam _param0) = 0;
	virtual void RegisterDelegates (PatternId_t pid, RecvMessageDelegate_t *_recvDelegate, NeighborDelegate *_nbrDelegate, DataflowDelegate_t *_dataNotifierDelegate, ControlResponseDelegate_t *_controlDelegate){} 
    virtual ~PatternEventDispatch_I() {}
  };
  
} //End of namespace

#endif //PATTERN_EVENT_DISPATCH_I_H_
