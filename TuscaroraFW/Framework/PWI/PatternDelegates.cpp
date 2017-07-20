////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include "PatternDelegates.h"

// extern bool DBG_PATTERN; // set in Lib/Misc/Debug.cpp
namespace PWI {
  PTN_DelegatesMap_t *ptnDelMap;
  
  template <PTN_EventNameE eventName, typename Param0>
  PatternEvent<eventName,Param0>::PatternEvent(){
  }
  
  template <PTN_EventNameE eventName, typename Param0>
  PatternEvent<eventName,Param0>::~PatternEvent(){
  }
   
  template <>
  bool PatternEvent<PTN_CONT_RES_EVT, ControlResponseParam> ::Invoke(PatternId_t pid, ControlResponseParam param0){
    if(ptnDelMap-> operator [](pid).controlDelegate) {
      ptnDelMap-> operator [](pid).controlDelegate->operator()(param0);
      return true;
    }
    
    Debug_Printf(DBG_PATTERN, "PatternEvent:: Something wrong: ControlResponse Delegate is not registered for %d pattern \n", pid);
    return false;
  }

  template <>
  bool PatternEvent<PTN_NBR_CHG_EVT, NeighborUpdateParam> ::Invoke(PatternId_t pid, NeighborUpdateParam param0){
    if(ptnDelMap-> operator [](pid).nbrDelegate) {
      ptnDelMap-> operator [](pid).nbrDelegate->operator()(param0);
      return true;
    }
    
    Debug_Printf(DBG_PATTERN, "PatternEvent:: Something wrong: NeighborUpdate Delegate is not registered for %d pattern \n", pid);
    return false;
  }

  template <>
  bool PatternEvent<PTN_RECV_MSG_EVT, FMessage_t&> ::Invoke(PatternId_t pid, FMessage_t& param0){
    if(ptnDelMap-> operator [](pid).recvDelegate) {
      ptnDelMap-> operator [](pid).recvDelegate->operator()(param0);
      return true;
    }
    Debug_Printf(DBG_PATTERN, "PatternEvent:: Something wrong: recv Delegate is not registered for %d pattern \n", pid);
    return false;
  }

  template <>
  bool PatternEvent<PTN_DATA_NTY_EVT, DataStatusParam> ::Invoke(PatternId_t pid, DataStatusParam param0){
    if(ptnDelMap-> operator [](pid).dataNotifierDelegate) {
      ptnDelMap-> operator [](pid).dataNotifierDelegate->operator()(param0);
      //printf("HOGE!\n");
      return true;
    }
    Debug_Printf(DBG_PATTERN,"PatternEvent:: Something wrong: Data Notification Delegate is not registered for %d pattern \n", pid);
    return false;
  }
  

//Template "Explicit Instantiation"  
template class PatternEvent<PTN_CONT_RES_EVT, ControlResponseParam>; 
template class PatternEvent<PTN_NBR_CHG_EVT, NeighborUpdateParam> ; 
template class PatternEvent<PTN_RECV_MSG_EVT, FMessage_t&> ;
template class PatternEvent<PTN_DATA_NTY_EVT, DataStatusParam> ;

} // End namespace
