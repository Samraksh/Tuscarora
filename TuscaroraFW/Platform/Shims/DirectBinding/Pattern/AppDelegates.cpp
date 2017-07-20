////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

//#include "AppDelegates.h"
//
//// extern bool DBG_PATTERN; // set in Lib/Misc/Debug.cpp
//namespace PWI {
//  PTN_DelegatesMap_t *ptnDelMap;
//
//  template <PTN_EventNameE eventName, typename Param0>
//  AppEvent<eventName,Param0>::AppEvent(){
//  }
//
//  template <PTN_EventNameE eventName, typename Param0>
//  AppEvent<eventName,Param0>::~AppEvent(){
//  }
//
//  template <>
//  bool AppEvent<PTN_CONT_RES_EVT, ControlResponseParam> ::Invoke(AppId_t pid, ControlResponseParam param0){
//    if(ptnDelMap-> operator [](pid).controlDelegate) {
//      ptnDelMap-> operator [](pid).controlDelegate->operator()(param0);
//      return true;
//    }
//
//    Debug_Printf(DBG_PATTERN, "AppEvent:: Something wrong: ControlResponse Delegate is not registered for %d pattern \n", pid);
//    return false;
//  }
//
//  template <>
//  bool AppEvent<PTN_NBR_CHG_EVT, NeighborUpdateParam> ::Invoke(AppId_t pid, NeighborUpdateParam param0){
//    if(ptnDelMap-> operator [](pid).nbrDelegate) {
//      ptnDelMap-> operator [](pid).nbrDelegate->operator()(param0);
//      return true;
//    }
//
//    Debug_Printf(DBG_PATTERN, "AppEvent:: Something wrong: NeighborUpdate Delegate is not registered for %d pattern \n", pid);
//    return false;
//  }
//
//  template <>
//  bool AppEvent<PTN_RECV_MSG_EVT, FMessage_t&> ::Invoke(AppId_t pid, FMessage_t& param0){
//    if(ptnDelMap-> operator [](pid).recvDelegate) {
//      ptnDelMap-> operator [](pid).recvDelegate->operator()(param0);
//      return true;
//    }
//    Debug_Printf(DBG_PATTERN, "AppEvent:: Something wrong: recv Delegate is not registered for %d pattern \n", pid);
//    return false;
//  }
//
//  template <>
//  bool AppEvent<PTN_DATA_NTY_EVT, DataStatusParam> ::Invoke(AppId_t pid, DataStatusParam param0){
//    if(ptnDelMap-> operator [](pid).dataNotifierDelegate) {
//      ptnDelMap-> operator [](pid).dataNotifierDelegate->operator()(param0);
//      //printf("HOGE!\n");
//      return true;
//    }
//    Debug_Printf(DBG_PATTERN,"AppEvent:: Something wrong: Data Notification Delegate is not registered for %d pattern \n", pid);
//    return false;
//  }
//
//
////Template "Explicit Instantiation"
//template class AppEvent<PTN_CONT_RES_EVT, ControlResponseParam>;
//template class AppEvent<PTN_NBR_CHG_EVT, NeighborUpdateParam> ;
//template class AppEvent<PTN_RECV_MSG_EVT, FMessage_t&> ;
//template class AppEvent<PTN_DATA_NTY_EVT, DataStatusParam> ;
//
//} // End namespace
