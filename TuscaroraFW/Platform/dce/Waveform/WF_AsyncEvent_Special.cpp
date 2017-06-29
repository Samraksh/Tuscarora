////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 


//#ifndef WF_ASYNC_EVENT_SPECIAL_H_
//#define WF_ASYNC_EVENT_SPECIAL_H_

#include <Interfaces/Waveform/WF_Types.h>
#include <Interfaces/Waveform/WF_Event.h>
#include "Framework/Core/WF_Events/RecvWFEvents.h"

using namespace PAL;
using namespace Core::Events;

// extern bool DBG_SHIM; // set in Lib/Misc/Debug.cpp

namespace Core{
namespace Events {
  extern RecvWFEvents<uint64_t>* waveformEventReceiver;
}
}

namespace Waveform{

//dummy empty contructor.
  /*
template <WF_SignalNameE eventName, typename Param0>
WF_Event<eventName,Param0>::WF_Event (){
    
}
*/
//specilized contructor for each type of Signals


template<>
WF_Event<WF_GREET_EVT, void* >::WF_Event (WaveformId_t id){
  //delegate=new Delegate<void, WF_LinkEstimationParam<uint64_t> >(waveformEventReceiver, &RecvWFEvents<uint64_t>::Recv_LinkEstimationEvent);
}

template<>
WF_Event<WF_LINK_EST_EVT,WF_LinkEstimationParam<uint64_t> >::WF_Event(WaveformId_t id){
  delegate=new Delegate<void, WF_LinkEstimationParam<uint64_t> >(waveformEventReceiver, &RecvWFEvents<uint64_t>::Recv_LinkEstimationEvent);
}

template<>
WF_Event<WF_RECV_MSG_EVT,WF_RecvMsgParam<uint64_t> >::WF_Event (WaveformId_t id){
  delegate=new Delegate<void, WF_RecvMsgParam<uint64_t> >(waveformEventReceiver, &RecvWFEvents<uint64_t>::Recv_RecvMsgEvent);
}

template<>
WF_Event<WF_DATA_NTY_EVT,WF_DataStatusParam<uint64_t> >::WF_Event (WaveformId_t id){
  delegate=new Delegate<void, WF_DataStatusParam<uint64_t> >(waveformEventReceiver, &RecvWFEvents<uint64_t>::Recv_DataNotificationEvent);
}
 
template<>
WF_Event<WF_CONT_RES_EVT,WF_ControlResponseParam>::WF_Event (WaveformId_t id){
  delegate=new Delegate<void, WF_ControlResponseParam>(waveformEventReceiver, &RecvWFEvents<uint64_t>::Recv_ControlResponseEvent);
}

template<>
WF_Event<WF_SCHD_UPD_EVT,WF_ScheduleUpdateParam>::WF_Event (WaveformId_t id){
  delegate=new Delegate<void, WF_ScheduleUpdateParam>(waveformEventReceiver, &RecvWFEvents<uint64_t>::Recv_ScheduleUpdateEvent);
}
 

template <WF_EventNameE eventName, typename Param0>
bool WF_Event<eventName,Param0>::Invoke (Param0 param0){
	if(delegate){
		delegate->operator () (param0);
		return true;
	}
	return false;
}

//specialization of invoke to convert from a waveform local parameter to parameter on the heap
/*
template <>
bool WF_Event<WF_CONT_RES_EVT,WF_ControlResponseParam>::Invoke (WF_ControlResponseParam param0){
  WF_ControlResponseParam h_param;
  h_param = param0;
  h_param.data = new uint8_t[param0.dataSize];
  memcpy(h_param.data, param0.data, param0.dataSize);
  Debug_Printf(DBG_SHIM, "WF_ASYNC_EVENT:: Control response id %d \n", param0.id);
  
  delegate->operator () (h_param);
  
  if(h_param.data != NULL){
    delete[]((uint8_t*)h_param.data);
    h_param.data = NULL;
    Debug_Printf(DBG_SHIM, "WF_ASYNC_EVENT:: Setting h_param.data to null\n");
  }
  return true;
}
*/

//specialization of invoke to convert from a waveform local parameter to parameter on the heap
/*
template <>
bool WF_Event<WF_RECV_MSG_EVT,WF_RecvMsgParam<uint64_t> >::Invoke (WF_RecvMsgParam<uint64_t> param0){
  WF_RecvMsgParam<uint64_t> *h_param = new WF_RecvMsgParam<uint64_t>;
  *h_param = param0;
  uint8_t* payload = NULL;
 
  if(param0.msg->GetPayloadSize() != 0){
	  payload = new uint8_t[param0.msg->payloadSize];
	  memcpy(payload, param0.msg->GetPayload(), param0.msg->GetPayloadSize());
  }

  h_param->msg = new WF_Message_n64_t;
  memcpy(h_param->msg, param0.msg, sizeof(WF_Message_n64_t));
  h_param->msg->SetPayload(payload);
  h_param->msg->SetPayloadSize(param0.msg->GetPayloadSize());

  h_param->msg->SetNumberOfDest(param0.msg->GetNumberOfDest());
  Debug_Printf(DBG_SHIM,"WF_ASYNC_EVENT::Address of msg is %p\n",h_param->msg);fflush(stdout);
  Debug_Printf(DBG_SHIM,"WF_ASYNC_EVENT::payload size is %d\n",h_param->msg->GetPayloadSize());
  delegate->operator () (*h_param);
  if(h_param != NULL){
     delete(h_param);
     h_param = NULL;
     Debug_Printf(DBG_SHIM, "WF_ASYNC_EVENT:: Setting h_param to null\n");
  }
  return true;
}
*/


//specialization of invoke to convert from a waveform local parameter to parameter on the heap
template <>
bool WF_Event<WF_DATA_NTY_EVT,WF_DataStatusParam<uint64_t> >::Invoke (WF_DataStatusParam<uint64_t> param0){
	//printf("WF_ASYNC_EVENT::Data Status Event invoke of type %d\n", param0.statusType[0]);fflush(stdout);
	Debug_Printf(DBG_SHIM,"WF_ASYNC_EVENT::Address of param0 is %p\n", &param0);fflush(stdout);
	Debug_Printf(DBG_SHIM,"WF_ASYNC_EVENT::Address of param0.dest is %p\n",param0.destArray);fflush(stdout);
	Debug_Printf(DBG_SHIM,"WF_ASYNC_EVENT::noOfDest is %d\n", param0.noOfDest);fflush(stdout);
	Debug_Printf(DBG_SHIM,"WF_ASYNC_EVENT::readyToReceiv is %d\n",param0.readyToReceive);fflush(stdout);
	Debug_Printf(DBG_SHIM,"WF_ASYNC_EVENT::Show dest List\n");
	for(uint16_t index =0 ; index < param0.noOfDest; index++){
	Debug_Printf(DBG_SHIM, "WF_ASYNC_EVENT:: %ld   address:at %p\n",param0.destArray[index], &(param0.destArray[index]));
	Debug_Printf(DBG_SHIM,"WF_ASYNC_EVENT::Status is %d\n",param0.statusValue[index]);fflush(stdout);
	Debug_Printf(DBG_SHIM,"WF_ASYNC_EVENT::ackType is %d\n", param0.statusType[index]);fflush(stdout);

	}

	/*  WF_DataStatusParam<uint64_t> h_param(param0.msgId, param0.wfId, (WF_MessageStatusE)param0.status);
	h_param.ackType = param0.ackType;
	h_param.noOfDest = param0.noOfDest;
	h_param.readyToReceive = param0.readyToReceive;
	//Maybe I should not allocate memory if noOfDest is 0
	if(param0.noOfDest == 0){
		Debug_Printf(DBG_SHIM,"WF_ASYNC_EVENT::Setting dest to null\n");fflush(stdout);
		h_param.dest = NULL;
	}else{
		h_param.dest = new uint64_t[param0.noOfDest];
		memcpy(h_param.dest, param0.dest, param0.noOfDest * sizeof(uint64_t));
	}

	Debug_Printf(DBG_SHIM,"WF_ASYNC_EVENT::Address of h_param.dest is %p\n",h_param.dest);fflush(stdout);
	Debug_Printf(DBG_SHIM,"WF_ASYNC_EVENT:: Message id is %d\n",h_param.msgId);
	for(unsigned int index = 0; index < h_param.noOfDest; index++){
	Debug_Printf(DBG_SHIM,"dest[%d] is %ld\n",index, h_param.dest[index]);fflush(stdout);
	}
	Debug_Printf(DBG_SHIM,"WF_ASYNC_EVENT::noOfDest is %d\n", h_param.noOfDest);fflush(stdout);
	Debug_Printf(DBG_SHIM,"WF_ASYNC_EVENT::Status is %d\n",h_param.status);fflush(stdout);
	Debug_Printf(DBG_SHIM,"WF_ASYNC_EVENT::readyTOreceiv is %d\n",h_param.readyToReceive);fflush(stdout);
	Debug_Printf(DBG_SHIM,"WF_ASYNC_EVENT::ackType is %d\n", h_param.ackType);fflush(stdout);
	delegate->operator () (h_param);

	if(param0.noOfDest == 0){
	Debug_Printf(DBG_SHIM,"WF_ASYNC_EVENT::Setting dest to null\n");fflush(stdout);
	h_param.dest = NULL;
	}else{
	delete[] h_param.dest;
	}*/
  delegate->operator () (param0);
  return true;
}


//Template "Explicit Instantiation"
template class WF_Event<WF_LINK_EST_EVT,WF_LinkEstimationParam<uint64_t> >;
template class WF_Event<WF_RECV_MSG_EVT,WF_RecvMsgParam<uint64_t> >;
template class WF_Event<WF_DATA_NTY_EVT,WF_DataStatusParam<uint64_t> >;
template class WF_Event<WF_CONT_RES_EVT,WF_ControlResponseParam>;
template class WF_Event<WF_SCHD_UPD_EVT,WF_ScheduleUpdateParam>;


} //End namespace

//#endif //WF_ASYNC_SIGNAL_SPECIAL_H_
