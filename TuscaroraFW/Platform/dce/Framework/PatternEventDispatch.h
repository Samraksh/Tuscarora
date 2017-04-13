////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef PATTERN_EVENT_DISPATCH_H_
#define PATTERN_EVENT_DISPATCH_H_

#include <Interfaces/Pattern/PatternEvent.h>
#include <Interfaces/PWI/Framework_I.h>
#include <Interfaces/Pattern/PatternBase.h>
#include <Lib/DS/BSTMapT.h>
#include "PatternDelegates.h"
#include <Interfaces/PWI/PatternEventDispatch_I.h>

namespace PWI {
  extern PTN_DelegatesMap_t *ptnDelMap;
  
 class PatternEventDispatch : public PatternEventDispatch_I {
    
    PatternEvent<PTN_CONT_RES_EVT, ControlResponseParam> controlEvent;
    PatternEvent<PTN_NBR_CHG_EVT, NeighborUpdateParam> nbrEvent;
    PatternEvent<PTN_RECV_MSG_EVT, FMessage_t&> rcvMsgEvent;
    PatternEvent<PTN_DATA_NTY_EVT, DataStatusParam> dataNotifierEvent;
    
    
  public:
    PatternEventDispatch(){
        printf("PatternEventDispatch Initializing for dce \n");
      ptnDelMap = new PTN_DelegatesMap_t();
    }
    
    void RegisterDelegates(PatternId_t _pid, RecvMessageDelegate_t *_recvDelegate, 
	NeighborDelegate *_nbrDelegate, DataflowDelegate_t *_dataNotifierDelegate, ControlResponseDelegate_t *_controlDelegate) {
      ptnDelMap->operator[](_pid).controlDelegate = _controlDelegate;
      ptnDelMap->operator[](_pid).recvDelegate = _recvDelegate;
      ptnDelMap->operator[](_pid).dataNotifierDelegate = _dataNotifierDelegate;
      ptnDelMap->operator[](_pid).nbrDelegate = _nbrDelegate;
    }
    
    bool InvokeControlEvent (PatternId_t pid, ControlResponseParam _param0) {
      return controlEvent.Invoke(pid, _param0);
    }
    bool InvokeRecvMsgEvent (PatternId_t pid, FMessage_t& _param0) {
      return rcvMsgEvent.Invoke(pid, _param0);
    }
    bool InvokeDataStatusEvent (PatternId_t pid, DataStatusParam _param0) {
      return dataNotifierEvent.Invoke(pid, _param0);
    }
    bool InvokeNeighborUpdateEvent (PatternId_t pid, NeighborUpdateParam _param0) {
      return nbrEvent.Invoke(pid, _param0);
    }
    
  };
  
} //End of namespace

#endif //PATTERN_EVENT_DISPATCH_H_
