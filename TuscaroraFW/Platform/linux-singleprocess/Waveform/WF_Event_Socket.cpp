////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



//#ifndef WF_ASYNC_EVENT_SPECIAL_H_
//#define WF_ASYNC_EVENT_SPECIAL_H_

#include <Interfaces/Waveform/WF_Types.h>
#include <Interfaces/Waveform/WF_Event.h>
//#include "Src/Framework/Core/WF_Events/RecvWFEvents.h"
#include "EventSocket.h"

using namespace PAL;
//using namespace Core::Events;
using namespace Waveform;

/*
//
// extern bool DBG_CORE_DATAFLOW; // set in Lib/Misc/Debug.cpp

namespace Core{
namespace Events {
  extern RecvWFEvents<uint64_t>* waveformEventReceiver;
}
}
*/


namespace Waveform{

char SerializedBuffer[2048];

static bool EventInitialized;
static EventSocket eventSocket;

template <typename Param0>
char* Serialize_WF_Event (WF_EventNameE event, Param0 param){
	//Implement serialization of parameters
	
  return SerializedBuffer;
}

	//common contructor for all Events, if not connected to event sink, initialize socket
template <WF_EventNameE EventName, typename Param0>
WF_Event<EventName,Param0>::WF_Event (){
	if(!EventInitialized){
		EventInitialized = eventSocket.Connect();
	}
}

//Common dummy invocation 
template <WF_EventNameE EventName, typename Param0>
bool WF_Event<EventName,Param0>::Invoke (Param0 param){
	
	char *sendBuf = Serialize_WF_Event<Param0>(EventName, param);
	bool ret = eventSocket.Invoke(sendBuf, 100);
  return ret;  
}


/*
//specialization of invoke to convert from a waveform local parameter to parameter on the heap
template <>
bool WF_Event<WF_CONT_RES_EVT,WF_ControlResponseParam>::Invoke (WF_ControlResponseParam param0){
  WF_ControlResponseParam h_param;
  h_param = param0;
  h_param.data = new uint8_t[param0.dataSize];
  memcpy(h_param.data, param0.data, param0.dataSize);
  Debug_Printf(DBG_CORE_DATAFLOW, "WF_ASYNC_EVENT:: Control response id %d \n", param0.id);
  
  return true;
}

//specialization of invoke to convert from a waveform local parameter to parameter on the heap
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
  Debug_Printf(DBG_CORE_DATAFLOW,"WF_ASYNC_EVENT::Address of msg is %p\n",h_param->msg);fflush(stdout);
  Debug_Printf(DBG_CORE_DATAFLOW,"WF_ASYNC_EVENT::payload size is %d\n",h_param->msg->GetPayloadSize());
  return true;
}

//specialization of invoke to convert from a waveform local parameter to parameter on the heap
template <>
bool WF_Event<WF_DATA_NTY_EVT,WF_DataStatusParam<uint64_t> >::Invoke (WF_DataStatusParam<uint64_t> param0){
  Debug_Printf(DBG_CORE_DATAFLOW,"\n\n\nWF_ASYNC_EVENT::Entered specialization of invoke\n");fflush(stdout);
  Debug_Printf(DBG_CORE_DATAFLOW,"WF_ASYNC_EVENT::Address of param0 is %p\n", &param0);fflush(stdout);
  Debug_Printf(DBG_CORE_DATAFLOW,"WF_ASYNC_EVENT::Address of param0.dest is %p\n",param0.destArray);fflush(stdout);
  Debug_Printf(DBG_CORE_DATAFLOW,"WF_ASYNC_EVENT::noOfDest is %d\n", param0.noOfDest);fflush(stdout);
  Debug_Printf(DBG_CORE_DATAFLOW,"WF_ASYNC_EVENT::readyToReceiv is %d\n",param0.readyToReceive);fflush(stdout);
  Debug_Printf(DBG_CORE_DATAFLOW,"WF_ASYNC_EVENT::Show dest List\n");
  for(uint16_t index =0 ; index < param0.noOfDest; index++){
   Debug_Printf(DBG_CORE_DATAFLOW, "WF_ASYNC_EVENT:: %ld   address:at %p\n",param0.destArray[index], &(param0.destArray[index]));
   Debug_Printf(DBG_CORE_DATAFLOW,"WF_ASYNC_EVENT::Status is %d\n",param0.statusValue[index]);fflush(stdout);
   Debug_Printf(DBG_CORE_DATAFLOW,"WF_ASYNC_EVENT::ackType is %d\n", param0.statusType[index]);fflush(stdout);

 }
  return true;
}
*/

//Template "Explicit Instantiation"
template class WF_Event<WF_LINK_EST_EVT,WF_LinkEstimationParam<uint64_t> >;
template class WF_Event<WF_RECV_MSG_EVT,WF_RecvMsgParam<uint64_t> >;
template class WF_Event<WF_DATA_NTY_EVT,WF_DataStatusParam<uint64_t> >;
template class WF_Event<WF_CONT_RES_EVT,WF_ControlResponseParam>;
template class WF_Event<WF_SCHD_UPD_EVT,WF_ScheduleUpdateParam>;


} //End namespace

//#endif //WF_ASYNC_SIGNAL_SPECIAL_H_
