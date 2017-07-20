////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

/*
* Implement connection between Packet Handler and WF Controller: Masahiro Nakagawa and Vinod Kulathumani, West Virginia University
*  May 1 2015 
*/
#include "WF_Controller.h"
#include "Interfaces/Waveform/Waveform_64b_I.h"
#include "Framework/PWI/FrameworkBase.h" //to provide definition of LinkEstimatorChangeHandler
//#include "Include/Interface/Waveform/WF_Types.h"



using namespace Waveform;

namespace Waveform {
    extern Waveform_64b_I *myWFPtr[MAX_WAVEFORMS];
}

namespace Core {
namespace WFControl {  
//declare map translates request Id and request status
//RequestToStatus_t rIdMap;

BSTMapT<RequestId_t, WF_ControlResponseParam> rIdToResult;//Stores result for given requestId.
WF_ControlP_StatusE WF_Control_Status[MAX_WAVEFORMS]; //max waveform number is 16

void WF_Controller::AttributeTimer_Handler(uint32_t flag){
   //restart timer and send attribute request again
   Waveform::myWFPtr[current_wfid]->AttributesRequest (current_rId);
   attributeTimer->Start();  
}

WF_Controller::WF_Controller(FrameworkBase *_fwBase,WaveformMap_t* _wfMap, PacketHandler* _ph)
{
 Debug_Printf(DBG_CORE_WFCONTROL, "Entering WF_Controller constructor\n");fflush(stdout);
  fwBase = _fwBase;
  wfMap = _wfMap;
  ph = _ph;
  //Initialize the timer structure for waveform attribute
  attributeTimerDelegate = new TimerDelegate(this,&WF_Controller::AttributeTimer_Handler);
  attributeTimer = new Timer(20000, ONE_SHOT, *attributeTimerDelegate);
  attribute_request_busy = false;
  current_wfid = 0;
 Debug_Printf(DBG_CORE_WFCONTROL, "wfMap at %p,   ph at %p\n",wfMap, ph);
 Debug_Printf(DBG_CORE_WFCONTROL, "Exiting WF_Controller constructor\n");fflush(stdout);
 
#ifdef FW_LOWER_SHIM_SOCKET
 shim = new WF_ControllerShim(wfMap);
#endif
  
}

void WF_Controller::BroadcastData(WaveformId_t wid, WF_MessageT< uint64_t >& _msg, uint16_t _payloadSize, WF_MessageId_t  _msgId)
{
   Debug_Printf(DBG_CORE_WFCONTROL, "Entered BroadcastData\n");fflush(stdout);

   Debug_Printf(DBG_CORE_WFCONTROL, "Exiting BroadcastData\n");fflush(stdout);
}

void WF_Controller::SendData(WaveformId_t wid, WF_MessageT< uint64_t >& _msg, uint16_t _payloadSize, uint64_t* _destArray, uint16_t _noOfDest, WF_MessageId_t  _msgId, bool _noAck)
{
   Debug_Printf(DBG_CORE_WFCONTROL, "Entered SendData\n");fflush(stdout);

   Debug_Printf(DBG_CORE_WFCONTROL, "Exiting SendData\n");fflush(stdout);
}
   

//Get hold of wave form address through AlwaysOn_Dce* myWFPt[16] in Waveform namespace.  

void WF_Controller::Send_ReplacePayloadRequest (WaveformId_t wid, RequestId_t rId, WF_MessageId_t  msgId, uint8_t *payload, uint16_t payloadSize){
    reqPending.InsertBack(rId); 
    wfMap->Find(wid).Second()->ReplacePayloadRequest(rId, msgId, payload, payloadSize);
}

void WF_Controller::Send_CancelDataRequest (WaveformId_t wid, RequestId_t rId, WF_MessageId_t  msgId,  uint64_t* _destArray, uint16_t _noOfDestinations){
   Debug_Printf(DBG_CORE_WFCONTROL, "CancelDataRequest\n");fflush(stdout);
    reqPending.InsertBack(rId); 
    wfMap->Find(wid).Second()->CancelDataRequest(rId,msgId,_destArray,_noOfDestinations);
}


void WF_Controller::Send_AttributesRequest (WaveformId_t wid, RequestId_t rId){
   Debug_Printf(DBG_CORE_WFCONTROL, "Requesting Attribute of wf%d, request id %d\n",wid, rId);fflush(stdout);
    reqPending.InsertBack(rId);
    if(!attribute_request_busy){
        current_wfid = wid;
        attribute_request_busy = true;
        //Add entry in rIdMap
        //rIdMap.Insert(rId,1);
        wfMap->Find(wid).Second()->AttributesRequest (rId);
    }else{
       Debug_Printf(DBG_CORE_WFCONTROL, "WF_Controller busy\n");
    }
}

void WF_Controller::Send_ControlStatusRequest (WaveformId_t wid, RequestId_t rId){
   Debug_Printf(DBG_CORE_WFCONTROL, "Send_ControlStatusRequest\n");fflush(stdout);
    reqTowfid.Insert(rId,wid);
    reqPending.InsertBack(rId); 
    //Add entry in rIdMap
    //rIdMap.Insert(rId,1);
    wfMap->Find(wid).Second()->ControlStatusRequest (rId);
  
}

void WF_Controller::Send_AddDestinationRequest(WaveformId_t wid, RequestId_t rId, WF_MessageId_t  mId, uint64_t* destArray, uint16_t noOfDest){
   Debug_Printf(DBG_CORE_WFCONTROL, "Send_AddDestinationRequest\n");fflush(stdout);
    reqPending.InsertBack(rId);
    //Add entry in rIdMap
    //rIdMap.Insert(rId,1);
    wfMap->Find(wid).Second()->AddDestinationRequest(rId, mId, destArray,noOfDest);
}

void WF_Controller::Send_DataStatusRequest (WaveformId_t wid, RequestId_t rId, WF_MessageId_t  mId){
   Debug_Printf(DBG_CORE_WFCONTROL, "Send_SetScheduleRequest\n");fflush(stdout);
    reqPending.InsertBack(rId);
    //Add entry in rIdMap
    //rIdMap.Insert(rId,1);
    wfMap->Find(wid).Second()->DataStatusRequest (rId, mId);
}

void WF_Controller::Send_SetScheduleRequest (WaveformId_t wid, RequestId_t rId, NodeId_t nodeId, WF_ScheduleTypeE type, ScheduleBaseI &schedule, uint16_t scheduleSize) {
    reqPending.InsertBack(rId);  
   Debug_Printf(DBG_CORE_WFCONTROL, "Send_SetScheduleRequest\n");fflush(stdout);
  
}

void WF_Controller::Send_ScheduleRequest (WaveformId_t wid, RequestId_t rId, NodeId_t nodeid, WF_ScheduleTypeE type){
    reqPending.InsertBack(rId);   
   Debug_Printf(DBG_CORE_WFCONTROL, "Send_ScheduleRequest\n");fflush(stdout);
  
}
    
void WF_Controller::Recv_ControlResponse (WF_ControlResponseParam param){
    //Debug_Printf(DBG_CORE_WFCONTROL, "Entered Recv_ControlResponce\n");fflush(stdout);
    reqPending.DeleteItem(param.id); // Get Responce so remove request ID from pending list.
   Debug_Printf(DBG_CORE_WFCONTROL, "WF_Controller::Entered Recv_ControleResponce. dataptr (%p) Forward infromaton to packet Handler. \n", param.data);
    //Needs to cancel attributeTimer if this is attribute Responce.
    if(param.type == AttributeResponse){
       Debug_Printf(DBG_CORE_WFCONTROL, "WF_Controller::Recieved AttributeResponse. Cancel attributeTimer\n");fflush(stdout);
        attributeTimer->Suspend();
        attribute_request_busy =false;
    }
    ph->RecvControlResponse_From_WF(param);
 
   Debug_Printf(DBG_CORE_WFCONTROL, "Leaving function\n");
}

void WF_Controller::WF_RecvLinkEstimationEvent(WF_LinkEstimationParam< uint64_t > param)
{
  //call the framewbase here
  //convert WF_LinkEstimationParam to NeighborUpdateParam.
 /* NeighborUpdateParam _param;
  _param.changeType = param.changeType;
  _param.nodeId = param.linkAddress;
  _param.link.metrics.quality = param.metrics.quality;
  _param.link.metrics.dataRate = param.metrics.dataRate;
  _param.link.metrics.avgLatency = param.metrics.avgLatency;
  _param.link.metrics.energy = param.metrics.energy;
  _param.link.type = param.type;
  _param.link.linkId.nodeId = param.linkAddress;
  _param.link.linkId.waveformId = param.wfid;
	*/
  fwBase->LinkEstimatorChangeHandler(param);
}


void WF_Controller::Recv_NewWaveformEvent(WaveformId_t wid)
{
	fwBase->InitializeWaveform(wid);
}



} //End of namespace
}
