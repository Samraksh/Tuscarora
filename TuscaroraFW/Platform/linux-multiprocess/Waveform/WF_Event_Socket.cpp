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
using namespace Types;
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
	
#define MAX_EVENT_BUFFER 2048
	
char SerializedBuffer[MAX_EVENT_BUFFER];

extern bool EventInitialized;
extern EventSocket eventSocket;

template <typename Param0>
char* Serialize_WF_Event (WF_EventNameE event, Param0 param){
	//Implement serialization of parameters
	//WFEventMsg *m = (WFEventMsg *)SerializedBuffer;
	switch(event){
		case WF_GREET_EVT: {
			break;
		}
		case WF_LINK_EST_EVT: {
			WF_LinkUpdateEventMsg *msg = (WF_LinkUpdateEventMsg *)SerializedBuffer;
			msg->eventType=WF_LINK_EST_EVT;
			memcpy (&(msg->param),&param, sizeof(param));
			break;
		}
		case WF_DATA_NTY_EVT: {
			WF_DataStatusEventMsg *msg = (WF_DataStatusEventMsg *)SerializedBuffer;
			msg->eventType=WF_DATA_NTY_EVT;
			memcpy (&(msg->param),&param, sizeof(param));
			break;
		}
		case WF_SCHD_UPD_EVT: {
			WF_ScheduleUpdateEventMsg *msg = (WF_ScheduleUpdateEventMsg *)SerializedBuffer;
			msg->eventType=WF_SCHD_UPD_EVT;
			memcpy (&(msg->param),&param, sizeof(param));
			break;
		}
		default:
			break;
	};
	
  return SerializedBuffer;
}

//common contructor for all Events, if not connected to event sink, initialize socket
template <WF_EventNameE EventName, typename Param0>
WF_Event<EventName,Param0>::WF_Event (WaveformId_t wid){
	WID=wid;
	
	if(!EventInitialized){
		printf("WF_Event: Connecting to Framework...");
		EventInitialized = eventSocket.Connect(wid);
		
		if(EventInitialized){
			printf(" SUCCESS\n\n");
		}else {
			printf(" FAILED\n\n");
		}
	}
	
}

//Common invocation 
template <WF_EventNameE EventName, typename Param0>
bool WF_Event<EventName,Param0>::Invoke (Param0 param){
	char *sendBuf = Serialize_WF_Event<Param0>(EventName, param);
	bool ret = eventSocket.Invoke(sendBuf, sizeof(param));
  return ret;  
}


//specialization of invoke to convert from a waveform local parameter to parameter on the heap
template <>
bool WF_Event<WF_CONT_RES_EVT,WF_ControlResponseParam>::Invoke (WF_ControlResponseParam param){
  //WF_ControlResponseParam h_param;
  //h_param = param0;
  //h_param.data = new uint8_t[param0.dataSize];
	uint16_t totalData =  sizeof(WF_CONT_RES_EVT)+sizeof(param)+param.dataSize;
	memset(SerializedBuffer,0,totalData);
	WF_ControlResponseEventMsg *msg = (WF_ControlResponseEventMsg *)SerializedBuffer;
	WF_Attributes attributes = *((WF_Attributes*)param.data);
	msg->eventType=WF_CONT_RES_EVT;
	msg->param = param;
	Debug_Printf(DBG_SHIM, "WF_Event_Socket:: Attributes values: waveform id %u , ack timer %u \n", attributes.wfId, attributes.ackTimerPeriod);
	//memcpy(SerializedBuffer+sizeof(WF_CONT_RES_EVT),&param, sizeof(param));
	memcpy(SerializedBuffer+sizeof(WF_CONT_RES_EVT)+sizeof(param), &attributes, sizeof(attributes));
	
	Debug_Printf(DBG_SHIM, "WF_Event_Socket:: Control response from waveform %u for request id %u \n", param.wfid, param.id);
	Debug_Printf(DBG_SHIM, "WF_Event_Socket:: Parameter size %lu, data size %u \n", sizeof(param), param.dataSize);
	bool ret = eventSocket.Invoke(SerializedBuffer, totalData);
  
  return ret;
}

//specialization of invoke to convert from a waveform local parameter to parameter on the heap
template <>
bool WF_Event<WF_RECV_MSG_EVT,WF_RecvMsgParam<uint64_t> >::Invoke (WF_RecvMsgParam<uint64_t> param0){
	uint16_t payloadSize = param0.msg->GetPayloadSize();
	uint16_t totalData =  sizeof(WF_RECV_MSG_EVT)+sizeof(MessageId_t)+sizeof(WF_Message_n64_t)+payloadSize;
	memset(SerializedBuffer,0,totalData);

	WF_RecvEventMsg *msg = (WF_RecvEventMsg *)SerializedBuffer;
	msg->eventType=WF_RECV_MSG_EVT;
	msg->param.wrn = param0.wrn;
	//msg->param=param0;
	memcpy (&(msg->param.msg),param0.msg, sizeof(WF_Message_n64_t)); 
	int offset = sizeof(WF_RECV_MSG_EVT)+sizeof(MessageId_t)+sizeof(WF_Message_n64_t);
	if(payloadSize != 0){
		memcpy(SerializedBuffer+offset, param0.msg->GetPayload(), payloadSize);
	}
	
	Debug_Printf(DBG_SHIM, "WF_Event_Socket:: Sending a received msg event for msg id %d, from waveform %d \n",  param0.wrn, WID);
	bool ret = eventSocket.Invoke(SerializedBuffer, totalData);
	//Debug_Printf(DBG_CORE_DATAFLOW,"WF_ASYNC_EVENT::Address of msg is %p\n",h_param->msg);fflush(stdout);
	//Debug_Printf(DBG_CORE_DATAFLOW,"WF_ASYNC_EVENT::payload size is %d\n",h_param->msg->GetPayloadSize());
	return ret;
}

/*
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
