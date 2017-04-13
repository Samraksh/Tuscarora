////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef WF_CONTROLLER_H
#define WF_CONTROLLER_H

#include <Lib/Misc/Debug.h>
#include <Interfaces/Waveform/WF_I.h>
#include "Framework/Core/DataFlow/PacketHandler.h"
#include <Lib/DS/ListT.h>

#ifdef PLATFORM_LINUX
#include "Platform/linux/Framework/WF_ControllerShim.h"
#endif

using namespace Waveform;

namespace PWI{
	class FrameworkBase;
}


namespace Core {
namespace WFControl {
  
  // No needtypedef BSTMapT<RequestId_t, int> RequestToStatus_t;
  //Converts the commands to appropriate message structure and sends them out. Receives the signals from waveforms and implements appropriate action
  class WF_Controller {
    ListT<RequestId_t,false, EqualTo<RequestId_t> > reqPending;  
    FrameworkBase *fwBase;
    WaveformMap_t *wfMap;
    PacketHandler *ph;
    //Add timer for Attributerequest.
    TimerDelegate *attributeTimerDelegate;
    Timer *attributeTimer;
    bool attribute_request_busy;
    WaveformId_t current_wfid;
    RequestId_t current_rId;
    BSTMapT<RequestId_t, WaveformId_t> reqTowfid;

#ifdef PLATFORM_LINUX
    WF_ControllerShim *shim;
#endif

  public:
    //WF_Controller(WaveformMap_t *_wfMap);
    WF_Controller(FrameworkBase *_fwBase, WaveformMap_t *_wfMap, PacketHandler *ph); //I need to call method in packethandller
    
    //DataPlane
    void SendData (WaveformId_t wid, WF_MessageT<uint64_t>& _msg, uint16_t _payloadSize, uint64_t *_destArray, uint16_t _noOfDest, MessageId_t _msgId, bool _noAck=false);
    void BroadcastData (WaveformId_t wid, WF_MessageT<uint64_t>& _msg, uint16_t _payloadSize, MessageId_t _msgId);
  
    
    //Control plane 
    void Send_ReplacePayloadRequest (WaveformId_t wid, RequestId_t rId, MessageId_t msgId, uint8_t *payload, uint16_t payloadSize);
    void Send_AttributesRequest (WaveformId_t wid, RequestId_t rId);
    void Send_ControlStatusRequest (WaveformId_t wid, RequestId_t rId);
    void Send_DataStatusRequest (WaveformId_t wid, RequestId_t rId, MessageId_t mId);
    void Send_SetScheduleRequest (WaveformId_t wid, RequestId_t rId, NodeId_t nodeId, WF_ScheduleTypeE type, ScheduleBaseI &schedule, uint16_t scheduleSize) ;
    void Send_ScheduleRequest (WaveformId_t wid, RequestId_t rId, NodeId_t nodeid, WF_ScheduleTypeE type);
    
    void Recv_ControlResponse (WF_ControlResponseParam _param);
	
	void Recv_NewWaveformEvent (WaveformId_t wid);
	
   /* template <typename WF_AddressType> 
    void WF_RecvMsgEvent(WF_RecvMsgParam<WF_AddressType> param){
      printf("Entering Recv_MessageEvent. Call method in Packethandler\n");fflush(stdout);
      ph->Recv_Handler_For_WF(*param.msg);
      //printf("Address of param.msg is %p\n", param.msg);fflush(stdout);
      //WF_Message_n64_t* msg = new WF_Message_n64_t;
      WF_MessageT<WF_AddressType>* msg = new  WF_MessageT<WF_AddressType>;
      printf("Allocated memory to copy msg recieved at %p\\n",msg);fflush(stdout);

      WF_MessageT<WF_AddressType>* msg_src = param.msg;
      memcpy(msg, msg_src, sizeof(WF_MessageT<uint64_t>));  
      printf("msg.payloadSize is %d\n",msg->GetPayloadSize());fflush(stdout);

      printf("Message type in copied message is %d, original message is %d\n",msg->type,param.msg->type);fflush(stdout);
      if(msg_src != NULL){
         delete(msg_src);
         param.msg = NULL;
      }else{
    	 printf("Should not delete Null ptr\n");
    	 abort();
      }
      ph->Recv_Handler_For_WF(*msg);
    }

    template <typename WF_AddressType> 
    void WF_RecvDataNotificationEvent(WF_DataNotifierParam<WF_AddressType> param){
      printf("Entering Recv_DataNotificationSignal. Call method in PacketHandler\n");
      printf("Address of param is %p\n", &param);fflush(stdout);
      //WF_DataNotifierParam_n64_t
      //WF_MessageStatusE Status = param.status;
      //WF_Message_n64_t* wfMsg = param.wfMsg;
      //WF_DataNotifierParam_n64_t* msg = new WF_DataNotifierParam_n64_t(param.wfMsg,param.status);
      //WF_DataNotifierParam<WF_AddressType>* msg = new WF_DataNotifierParam<WF_AddressType>(*(WF_Message_n64_t*)param.wfMsg,(WF_MessageStatusE)param.status);


      WF_DataNotifierParam<WF_AddressType>* msg = new WF_DataNotifierParam<WF_AddressType>(param.msgId,param.wfId,(WF_MessageStatusE)param.status);
      memcpy(msg, &param, sizeof(WF_DataNotifierParam_n64_t));
      //printf("msg points to address %p\n",param.wfMsg);
      if(param.noOfDest != 0){
          delete(param.dest);
          param.dest = NULL;
      }else{
     	 printf("Broadcast has not dest. Should not delete Null ptr\n");
      }
      //delete(&param);
      ph->SendAck_Handler_For_WF(*msg);
    }*/
   
    void WF_RecvLinkEstimationEvent(WF_LinkEstimationParam<uint64_t>  param);
    void AttributeTimer_Handler(uint32_t flag);

    //Masahiro adding below as it was missing from WF_Controller.
    void Send_CancelDataRequest(WaveformId_t wid, RequestId_t rId, MessageId_t msgId,  uint64_t* _destArray, uint16_t _noOfDestinations);
    void Send_AddDestinationRequest(WaveformId_t wid, RequestId_t rId, MessageId_t mId, uint64_t* destArray, uint16_t noOfDest);
  };
   
  
} //End of namespace
}



#endif //WF_CONTROLLER_H
