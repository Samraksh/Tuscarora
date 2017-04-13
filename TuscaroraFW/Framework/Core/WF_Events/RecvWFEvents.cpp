////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include "RecvWFEvents.h"

using namespace Waveform;

// extern bool DBG_WF_EVENT; // set in Lib/Misc/Debug.cpp

namespace Core {
namespace Events {

  template <typename WF_AddressType>
  RecvWFEvents<WF_AddressType>::RecvWFEvents (PacketHandler *_ph, WF_Controller *_wfControl){
    ph = _ph;
    wfControl = _wfControl;
   Debug_Printf(DBG_WF_EVENT, "Core::RecvWFEvents:: Initialized the Waveform event receiver in the framwork\n");
  }
  
  template <typename WF_AddressType>
  RecvWFEvents<WF_AddressType>::RecvWFEvents (){
    ph = 0;
    wfControl = 0;
   Debug_Printf(DBG_WF_EVENT, "Core::RecvWFEvents:: Initialized without packet handler and waveform controller\n");
  }
  
  template <typename WF_AddressType>
  void RecvWFEvents<WF_AddressType>::Recv_RecvMsgEvent(WF_RecvMsgParam<WF_AddressType> param){
    
    //Now this infromatioin goes back to PacketHandler.
    if(ph){
     Debug_Printf(DBG_WF_EVENT, "Core::RecvWFEvents:: Received Message event. Now forwarding it to PacketHandler\n");
      ph->Recv_Handler_For_WF(*param.msg, param.wrn);
    }else {
     Debug_Printf(DBG_WF_EVENT, "Core::RecvWFEvents:: Received Message event. No Action taken\n");
    }
  }
  
  template <typename WF_AddressType>
  void RecvWFEvents<WF_AddressType>::Recv_LinkEstimationEvent(WF_LinkEstimationParam<WF_AddressType>  param){
    
    if(ph){
      wfControl->WF_RecvLinkEstimationEvent(param);
     Debug_Printf(DBG_WF_EVENT, "Core::RecvWFEvents:: Link Estimation Event, Need to route this to neighbor table\n");
    }else {
     Debug_Printf(DBG_WF_EVENT, "Core::RecvWFEvents:: Link Estimation Event. No Action taken\n");
    }
  }
  
  template <typename WF_AddressType>
  void RecvWFEvents<WF_AddressType>::Recv_ScheduleUpdateEvent(WF_ScheduleUpdateParam param){
   Debug_Printf(DBG_WF_EVENT, "Core::RecvWFEvents:: Schedule update event. No action taken\n");
    
  }
  
  template <typename WF_AddressType>
  void RecvWFEvents<WF_AddressType>::Recv_DataNotificationEvent(WF_DataStatusParam<WF_AddressType> param){
   Debug_Printf(DBG_WF_EVENT, "Core::RecvWFEvents:: Data Notification received from waveform..\n");
    
   Debug_Printf(DBG_WF_EVENT, "Core::RecvWFEvents::Forwarding message to PacketHandler\n");
   Debug_Printf(DBG_WF_EVENT, "Core::RecvWFEvents::the address of param is  %p\n",&param)
    if(ph){
      ph->Recv_DN_Handler_For_WF(param);
    }else {
     Debug_Printf(DBG_WF_EVENT, "Core::RecvWFEvents:: Received Data status for message %lu from waveform %d. No Action taken\n", param.wfMsgId, param.wfId);
    }
  }
  
  template <typename WF_AddressType>
  void RecvWFEvents<WF_AddressType>::Recv_ControlResponseEvent(WF_ControlResponseParam param){
   Debug_Printf(DBG_WF_EVENT, "Core::RecvWFEvents:: Control Response Event\n");
    
    if(wfControl){
      wfControl->Recv_ControlResponse(param);
     Debug_Printf(DBG_WF_EVENT, "Core::RecvWFEvents::I am in Recv_ControlResponseEvent showing address of wfControl %p\n",wfControl);
    }else {
     Debug_Printf(DBG_WF_EVENT, "Core::RecvWFEvents:: Received Control response Event of type %d. No Action taken\n", param.type);
    }
    //Debug_Printf(DBG_WF_EVENT, "Core::RecvWFEvents:: Exiting Control Response Event\n");
  }
  
	template <typename WF_AddressType>  
	void RecvWFEvents<WF_AddressType>::Recv_NewWaveformEvent(WaveformId_t wid)
	{
		if(wfControl){
			wfControl->Recv_NewWaveformEvent(wid);
			Debug_Printf(DBG_WF_EVENT, "Core::RecvWFEvents::I am in Recv_NewWaveformEvent for %d \n", wid);
		}else {
			Debug_Printf(DBG_WF_EVENT, "Core::RecvWFEvents:: Received NewWaveform event for  %d. No Action taken\n", wid);
		}
	}

  
  template class RecvWFEvents<uint64_t>;

}//end of namespace
}
