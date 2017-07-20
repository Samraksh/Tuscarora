////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 


#include <math.h>

#include "PacketHandler.h"
//#include "Src/Framework/Core/Estimation/Periodic.h"
//#include "Src/Framework/Core/Estimation/Scheduled.h"
//#include "Framework/Core/Estimation/ConflictAvoid.h"
#include "Framework/PWI/FrameworkBase.h"
#include "../WF_Events/RecvWFEvents.h"
#include "Framework/Core/Neighborhood/Nbr_Utils.h"
#include "Framework/Core/ControlPlane/WF_Controller.h"

#include "Platform/PlatformSelector.h"
#define SCWF(x) static_cast <Waveform::WF_Message_n64_t *>(x)
//#include <sstream> // create map key.


using namespace Core::Neighborhood;

uint16_t newcount;
uint16_t deletecount;

extern NodeId_t MY_NODE_ID;


namespace Core{
namespace Events {
RecvWFEvents<uint64_t> *waveformEventReceiver;
}
}


namespace Core {
namespace WFControl {
WF_Controller *wfControl;
//extern RequestToStatus_t rIdMap;  //map between requestId and its status
extern BSTMapT<RequestId_t, WF_ControlResponseParam> rIdToResult;
}
}

using namespace Core::Events;

namespace Core {
namespace Dataflow {
uint32_t getenv_uint32_t(const char *s){
	double x=atof(getenv(s));
	return static_cast<uint32_t>(x);
}
uint32_t x(){return getenv_uint32_t("CURX");}
uint32_t y(){return getenv_uint32_t("CURY");}


void PacketHandler::AddEntryWFMap(WaveformId_t id, WF_Attributes attr){
	Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: AddEntryWFMap:: Adding entry to wfAttributeMap. wfid %d\n",id);
	wfAttributeMap->Insert(id,attr);
}

uint32_t PacketHandler::GetRemainingPacket(WaveformId_t wfId){
	return mc->GetNumberWaveformOfMessages(wfId);
}


PacketHandler::PacketHandler(){
	newReqnumber = 0;
	frameworkMsgId=0;
}

void PacketHandler::Initialize (WaveformMap_t *_wfMap, WF_AttributeMap_t *_wfAtrribMap, AddressMap *_addressMap, PatternClientMap_t *clients,
							  FrameworkBase *_fwBase, NeighborTable& _coreNbrTable, bool* attribute_request_pending_ptr, 
							  PatternEventDispatch_I *_eventDispatcher, PiggyBacker<uint64_t> *_pb)
{
  
	//messageList = new ListT<MessageId_t,false, EqualTo<MessageId_t> >[16]; //Create one list for each waveform
	attribute_request_pending = attribute_request_pending_ptr; //need to share this information between framework
	//Set ACK_WF_RECEV_Timeout_Period
	for(int wf =0; wf < MAX_WAVEFORMS; wf++){
	   WDN_WF_RECV_Timeout_Period[wf] = 20000;
	}

	fwBase = _fwBase;
	//discoveryDelegate = _discoveryDelegate;
	wfMap = _wfMap;
	addressMap = _addressMap;
	patternClients = clients;
	coreNbrTable = &_coreNbrTable;
	wfAttributeMap = _wfAtrribMap;
	eventDispatcher = _eventDispatcher;
	piggyBacker = _pb;


	//Initialize timers...
	//This timer periodically checks if framework received WDN_WF_RECV from waveform.
	TimerDelegate *ackWFRECVTimerDelegate = new TimerDelegate(this, &PacketHandler::WDN_WF_RECV_Checker_Handler);
	Timer *ackWFRECVTimer = new Timer(ackWFRecvTimer_Ack*1000, PERIODIC, *ackWFRECVTimerDelegate);
	ackWFRECVTimer->Start();

	//This timer periodically checks if framework received WDN_WF_SENT if framework broadcast packet
	TimerDelegate *ackWFSENTTimerDelegate = new TimerDelegate(this, &PacketHandler::WDN_WF_SENT_Checker_Handler);
	Timer *ackWFSENTTimer = new Timer(ackWFSent_Ack*1000, PERIODIC, *ackWFSENTTimerDelegate);
	ackWFSENTTimer->Start();

	//This timer periodically checks if framework received WDN_DST_RECV if framework unicast packet.
	TimerDelegate *ackDSTRECVTimerDelegate = new TimerDelegate(this, &PacketHandler::WDN_DST_RECV_Checker_Handler);
	Timer *ackDSTRECVTimer  = new Timer(ackWFDSTRECV_Ack *1000, PERIODIC, *ackDSTRECVTimerDelegate);
	ackDSTRECVTimer->Start();

	//This timer periodically checks if undeleted framworkMsgMap entry exists and clear it.
	TimerDelegate *ackGarbageCollectorTimerDelegate = new TimerDelegate(this, &PacketHandler::CleanMC_Handler);
	Timer *ackGarbageCollectorTimer  = new Timer(msgCleanupTimer *1000, PERIODIC, *ackGarbageCollectorTimerDelegate);
	ackGarbageCollectorTimer->Start();


	// This random number generator is used get small delays with packet Event.

	rnd = new UniformRandomInt(300,700);

	//clear Sned_Busy flag indicates WF status
	for(int wf =0; wf < MAX_WAVEFORMS; wf++){
		Send_Busy[wf] = false;
	}
	//parentFI = static_cast <StandardFI*> (fi);
	//linkEstimationDelegate = _linkEstimationDelegate;
	//LESptr = _les;

	mc = new MessageCabinet(wfAttributeMap, &wfMsgToFMsgMap,addressMap);

	Debug_Printf(DBG_CORE_DATAFLOW, "Instantiating wfControl and RecvWFEvents here\n");
	Core::WFControl::wfControl = new Core::WFControl::WF_Controller(fwBase, _wfMap, this);
	Debug_Printf(DBG_CORE_DATAFLOW, "Just executed WF_Controller constructor\n");
	waveformEventReceiver = new RecvWFEvents<uint64_t>(this, wfControl);

}





void PacketHandler::RecvControlResponse_From_WF(WF_ControlResponseParam param){

	//printf("PacketHandler::RecvControlResponse_From_WF: Received a Control Response from some Waveform \n"); fflush(stdout);
	Debug_Printf(DBG_CORE_DATAFLOW, "Received a Control Response from some Waveform in param %p \n", &param);
	if(param.type == AttributeResponse){
		//printf("PacketHandler::RecvControlResponse_From_WF: Received a Attribute Response from some Waveform \n"); fflush(stdout);
		WF_Attributes* data = (WF_Attributes*) param.data; //moved it to outside of if statement 
		Debug_Printf(DBG_CORE_DATAFLOW, "Setting Waveform Attributes for wfid %d\n",data->wfId);fflush(stdout);

		attribute_request_pending[data->wfId] = false; // no pending request.

		Debug_Printf(DBG_CORE_DATAFLOW, "Clearing attribute_request_Pending for wf %d\n",data->wfId);fflush(stdout);
		//copy WF_Attributes. param is in heap so need to delete. Before deleting, copy WF_Attributes. 
		WF_AttributeMap_t ::Iterator it_wf = wfAttributeMap->Find(data->wfId);
		WF_Attributes* attribute;
		if(it_wf != wfAttributeMap->End()){
			Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::wslimtribute for wfid %d already exits\n",data->wfId);
			attribute = &(wfAttributeMap->operator[](data->wfId));
			memcpy(attribute, data, sizeof(WF_Attributes));
			Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: Updated wf attribute of wfid %d\n",data->wfId); fflush(stdout);
		}
		else {
			newcount++;
			attribute = new WF_Attributes;  
			memcpy(attribute, data, sizeof(WF_Attributes));
			AddEntryWFMap(attribute->wfId,*attribute);
		}

		switch(data->type){
		case ALWAYS_ON_LOCAL_WF:
			Debug_Printf( DBG_CORE_DATAFLOW,"ALWAYS_ON_LOCAL_WF\n");
			break;
		case TDMA_LOCAL_WF:
			Debug_Printf( DBG_CORE_DATAFLOW,"TDMA_LOCAL_WF\n");
			break;
		case LONG_LINK_WF:
			Debug_Printf( DBG_CORE_DATAFLOW,"LONG_LINK_WF\n");
			break;
		default:
			Debug_Printf( DBG_CORE_DATAFLOW,"Unknown Waveform Type!\n");
		}

		fflush(stdout);

		switch(data->estType){
		case WF_FULL_EST: Debug_Printf( DBG_CORE_DATAFLOW,"WF_FULL_EST\n"); break;
		case WF_NO_EST:
			//printf("XXXXXXXXXXXXXXLook HereXXXXXXXXXXXX\n");
			if(data->wfId != LONG_LINK_WID)
				fwBase->CreateLinkEstimator(data->wfId);
			Debug_Printf( DBG_CORE_DATAFLOW,"WF_NO_EST\n");
			break;
		default: Debug_Printf( DBG_CORE_DATAFLOW,"Unknown estimation Type!\n");
		}

		fflush(stdout);
		switch(data->antType){
		case OMNI_ANT:
			Debug_Printf( DBG_CORE_DATAFLOW,"OMNI_ANT\n"); break;
		case DIRECTIONAL_ANT:
			Debug_Printf( DBG_CORE_DATAFLOW,"DIRECTIONAL_ANT\n"); break;
		default:
			Debug_Printf( DBG_CORE_DATAFLOW,"Unknown ant Type!\n");
		}
		fflush(stdout);
		switch(data->mode){
		case WF_MODE_NORMAL:
			Debug_Printf( DBG_CORE_DATAFLOW,"WF_MODE_NORMAL\n"); break;
		case WF_MODE_AJ:
			Debug_Printf( DBG_CORE_DATAFLOW,"WF_MODE_AJ\n"); break;
		case WF_MODE_LPD:
			Debug_Printf( DBG_CORE_DATAFLOW,"WF_MODE_LPD\n"); break;
		case WF_MODE_LOWPOWER:
			Debug_Printf( DBG_CORE_DATAFLOW,"WF_MODE_LOWPOWER\n"); break;
		case WF_MODE_RADIO_SILENCE:
			Debug_Printf( DBG_CORE_DATAFLOW,"WF_MODE_RADIO_SILENCE\n"); break;
		default:
			Debug_Printf( DBG_CORE_DATAFLOW,"Unknown mode!\n");
		}

		if(data->broadcastSupport){
			Debug_Printf( DBG_CORE_DATAFLOW,"Boradcast is supported \n");
		}else{
			Debug_Printf( DBG_CORE_DATAFLOW,"Broadcast is NOT supported\n");
		}



		if(data->destReceiveAckSupport){
			Debug_Printf( DBG_CORE_DATAFLOW,"destReceiveAck is supported  Threshold is %d\n",attribute->ackTimerPeriod);
			WDN_DST_RECV_Timeout_Period[data->wfId] = attribute->ackTimerPeriod;//40000;
			WDN_WF_SENT_Timeout_Period[data->wfId] = attribute->ackTimerPeriod;//40000;
		}else{
			Debug_Printf( DBG_CORE_DATAFLOW,"destReceiveAck is NOT supported. Threshold is %d\n",attribute->ackTimerPeriod);
			WDN_DST_RECV_Timeout_Period[data->wfId] = attribute->ackTimerPeriod;//100000;
			WDN_WF_SENT_Timeout_Period[data->wfId] = attribute->ackTimerPeriod;//100000;
		}
		fflush(stdout);
		if(data->scheduleInfoSupport){
			Debug_Printf( DBG_CORE_DATAFLOW,"scheduleInfo is supported\n");
		}else{
			Debug_Printf( DBG_CORE_DATAFLOW,"ScheduleInfo is NOT supported\n");
		}
		fflush(stdout);
		Debug_Printf( DBG_CORE_DATAFLOW,"ifindex is %d\n",data->ifindex);
		Debug_Printf( DBG_CORE_DATAFLOW,"name is %s\n",data->name);
		Debug_Printf( DBG_CORE_DATAFLOW,"Header size is %d\n",data->headerSize);
		Debug_Printf( DBG_CORE_DATAFLOW,"Max Payload Size is %d\n",data->maxPayloadSize);
		Debug_Printf( DBG_CORE_DATAFLOW,"Max Packet Size is %d\n",data->maxPacketSize);
		Debug_Printf( DBG_CORE_DATAFLOW,"min Packet Size is %d\n",data->minPacketSize);
		Debug_Printf( DBG_CORE_DATAFLOW,"min inter packet delay is %d\n",data->minInterPacketDelay);
		Debug_Printf( DBG_CORE_DATAFLOW,"Channel rate is %d\n",data->channelRate);
		Debug_Printf( DBG_CORE_DATAFLOW,"Maximum burst rate is %d\n",data->maxBurstRate);
		fflush(stdout);

		if(param.data != NULL){
			Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: Setting param->data to null\n");
		}
		//delete(&param);
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: deleted param\n");
		fflush(stdout);
	}
	else if(param.type == CancelPacketResponse){

		WF_CancelDataResponse_Param* cancel_data = (WF_CancelDataResponse_Param*) param.data;

		//Create control responce to pattern

		CancelDataResponse_Data data;
		data.noOfDest = cancel_data->noOfDest;
		data.status = true;
		ControlResponseParam rparam;
		rparam.type = PTN_CancelDataResponse;
		rparam.data = &data;
		PatternId_t patternId;
		// Find the fragment list,
		// If Success,
		// 		go through the fragmentlist and remove destination from all of them

		// If not success,
		//		pass negative result up


		auto wfMsgToFMsgMap_itt = wfMsgToFMsgMap.Find(cancel_data->msgId);
		if(wfMsgToFMsgMap_itt != wfMsgToFMsgMap.End()){
			FragmentList* fragmentlist_ptr = wfMsgToFMsgMap_itt->Second();
			FMsgMap::Iterator it_fmsg = mc->framworkMsgMap->Find(fragmentlist_ptr->frameworkMsgID);
			if(it_fmsg != mc->framworkMsgMap->End()){
				FMessageQElement* fmsg_ptr = it_fmsg->Second();
				data.msgId = fmsg_ptr->messageId;
				data.noOfDest = 0;
				auto pid_itr = msgIdToPidMap.Find(fmsg_ptr->messageId);
				if(pid_itr == msgIdToPidMap.End()){
					patternId = 0;
					Debug_Error("PacketHandler::RecvControlResponse_From_WF CancelPacketResponse  ERROR! Cannot find pattern ID  !\n");
					return;
				}
				else{
					patternId = pid_itr->Second();
				}

				for(uint8_t index = 0; index < cancel_data->noOfDest; ++index){
					LinkId link = addressMap->LookUpLinkId(NbrUtils::ConvertU64ToEthAddress(cancel_data->destArray[index]));
					data.destArray[index] = link.nodeId;


					if(cancel_data->cancel_status[index]){ //If cancelling was successful
						bool all_fragment_dest_removal_success = true;
						for(auto fragmentmap_itt = fragmentlist_ptr->fragmentmap.RBegin(); fragmentmap_itt != fragmentlist_ptr->fragmentmap.REnd(); ++fragmentmap_itt){
							FragmentDeliveryStatusMap_t* del_status_map = &(fragmentmap_itt->Second());
							if((fragmentmap_itt->Second())[cancel_data->destArray[index]] <= FRAG_WF_RECV){
								bool erase_res = del_status_map->Erase(cancel_data->destArray[index]);
								if(erase_res){
									Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::RecvControlResponse_From_WF CancelPacketResponse  Remove the destination %d from wfmsgid = %d !\n", cancel_data->destArray[index], fragmentmap_itt->First());
								}
								else{
									all_fragment_dest_removal_success = false;
									Debug_Error("PacketHandler::RecvControlResponse_From_WF CancelPacketResponse  ERROR! Cannot remove the destination %d from wfmsgid = %d !\n", cancel_data->destArray[index], fragmentmap_itt->First());
								}
							}
							else{
								all_fragment_dest_removal_success = false;
								Debug_Error("PacketHandler::RecvControlResponse_From_WF CancelPacketResponse  ERROR! Another fragment was already sent for dest = %llu with fragment status = %d wfmsgid = %d \n", cancel_data->destArray[index], (fragmentmap_itt->Second())[cancel_data->destArray[index]] , fragmentmap_itt->First() );
							}
						}

						if(all_fragment_dest_removal_success){
							data.cancel_status[data.noOfDest] = true;
						}
						else{
							data.cancel_status[data.noOfDest] = false;
							data.status = false;
							Debug_Error("PacketHandler::RecvControlResponse_From_WF CancelPacketResponse  ERROR! Not all  destinations have been removed!! \n" );
						}

					}
					else{//If cancelling was unsuccessful --> pass negative result up
						data.cancel_status[data.noOfDest] = false;
						data.status = false;
					}
					++data.noOfDest;

				}
			}
			else{
				Debug_Error("PacketHandler::RecvControlResponse_From_WF CancelPacketResponse  ERROR! Cannot find the FrameworkMessageQElement\n");
				return;
			}
		}
		else{
			Debug_Error("MessageCabinet::RecvControlResponse_From_WF CancelPacketResponse ERROR! Cannot find the fragmentation list");
			return;
		}




		//Create control responce to pattern
		eventDispatcher->InvokeControlEvent(patternId, rparam);

	}
	else if(param.type == ReplacePayloadResponse){
		WF_ReplacePayloadResponse_Param* replace_data = (WF_ReplacePayloadResponse_Param*)param.data;

		ReplacePayloadResponse_Data data;
		data.noOfDest = replace_data->noOfDest;
		data.status = true;
		ControlResponseParam rparam;
		rparam.type = PTN_ReplacePayloadResponse;
		rparam.data = &data;
		PatternId_t patternId;



		auto wfMsgToFMsgMap_itt = wfMsgToFMsgMap.Find(replace_data->msgId);
		if(wfMsgToFMsgMap_itt != wfMsgToFMsgMap.End()){
			FragmentList* fragmentlist_ptr = wfMsgToFMsgMap_itt->Second();
			FMsgMap::Iterator it_fmsg = mc->framworkMsgMap->Find(fragmentlist_ptr->frameworkMsgID);
			if(it_fmsg != mc->framworkMsgMap->End()) {
				FMessageQElement* fmsg_ptr = it_fmsg->Second();
				if(fmsg_ptr->n_pending_replace_payload_requests > 0) { //If there was a request
					data.msgId = fmsg_ptr->messageId;
					data.noOfDest = 0;
					auto pid_itr = msgIdToPidMap.Find(fmsg_ptr->messageId);
					if(pid_itr == msgIdToPidMap.End()){
						patternId = 0;
						Debug_Error("PacketHandler::RecvControlResponse_From_WF ReplacePayloadResponse  ERROR! Cannot find pattern ID  !\n");
						return;
					}
					else{
						patternId = pid_itr->Second();
					}

					for(uint8_t index = 0; index < replace_data->noOfDest; ++index) {
						LinkId link = addressMap->LookUpLinkId(NbrUtils::ConvertU64ToEthAddress(replace_data->destArray[index]));
						data.destArray[index] = link.nodeId;


						if(replace_data->replace_status[index]) { //If replacing was successful
							if(fmsg_ptr->replace_payload_ptr){
								data.replace_status[data.noOfDest] = true;
								mc->Replace_Ptr(fmsg_ptr,fmsg_ptr->replace_payload_ptr, fmsg_ptr->replace_payload_ptr_size, param.wfid);
							}
							else{
								Debug_Error("PacketHandler::RecvControlResponse_From_WF ReplacePayloadResponse  ERROR! The replace_payload_ptr is NULL \n" );
							}
						}
						else{//If cancelling was unsuccessful --> pass negative result up
							data.replace_status[data.noOfDest] = false;
							data.status = false;
						}
						++data.noOfDest;
					}

					fmsg_ptr->n_pending_replace_payload_requests--;
					if(fmsg_ptr->n_pending_replace_payload_requests == 0){
						fmsg_ptr->replace_payload_ptr = NULL;
						fmsg_ptr->replace_payload_ptr_size = 0;
					}
				}
				else{
					Debug_Error("PacketHandler::RecvControlResponse_From_WF ReplacePayloadResponse  ERROR! No outstanding request to change the payload!! !\n");
				}



			}
			else{
				Debug_Error("PacketHandler::RecvControlResponse_From_WF ReplacePayloadResponse  ERROR! Cannot find the FrameworkMessageQElement\n");
				return;
			}
		}
		else{
			Debug_Error("MessageCabinet::RecvControlResponse_From_WF ReplacePayloadResponse ERROR! Cannot find the fragmentation list");
			return;
		}

		eventDispatcher->InvokeControlEvent(patternId, rparam);

//		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: Got ReplacePayloadResponse from wf %d\n",param.wfid);
//		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: Show address of param %p and data %p\n", &param, param.data);
//		//Notify pattern,
//		WF_ReplacePayloadResponse_Param* replace_data = (WF_ReplacePayloadResponse_Param*)param.data;
//		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Retrieve PatternId for this messageId %d\n",replace_data->msgId);
//		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Just in case, check msgId exists in msgIdToPidMap\n");
//		BSTMapT <FMessageId_t , PatternId_t> ::Iterator it_msg = msgIdToPidMap.Find(replace_data->msgId);
//		if(it_msg == msgIdToPidMap.End()){
//			Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::MessageId %d not found.\n",replace_data->msgId);
//			return;
//		}
//		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Show number of node in this reply %d\n",replace_data->noOfDest);
//		//convert WF_ReplacePayloadResponse_param to ReplacePayloadResponse_param
//		ReplacePayloadResponse_Data data;
//		data.msgId = replace_data->msgId;
//		data.noOfDest = replace_data->noOfDest;
//		data.status = replace_data->status;
//		for(uint16_t index =0; index < replace_data->noOfDest; index++){
//			Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Node %d :: Status %d\n",replace_data->destArray[index], replace_data->replace_status[index]);
//			data.replace_status[index] = replace_data->replace_status[index];
//			data.destArray[index] = replace_data->destArray[index];
//		}
//
//		ControlResponseParam rparam;
//		rparam.data = &data;
//		rparam.type = PTN_ReplacePayloadResponse;
//		PatternId_t patternId = msgIdToPidMap[replace_data->msgId];
//		eventDispatcher->InvokeControlEvent(patternId, rparam);




	}

	//Mukundan: DataStatusResponse will not be a control response
	/*else if(param.type == DataStatusResponse){
    WF_DataStatusParam<uint64_t>* rv = (WF_DataStatusParam<uint64_t>*)(param.data);
    //if(param.status){
    ///WF_DataStatusParam<uint64_t>* rv = (WF_DataStatusParam<uint64_t>*)(param.data);
    Debug_Printf( DBG_CORE_DATAFLOW, "PacketHandler::Address of rv is %p\n",rv);
    switch(rv->ackType){
      case WDN_WF_RECV:    Debug_Printf( DBG_CORE_DATAFLOW,"Message status is WDN_WF_RECV\n");break;
      case WDN_WF_SENT:    Debug_Printf( DBG_CORE_DATAFLOW,"Message status is WDN_WF_SENT\n");break;
      case WDN_DST_RECV:   Debug_Printf( DBG_CORE_DATAFLOW,"Message status is WDN_DST_RECV\n");break;
      //case READY_TO_RECV:  Debug_Printf( DBG_CORE_DATAFLOW,"Message status is READY_TO_RECV\n");break;
      default:             Debug_Printf( DBG_CORE_DATAFLOW, "Unknown status\n");
    }
  }*/
	else if(param.type == AddDestinationResponse){
		WF_AddDestinationResponse_Param* add_dest = (WF_AddDestinationResponse_Param*) param.data;



		// Find the fragment list,
		// Find the fragment that this wfmsg corresponds to
		// Change fragment status to FRAG_WF_RECV for this dest
		//


		// If it is not success
		// Find the fragment list
		// Remove destination from all the fragments.
		//
		auto wfMsgToFMsgMap_itt = wfMsgToFMsgMap.Find(add_dest->msgId);
		if(wfMsgToFMsgMap_itt != wfMsgToFMsgMap.End()){
			FragmentList* fragmentlist_ptr = wfMsgToFMsgMap_itt->Second();

			FMsgMap::Iterator it_fmsg = mc->framworkMsgMap->Find(fragmentlist_ptr->frameworkMsgID);
			if(it_fmsg != mc->framworkMsgMap->End()){
				FMessageQElement* fmsg_ptr = it_fmsg->Second();

				for(uint8_t index = 0; index < add_dest->noOfDest; ++index){
					if(add_dest->add_status[index]){ //If adding was successful
						for(auto fragmentmap_itt = fragmentlist_ptr->fragmentmap.RBegin(); fragmentmap_itt != fragmentlist_ptr->fragmentmap.REnd(); ++fragmentmap_itt){
							if(fragmentmap_itt->First() == add_dest->msgId){ // If this is the FragmentDeliveryStatusMap_t corresponding to the entry
								if((fragmentmap_itt->Second())[add_dest->destArray[index]] == FRAG_FRAG_CREATED){
									(fragmentmap_itt->Second())[add_dest->destArray[index]] = FRAG_WF_RECV;
								}
								else{
									Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::RecvControlResponse_From_WF ERROR UnexpectedStatus fragmentmap[%d][%d]  = %d \n",fragmentmap_itt->First(), add_dest->destArray[index],(fragmentmap_itt->Second())[add_dest->destArray[index]]  );
								}
							}//Otherwise make sure there are elements for other fragments
							else{
								FragmentDeliveryStatusMap_t* fragment_del_status_map_ptr = &(fragmentmap_itt->Second());
								if(fragment_del_status_map_ptr->Find(add_dest->destArray[index]) == fragment_del_status_map_ptr->End()){
									fragment_del_status_map_ptr->Insert(add_dest->destArray[index],FRAG_FRAG_CREATED);
								}
							}
						}
					}
					else{//If adding was unsuccessful
						for(auto fragmentmap_itt = fragmentlist_ptr->fragmentmap.RBegin(); fragmentmap_itt != fragmentlist_ptr->fragmentmap.REnd(); ++fragmentmap_itt){
							if( fragmentmap_itt->Second().Erase(add_dest->destArray[index])){
								Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::RecvControlResponse_From_WF Erased destination from fragment list since WF rejected to add the destination fragmentmap[%d][%d] \n",fragmentmap_itt->First(), add_dest->destArray[index]  );
							}
							else{
								Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::RecvControlResponse_From_WF ERROR Cannot erase destination from fragment list fragmentmap[%d][%d] \n",fragmentmap_itt->First(), add_dest->destArray[index]  );
							}
						}

						bool hoge; //just need this param to call function but it has no meaning
						mc->Remove_Node(fmsg_ptr,1,add_dest->destArray[index], param. wfid, add_dest->msgId, hoge);

					}

				}

				AddDestinationResponse_Data data;
				data.msgId = fmsg_ptr->messageId;
				data.noOfDest = add_dest->noOfDest;
				data.status = add_dest->status;
				for(uint16_t index=0; index < add_dest->noOfDest; index++){
					LinkId link = addressMap->LookUpLinkId(NbrUtils::ConvertU64ToEthAddress(add_dest->destArray[index]));
					data.destArray[index] = link.nodeId;
					data.add_status[index] = add_dest->add_status[index];
				}
				ControlResponseParam rparam;
				rparam.data = &data;
				rparam.type = PTN_AddDestinationResponse;
				PatternId_t patternId = msgIdToPidMap[fmsg_ptr->messageId];
				eventDispatcher->InvokeControlEvent(patternId, rparam);
			} else{
				Debug_Error("PacketHandler::Add_Node ERROR! Cannot find the FrameworkMessageQElement\n");
			}
		} else{
			Debug_Error("MessageCabinet::Add_Node ERROR! Cannot find the fragmentation list");
		}

	}
	else if(param.type == ControlStatusResponse){
		WF_ControlP_StatusE* rv = (WF_ControlP_StatusE*)param.data; //moved it to outside of if
		//if(param.status){
		//WF_ControlP_StatusE* rv = (WF_ControlP_StatusE*)param.data;
		switch(*rv){
		case WF_NORMAL:
			Debug_Printf( DBG_CORE_DATAFLOW, "PackeetHandler:: State is WF_NORMAL\n"); break;
		case WF_BUSY:
			Debug_Printf( DBG_CORE_DATAFLOW, "PackeetHandler:: State is WF_BUSY\n"); break;
		case WF_ERROR:
			Debug_Printf( DBG_CORE_DATAFLOW, "PackeetHandler:: State is WF_ERROR\n"); break;
		case WF_BUFFER_LOW:
			Debug_Printf( DBG_CORE_DATAFLOW, "PackeetHandler:: State is WF_BUFFER_LOW\n");
			Debug_Printf(DBG_CORE_DATAFLOW,"PackeetHandler:: Clearing Send_Busy[%d] \n",param.wfid);
			Send_Busy[param.wfid] = false;
			ProcessOutgoingMessages(param.wfid);
			break;
		case WF_BUFFER_FULL:
			Debug_Printf( DBG_CORE_DATAFLOW, "PackeetHandler:: State is WF_BUFFER_FULL\n"); break;
		}
	}

	Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Send_Control_ACK::Deleting controll responce param at %p\n",&param);
	//printf("PacketHandler::Existing control response handler\n");
	deletecount++;
}


void PacketHandler::Recv_DN_Handler_For_WF(WF_DataStatusParam_n64_t& ack){

	WaveformId_t wfid = ack.wfId;
	WF_MessageId_t wfMsgId = ack.wfMsgId;
	WFMsgIdToFragmentListMap_t::Iterator iter = wfMsgToFMsgMap.Find(wfMsgId);
	//printf("PacketHandler::Recv_DN_Handler_For_WF : Got Ack for WF Message Id : %lu on Waveform %u \n",wfMsgId, wfid);
	Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Recv_DN_Handler_For_WF : Got Ack for WF Message Id : %lu on Waveform %u \n",wfMsgId, wfid);
	if(iter == wfMsgToFMsgMap.End()){
		Debug_Warning("PacketHandler::Recv_DN_Handler_For_WF: Got Ack for WF Message Id : %lu on Waveform %u , which is not in wfMstToFmsgMap. This is potentially a fragmented packet \n",wfMsgId, wfid);
		return;
	}
	FMessageId_t fwMsgId = iter->Second()->frameworkMsgID;

	Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Recv_DN_Handler_For_WF Show current Send_Busy[%d]: %d\n", ack.wfId, Send_Busy[ack.wfId]);
	Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Recv_DN_Handler_For_WF Show readyToRecv is %d\n",ack.readyToReceive);
	Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Recv_DN_Handler_For_WF PacketHandler got acknowledgment from waveform\n");fflush(stdout);

	bool buffer_full=false;
	bool pktToobig=false;

	for (uint16_t i=0; i< ack.noOfDest; i++){
		buffer_full |= (ack.statusValue[i]==Waveform::WF_ST_BUFFER_FULL);
		pktToobig |= (ack.statusValue[i]==Waveform::WF_ST_PKT_TOO_BIG);
	}

	FMsgMap::Iterator it_fmsg = mc->framworkMsgMap->Find(fwMsgId);
	if(it_fmsg == mc->framworkMsgMap->End()){
		Debug_Printf(DBG_CORE_DATAFLOW, "msgId not in frameworkMsgMap!\n");
	}else{
		if(!buffer_full && (it_fmsg->Second()->msg->GetType() == Types::ACK_REPLY_MSG)){
			Debug_Printf(DBG_CORE_DATAFLOW, "this is software acknowledgement.\n");
			Debug_Printf(DBG_CORE_DATAFLOW, "As the message was accepted by WF, remove data\n");
			mc->framworkMsgMap->Erase(it_fmsg);
			BSTMapT <FMessageId_t , WF_MessageQElement*> ::Iterator it_qElem = msgToQElement[ack.wfId].Find(fwMsgId);
			msgToQElement[ack.wfId].Erase(it_qElem);
		}
	}

	if(buffer_full){
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Recv_DN_Handler_For_WF: Previous message sending failed\n");
		Debug_Printf(DBG_CORE_DATAFLOW, "Waveform was busy\n");
		//Delete corresponding infromation
		ackTimer_flag[ack.wfId] = false;
		//get hold of frameworkMsg
		FMsgMap::Iterator it_fmsg = mc->framworkMsgMap->Find(fwMsgId);
		if(it_fmsg == mc->framworkMsgMap->End()){
			Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: message id %d does not have entry in framworkMsgMap\n",fwMsgId);
		}else{
			//Debug_Printf(DBG_CORE_DATAFLOW, "MessageType is %d\n",it_fmsg->Second()->msg->GetType());
			//if packet is link estimation or discovery, drop it.
			if( (it_fmsg->Second()->msg->GetType() == Types::ESTIMATION_MSG) || (it_fmsg->Second()->msg->GetType() == Types::DISCOVERY_MSG)){
				Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Dropping packet as waveform could not accept it\n");
			}else{
				Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Put packet back to MC\n");
				//get hold of WF_MessageQElement to be put back to MC
				WF_MessageQElement* element = msgToQElement[ack.wfId][fwMsgId];
				//put it back to MC
				Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::show msg address via msgToQElement %p",element->msg);
				mc->AddToMC(ack.wfId,element);
				//get it back
				WF_MessageQElement *qElement = mc->GetNextMessage(ack.wfId);
				Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Show wElement address fetched:%p  stored:%p\n",qElement,element);fflush(stdout);
				//show what I got
				Debug_Printf(DBG_CORE_DATAFLOW, "it_fmsg->Second()->msg is %p\n",it_fmsg->Second()->msg);
				//Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::msgId %d, qElem at %p, msg at %p, msg type is %d\n",qElement->msg->GetFrameworkMessageID(),
				//		qElement, qElement->msg, qElement->msg->GetType());
				//put it back again
				mc->AddToMC(ack.wfId,qElement);
				BSTMapT <FMessageId_t , WF_MessageQElement*> ::Iterator it_qElem = msgToQElement[ack.wfId].Find(fwMsgId);
				msgToQElement[ack.wfId].Erase(it_qElem);
			}
		}
	}else if(pktToobig){
		//TODO::Check the attributes and check check if you are setting the framework attribute correctly.
		//drop packet and clear packet info from MC
		BSTMapT <FMessageId_t , PatternId_t> ::Iterator it_msg = msgIdToPidMap.Find(fwMsgId);
		if(it_msg == msgIdToPidMap.End())
		{
			Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Recv_DN_Handler_For_WF: MessageId %d has no entry in msgIdToPidMap\n",fwMsgId);
		}else{
			Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Recv_DN_Handler_For_WF: Drop packet: MsgId %d, send from patternId %d\n",fwMsgId,msgIdToPidMap[fwMsgId]);
		}
		//update framwrokMsgMap entry
		FMsgMap::Iterator it_fmsg = mc->framworkMsgMap->Find(fwMsgId);
		if(it_fmsg == mc->framworkMsgMap->End()){
			Debug_Error( "PacketHandler::Recv_DN_Handler_For_WF: FMessageQElement for msgId %d not found\n",fwMsgId);
		}
		BSTMapT <FMessageId_t , WF_MessageQElement*> ::Iterator it_qElem = msgToQElement[ack.wfId].Find(fwMsgId);
		if(it_qElem == msgToQElement[ack.wfId].End()){
			Debug_Error("PacketHandler::Recv_DN_Handler_For_WF:  msgId %d not found in msgToQElement[%d]",fwMsgId,ack.wfId);
		}
		it_qElem->Second()->noOfDest;
		it_fmsg->Second()->dnFromDest_remaining -= it_qElem->Second()->noOfDest;
		it_fmsg->Second()->dnSentByWF_remaining -= it_qElem->Second()->noOfDest;
		it_fmsg->Second()->dnRecvByWF_remaining -= it_qElem->Second()->noOfDest;
		ClearMap(ack.wfId,fwMsgId);
		if((it_fmsg->Second()->dnRecvByWF_remaining == 0) &&
				(it_fmsg->Second()->dnSentByWF_remaining == 0) &&
				(it_fmsg->Second()->dnFromDest_remaining == 0))
		{
			FMessageQElement* fmsg_ptr = it_fmsg->Second();
			mc->framworkMsgMap->Erase(it_fmsg);
			delete(fmsg_ptr);
			fmsg_ptr = NULL;
			deletecount++;
		}
		//Send Framework Level Attribute again. Something might have changed.....
		//it is nice if I could call framework API but....
		wfControl->Send_AttributesRequest (ack.wfId,GetNewReqId());
	}
	else {
//		ProcessWFAck(ack);
		ProcessWFAckWithFragmentation(ack);
	}
}  

void PacketHandler::ProcessWFAckWithFragmentation(WF_DataStatusParam_n64_t& ack){

	bool invokeHighLevelAck = false;
	WaveformId_t _wfid = ack.wfId;
	WF_MessageId_t _wfMsgId = ack.wfMsgId;
	WFMsgIdToFragmentListMap_t::Iterator iter = wfMsgToFMsgMap.Find(_wfMsgId);
	Debug_Printf(DBG_CORE_DATAFLOW, "ProcessWFAckWithFragmentation::ProcessWFAck: Got Ack for WF Message Id : %lu on Waveform %u \n",_wfMsgId, _wfid); //,ack.wfMsg->GetFrameworkMessageID());
	if(iter == wfMsgToFMsgMap.End()){
		Debug_Printf(DBG_CORE_DATAFLOW, "ProcessWFAckWithFragmentation::ProcessWFAck: Got Ack for WF Message Id : %lu on Waveform %u , which is not in wfMstToFmsgMap \n",_wfMsgId, _wfid);
		return;
	}

	FragmentList* fragmentlist_ptr =  iter->Second();
	auto fragstatiter = fragmentlist_ptr->fragmentmap.Find(_wfMsgId);
	if(fragstatiter != fragmentlist_ptr->fragmentmap.End()){
		uint16_t index = 0;
		do
		{
			if((ack.statusValue[index]==WF_ST_SUCCESS)){
				switch(ack.statusType[index]){
				case WDN_WF_RECV:
					if(fragstatiter->Second()[ack.destArray[index]] == FRAG_FRAG_CREATED){
						fragstatiter->Second()[ack.destArray[index]] = FRAG_WF_RECV;
					}
					else{
						Debug_Printf(DBG_CORE_DATAFLOW, "ProcessWFAckWithFragmentation:: ERROR! UNEXPECTED ACK STATUS! Recorded FragmentStatusTypeE = %d , ack.statusType[index] = %d",fragstatiter->Second(), ack.statusType[index] );
					}
					break;
				case WDN_WF_SENT:
					if(fragstatiter->Second()[ack.destArray[index]] == FRAG_WF_RECV){
						fragstatiter->Second()[ack.destArray[index]] = FRAG_WF_SENT;
					}
					else{
						Debug_Printf(DBG_CORE_DATAFLOW, "ProcessWFAckWithFragmentation:: ERROR! UNEXPECTED ACK STATUS! Recorded FragmentStatusTypeE = %d , ack.statusType[index] = %d",fragstatiter->Second(), ack.statusType[index] );
					}
					break;
				case WDN_DST_RECV: //BK: This is generated by the upper layer after the packet is defragmented on the rx side. Hence the tx deduce all the segments were successful.
					//					if(fragstatiter->Second()[ack.destArray[index]] == FRAG_WF_SENT){
					//						fragstatiter->Second()[ack.destArray[index]] = FRAG_DST_RECV;
					//					}
					for(auto it = fragmentlist_ptr->fragmentmap.Begin(); it != fragmentlist_ptr->fragmentmap.End(); ++it){
						if(it->Second()[ack.destArray[index]] == FRAG_WF_SENT){
							it->Second()[ack.destArray[index]] = FRAG_DST_RECV;
						}
						else{
							Debug_Printf(DBG_CORE_DATAFLOW, "ProcessWFAckWithFragmentation:: ERROR! UNEXPECTED ACK STATUS! Recorded FragmentStatusTypeE = %d , ack.statusType[index] = %d",fragstatiter->Second(), ack.statusType[index] );
						}
					}
					break;
				default:
					Debug_Printf(DBG_CORE_DATAFLOW, "ProcessWFAckWithFragmentation::Unknown type!\n");
					break;
				}
			}
			else{

			}
			index++;
		}while(index < ack.noOfDest);

	}
	else{
		Debug_Printf(DBG_CORE_DATAFLOW, "ProcessWFAckWithFragmentation:: ERROR. MSGID DOES NOT EXIST IN THE FRAGMENTATION STATUS LIST! \n");
	}

	//Decide whether or not to invoke a higher level ACK
	WF_DataStatusParam_n64_t combinedACKObj(ack.wfMsgId, ack.wfId);
//	WF_DataStatusParam_n64_t* combinedACK = new WF_DataStatusParam_n64_t(ack.wfMsgId, ack.wfId);
	WF_DataStatusParam_n64_t* combinedACK = &combinedACKObj;

	combinedACK->readyToReceive = ack.readyToReceive;
	combinedACK->noOfDest = 0;
	combinedACK->readyToReceive = ack.readyToReceive;

	uint16_t index = 0;
	uint16_t indexdext = 0;
	do
	{
		if((ack.statusValue[index]==WF_ST_SUCCESS)){
			bool isAll_FRAG_WF_RECV = true;
			for(auto fragmentmapiter = fragmentlist_ptr->fragmentmap.Begin(); fragmentmapiter != fragmentlist_ptr->fragmentmap.End(); ++fragmentmapiter ){
				if(fragmentmapiter->Second()[ack.destArray[index]] < FRAG_WF_RECV){
					isAll_FRAG_WF_RECV = false; break;
				}
			}
			if(isAll_FRAG_WF_RECV) {
				combinedACK->statusType[indexdext] = WDN_WF_RECV;

			}


			bool isAll_FRAG_WF_SENT = true;
			for(auto fragmentmapiter = fragmentlist_ptr->fragmentmap.Begin(); fragmentmapiter != fragmentlist_ptr->fragmentmap.End(); ++fragmentmapiter ){
				if(fragmentmapiter->Second()[ack.destArray[index]] < FRAG_WF_SENT){
					isAll_FRAG_WF_SENT = false; break;
				}
			}
			if(isAll_FRAG_WF_SENT) combinedACK->statusType[indexdext] = WDN_WF_SENT;

			bool isAll_FRAG_DST_RECV = true;
			for(auto fragmentmapiter = fragmentlist_ptr->fragmentmap.Begin(); fragmentmapiter != fragmentlist_ptr->fragmentmap.End(); ++fragmentmapiter ){
				if(fragmentmapiter->Second()[ack.destArray[index]] < FRAG_DST_RECV){
					isAll_FRAG_DST_RECV = false; break;
				}
			}
			if(isAll_FRAG_DST_RECV) combinedACK->statusType[indexdext] = WDN_DST_RECV;

			if(isAll_FRAG_WF_RECV || isAll_FRAG_WF_SENT || isAll_FRAG_DST_RECV) {
				combinedACK->statusValue[indexdext] = WF_ST_SUCCESS;
				combinedACK->destArray[indexdext] = ack.destArray[index];


				auto it = fragmentlist_ptr->lastSentCombinedStatusMap.Find(ack.destArray[index]);
				if(it == fragmentlist_ptr->lastSentCombinedStatusMap.End() ){
					fragmentlist_ptr->lastSentCombinedStatusMap.Insert(combinedACK->destArray[indexdext],combinedACK->statusType[indexdext]);
					invokeHighLevelAck = true;
				}
				else if(it->Second() != combinedACK->statusType[indexdext]){
					it->Second() = combinedACK->statusType[indexdext];
					invokeHighLevelAck = true;
				}


				indexdext++;
				combinedACK->noOfDest++;
			}
		}
		else{ //TODO: BK: Implement behaviour for unseccessful notifications

		}
		index++;
	}while(index < ack.noOfDest);

	if(invokeHighLevelAck){
		ProcessWFAck(*combinedACK);
	}
	else{
		//delete combinedACK;
	}


}


void PacketHandler::ProcessWFAck(WF_DataStatusParam_n64_t& ack)
{
	WaveformId_t _wfid = ack.wfId;
	WF_MessageId_t _wfMsgId = ack.wfMsgId;
	WFMsgIdToFragmentListMap_t::Iterator iter = wfMsgToFMsgMap.Find(_wfMsgId);
	Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::ProcessWFAck: Got Ack for WF Message Id : %lu on Waveform %u \n",_wfMsgId, _wfid); //,ack.wfMsg->GetFrameworkMessageID());
	if(iter == wfMsgToFMsgMap.End()){
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::ProcessWFAck: Got Ack for WF Message Id : %lu on Waveform %u , which is not in wfMstToFmsgMap \n",_wfMsgId, _wfid); 
		return;
	}
	FMessageId_t fwMsgId = iter->Second()->frameworkMsgID;

	BSTMapT <FMessageId_t , WF_MessageQElement*>:: Iterator it = msgToQElement[_wfid].Find(fwMsgId);
	if(it == msgToQElement[_wfid].End()){ //packet does not exist in wf heap
		Debug_Printf(DBG_CORE_DATAFLOW,"PacketHandler::messageId %u was not found on msgToQElement[%d]\n",fwMsgId,_wfid);
		///if(ack.wfMsg->GetType() == Ack_Reply_Type){
		//if(ack.type == Ack_Reply_Type){
		//if(it->Second()->msg->GetType() == Ack_Reply_Type){
		//check if framewrokMsg entry for this messagea is still there.
		FMsgMap::Iterator it_fmsg = mc->framworkMsgMap->Find(fwMsgId);
		if(it_fmsg != mc->framworkMsgMap->End()){
			Debug_Printf(DBG_CORE_DATAFLOW, "Show message type %d via framworkMsgMap\n",it_fmsg->Second()->msg->GetType());
			WF_MessageQElement dummyelement(NULL, NULL, _wfMsgId, fwMsgId, NULL, 0, false  );
			WF_MessageQElement* dummyelementptr = &dummyelement;
			auto src_res_ptr = it_fmsg->Second()->wfQElement[_wfid].Search(dummyelementptr);

			if(src_res_ptr != NULL && src_res_ptr->GetData()->msg->GetType() == Types::ACK_REPLY_MSG){
				//			if(it_fmsg->Second()->wfQElement[_wfid]->msg->GetType() == Types::ACK_REPLY_MSG){
				Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: ProcessWFAck:This packet is Framework Acknowledgment.\n");
			}
			else
			{
				Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::ProcessWFAck: Illegal. MessageID not found in msgToQElement map.\n");
				Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::ProcessWFAck:MessageId never exists or waveform took too long to reply\n");
				//check WF status
				if(ack.readyToReceive == true){
					if(Send_Busy[_wfid] == true){
						Debug_Printf(DBG_CORE_DATAFLOW,"PacketHandler::ProcessWFAck: clearing Send_Busy[%d]\n",_wfid);
						Send_Busy[_wfid] = false; //WF is not busy
					}else{
						//Debug_Printf(DBG_CORE_DATAFLOW, "Send_Busy [%d] is not set. No need to clear it\n",ack.wfMsg->waveformId);
					}
					Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::ProcessWFAck: WF can accept  more message\n");
					Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::ProcessWFAck: Now Framework is allowed to fetch new message from waveform[%d]\n", _wfid);
					//u int64_t _delay = rnd->GetNext();
					//wfTimer[ack.wfMsg->waveformId]->Change(_delay, ONE_SHOT);
					ProcessOutgoingMessages(_wfid);
				}
			}
		}
		else{
			Debug_Printf(DBG_CORE_DATAFLOW, "messageId %d was not found in framworkMsgMap. It took too long to receive acknowledgement from NS-3\n",fwMsgId);
			//check WF status
			if(ack.readyToReceive == true){
				if(Send_Busy[_wfid] == true){
					Debug_Printf(DBG_CORE_DATAFLOW,"PacketHandler::clearing Send_Busy[%d]\n",_wfid);
					Send_Busy[_wfid] = false; //WF is not busy
				}else{
					//Debug_Printf(DBG_CORE_DATAFLOW, "Send_Busy [%d] is not set. No need to clear it\n",ack.wfMsg->waveformId);
				}
				Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::WF can accept  more message\n");
				Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Now Framework is allowed to fetch new message from waveform[%d]\n", _wfid);
				//u int64_t _delay = rnd->GetNext();
				//wfTimer[ack.wfMsg->waveformId]->Change(_delay, ONE_SHOT);
				ProcessOutgoingMessages(_wfid);
			}
		}
		return; //do not process this ack, its not for a regular message
	}

	//Check msg for which patternInstance it is meant for and call the appropriate delegate
	Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: Now check message Type(%d  :ptr %p) and take appropriate action\n",it->Second()->msg->GetType(), it->Second());
	FMsgMap::Iterator it_fmsg_test = mc->framworkMsgMap->Find(fwMsgId);
	if(it_fmsg_test == mc->framworkMsgMap->End()) {
		//Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: FMessageQElement not found/n");
	}else{
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: Get message type %d via FMessageQElement.\n",it_fmsg_test->Second()->msg->GetType());
	}


	/*debugging********************************************
  BSTMapT<NodeId_t, bool>::Iterator it_fuga = mc->framworkMsgMap->operator [](fwMsgId)->dnFromDest.Begin();
    	  while(it_fuga != mc->framworkMsgMap->operator [](fwMsgId)->dnFromDest.End()){
    		  Debug_Printf(DBG_CORE_DATAFLOW, "1 PacketHandler:: associated pair %d : %d \n",it_fuga->First(), it_fuga->Second());
    		  Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: Show address of dnFromDest %p \n",&mc->framworkMsgMap->operator [](fwMsgId)->dnFromDest);
    	      ++it_fuga;
    	  }
	 ************************************************************************/
	switch(it->Second()->msg->GetType()){ 
	case ESTIMATION_MSG:
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Got acknowledgment for LE_Type\n");
		Process_WF_DataStatus(ack);
		break;
	case DISCOVERY_MSG:
		break;
		//case TimeSync_Type:
		//break;
	case PATTERN_MSG:
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: Processing a DN for Pattern message sent out\n");
		Process_WF_DataStatus(ack);
		break;
		//case Data_Type:
		//break;
	case ACK_REQUEST_MSG: //Masahiro adding it 5/25/2015.
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: Got Ack_Request_Type\n");
		Process_WF_DataStatus(ack);
		break;
	default:
		Debug_Printf(DBG_CORE_DATAFLOW,"PacketHandler::Recv_DN_Handler_For_WF:: Received a Ack Unknown Message Type : %d\n", it->Second()->msg->GetType());//ack.wfMsg->GetType());
		break;
	} 
	Debug_Printf(DBG_CORE_DATAFLOW,"PacketHandler::MessageId %d exists\n",fwMsgId);
	//Check current wfStatus
	if(ack.readyToReceive == true){
		if(Send_Busy[_wfid] == true){
			Debug_Printf(DBG_CORE_DATAFLOW,"PacketHandler::clearing Send_Busy[%d]\n",_wfid);
			Send_Busy[_wfid] = false; //WF is not busy
		}else{
			Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Send_Busy [%d] is not set. No need to clear it\n",_wfid);
			Send_Busy[_wfid] = false;
		}
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::WF can accept 1 more message\n");
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Now Framework is allowed to fetch new message from waveform[%d]\n", _wfid);
		ProcessOutgoingMessages(_wfid);
	}else{
		//Wf notified FW not to send any more.
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Waveform %d cannot accept anymore data. Stop Fetching\n",_wfid);
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Setting Send_Busy[%d] to true\n",_wfid);
		Send_Busy[_wfid] = true;
	}
}


void PacketHandler::Process_WF_DataStatus(WF_DataStatusParam_n64_t& ack)
{
	WaveformId_t _wfid = ack.wfId;
	WF_MessageId_t _wfMsgId = ack.wfMsgId;
	WFMsgIdToFragmentListMap_t::Iterator iter = wfMsgToFMsgMap.Find(_wfMsgId);
	//Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::ProcessWFAck: Got Ack for WF Message Id : %lu on Waveform %u \n",_wfMsgId, _wfid);
	//printf("PacketHandler::Process_WF_DataStatus: Got Ack for WF Message Id : %lu on Waveform %u \n",_wfMsgId, _wfid);
	if(iter == wfMsgToFMsgMap.End()){
		Debug_Warning("PacketHandler::Process_WF_DataStatus: Got Ack for WF Message Id : %lu on Waveform %u , which is not in wfMsgIdToFmsgIdMap \n",_wfMsgId, _wfid); 
		return;
	}
	FMessageId_t fwMsgId = iter->Second()->frameworkMsgID;

	//update this if new info is available for pattern
	bool shouldSendDataStatusToPattern=false;

	//Debugging masahiro show all destination contained in ack
	for(uint16_t index =0; index < ack.noOfDest ; index++){
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Process_WF_DataStatus ack.dest[%d] is %ld  :ptr %p\n",index, ack.destArray[index], &ack.destArray[index]);
	}


	FMsgMap::Iterator it_fmsg = mc->framworkMsgMap->Begin();
	if(it_fmsg == mc->framworkMsgMap->End()){
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Process_WF_DataStatus: framworkMsgMap has no messages \n");
	}
	while(it_fmsg !=  mc->framworkMsgMap->End()){
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Process_WF_DataStatus message id %d is in framworkMsgMap\n",it_fmsg->First());
		++it_fmsg;
	}
	it_fmsg = mc->framworkMsgMap->Find(fwMsgId);
	if(it_fmsg == mc->framworkMsgMap->End()){
		Debug_Error("PacketHandler::Process_WF_DataStatus Ack recieved for messageId %d but it does not exist in framworkMsgMap.\n",fwMsgId);
	}

	/*debugging********************************************
	BSTMapT<NodeId_t, bool>::Iterator it_fuga = mc->framworkMsgMap->operator [](ack.msgId)->dnFromDest.Begin();
			while(it_fuga != mc->framworkMsgMap->operator [](ack.msgId)->dnFromDest.End()){
				Debug_Printf(DBG_CORE_DATAFLOW, "2 PacketHandler:: associated pair %d : %d \n",it_fuga->First(), it_fuga->Second());
				Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: Show address of dnFromDest %p \n",&mc->framworkMsgMap->operator [](ack.msgId)->dnFromDest);
				++it_fuga;
			}
	 ************************************************************************/

	Debug_Printf(DBG_CORE_DATAFLOW,"PacketHandler::Process_WF_DataStatus Finding the qelement for framework message id %d\n",it_fmsg->First()); fflush(stdout);
	//Debug_Printf(DBG_CORE_DATAFLOW,"PacketHandler:: Message Type is %d\n",mc->framworkMsgMap->operator [](fwMsgId)->msg->GetType()); fflush(stdout);
	Debug_Printf(DBG_CORE_DATAFLOW,"PacketHandler::Process_WF_DataStatus Message(ptr: %p) Type is %d\n",it_fmsg->Second()->msg,it_fmsg->Second()->msg->GetType()); fflush(stdout);

	//For Debugging, try cheking msgToQElement
	BSTMapT <FMessageId_t , WF_MessageQElement* > ::Iterator it_qElem_test = msgToQElement[ack.wfId].Find(fwMsgId);
	if(it_qElem_test != msgToQElement[ack.wfId].End()){
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: refer to msgToQEleme Message(ptr:%p Type is %d\n",it_qElem_test->Second()->msg,it_qElem_test->Second()->msg->GetType());
	}else{
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: this WF_MessageElement is already removed from map!\n");
		return ;
	}
	WF_MessageQElement* qElement = it_qElem_test->Second();
	//Retreave wfid from ack
	WaveformId_t wfid = ack.wfId;
	//if this message is Type 1, do not retreave pid, just set 0
	PatternId_t pid;

	if(it_qElem_test->Second()->msg->GetType() == Types::ESTIMATION_MSG){
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: This message is LE_Type. Do not use msgIdToPidMap\n");
		pid = 0;
	}else{
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: Make sure msgIdToPidMap has corresponding entry\n");
		BSTMapT <FMessageId_t  ,PatternId_t> ::Iterator it_msg = msgIdToPidMap.Find(fwMsgId);
		if(it_msg == msgIdToPidMap.End()){
			Debug_Error("PacketHandler::Process_WF_DataStatus:Something wrong? msgId not found in msgIdToPidMap\n");
		}
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: Use msgIdToPidMap to retrieve pid corresponding msgId %d\n",fwMsgId);
		pid = msgIdToPidMap[fwMsgId];
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: Pid %d is associated to messageId %d\n",pid,fwMsgId);
	}
	Debug_Printf(DBG_CORE_DATAFLOW,"PacketHandler::Process_WF_DataStatus::PatternId for msgId %d is %d\n",fwMsgId, pid);


	//get hold of message Element before we erase entry from framworkMsgMap.
	//FMessageQElement* mElement = mc->LookUpFrameworkMessage(fwMsgId);
	// Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Message (ptr: %p) type for this ack is %d \n",mElement->msg, mElement->msg->GetType());
	//PatternId_t pid;

	FMsgMap::Iterator ii= mc->framworkMsgMap->Find(fwMsgId);
	if(ii==mc->framworkMsgMap->End()){
		Debug_Error("PacketHandler::Process_WF_DataStatus: something seriously wrong %d is not in MC, skipping\n",ii->First());
	}
	FMessageQElement* mElement = ii->Second();
	Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Message (ptr: %p) type for this ack is %d \n",mElement->msg, mElement->msg->GetType());
	Debug_Printf(DBG_CORE_DATAFLOW,"PacketHandler:: Message(ptr: %p) Type is %d\n",it_fmsg->Second()->msg, it_fmsg->Second()->msg->GetType()); fflush(stdout);
	Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::payload %p , payload %p\n",mElement->msg->GetPayload(), it_fmsg->Second()->msg->GetPayload());
	bool IsBroadcast = mElement->broadcast;
	Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: mElement->broadcast is %d\n",IsBroadcast);


	//below code only handles unicast case. Broadcast case is missing so I am changing for loop to do while
	//for(int index = 0 ; index < ack.noOfDest; index++){
	uint16_t index = 0;
	do
	{
		LinkId link = addressMap->LookUpLinkId(NbrUtils::ConvertU64ToEthAddress(ack.destArray[index]));
		switch(ack.statusType[index]){
		case WDN_WF_RECV: 

			//printf("PacketHandler::Got WDN_WF_RECV... node id %ld\n",ack.destArray[index]);
			Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Got WDN_WF_RECV... node id %ld\n",link.nodeId);
			Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: dest %ld is updated to %d\n",link.nodeId,PDN_WF_RECV);
			if(!mElement->Check_Item_Exists_dnRecvByWF(link.nodeId)){
				mElement->dnRecvByWF[link.nodeId] = (ack.statusValue[index]==WF_ST_SUCCESS);
				mElement->dnRecvByWF_remaining--;
				shouldSendDataStatusToPattern=true;
				Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: dnRecvByWF_remaining: %d \n",mElement->dnRecvByWF_remaining);
				if((mElement->dnRecvByWF_remaining==0 && qElement->msg->GetType() == Types::PATTERN_MSG) ||
						(mElement->dnRecvByWF_remaining==0 && qElement->msg->GetType() == Types::ACK_REQUEST_MSG))
				{
					(*patternClients)[pid]->noOfOutstandingPkts--;
					//printf("PacketHandler::Process_WF_DataStatus: Decrementing noOfOutstandingPkt to %d \n",(*patternClients)[pid]->noOfOutstandingPkts);
					//Debug_Printf(DBG_CORE_DATAFLOW, "Decrementing noOfOutstandingPkt to %d \n",(*patternClients)[pid]->noOfOutstandingPkts);
					//}
					//I do not care about framework acknowledgement packet as it will not increment outstanding paket
					(*patternClients)[pid]->readyToReceive = true;
					Debug_Printf(DBG_CORE_DATAFLOW, "setting readyToReceive to true\n");
				}
			}

			if(msgToQElement[wfid][fwMsgId]->noOfDest == 0){
				Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::This is broadcast\n");
				mElement->dnRecvByWF_remaining=0;
				Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: dnRecvByWF_remaining: %d\n",mElement->dnRecvByWF_remaining);
			}

			/* moving this to above if statement as this should be executed only once
			if((mElement->dnRecvByWF_remaining==0 && qElement->msg->GetType() == Types::Pattern_Type) ||
			(mElement->dnRecvByWF_remaining==0 && qElement->msg->GetType() == Types::Ack_Request_Type)){
			(*patternClients)[pid]->noOfOutstandingPkts -=1;
			Debug_Printf(DBG_CORE_DATAFLOW, "Decrementing noOfOutstandingPkt to %d \n",(*patternClients)[pid]->noOfOutstandingPkts);
			//}
			//I do not care about framework acknowledgement packet as it will not increment outstanding paket
			(*patternClients)[pid]->readyToReceive = true;
			Debug_Printf(DBG_CORE_DATAFLOW, "setting readyToReceive to true\n");
			}
			 */

			Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Updating Waiting_ACK time\n");
			gettimeofday(&(msgToQElement[wfid][fwMsgId]->Waiting_ACK),NULL); 

			break;
		case WDN_WF_SENT:  
			Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::WDN_WF_SENT\n");      
			Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Ptr: %p, index %d  dest %ld is updated to %d\n", &link.nodeId, index,link.nodeId,PDN_WF_SENT);
			//increment number of pkt sent

			(*patternClients)[pid]->accounts.AddPacket(mElement->msg->GetPayloadSize(),ack.noOfDest);

			if(!mElement->Check_Item_Exists_dnSentByWF(ack.destArray [index])){
				mElement->dnSentByWF[link.nodeId] = (ack.statusValue[index]==WF_ST_SUCCESS);
				mElement->dnSentByWF_remaining--;
				shouldSendDataStatusToPattern=true;
			}    
			Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: dnSentByWF_remaining: %d\n",mElement->dnSentByWF_remaining);

			if(msgToQElement[wfid][fwMsgId]->noOfDest == 0)
			{
				Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::This is broadcast\n");
				Debug_Printf(DBG_CORE_DATAFLOW, "Set dnSentByWF_remaining to 0 & Erase entry from wfid %d",wfid);
				mElement->dnSentByWF_remaining=0;

				//find entry
				BSTMapT< FMessageId_t  , WF_MessageQElement* > ::Iterator it_qElem = msgToQElement[wfid].Find(fwMsgId);
				if(it_qElem == msgToQElement[wfid].End()){
					Debug_Printf(DBG_CORE_DATAFLOW, "msgToQElement[%d][%d] does not exits\n",wfid,fwMsgId);
				}else{
					Debug_Printf(DBG_CORE_DATAFLOW, "msgToQElement[%d][%d] found. Deleting it.....\n",wfid,fwMsgId);
					WF_MessageQElement* qElement = it_qElem->Second();
					msgToQElement[wfid].Erase(it_qElem);
					//Debug_Printf(DBG_CORE_DATAFLOW, "Deleting entry  9 (%d, %d) from msgIdToPidMap\n",fwMsgId,msgIdToPidMap[fwMsgId]);
					//pid = msgIdToPidMap[fwMsgId];
					/*
					Debug_Printf(DBG_CORE_DATAFLOW, "Before Erasing entry from msgIdToPidMap, make sure the entry exist for this msgId\n");
					BSTMapT <FMessageId_t , PatternId_t> ::Iterator it_msg = msgIdToPidMap.Find(fwMsgId);
					if(it_msg == msgIdToPidMap.End()){
						Debug_Printf(DBG_CORE_DATAFLOW, "Corresponding pid does not exist. Maybe this is not Pattern message\n");
					}else{
						msgIdToPidMap.Erase(fwMsgId);
					}
					 */
					delete(qElement);
				}
			}
			break;

		case WDN_DST_RECV:{
			Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: Case WDN_DST_RECV\n"); 
			Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Update acknowledgment status\n");fflush(stdout);

			BSTMapT<NodeId_t, bool>::Iterator it_status = mc->framworkMsgMap->operator [](fwMsgId)->dnFromDest.Find(link.nodeId);

			///////////////////////////////////////////////////////////////////////////////////////////////
			/*
			Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: Show size of dnFromDest map %d\n",mc->framworkMsgMap->operator [](fwMsgId)->dnFromDest.Size());
			BSTMapT<NodeId_t, bool>::Iterator it_fuga = mc->framworkMsgMap->operator [](fwMsgId)->dnFromDest.Begin();
			while(it_fuga != mc->framworkMsgMap->operator [](fwMsgId)->dnFromDest.End()){
				Debug_Printf(DBG_CORE_DATAFLOW, "3 PacketHandler:: associated pair %d : %d \n",it_fuga->First(), it_fuga->Second());
				Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: Show address of dnFromDest %p \n",&mc->framworkMsgMap->operator [](ack.msgId)->dnFromDest);
				++it_fuga;
			}
			 */
			/////////////////////////////////////////////////////////////////////////////////////////////

			if(!mElement->Check_Item_Exists_dnFromDest(link.nodeId)){
				Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: 2 dest %ld is updated to %d\n",msgToQElement[wfid][fwMsgId]->destArray[index],PDN_DST_RECV);
				mElement->dnFromDest[link.nodeId] = (ack.statusValue[index]==WF_ST_SUCCESS);
				mElement->dnFromDest_remaining--;
				shouldSendDataStatusToPattern=true;
				Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: dnFromDest_remaining: %d\n",mElement->dnFromDest_remaining);
			}
			//Before accessing msgTOQElement, make sure there is entry fo this
			BSTMapT <FMessageId_t , WF_MessageQElement *> ::Iterator it_qElem_test = msgToQElement[wfid].Find(fwMsgId);
			if(it_qElem_test == msgToQElement[wfid].End()){
				Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: MessageId %d does not exist in msgToQElement map\n",fwMsgId);
			}else{
				if(msgToQElement[wfid][fwMsgId]->noOfDest == 0){
					Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::This is broadcast\n");
					mElement->dnFromDest_remaining=0;
				}
			}

			//broadcast will never ever get this acknowledgment type.
			break;
		}
		default: Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Unknown type!\n");
		}
		index++;
	}while(index < ack.noOfDest);

	Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: About to send DataStatus to Pattern. Sanity check: Show mElement %p\n",mElement);
	if(mElement && shouldSendDataStatusToPattern){
		PrepareAndSendDataStatusResponse(mElement->messageId);
	}
}

/*
void PacketHandler::ProcessLinkEstimationMsg (WF_MessageBase& msg){
  //WF_Message_n64_t *rcvmsg = (WF_Message_n64_t *)&msg;
  Debug_Printf(DBG_CORE,"FrameworkBase::ProcessLinkEstimationMsg:: msg.GetWaveform %d\n", msg.GetWaveform()); fflush(stdout);
  LinkEstimatorI *li = fwBase->LES.estimatorHash[msg.GetWaveform()];
  if(li != 0){
    //Debug_Printf(DBG_CORE_ESTIMATION,"FrameworkBase:: ProcessLinkEstimationMsg:: Address of link estimator is %p\n", li);fflush(stdout);
    li->OnPacketReceive(&msg);
  }else {
    Debug_Printf(DBG_CORE,"FrameworkBase::ProcessLinkEstimationMsg:: Error, link estimation reference is not set for waveform %d\n", msg.GetWaveform()); fflush(stdout);
  }
  Debug_Printf(DBG_CORE_DATAFLOW,"FrrameworkBase::ProcessLinkEstimationMsg:: Deleting WF_MessageBase obj at %p\n",&msg);
}
 */



bool PacketHandler::AddOrUpdateWFAddress(WaveformId_t  wid, uint64_t wfAddress, NodeId_t srcNodeId)
{
	// Handle wf address to nodeId conversion
	//char addr[18];
	//NbrUtils::ConvertU64ToEthAddress(wfAddress,addr);
	string wfAddrStr = NbrUtils::ConvertU64ToEthAddress(wfAddress);
	LinkId link = addressMap->LookUpLinkId(wfAddrStr);
	if(link.waveformId == NULL_WF_ID || link.nodeId == NULL_NODE_ID) {
		if (srcNodeId== NULL_NODE_ID) {
			//Address not set
			//Lets generate a node id:
			addressMap->GenerateNodeId(wfAddrStr, wid);
		}else 
		{
			addressMap->AddNodeID(wfAddrStr, srcNodeId, wid);
			NodeId_t tempid=addressMap->GenerateNodeId(wfAddrStr, wid);
			Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler: AddOrUpdateWFAddress: Temp Id: %u, Src Node Id: %u, wf 64Id: %lu, wf address: %s \n", tempid, srcNodeId, wfAddress, wfAddrStr.c_str());
		}
		return false;
	}
	//wf address is already in table.
	return true;
}

void PacketHandler::ProcessPiggyBackedData(WF_MessageBase& _wfMsg){
#if ENABLE_PIGGYBACKING==1 && ENABLE_EXPLICIT_SENDER_TIMESTAMPING==1
	WaveformId_t wfId = _wfMsg.GetWaveform();
	uint16_t piggyPayloadSize =_wfMsg.GetPiggyPayloadSize();

	if(piggyBacker->GetNumberOfRegisteredPiggyBackees(wfId) > 0){
		uint8_t expectedPayloadSize = piggyBacker->GetPiggyPaylaodSize(wfId);
		if( piggyPayloadSize >= expectedPayloadSize){
			PiggyBackeeI<uint64_t>* pbPtr = static_cast<PiggyBackeeI<uint64_t>*>(piggyBacker->GetPiggyBackeePtr(wfId));
			//uint8_t *piggyPayload= _wfMsg.GetPiggyPaylaod();
			pbPtr->ReceiveData (_wfMsg.GetSrcNodeID(),_wfMsg.GetPiggyPayload(), piggyPayloadSize, _wfMsg.GetRecvTimestamp());
		}
	}else {
		Debug_Error("Something is wrong, I am getting piggybacked data while I dont have any registered piggybackees");
	}
#elif ENABLE_IMPLICIT_SYNC_TIMESTAMPING==1
	WaveformId_t wfId = _wfMsg.GetWaveform();
	//uint16_t piggyPayloadSize =_wfMsg.GetPiggyPayloadSize();

	if(piggyBacker->GetNumberOfRegisteredPiggyBackees(wfId) > 0){
		//uint8_t expectedPayloadSize = piggyBacker->GetPiggyPaylaodSize(wfId);

		PiggyBackeeI<uint64_t>* pbPtr = static_cast<PiggyBackeeI<uint64_t>*>(piggyBacker->GetPiggyBackeePtr(wfId));
		//uint8_t *piggyPayload= _wfMsg.GetPiggyPaylaod();
		pbPtr->ReceiveData (_wfMsg.GetSrcNodeID(),0, 0, _wfMsg.GetRecvTimestamp());

	}else {
		Debug_Error("Something is wrong, I am getting piggybacked data while I dont have any registered piggybackees");
	}

#endif //end piggbacking

}

void PacketHandler::Recv_Handler_For_WF_MSG_FRAGMENT(WF_MessageBase& msg, FMessageId_t  wrn){
	//Deserialize the incoming FragmentationHeader
	FragmentedMessageQueueElement* qelement;
	FragmentationHeader* frag_header_ptr =  reinterpret_cast<FragmentationHeader*>(msg.GetPayload());
	uint8_t* header_stripped_payload = msg.GetPayload() + sizeof(FragmentationHeader);
	uint16_t header_stripped_payload_size =  msg.GetPayloadSize() - sizeof(FragmentationHeader);

	uint16_t previously_embedded_payload_size;

	//Search for the message fragment in the queue
	auto it = incoming_fragmented_msg_queue.Find(msg.GetSrcNodeID());
	if(it == incoming_fragmented_msg_queue.End() ){
		//Create the correspoding message
		qelement = &(incoming_fragmented_msg_queue[msg.GetSrcNodeID()]);
		qelement->assembledMsg= new WF_Message_n64_t(frag_header_ptr->total_payload_size);

		qelement->assembledMsg->SetWaveformMessageID(msg.GetWaveformMessageID());
		qelement->assembledMsg->SetInstance(msg.GetInstance());
		qelement->assembledMsg->flags = msg.flags;
		qelement->assembledMsg->nodeID = msg.nodeID; //qelement->assembledMsg->SetSrcNodeID(msg.GetSrcNodeID());
		qelement->assembledMsg->waveformId = msg.waveformId;
		qelement->assembledMsg->type = msg.type;
		qelement->assembledMsg->metadata = msg.metadata;

		reinterpret_cast<WF_Message_n64_t*>(qelement->assembledMsg)->SetSource(reinterpret_cast<WF_Message_n64_t*>(&msg)->GetSource());
		reinterpret_cast<WF_Message_n64_t*>(qelement->assembledMsg)->SetNumberOfDest(reinterpret_cast<WF_Message_n64_t*>(&msg)->GetNumberOfDest());


	}
	else{
		//Push received messages into a queue
		qelement = &(it->Second());
	}

	//WF_Message_n64_t* wmsg = static_cast<WF_Message_n64_t*> msg;

	uint16_t maxPayloadSize = wfAttributeMap->Find(msg.GetWaveform())->Second().maxPayloadSize - sizeof(FragmentationHeader);
	uint16_t numberOfMsgSegmentsRequired = frag_header_ptr->total_payload_size / maxPayloadSize;
	if(frag_header_ptr->total_payload_size  % maxPayloadSize > 0 ) ++numberOfMsgSegmentsRequired;


	bool isTrueFragment = true;

	if(frag_header_ptr->total_payload_size != qelement->assembledMsg->GetPayloadSize()){
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Recv_Handler_For_WF_MSG_FRAGMENT Total size does not match the total FMessageSize");
		isTrueFragment = false;
	}
	else if(msg.GetSrcNodeID() != qelement->assembledMsg->GetSrcNodeID()){
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Recv_Handler_For_WF_MSG_FRAGMENT ID does not match");
		isTrueFragment = false;
	}
	else if(frag_header_ptr->frag_number >= numberOfMsgSegmentsRequired){
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Recv_Handler_For_WF_MSG_FRAGMENT Segment number is out of bounds");
		isTrueFragment = false;
	}
	else if( (qelement->received_segment_numbers.Search(frag_header_ptr->frag_number)) != NULL ){
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Recv_Handler_For_WF_MSG_FRAGMENT Already received this segment");
		isTrueFragment = false;
	}
	//	qelement->assembledMsg-
	//
	else{
		previously_embedded_payload_size = (frag_header_ptr->frag_number)*maxPayloadSize;
		if( previously_embedded_payload_size + header_stripped_payload_size > frag_header_ptr->total_payload_size){
			Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Recv_Handler_For_WF_MSG_FRAGMENT Exceeding total size with the current msg");
			isTrueFragment = false;
		}
		else{
			isTrueFragment = true;
		}
	}

	if (isTrueFragment){
		//Copy the segment
		memcpy(qelement->assembledMsg->GetPayload()+previously_embedded_payload_size, header_stripped_payload,  msg.GetPayloadSize() - sizeof(FragmentationHeader));
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Recv_Handler_For_WF_MSG_FRAGMENT CopyIthSegmentTo succeed!");
		// Mark the incoming segment as received
		qelement->received_segment_numbers.Insert(frag_header_ptr->frag_number);
		//Delete the msg
		WF_MessageBase* msgbase_ptr = &msg;
		delete msgbase_ptr;

		// Check if all the segments have completed and if so pop the message and process it
		if (qelement->received_segment_numbers.Size() == numberOfMsgSegmentsRequired){
			if(qelement != &(it->Second())){
				it = incoming_fragmented_msg_queue.Find(msg.GetSrcNodeID());
			}
			//			delete(qelement->received_segment_numbers);
			WF_Message_n64_t* assembledMsg = reinterpret_cast<WF_Message_n64_t*>(qelement->assembledMsg);
			incoming_fragmented_msg_queue.Erase(it);
			assembledMsg->ClearFlag(WF_FLG_FRAGMENTED);
			Recv_Handler_For_WF(*assembledMsg, wrn);
			delete assembledMsg;
		}

	}
	else{//TODO: Implement a non-matching policy such as dropping the current segments and start over

	}




}

void PacketHandler::Recv_Handler_For_WF(WF_MessageBase& _wfMsg, FMessageId_t  wrn){

	//FMessageId_t   wfMsgId = _wfMsg.GetWaveformMessageID();


	//Mukundan:This needs review, whats going on down is not correct
	/* BSTMapT <FMessageId_t , FMessageId_t > ::Iterator it_msg = wfMsgToFMsgMap[wid].Find(wfMsgId);
  if(it_msg == wfMsgToFMsgMap[wid].End()){
	  //if this message is Framework Acknowledgement, it will not exist in map. s
	  Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Check msg type.\n");
	  if((_wfMsg.GetType() == Ack_Reply_Type) || (_wfMsg.GetType() == Ack_Request_Type)){
		  Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::incoming message type is %d\n",_wfMsg.GetType());
		  //do not modify msgId;

	  }else{
	      FMessageId_t  fwMsgId =  GetNewFrameworkMsgId();
	      Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::wfMsgId %d not found in map. Assign new message Id: %d and  adding it to Map\n",wfMsgId, fwMsgId);
          wfMsgToFMsgMap[wid].Insert(wfMsgId,fwMsgId);
          Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Update last seen wfMsgId on wfid  %d\n",wid);
          last_wfMsg_Seen[wid] = wfMsgId;
          _wfMsg.SetFrameworkMessageID(fwMsgId);
	  }
  }else{
	  FMessageId_t  fwMsgId = wfMsgToFMsgMap[wid][wfMsgId];
	  Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::wfMsgId found. Corresponding fwMsgId is %d\n",fwMsgId);
	  _wfMsg.SetFrameworkMessageID(fwMsgId);
  }
	 */
	/*
  if(_wfMsg.GetType() == Types::Ack_Request_Type){
	  SoftwareAcknowledgement* data1 = (SoftwareAcknowledgement*) _wfMsg.GetPayload();
	  Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: show payload: %d %d %d \n",data1->src, data1->wfId, data1->msgId);
  }
	 */

	FMessage_t *goingUpMsg;
	WF_MessageBase* message_temp = &_wfMsg;
	WF_Message_n64_t* message = static_cast<WF_Message_n64_t*> (message_temp);
	WaveformId_t  wid = _wfMsg.GetWaveform();
	uint64_t wfAddress =message->GetSource();
	string wfAddrStr = NbrUtils::ConvertU64ToEthAddress(wfAddress);

	Debug_Printf(DBG_CORE_DATAFLOW, "Entering Recv_Handler_For_WF: Received a message from, wf address %lu, str addr %s, node id %d on wf id %u\n", wfAddress, wfAddrStr.c_str(), message->GetSrcNodeID(), wid);
	AddOrUpdateWFAddress(wid, wfAddress,_wfMsg.GetSrcNodeID());
	LinkId link = addressMap->LookUpLinkId(wfAddrStr); // by now we have converted wfAddress to link id
	Debug_Printf(DBG_CORE_DATAFLOW, "Entering Recv_Handler_For_WF: After addressMap update, str addr %s, node id %d on wf id %u\n", wfAddrStr.c_str(), link.nodeId, link.waveformId);

	Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: message->GetType is %d\n",message->GetType());
	//Debug_Printf(DBG_CORE_DATAFLOW, "Address of wfMsg is at %p\n",message);

	//Process PiggyBacking first if enabled.
	///If implicit sync timestamping is enabled, every packet will need to call piggybackee
#if ENABLE_PIGGYBACKING==1 || ENABLE_IMPLICIT_SYNC_TIMESTAMPING==1
	if(_wfMsg.IsFlagSet(WF_FLG_PIGGYBACKING) || ENABLE_IMPLICIT_SYNC_TIMESTAMPING){
		ProcessPiggyBackedData(_wfMsg);
	}
#endif

#if ENABLE_FRAGMENTATION ==1
	if(_wfMsg.IsFlagSet(WF_FLG_FRAGMENTED)){
		Recv_Handler_For_WF_MSG_FRAGMENT(_wfMsg, wrn);
		return;
	}
#endif


	switch(_wfMsg.GetType()){
	case ESTIMATION_MSG:
	{
		Debug_Printf(DBG_CORE_DATAFLOW,"PacketHandler::Sending received message to Link Estimation\n");
		fwBase->LES.linkEstimationDelegate-> operator () (_wfMsg);
		break;
	}
	case DISCOVERY_MSG:
	{
		Debug_Printf(DBG_CORE_DATAFLOW,"Sending received message to discovery\n");
		fwBase->LES.discoveryDelegate-> operator ()(_wfMsg);
		break;
	}
	/* case TimeSync_Type:
    {
      Debug_Printf(DBG_CORE_DATAFLOW, "Got a timesync message %p, not doing anything \n",message);
      break;
    }
    case Data_Type:
    {
      break;
    }*/
	case ACK_REQUEST_MSG:
	{
		SendSoftwareAck(*message); // I think I need to pass address of object. Masahiro
		/*switch(message->numberOfDest){
          case 0: break;
      	  case 1: delete(message->numberOfDest);break;
      	  default: delete[] (message->numberOfDest);
      }*/
		/*if(message != NULL){
    	  Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Recv_Handler Deleting WF_Base message casted as 64 at %p\n",message);
          delete (message);
          message = NULL;
          deletecount++;
      }*/
		//break;
	}
	case PATTERN_MSG:
	{
		Debug_Printf(DBG_CORE_DATAFLOW,"PacketHandler::Received a message on waveform %d,  for pattern instance %d\n",_wfMsg.GetWaveform(),  _wfMsg.GetInstance());
		newcount++;
		//Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Show address of payload, %p\n",message->GetPayload());
		//Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Show payload size %d\n",message->GetPayloadSize());
		//Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Show address of payload, %p\n",_wfMsg.GetPayload());
		//Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Show payload size %d\n",_wfMsg.GetPayloadSize());
		goingUpMsg= new FMessage_t(_wfMsg.GetPayloadSize());
		//goingUpMsg = new FMessage_t();
		//Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::1 Show goingupMsg payload ptr %p\n",goingUpMsg->GetPayload());
		//uint8_t* ptr = new uint8_t[_wfMsg.GetPayloadSize()];
		//Debug_Printf(DBG_CORE_DATAFLOW, "Show address of allocated memory %p\n",ptr);
		//Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::2 Show goingupMsg payload ptr %p\n",goingUpMsg->GetPayload());
		//goingUpMsg->SetPayload(ptr);
		//Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::3 Show goingupMsg payload ptr %p\n",goingUpMsg->GetPayload());
		//goingUpMsg->SetPayloadSize(_wfMsg.GetPayloadSize());
		//Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Show goingupMsg payloadsize %d\n",goingUpMsg->GetPayloadSize());
		//Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::4 Show goingupMsg payload ptr %p\n",goingUpMsg->GetPayload());

		//memcpy(goingUpMsg->GetPayload(), _wfMsg.GetPayload(), _wfMsg.GetPayloadSize());
		memcpy(goingUpMsg->GetPayload(), _wfMsg.GetPayload(), _wfMsg.GetPayloadSize()); //TODO: BK: We need to change this. This is an unncessary copy!!!
		//Masahiro, it seems sometimes, memcpy overlaps.... trying memmove;
		//memmove(goingUpMsg->GetPayload(), _wfMsg.GetPayload(), _wfMsg.GetPayloadSize());
		//goingUpMsg->SetInstance(_wfMsg.GetInstance());
		//TODO::Convert waveform address to NodeId_t
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Show goingupMsg payloadsize %d\n",goingUpMsg->GetPayloadSize());
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Show goingupMsg payload ptr %p\n",goingUpMsg->GetPayload());

		goingUpMsg->SetWaveform(_wfMsg.GetWaveform());
		goingUpMsg->SetSource(link.nodeId);

		goingUpMsg->SetType(Types::PATTERN_MSG);
		//Update the neighbor table
		string wfAddrStr = NbrUtils::ConvertU64ToEthAddress(wfAddress);
		coreNbrTable->TouchNeighbor(addressMap->LookUpLinkId(wfAddrStr).nodeId, wid);

		ProcessRcvdPatternMsg(_wfMsg.GetInstance(), *goingUpMsg);

		// delete goingUpMsg, which is dynamically allocated in this method to pass data to pattern.
		//Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Show address of goingUpMsg about to be deleted %p\n",goingUpMsg);
		//delete(goingUpMsg);
		//goingUpMsg = NULL;
		break;
	}
	case ACK_REPLY_MSG:
	{
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: Got Ack_Reply_Type. Process ack.\n")
    				  ProcessSoftwareAck(*message); //I think I need to pass address of object. Masahiro
		break;
	}
	default:
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Recv_Handler_For_WF: Message Receive: Unknown Message Type : %d\n", _wfMsg.GetType());
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Deleting ack message at %p\n",message);
		break;
	}
}

void PacketHandler::ProcessSoftwareAck(WF_Message_n64_t& wfmsg)
{
	SoftwareAcknowledgement* data = (SoftwareAcknowledgement*) wfmsg.GetPayload();
	Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: Got software acknowledgemet from src %d, wfid %d, wf msgId %lu\n",data->src, data->wfId, data->wfMsgId);
	WaveformId_t wfid = data->wfId;
	WF_MessageId_t wfMsgId = data->wfMsgId;

	newcount++;
	WF_DataStatusParam_n64_t *ack = new WF_DataStatusParam_n64_t(wfMsgId, wfid);
	ack->statusType[0] = WDN_DST_RECV;
	ack->noOfDest=1;
	ack->statusValue[0]=Waveform::WF_ST_SUCCESS;
	Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: Show Send_Busy[%d]: %d\n",wfid,Send_Busy[wfid]);
	ack->readyToReceive = !Send_Busy[wfid];
	Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: Show ack.readyToRecv %d\n",ack->readyToReceive);
	newcount++;
	ack->destArray[0]= wfmsg.GetSource();
	Recv_DN_Handler_For_WF(*ack); // I think I need pass *ack not &ack. Masahiro
	//delete(ack->dest);  Not deleted. As this pointer is private, I could not access it.
	//delete(ack); deleted in WF_AsyncEvent_Special
	Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Got Acknowledgment packet from Node %lu, for wf messageId %lu",wfmsg.GetSource(),wfMsgId);
}


void PacketHandler::SendSoftwareAck(WF_Message_n64_t& _wfmsg)
{
	WaveformId_t wfid = _wfmsg.waveformId;
	FMessageId_t msgId = this->GetNewFrameworkMsgId();//_wfmsg.GetFrameworkMessageID();

	Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Received Ack_Request_Type. Need to send Framework Ack\n");
	//check if WF supports acknowledgement or not.
	if((*wfAttributeMap)[_wfmsg.waveformId].destReceiveAckSupport == false){
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Waveform %d does not support acknowledgment\n",_wfmsg.waveformId);
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::If this message was unicast, need to send acknowledgment.\n");
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::This message is unicast. Send Acknowledgement. Prepare packet\n");
		//prepare acknowledgement packet.
		Link* destLinkArray[1];
		//store nodeid for each waveform id.
		//NodeId_t* destArray = new NodeId_t[1];
		NodeId_t destArray[1];
		destLinkArray[0]= coreNbrTable->GetNeighborLink(_wfmsg.GetSrcNodeID(),wfid);
		if(destLinkArray[0]){
			Debug_Printf(DBG_CORE_DATAFLOW,"PacketHandler::FrameworkBase::Send:: Node %d found in Neighbor Table. Node id from table %d, wf is %d\n", destArray[0], destLinkArray[0]->linkId.nodeId, destLinkArray[0]->linkId.waveformId); fflush(stdout);
			destArray[0] = destLinkArray[0]->linkId.nodeId;
			Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Adding node %d to wf %d\n",destLinkArray[0]->linkId.nodeId,destLinkArray[0]->linkId.waveformId);
		}else{
			Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::cant find src node as nbr; cant send fw ack; one-way link?");
			return;
		}
		//create message
		FMessage_t* sendMsg = new FMessage_t;
		uint8_t* a = new uint8_t[sizeof(SoftwareAcknowledgement)];
		Debug_Printf(DBG_CORE_DATAFLOW,"PacketHandler:: Setting Payload ptr to %p\n",a);
		sendMsg->SetPayload(a);
		sendMsg->SetPayloadSize(sizeof(SoftwareAcknowledgement));;

		// FMessage_t sendMsg(sizeof(SoftwareAcknowledgement));
		SoftwareAcknowledgement data;
		data.wfId = wfid;
		data.src  = MY_NODE_ID;
		data.wfMsgId = _wfmsg.GetWaveformMessageID();
		// memset(sendMsg.GetPayload(), 0, sizeof(SoftwareAcknowledgement));
		//memcpy(sendMsg.GetPayload(),&data, sizeof(SoftwareAcknowledgement));
		memcpy(sendMsg->GetPayload(),&data, sizeof(SoftwareAcknowledgement));
		///////Debugging
		SoftwareAcknowledgement* data_ptr = (SoftwareAcknowledgement *)sendMsg->GetPayload();
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: Show payload; src %d , wfid %d, wf msgId %lu\n",data_ptr->src, data_ptr->wfId, data_ptr->wfMsgId);
		Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet: Show payload ptr %p\n", data_ptr);

		sendMsg->SetSource(MY_NODE_ID);  //set Src

		Debug_Printf(DBG_CORE_DATAFLOW, "Message size I created is %d\n", sendMsg->GetPayloadSize());
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Setting message type to Ack_Reply_Type\n");
		sendMsg->SetType(Types::ACK_REPLY_MSG);
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Address of linkArray is %p\n",destLinkArray);
		Debug_Printf(DBG_CORE_DATAFLOW,"PacketHandler::Enqueing mesasge type %d. wf %d, noOfDest %d\n",sendMsg->GetType(),wfid,1);
		EnqueueToMC(_wfmsg.GetInstance(), sendMsg, destArray, destLinkArray, (uint16_t)1,false,wfid,msgId, 0, false);
		Debug_Printf(DBG_CORE_DATAFLOW,"PacketHandler::number of remaining packet on wf %d is %d\n",wfid,GetRemainingPacket(wfid));
		if(GetRemainingPacket(wfid) >= 1){
			//uint64_t _delay = rnd->GetNext(); //wfTimer[wfid]->Change(_delay, ONE_SHOT);
			Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Triger Fetchinig message from MC on %d\n",wfid);
			ProcessOutgoingMessages(wfid);
		}
	}else{
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Waveform %d supports acknowledgement.\n",wfid);
	}
}



void PacketHandler::ProcessRcvdPatternMsg (PatternId_t _pid, FMessage_t& msg){
	//first convert message to FMessage_t

	//Check msg for which patternInstance it is meant for and call the appropriate delegate


	Debug_Printf(DBG_CORE_DATAFLOW, "RAL::PacketHandler:: Received a Pattern Message from node %d, with instance no %d\n",msg.GetSource(), _pid);fflush(stdout);
	PatternClientMap_t::Iterator it = patternClients->Find(_pid);
	if(it==patternClients->End()){
		Debug_Printf(DBG_CORE_DATAFLOW,"PacketHandler::ProcessRcvdPatternMsg: Could not find the ASNP instance %d\n", _pid);
	}
	else {
		eventDispatcher->InvokeRecvMsgEvent(_pid, msg);
		//(*patternClients)[patternInstance].recvDelegate ->operator()(msg);
		//Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::ProcessRcvdPatternMsg deleting FMessage_t  %p\n",&msg);
		//delete(&msg); // This is deleted in Recv_Handler_For_WF where memory was allocated.
		//deletecount++;

	}
}

/*
void PacketHandler::ProcessCollectors(uint8_t wfId,  uint8_t noOfPattern, map<uint8_t, uint8_t> *patternPriority,  uint8_t noOfPktsToSend){
  if(noOfPattern > 0){
    //Go through patternPriority hash and pick instances one by one. Check their collector size and pick a packet to send out.
    Debug_Printf(DBG_CORE_DATAFLOW, "Processing packets for radio id: %d \n", wfId);
    for (uint8_t i=0; i < noOfPattern; i++) {
      uint8_t patternInstance =(*patternPriority)[i];

      uint8_t pendingPkts = (*patternClients)[patternInstance].pktCollector->GetNumberPendingPackets(wfId,0);
      Debug_Printf(DBG_CORE_DATAFLOW, "Core::PacketHandler:: Looking for packets of pattern instance with id %d and found %d packets \n", patternInstance, pendingPkts);fflush(stdout);
      Message_n64_t *sendMsg;
      for (uint8_t ii=0; ii<pendingPkts  && ii < noOfPktsToSend; ii++){
	FMessage_t &bufMsg = (*patternClients)[patternInstance].pktCollector->GetFirstPacket(wfId);
	sendMsg = new Message_n64_t(bufMsg.GetPayloadSize());
	//Debug_Printf(DBG_CORE_DATAFLOW, "Core::PacketHandler:: Dequeued packet (ptr %lu) of payloadsize %d belonging to pattern  %u \n", (uint64_t)&bufMsg, bufMsg.GetPayloadSize(), bufMsg.GetInstance());
	//sendMsg->Copy(bufMsg);

	memcpy(sendMsg->payload, bufMsg.payload, bufMsg.payloadSize);
	sendMsg->type =bufMsg.GetType();
	sendMsg->inst = bufMsg.GetInstance();
	//payloadSize = _msg.GetPayloadSize();
	sendMsg->waveformId=bufMsg.waveformId;
	sendMsg->payloadSize=bufMsg.payloadSize;
	sendMsg->expiryTime=bufMsg.expiryTime;
	sendMsg->src = (uint64_t)bufMsg.src;
	sendMsg->dest = (uint64_t)bufMsg.dest;

	//set the type of message
	sendMsg->SetType(Pattern_Type);

	sendMsg->SetInstance(patternInstance);
	//Debug_Printf(DBG_CORE_DATAFLOW, "Core::PacketHandler:: Sending packet of payloadsize %d belonging to pattern  %u \n", sendMsg->GetPayloadSize(), sendMsg->GetInstance());
	//Check if the message is a unicast or broadcast message and send it out.

	if(bufMsg.dest == FRAMEWORK_LOCAL_BCAST_ADDRESS){
	  //Debug_Printf(DBG_CORE_DATAFLOW, "\n\nCAL pointer is %x\n", (ptr));
	  if((*wfHash)[wfId]->IsBroadcastSupported()){
	    if((*wfHash)[wfId]-> Send(0xFFFFFFFFFFFFFFFF,*sendMsg, sendMsg->GetPayloadSize())!=WF_ST_SUCCESS){
		    Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: Problem sending message broadcast msg to WF layer\n");
	    }else {
		    Debug_Printf(DBG_CORE_DATAFLOW,"Core::Dataflow::PacketHandler:: Sent broadcast message of size %ul to Waveform\n", sendMsg->GetPayloadSize());
	    }
	  }else {
	    Debug_Printf(DBG_CORE_DATAFLOW, "Core::Dataflow::PacketHandler: Error: Broadcast not supported by the waveform\n");
	    return ;
	  }
	}else {
	  //Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: Sending a unicast message to CAL, with dest %d\n", sendMsg->dest);fflush(stdout);
	  if((*wfHash)[wfId]-> Send(sendMsg->dest, *sendMsg, sendMsg->GetPayloadSize())!=WF_ST_SUCCESS){
		  Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: Problem sending uinicast message to WF layer\n");
	  }
	}
	noOfPktsToSend--;
	delete(&bufMsg);
      }
    }
  }
  else {
    //Debug::PrintTimeMilli();
    //Debug_Printf(DBG_CORE_DATAFLOW, "No ASNP registered \n");
  }
}
 */

FMessageId_t PacketHandler::EnqueueToMC(PatternId_t pid,  FMessage_t *msg, NodeId_t *nodeArray, Link **linkPtrArray, uint16_t noOfDest, bool broadcast, WaveformId_t wfid, FMessageId_t  newFMsgId, uint16_t nonce, bool softwarebroadcast){
	Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Adding message (%p), with type %d to MessageCabinet\n", msg, msg->GetType());
	//return wfToMC[wfid]->AddNewFrameworkMessage(pid, msg, nodeArray, linkPtrArray, noOfDest,broadcast, wfid, newFMsgId);
	return mc->AddNewFrameworkMessage(pid, msg, nodeArray, linkPtrArray, noOfDest, broadcast,wfid,newFMsgId, nonce, softwarebroadcast);
}

///Returns the number of messages left in the cabinet after sending a packet.
uint16_t PacketHandler::ProcessOutgoingMessages(WaveformId_t wfId)
{
	//uint8_t pendingPkts = wfToMC[wfId]->GetNumberWaveformOfMessages(wfId);
	uint8_t pendingPkts = mc->GetNumberWaveformOfMessages(wfId);
	Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Entered ProcessOutgoingMesssages\n");
	/*FMsgMap::Iterator it_fmsg2 = mc->framworkMsgMap->Begin();
	while(it_fmsg2 != mc->framworkMsgMap->End()){
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::ProcessOutgoingMessages: MsgId %d FMsgMap entry is at %p, msg_ptr is %p, payload_ptr is %p, msg Type is %d \n",
				it_fmsg2->First(),it_fmsg2->Second(),it_fmsg2->Second()->msg, it_fmsg2->Second()->msg->GetPayload(), it_fmsg2->Second()->msg->GetType());
		++it_fmsg2;
	}*/

	//if busy flag is set, do not retrieve anything from qElement as only one thing should go inside
	if(Send_Busy[wfId] == true){
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::ProcessOutgoingMessages Send_Busy[%d] True; Postponing\n",wfId);
		return 0;
	}       

	Debug_Printf(DBG_CORE_DATAFLOW, "Core::PacketHandler::ProcessOutgoingMessages: %d total packets in MC, Looking for packets for waveform %u\n",pendingPkts, wfId);fflush(stdout);

	if(pendingPkts > 0){

		Debug_Printf(DBG_CORE_DATAFLOW,"PacketHandler::pendingPkt exits in MC for wf %d\n",wfId);
		//WF_MessageQElement *qElement = wfToMC[wfId]->GetNextMessage(wfId);
		WF_MessageQElement *qElement = mc->GetNextMessage(wfId);
		if(qElement == NULL){
			Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: fetched message type is pattern. Do not send out");
			return 0;
		}
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: Show framework msgId of packet fetched: %lu\n",qElement->msg->GetWaveformMessageID());
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: Sending msg type %d to wf\n",qElement->msg->GetType());
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: Show address of msg %p\n",qElement->msg);
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::show destinations of this msg.");
		for(int index =0; index < qElement->noOfDest; index++){
			Debug_Printf(DBG_CORE_DATAFLOW, " %ld",qElement->destArray[index]);
		}
		Debug_Printf(DBG_CORE_DATAFLOW, "\n");

		if(qElement->msg->GetType() != Types::ACK_REPLY_MSG){
			//Add qElement into map 
			Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Adding qElement into map with key %d\n",qElement->frameworkMsgId);
			msgToQElement[wfId][qElement->frameworkMsgId] = qElement;
		}else{
			//To hold data to be put back to MC create entry.
			msgToQElement[wfId][qElement->frameworkMsgId] = qElement;

			Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::ProcessOutgoingMessages: This is Ack_Reply_Type so do not add this into msgToQElement map\n");
			SoftwareAcknowledgement* data1 = (SoftwareAcknowledgement*) qElement->msg->GetPayload();
			Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::ProcessOutgoingMessages: show payload: %d %d %lu \n",data1->src, data1->wfId, data1->wfMsgId);
			Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::ProcessOutgoingMessages: Show payload ptr %p\n", data1);
		}
		if(qElement){    
			WF_Message_n64_t *bufMsg = (WF_Message_n64_t *)qElement->msg;
			Debug_Printf(DBG_CORE_DATAFLOW,"Core::PacketHandler:: Dequeued element,destArray (ptr %p, %p) with %d dest(s), payloadsize %d, belonging to pattern  %u \n", qElement, qElement->destArray, qElement->noOfDest, bufMsg->GetPayloadSize(), bufMsg->GetInstance());

			//for (int i=0; i< qElement->noOfDest; i++) {
			//Debug_Printf(DBG_CORE_DATAFLOW, "Core::PacketHandler: Destination at location %d is: %lu  \n", i, qElement->destArray[i]);
			//}
			//This is broadcast
			Debug_Printf(DBG_CORE_DATAFLOW,"PacketHandler::Decide if this is broadcast or unicast\n");fflush(stdout);
			if(qElement->broadcast) {
				Debug_Printf(DBG_CORE_DATAFLOW,"PacketHandler:: Processing a broadcast message\n");fflush(stdout);
				if((*wfAttributeMap)[wfId].broadcastSupport) 
				{
					/*if((*wfHash)[wfId]->BroadcastData(*bufMsg, bufMsg->GetPayloadSize(), qElement->wfmsgId)){
					Debug_Printf(DBG_CORE_DATAFLOW,"Core::Dataflow::PacketHandler:: Sent broadcast message %d of size %ul to Waveform\n",bufMsg->GetFrameworkMessageID(), bufMsg->GetPayloadSize());
					}else {
						Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: Problem sending message broadcast msg to WF layer\n");
					}*/
					//Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler ack timer started to reschedule Broadcast\n");
					//Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler ack timier[%d] started\n",wfId);
					// ackTimer[wfId]->Start();

					//Lets check if there are modules wanting to piggyback
#if ENABLE_PIGGYBACKING==1 && ENABLE_EXPLICIT_SENDER_TIMESTAMPING==1
#warning "Piggybacking is enabled ..."
					Debug_Printf(DBG_CORE_API, "PacketHandler:: Piggybacking is enabled\n");
					uint16_t bytesLeft = wfAttributeMap->operator[](wfId).maxPayloadSize - bufMsg->GetPayloadSize();

					if(piggyBacker->GetNumberOfRegisteredPiggyBackees(wfId) > 0){
						uint8_t piggyPayloadSize = piggyBacker->GetPiggyPaylaodSize(wfId);
						if( piggyPayloadSize <= bytesLeft){
							PiggyBackeeI<uint64_t>* pbPtr = static_cast<PiggyBackeeI<uint64_t>*>(piggyBacker->GetPiggyBackeePtr(wfId));
							uint8_t *piggyPayload=0;
							uint8_t actualpiggyPayloadSize = pbPtr-> CreateData (qElement->destArray[0], true, piggyPayload, piggyPayloadSize);
							if(actualpiggyPayloadSize != piggyPayloadSize) {

							}
							Debug_Printf(DBG_PIGGYBACK, "PacketHandler::Piggybacked Send: Payloadsize %u, payload ptr: %p \n",actualpiggyPayloadSize, piggyPayload);
							bufMsg->SetPiggyPayload(piggyPayload, piggyPayloadSize);
							bufMsg->SetFlag(WF_FLG_PIGGYBACKING);
#if ENABLE_EXPLICIT_SENDER_TIMESTAMPING==1 
							bufMsg->SetFlag(WF_FLG_TIMESTAMP);
							Debug_Printf(DBG_PIGGYBACK, "PacketHandler::Piggybacked Send: Set both TS and Piggy flags\n");
#endif
						}
						else {
							Debug_Printf(DBG_CORE_API, "PacketHandler::Piggybacked Send: Not enough space left in packet\n");
						}
					}else {
						Debug_Printf(DBG_CORE_API, "PacketHandler:: No registered piggybackees\n");
					}
#endif //end piggbacking

					//update time
					Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: Update waiting_in_framework time\n");
					gettimeofday(&qElement->waiting_in_framework,NULL);    
					Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: Calling wf BroadcasatData method\n");
					//Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Setting Send_Busy[%d] to true\n",wfId); 
					if(Send_Busy[wfId] == true){
						Debug_Printf(DBG_ERROR, "PacketHandler::ProcessOutgoingMessages:Something went wrong. Send_Busy is already set\n");
						//printf("PacketHandler::ProcessOutgoingMessages: Something went Terribly Wrong. Send_Busy is already set for waveform %d\n", wfId);
					}
					//Send_Busy[wfId] = true;  wait for wf reply. 

					Send_Busy[wfId] = true; 
					Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: Calling wf BroadcasatData method on waveform %d for msg id %d\n", wfId, qElement->wfmsgId);
					(*wfMap)[wfId]->BroadcastData(*bufMsg, bufMsg->GetPayloadSize(), qElement->wfmsgId); 

					double delayMicro;
					CalculatePacketWaitTime(qElement, &delayMicro);
					if(qElement->msg->GetType() == Types::ESTIMATION_MSG) {
						coreNbrTable->UpdateTransmissionDelay(wfId, delayMicro);
					}


				}else {
					Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler: Error: Broadcast not supported by the waveform %d\n", wfId);
					//Needs to brake broadcast into multiple unicast.  
				}  
			}
			else {
				//this is unicast
				Debug_Printf(DBG_CORE_DATAFLOW,"PacketHandler::unicast\n");fflush(stdout);

				/*No need to clear array as we are not using them
				for(int index = 0; index < qElement->noOfDest ; index++){
					//initializing all  WDN acknowledgement maps
					qElement->ackFromDest[index] = false;
					qElement->msgSentByWF[index] = false;
					qElement->ackFromWF[index] = false;
				}*/
				//Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler ack timer started\n");
				//Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler ack timier[%d] started to reschedule Send again\n",wfId);
				//ackTimer[wfId]->Start();
				//Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Setting Send_Busy[%d] to true\n",wfId);
				if(Send_Busy[wfId] == true){
					Debug_Printf(DBG_ERROR, "PacketHandler::ProcessOutgoingMessages: Something went wrong. Send_Busy is already set\n");
				}
				FMsgMap::Iterator it_fmsg1 = mc->framworkMsgMap->Find(qElement->frameworkMsgId);
				if(it_fmsg1 != mc->framworkMsgMap->End()){
					Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: FMsgMap entry is at %p, msg_ptr is %p, payload_ptr is %p, msg Type is %d",
							it_fmsg1->Second(),it_fmsg1->Second()->msg, it_fmsg1->Second()->msg->GetPayload(), it_fmsg1->Second()->msg->GetType());
				}
				Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: Sending msg type %d to wf %d\n",bufMsg->GetType(),wfId);
				Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: Sending msg type %d to wf %d\n",qElement->msg->GetType(),wfId);

				//Lets check if there are modules wanting to piggyback
#if ENABLE_PIGGYBACKING==1 && ENABLE_EXPLICIT_SENDER_TIMESTAMPING==1
				uint16_t bytesLeft = wfAttributeMap->operator[](wfId).maxPayloadSize - bufMsg->GetPayloadSize();
				Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: Piggybacking is enabled\n");

				if(piggyBacker->GetNumberOfRegisteredPiggyBackees(wfId) > 0){
					uint8_t piggyPayloadSize = piggyBacker->GetPiggyPaylaodSize(wfId);
					if( piggyPayloadSize < bytesLeft){
						PiggyBackeeI<uint64_t>* pbPtr = static_cast<PiggyBackeeI<uint64_t>*>(piggyBacker->GetPiggyBackeePtr(wfId));
						uint8_t *piggyPayload=0;
						pbPtr-> CreateData (qElement->destArray[0], false, piggyPayload, piggyPayloadSize);
						bufMsg->SetPiggyPayload(piggyPayload, piggyPayloadSize);
						bufMsg->SetFlag(WF_FLG_PIGGYBACKING);

#if ENABLE_EXPLICIT_SENDER_TIMESTAMPING==1 
						bufMsg->SetFlag(WF_FLG_TIMESTAMP);
						Debug_Printf(DBG_PIGGYBACK, "PacketHandler::Piggybacked Send: Set both TS and Piggy flags\n");
#endif
					}
				}
#endif //end piggbacking

				Send_Busy[wfId] = true; //wait wf to set busy flag in their reply
				//Call WF method
				(*wfMap)[wfId]->SendData(*bufMsg, bufMsg->GetPayloadSize(),qElement->destArray, qElement->noOfDest, qElement->wfmsgId, false); 

				double delayMicro;
				CalculatePacketWaitTime(qElement, &delayMicro);
				for (int i=0; i< qElement->noOfDest; i++){
					coreNbrTable->UpdateTransmissionDelay(wfId, qElement->destArray[i], delayMicro);
				}

			}


			//pkt sent

			//Send_Busy[wfId] = true; //should not send out any mesage
			fflush(stdout);
			pendingPkts--;
			//Start Timer to wait for WDN_WF_RECV

			return pendingPkts;
		}else {
			Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: Problem messages in MC, but look up seems to have failed\n");
			return 0;
		}

	}
	else {
		Debug_Printf(DBG_CORE_DATAFLOW,  "PacketHandler::Nothing is in cabnet for wf %d\n",wfId);
		return 0;
	}
}




/***********************************************************Timer 0 ************************************************************/
void PacketHandler::WDN_WF_RECV_Checker_Handler(uint32_t flag){
	// Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::WDN_WF_RECV Check timer expired.\n");
	//Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: Show framworkMsgMap size %d\n",mc->framworkMsgMap->Size());
	FMsgMap::Iterator it_fmsg = mc->framworkMsgMap->Begin();
	if(it_fmsg == mc->framworkMsgMap->End()){
		//Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: framworkMsgMap has no entry\n");
	}
	while(it_fmsg != mc->framworkMsgMap->End()){
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: frameworkMsgMap has key %d\n", it_fmsg->First());
		++it_fmsg;
	}

	//For each waveform, check if there is pending packet exists or not. If there is one, send it to framework
	for(int wfid = 0; wfid < MAX_WAVEFORMS ; wfid++){
		//Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Checking wf %d... \n",wfid);
		//Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::size of msgTOQelement is %d\n",msgToQElement[wfid].Size());
		//for each message in msgToQElement map
		BSTMapT< FMessageId_t  , WF_MessageQElement* >::Iterator it = msgToQElement[wfid].Begin();
		while(it !=msgToQElement[wfid].End()){
			FMessageId_t msgId = it->First();
			Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Checking wf %d... \n",wfid);
			Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::size of msgTOQelement on wfid %d is %d\n",wfid, msgToQElement[wfid].Size());
			Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Entered while statement.. Current msgId is %d\n",msgId);
			//if(it->Second()->ackFromWF[0] == true){
			//First, check if map has value for key, first destination for this waveform.
			Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: Checking entry dnRecvByWF[%ld] exists or not \n",it->Second()->destArray[0]);


			FMsgMap::Iterator ii = mc->framworkMsgMap->Find(msgId);
			if(ii != mc->framworkMsgMap->End()){
				if(ii->Second()->Check_Item_Exists_dnRecvByWF(it->Second()->destArray[0]) == false){
					Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Map entry does not exist. Check if this is broadcast\n");
					if(it->Second()->broadcast == false){
						Debug_Printf(DBG_ERROR, "PacketHandler::WDN_WF_RECV_Checker_Handler: This is unicast, something went terribly wrong. It should have corresponding entry\n");
					}
					Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::This is broadcast.So check remaining_node to see if it received response or not\n");
					if(ii->Second()->dnRecvByWF_remaining == 0){
						Debug_Printf(DBG_CORE_DATAFLOW, "MessageId %d on waveform %d is correctly send out\n",msgId,wfid);
						++it;
						continue;
					}
				}
			}else{
				Debug_Printf(DBG_ERROR, "PacketHandler::WDN_WF_RECV_Checker_Handler: Something seriously worng. %d msg exist in waveform heap but not in MC, so skipping it\n", msgId);
				++it;
			}
			if((ii != mc->framworkMsgMap->End()) && ii->Second()->dnRecvByWF[it->Second()->destArray[0]] == true){
				Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::This message (id:%d) is already send to framework correctly\n",it->First());
				++it;
				continue;
			}
			//if(it->Second()->ackFromWF[0] == false){
			if((ii != mc->framworkMsgMap->End()) && ii->Second()->dnRecvByWF[it->Second()->destArray[0]] == false){
				struct timeval current_time;
				gettimeofday(&current_time , NULL);
				//find out the time elapsed since first attempt
				if(current_time.tv_sec == it->Second()->waiting_in_framework.tv_sec){
					//check usec
					if(current_time.tv_usec - it->Second()->waiting_in_framework.tv_usec >= WDN_WF_RECV_Timeout_Period[wfid]){
						if(current_time.tv_usec + (1000000 - it->Second()->waiting_in_framework.tv_usec) > WDN_WF_RECV_Timeout_Period[wfid]){
							//retry
							Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::This message (id:%d) was not received by framework. Try again\n",it->First());
							WF_MessageQElement* qElement = it->Second();
							WF_Message_n64_t *bufMsg = (WF_Message_n64_t *)qElement->msg;
							Debug_Printf(DBG_CORE_DATAFLOW,"Core::PacketHandler::current element,destArray (ptr %p, %p) with %d dest(s), payloadsize %d, belonging to pattern  %u \n", qElement, qElement->destArray, qElement->noOfDest, bufMsg->GetPayloadSize(), bufMsg->GetInstance());
							Debug_Printf(DBG_CORE_DATAFLOW,"PacketHandler::Decide if this is broadcast or unicast\n");fflush(stdout);
							if(qElement->broadcast) {
								Debug_Printf(DBG_CORE_DATAFLOW,"Core::Dataflow::PacketHandler:: Processing a broadcast message\n");
								Debug_Printf(DBG_CORE_DATAFLOW,"PacketHandler::broadcast\n");fflush(stdout);
								if((*wfAttributeMap)[0].broadcastSupport) {
									(*wfMap)[0]->BroadcastData(*bufMsg, bufMsg->GetPayloadSize(), qElement->wfmsgId);
								}else{
									Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler: Error: Broadcast not supported by the waveform %d\n", 0);
									//Needs to brake broadcast into multiple unicast.
								}
							}else{
								//this is unicast
								Debug_Printf(DBG_CORE_DATAFLOW,"PacketHandler::unicast\n");fflush(stdout);
								for (int i=0; i< qElement->noOfDest; i++){
									Debug_Printf(DBG_CORE_DATAFLOW,"PacketHandler:: Packet destition %d  is %lu \n",i, qElement->destArray[i]);
								}
								//Call WF method
								(*wfMap)[wfid]->SendData(*bufMsg, bufMsg->GetPayloadSize(),qElement->destArray, qElement->noOfDest, qElement->wfmsgId, false);
							}
							Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::PacketHandler send packet to framewrok again\n");

						}
					}
				}else{
					//second order is different. So elapst time is current_time.tv_usec + (1000000 - waiting_in_framework.tv_usec)
					if(current_time.tv_usec + (1000000 - it->Second()->waiting_in_framework.tv_usec) > WDN_WF_RECV_Timeout_Period[wfid]){
						//retry
						Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::This message (id:%d) was not received by framework. Try again\n",it->First());
						WF_MessageQElement* qElement= it->Second();
						WF_Message_n64_t *bufMsg = (WF_Message_n64_t *)qElement->msg;
						if(bufMsg == 0){
							Debug_Error("PacketHandler::WDN_WF_RECV_Checker_Handler: Illegal. Terminate\n");fflush(stdout);
						}
						Debug_Printf(DBG_CORE_DATAFLOW,"Core::PacketHandler::current element,destArray (ptr %p, %p) with %d dest(s), payloadsize %d, belonging to pattern  %u \n", qElement, qElement->destArray, qElement->noOfDest, bufMsg->GetPayloadSize(), bufMsg->GetInstance());
						Debug_Printf(DBG_CORE_DATAFLOW,"PacketHandler::Decide if this is broadcast or unicast\n");fflush(stdout);
						if(qElement->broadcast) {
							Debug_Printf(DBG_CORE_DATAFLOW,"Core::Dataflow::PacketHandler:: Processing a broadcast message\n");
							Debug_Printf(DBG_CORE_DATAFLOW,"PacketHandler::broadcast\n");
							if((*wfAttributeMap)[wfid].broadcastSupport) {
								(*wfMap)[wfid]->BroadcastData(*bufMsg, bufMsg->GetPayloadSize(), qElement->wfmsgId);
							}else{
								Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler: Error: Broadcast not supported by the waveform %d\n", 0);
								//Needs to brake broadcast into multiple unicast.
							}
						}else{
							//this is unicast
							Debug_Printf(DBG_CORE_DATAFLOW,"PacketHandler::unicast\n");fflush(stdout);
							for (int i=0; i< qElement->noOfDest; i++){
								Debug_Printf(DBG_CORE_DATAFLOW,"PacketHandler:: Packet destition %d  is %lu \n",i, qElement->destArray[i]);
							}
							//Call WF method
							(*wfMap)[wfid]->SendData(*bufMsg, bufMsg->GetPayloadSize(),qElement->destArray, qElement->noOfDest, qElement->wfmsgId, false);
						}
						Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::PacketHandler send packet to framewrok again\n");
					}
				}
				Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Increment iterator\n");
				++it;
			}
		}
	}
}

/********************************************** Timer 1***********************************************************/

void PacketHandler::WDN_WF_SENT_Checker_Handler(uint32_t flag)
{
	//Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::WDN_WF_SENT Check Timer expired.\n");
	FMsgMap::Iterator it_fmsg = mc->framworkMsgMap->Begin();
	if(it_fmsg == mc->framworkMsgMap->End()){
		//Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: No message in framworkMsgMap\n");
	}
	while(it_fmsg !=  mc->framworkMsgMap->End()){
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::WDN_WF_SENT_Checker_Handler message id %d is in framworkMsgMap\n",it_fmsg->First());
		++it_fmsg;
	}

	//for each waveform
	for(int wfid = 0; wfid < MAX_WAVEFORMS; wfid++)
	{
		//check for all message, if it receives WDN_WF_SENT
		BSTMapT <FMessageId_t , WF_MessageQElement*>:: Iterator it = msgToQElement[wfid].Begin();
		//Debug_Printf(DBG_CORE_DATAFLOW, "msgToQElement size is %d\n", msgToQElement[wfid].Size());
		while(it != msgToQElement[wfid].End())
		{
			FMsgMap::Iterator ii= mc->framworkMsgMap->Find(it->First());
			if(ii==mc->framworkMsgMap->End()){
				Debug_Error("PacketHandler::WDN_WF_SENT_Checker_Handler something seriously wrong %d is not in MC, skipping\n",it->First());
				++it;
				continue;
			}
			//get first QElement  in map.
			WF_MessageQElement* qElement = it->Second();
			if(qElement->broadcast == false){
				Debug_Printf(DBG_CORE_DATAFLOW, "This message %d is unicast. Skip this message\n",it->First());
				++it;
				continue;
			}
			Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::WDN_WF_SENT_Checker_Handler Checking messageId %d\n", it->First());
			Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::WDN_WF_SENT_Checker_Handler Show stored time. %ld::%ld\n",qElement->Waiting_ACK.tv_sec, qElement->Waiting_ACK.tv_usec);
			uint8_t ack_received =0;
			struct timeval current_time;
			gettimeofday(&current_time , NULL);
			//if qElement is waiting for acknowledgement more than 40msec, need to clear it.
			if(current_time.tv_sec == qElement->Waiting_ACK.tv_sec){
				//check usec portiong
				//if(current_time.tv_usec -qElement->Waiting_ACK.tv_usec > 40000){
				if(current_time.tv_usec -qElement->Waiting_ACK.tv_usec > WDN_WF_SENT_Timeout_Period[wfid])
				{ 
					Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::WDN_WF_SENT_Timeout_Period is %d\n",WDN_WF_SENT_Timeout_Period[wfid]);
					//delete 
					for(int index = 0; index < qElement->noOfDest ; index++){	
						FMsgMap::Iterator ii = mc->framworkMsgMap->Find(it->First());
						if(ii != mc->framworkMsgMap->End()){
							if(!(ii->Second()->Check_Item_Exists_dnSentByWF(qElement->destArray[index]))){
								Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Node %ld has not received acknowledgment\n",qElement->destArray[index]);
								++it;
								continue; //skip rest
							}
							if(ii->Second()->dnSentByWF[it->Second()->destArray[index]] == true){
								Debug_Printf(DBG_CORE_DATAFLOW,"PacketHandler::message id %d destination %d received WDN_WF_SENT\n",it->First(),index);
								ack_received++;
							}else{
								Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::message id %d destination %d did not receive WDN_WF_SENT\n",it->First(), index);
							}
						}else {
							Debug_Error("PacketHandler::WDN_WF_SENT_Checker_Handler Something seriously wrong\n");
						}
					}
					if(qElement->broadcast){
						Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Broadcast message id %d did not receive ACK_WF_SETN\n",it->First());
						//timer expired but not all ack received. 
						Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Some node did not receive ack..\n");
						//prepare to send information back to pattern. for now just delete it.
						Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Waveform 2 did not receive acknowledgment within specified time period\n");
						Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Needs to send NACK to Pattern?\n");
						//Delete corresponding infromation
						ClearMap(wfid,it->First());  
						//destroy framworkMsgMap entry
						FMsgMap ::Iterator it_fmsg = mc->framworkMsgMap->Find(it->First());
						if(it_fmsg == mc->framworkMsgMap->End()){
							Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::WDN_WF_SENT_Checker_Handler: framworkMsgMap not found for msgId %d\n",it->First());
						}else{
							FMessageQElement* fmsg_ptr = it_fmsg->Second();
							Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::WDN_WF_SENT_Checker_Handler: Deleting framworkMsgMap for msgId %d at %p\n",it->First(),fmsg_ptr);
							mc->framworkMsgMap->Erase(it->First());
							delete(fmsg_ptr);
							fmsg_ptr = NULL;
							Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::WDN_WF_SENT_Checker_Handler: Before Erasing entry from msgIdToPidMap, make sure the entry exist for this msgId\n");
							BSTMapT <FMessageId_t , PatternId_t> ::Iterator it_msg = msgIdToPidMap.Find(it->First());
							if(it_msg == msgIdToPidMap.End()){
								Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Corresponding pid does not exist. Maybe this is not Pattern message\n");
							}else{
								Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Deleting entry 3 (%d, %d) from msgIdToPidMap\n",it_fmsg->First(),msgIdToPidMap[it_fmsg->First()]);
								msgIdToPidMap.Erase(it->First());
							}
							//clear busy flag
							//Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Clear Send_Busy[%d]\n",wfid);
							//Send_Busy[wfid] = false;
						}
					}

					if(ack_received != qElement->noOfDest){
						//timer expired but not all ack received. 
						Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Some node did not receive ack..\n");
						//prepare to send information back to pattern. for now just delete it.
						Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Waveform 2 did not receive acknowledgment within specified time period\n");
						Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Needs to send NACK to Pattern?\n");
						//Delete corresponding infromation
						Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: Clear Send_Busy[%d]\n",wfid);
						Send_Busy[wfid] = false;
						ClearMap(wfid, it->First());
						Debug_Error("PacketHandler::WDN_WF_SENT_Checker_Handler Clear Send_Busy[%d]\n",wfid);
					}
				}
			}
			else {

				if(current_time.tv_usec + (1000000 - qElement->Waiting_ACK.tv_usec) > WDN_WF_SENT_Timeout_Period[wfid]){
					Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::WDN_WF_SENT_Timeout_Period is %d\n",WDN_WF_SENT_Timeout_Period[wfid]);
					//delete 
					for(int index = 0; index < qElement->noOfDest ; index++){
						FMsgMap::Iterator ii = mc->framworkMsgMap->Find(it->First());


						if(ii!=mc->framworkMsgMap->End()){
							if(!ii->Second()->Check_Item_Exists_dnSentByWF(qElement->destArray[index])){
								Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Node %ld has not received acknowledgment\n",qElement->destArray[index]);
								++it;
								continue; //skip rest
							}

							if(ii->Second()->dnSentByWF[it->Second()->destArray[index]] == true){
								Debug_Printf(DBG_CORE_DATAFLOW,"PacketHandler::message id %d destination %d received WDN_WF_SENT\n",it->First(),index);
								ack_received++;
							}else{
								Debug_Error("PacketHandler::WDN_WF_SENT_Checker_Handler message id %d destination %d did not recieve WDN_WF_SENT\n",it->First(), index);
							}

						}else {
							Debug_Error("PacketHandler::WDN_WF_SENT_Checker_Handler Something seriously wrong, message id %d not in MC\n",it->First());
						}
					} 
					if(qElement->broadcast){
						Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Broadcast message id %d did not receive ACK_WF_SETN\n",it->First());
						//timer expired but not all ack received. 
						Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Some node did not recieve ack..\n");
						//prepare to send information back to pattern. for now just delete it.
						Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Waveform 2 did not recieve acknowledgement within specified time period\n");
						Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Needs to send NACK to Pattern?\n");
						//Delete corresponding infromation
						ClearMap(wfid,it->First());
						//destroy framworkMsgMap entry
						FMsgMap ::Iterator it_fmsg = mc->framworkMsgMap->Find(it->First());
						if(it_fmsg == mc->framworkMsgMap->End()){
							Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::framworkMsgMap not found for msgId %d\n",it->First());
						}else{
							FMessageQElement* fmsg_ptr = it_fmsg->Second();
							Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Deleting framworkMsgMap for msgId %d at %p\n",it->First(),fmsg_ptr);
							mc->framworkMsgMap->Erase(it->First());
							delete(fmsg_ptr);
							fmsg_ptr = NULL;
							Debug_Printf(DBG_CORE_DATAFLOW, "Before Erasing entry from msgIdToPidMap, make sure the entry exist for this msgId\n");
							BSTMapT <FMessageId_t , PatternId_t> ::Iterator it_msg = msgIdToPidMap.Find(it->First());
							if(it_msg == msgIdToPidMap.End()){
								Debug_Printf(DBG_CORE_DATAFLOW, "Corresponding pid does not exist. Maybe this is not Pattern message\n");
							}else{
								Debug_Printf(DBG_CORE_DATAFLOW, "Deleting entry 4 (%d, %d) from msgIdToPidMap\n",it->First(),msgIdToPidMap[it->First()]);
								msgIdToPidMap.Erase(it->First());
							}

						}
						//Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: Clear Send_Busy[%d]\n",wfid);
						//                       Send_Busy[wfid] = false;
					}  

					if(ack_received != qElement->noOfDest){
						//timer expired but not all ack received. 
						Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Some node did not recieve ack..\n");
						//prepare to send information back to pattern. for now just delete it.
						Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Waveform 2 did not recieve acknowledgement within specified time period\n");
						Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Needs to send NACK to Pattern?\n");
						//Delete corresponding infromation
						ClearMap(wfid,it->First());
					}
				}
			}
			//move iterator to next one
			++it;

		} //End while
	}//End for 
}

bool PacketHandler::PrepareAndSendDataStatusResponse( FMessageId_t  msgId)
{
	///Check if msg exist in FMsgMap
	FMsgMap::Iterator ii= mc->framworkMsgMap->Find(msgId);
	if(ii == mc->framworkMsgMap->End()){
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::PrepareAndSendDataStatusResponse:: Somethig seriously wrong: %d is not in MC, skipping\n",msgId);
		return false;
	}

	////Check if pid exist
	PatternId_t pid;
	Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Before getting Pid, check msgId %d exists in map\n",msgId);
	BSTMapT <FMessageId_t , PatternId_t> ::Iterator it_msg = msgIdToPidMap.Find(msgId);
	if(it_msg == msgIdToPidMap.End())
	{
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::msgId %d does not have entry in msgIdToPidMap. Maybe this is not pattern messsage\n",msgId);
		pid = 0;

		if(ii->Second()->broadcast && ii->Second()->dnSentByWF_remaining == 0)
		{
			FMessageQElement* fmsg_ptr = ii->Second();
			Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::deleting framworkMsgMap for msgId %d at %p\n",msgId,fmsg_ptr);
			mc->framworkMsgMap->Erase(msgId);
			delete(fmsg_ptr);
			fmsg_ptr =NULL;

			FMsgMap::Iterator it_fmsg_test = mc->framworkMsgMap->Find(msgId);
			if(it_fmsg_test == mc->framworkMsgMap->Find(msgId)){
				Debug_Printf(DBG_CORE_DATAFLOW, "PackHandler:: messageId %d is successfully erased\n",msgId);
			}else{
				Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: !? messageId %d found. Why",msgId);
			}

			for(uint16_t wf_index =0; wf_index < MAX_WAVEFORMS; wf_index++){
				BSTMapT <FMessageId_t , WF_MessageQElement* >::Iterator it_qElem = msgToQElement[wf_index].Find(msgId);
				if(it_qElem == msgToQElement[wf_index].End()){
					//Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: msgId %d does not found in msgToQElement[%d] map!\n",msgId,wf_index);
				}else{
					Debug_Printf(DBG_CORE_DATAFLOW, "PackeetHandler:: msgId %d found. erase this entry from msgToQElement[%d]\n",msgId, wf_index);
					msgToQElement[wf_index].Erase(msgId);
				}
			}
			Debug_Printf(DBG_CORE_DATAFLOW, "Before Erasing entry from msgIdToPidMap, make sure the entry exist for this msgId\n");
			BSTMapT <FMessageId_t , PatternId_t> ::Iterator it_msg = msgIdToPidMap.Find(msgId);
			if(it_msg == msgIdToPidMap.End()){
				Debug_Printf(DBG_CORE_DATAFLOW, "Corresponding pid does not exist. Maybe this is not Pattern message\n");
			}else{
				//Debug_Printf(DBG_CORE_DATAFLOW, "Deleting entry 1 (%d, %d) from msgIdToPidMap\n",fmid,msgIdToPidMap[fmid]);
				msgIdToPidMap.Erase(msgId);
			}
			//Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Deleting entry %d entry from msgIdToPidMapp\n",fmid);
		}
		return true;
	}
	else{
		pid = msgIdToPidMap[msgId];
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::pid is %d\n",pid);
	}
	//create the param
	DataStatusParam param;
	FMessageQElement *fmap_element = ii->Second();
	param.nonce = fmap_element->nonce;
	param.noOfDest=fmap_element->noOfDest;

	//deal with broacast
	if(fmap_element->broadcast == true)
	{
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::This message %d is broadcast. Skip this message\n",msgId);
		if(fmap_element->dnFromDest_remaining==0){
			param.statusType[0]=PDN_DST_RECV;
			param.statusValue[0]=true;
		}
		else if(fmap_element->dnSentByWF_remaining==0){
			param.statusType[0]=PDN_WF_SENT;
			param.statusValue[0]=true;
		}else if(fmap_element->dnRecvByWF_remaining==0){
			param.statusType[0]=PDN_WF_RECV;
			param.statusValue[0]=true;
		}else {
			param.statusType[0]=PDN_FW_RECV;
			param.statusValue[0]=true;
		}

		if(fmap_element->broadcast && fmap_element->dnSentByWF_remaining == 0){
			FMessageQElement* fmsg_ptr = ii->Second();
			Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::deleting framworkMsgMap for msgId %d at %p\n",msgId,fmsg_ptr);
			mc->framworkMsgMap->Erase(msgId);
			delete(fmsg_ptr);
			fmsg_ptr =NULL;

			FMsgMap::Iterator it_fmsg_test = mc->framworkMsgMap->Find(msgId);
			if(it_fmsg_test == mc->framworkMsgMap->Find(msgId)){
				Debug_Printf(DBG_CORE_DATAFLOW, "PackHandler:: messageId %d is successfully erased\n",msgId);
			}else{
				Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: !? messageId %d found. Why",msgId);
			}

			for(uint16_t wf_index =0; wf_index < MAX_WAVEFORMS; wf_index++){
				BSTMapT <FMessageId_t , WF_MessageQElement* >::Iterator it_qElem = msgToQElement[wf_index].Find(msgId);
				if(it_qElem == msgToQElement[wf_index].End()){
					Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: msgId %d does not found in msgToQElement[%d] map!\n",msgId,wf_index);
				}else{
					Debug_Printf(DBG_CORE_DATAFLOW, "PackeetHandler:: msgId %d found. erase this entry from msgToQElement[%d]\n",msgId, wf_index);
					msgToQElement[wf_index].Erase(msgId);
				}
			}

			Debug_Printf(DBG_CORE_DATAFLOW, "Before Erasing entry from msgIdToPidMap, make sure the entry exist for this msgId\n");
			BSTMapT <FMessageId_t , PatternId_t> ::Iterator it_msg = msgIdToPidMap.Find(msgId);
			if(it_msg == msgIdToPidMap.End()){
				Debug_Printf(DBG_CORE_DATAFLOW, "Corresponding pid does not exist. Maybe this is not Pattern message\n");
			}else{
				//Debug_Printf(DBG_CORE_DATAFLOW, "Deleting entry 1 (%d, %d) from msgIdToPidMap\n",fmid,msgIdToPidMap[fmid]);
				msgIdToPidMap.Erase(msgId);
			}
			//Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Deleting entry %d entry from msgIdToPidMapp\n",fmid);
		}
	}
	else {  
		Debug_Printf(DBG_CORE_DATAFLOW, "Show FMapMsg noOfDest %d\n",ii->Second()->noOfDest);
		for(int index = 0; index < ii->Second()->noOfDest ; index++){
			NodeId_t node_id = ii.Second()->linkArray[index]->linkId.nodeId;
			param.destArray[index] = node_id;
			if(fmap_element->Check_Item_Exists_dnFromDest(node_id)){
				Debug_Printf(DBG_CORE_DATAFLOW,"PacketHandler::message id %d destination %d received WDN_DST_RECV\n",msgId,node_id);//index);
				param.statusType[index]=PDN_DST_RECV;
				param.statusValue[index]=fmap_element->dnFromDest[node_id];  
			}
			else if(fmap_element->Check_Item_Exists_dnSentByWF(node_id)){
				Debug_Printf(DBG_CORE_DATAFLOW,"PacketHandler::message id %d destination %d received WDN_WF_SENT\n",msgId,node_id);//index);
				param.statusType[index]=PDN_WF_SENT;
				param.statusValue[index]=fmap_element->dnSentByWF[node_id];
			}
			else if(fmap_element->Check_Item_Exists_dnRecvByWF(node_id)){
				Debug_Printf(DBG_CORE_DATAFLOW,"PacketHandler::message id %d destination %d received WDN_WF_RECV\n",msgId,node_id);//index);
				param.statusType[index]=PDN_WF_RECV;
				param.statusValue[index]=fmap_element->dnRecvByWF[node_id];
			}
			else{
				Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Node %d has not received acknowledgment\n",node_id);
				param.statusType[index]=PDN_FW_RECV;
				param.statusValue[index]=true;
			}
		}	      
	}  
	//message Id was not set.... but sometimes pattern gets correct information....?
	param.messageId = msgId;
	Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: readyToReceive is %d\n",(*patternClients)[pid]->readyToReceive);
	param.readyToReceive = (*patternClients)[pid]->readyToReceive;
	eventDispatcher->InvokeDataStatusEvent(pid,param);

	if(fmap_element->dnFromDest_remaining==0){
		FMessageQElement* fmsg_ptr = ii->Second();
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::deleting framworkMsgMap for msgId %d at %p\n",msgId,fmsg_ptr);
		mc->framworkMsgMap->Erase(msgId);
		delete(fmsg_ptr);
		fmsg_ptr =NULL;

		FMsgMap::Iterator it_fmsg_test = mc->framworkMsgMap->Find(msgId);
		if(it_fmsg_test == mc->framworkMsgMap->Find(msgId)){
			Debug_Printf(DBG_CORE_DATAFLOW, "PackHandler:: messageId %d is successfully erased\n",msgId);
		}else{
			Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: !? messageId %d found. Why",msgId);
		}

		for(uint16_t wf_index =0; wf_index < MAX_WAVEFORMS; wf_index++){
			BSTMapT <FMessageId_t , WF_MessageQElement* >::Iterator it_qElem = msgToQElement[wf_index].Find(msgId);
			if(it_qElem == msgToQElement[wf_index].End()){
				Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: msgId %d does not found in msgToQElement[%d] map!\n",msgId,wf_index);
			}else{
				Debug_Printf(DBG_CORE_DATAFLOW, "PackeetHandler:: msgId %d found. erase this entry from msgToQElement[%d]\n",msgId, wf_index);
				msgToQElement[wf_index].Erase(msgId);
			}
		}
		Debug_Printf(DBG_CORE_DATAFLOW, "Before Erasing entry from msgIdToPidMap, make sure the entry exist for this msgId\n");
		BSTMapT <FMessageId_t , PatternId_t> ::Iterator it_msg = msgIdToPidMap.Find(msgId);
		if(it_msg == msgIdToPidMap.End()){
			Debug_Printf(DBG_CORE_DATAFLOW, "Corresponding pid does not exist. Maybe this is not Pattern message\n");
		}else{
			//Debug_Printf(DBG_CORE_DATAFLOW, "Deleting entry 1 (%d, %d) from msgIdToPidMap\n",fmid,msgIdToPidMap[fmid]);
			msgIdToPidMap.Erase(msgId);
		}
		//Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Deleting entry %d entry from msgIdToPidMapp\n",fmid);
	}
	return true;
}


/********************************************** Timer 2***********************************************************/

void PacketHandler::WDN_DST_RECV_Checker_Handler(uint32_t flag)
{
	//Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::WDN_DST_RECV Check timer expired. frameworkMsgMap size is %d\n",mc->framworkMsgMap->Size());
	//Debug_Printf(DBG_CORE_DATAFLOW, "(%d , %d) \n",x(),y());
	FMsgMap::Iterator it_fmsg = mc->framworkMsgMap->Begin();

	if(it_fmsg == mc->framworkMsgMap->End()){
		//Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: No message in framworkMsgMap\n");
		return;
	}
	Debug_Printf(DBG_CORE_DATAFLOW, "PacketHanndler::operationComplete %d",it_fmsg.Second()->operationComplete);
	while(it_fmsg !=  mc->framworkMsgMap->End() && !(it_fmsg.Second()->operationComplete)){
		FMessageId_t msgId = it_fmsg.First();
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::WDN_DST_RECV_Checker_Handler:: message id %d is in framworkMsgMap\n",msgId);
		//for each waveform used for the msg
		uint16_t wfCountForThisMsg = it_fmsg.Second()->wfCount;
		if (wfCountForThisMsg ==0){
			Debug_Error("PacketHandler::WDN_DST_RECV_Checker_Handler:: Something seriously wrong: message id %d was not sent on any waveform\n",msgId);
		}
		//FMessageQElement* fmsg_qElem = it_fmsg.Second();
		bool timeOutHappened[wfCountForThisMsg];
		//I just feel like initializing array just in case..
		for(uint16_t i=0; i < wfCountForThisMsg; i++){
			timeOutHappened[i] = false;
		}
		for(uint k=0;k< wfCountForThisMsg; k++){
			WaveformId_t wfid = it_fmsg.Second()->waveform[k];
			//WF_MessageQElement *wfQElement =  it_fmsg.Second()->wfQElement[k];
			//calculate correct threshold value.
			uint32_t Threshold = WDN_DST_RECV_Timeout_Period[wfid] *10;

			//debugging
			WF_MessageQElement *debug_ptr = 0;
			BSTMapT <FMessageId_t , WF_MessageQElement* > ::Iterator debug = msgToQElement[wfid].Find(msgId);
			if(debug != msgToQElement[wfid].End()){
				debug_ptr =  debug->Second();
				Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: Show WfQElement ptr via msgtoQElement %p\n",debug_ptr);
			}else{
				continue;
			}

			// 		if(!wfQElement){
			// 			Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: WF quele element is missing for %d message %d waveform \n", msgId, wfid);
			// 			continue;
			// 		}
			Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::msgToQElement size is %d\n", msgToQElement[wfid].Size());
			//get first QElement  in map.
			Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::msgId %d, wfid %d\n",msgId, wfid);

			//FMessageId_t msgId = it->First();
			PatternId_t pid;
			Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Before getting Pid, check msgId %d exists in map\n",msgId);
			BSTMapT <FMessageId_t , PatternId_t> ::Iterator it_msg = msgIdToPidMap.Find(msgId);
			if(it_msg == msgIdToPidMap.End()){
				Debug_Printf(DBG_CORE_DATAFLOW, "msgId %d does not have entry in msgIdToPidMap. Maybe this is not pattern messsage\n",msgId);
				pid = 0;
			}else{
				pid = msgIdToPidMap[msgId];
				Debug_Printf(DBG_CORE_DATAFLOW, "pid is %d\n",pid);
			}

			FMsgMap::Iterator ii = mc->framworkMsgMap->Find(msgId);
			if(ii != mc->framworkMsgMap->End()){
				Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::current remaining node on this msgId is %d\n",ii->Second()->dnFromDest_remaining);
			}
			//uint16_t noOfDestinations = wfQElement->noOfDest;
			Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Checking messageId %d\n", msgId);
			//Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Show stored time. %ld::%ld\n",wfQElement->Waiting_ACK.tv_sec, wfQElement->Waiting_ACK.tv_usec);
			Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Show stored time. %ld::%ld\n",debug_ptr->Waiting_ACK.tv_sec, debug_ptr->Waiting_ACK.tv_usec);
			struct timeval current_time;
			gettimeofday(&current_time , NULL);
			//if qElement is waiting for acknowledgment more than 40msec, need to clear it.
			if(current_time.tv_sec == debug_ptr->Waiting_ACK.tv_sec){
				if(current_time.tv_usec - debug_ptr->Waiting_ACK.tv_usec > Threshold){// WDN_DST_RECV_Timeout_Period[wfid]){
					Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::WDN_DST_RECV_Timeout_Period is %d\n", Threshold);//WDN_DST_RECV_Timeout_Period[wfid]);
					timeOutHappened[k]=true;
				}
			}
		}
		Debug_Printf(DBG_CORE_DATAFLOW,"Checking if all waveforms timed out or not.\n");
		Debug_Printf(DBG_CORE_DATAFLOW,"number of waveform is %d\n",wfCountForThisMsg);
		//Check if timeout happened on all waveforms
		bool sendNack=true;
		for(uint32_t k=0;k< wfCountForThisMsg; k++){
			sendNack &= timeOutHappened[k];
			if(timeOutHappened[k]){
				Debug_Printf(DBG_CORE_DATAFLOW,"timemOUtHappened[%d] is %d\n",k,timeOutHappened[k]);
			}
		}
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::sendNack is %d\n",sendNack);
		if(sendNack){
			PrepareAndSendDataStatusResponse(msgId);
			it_fmsg.Second()->operationComplete=true;

			//Dont Delete here
			//ClearMap(wfid,msgId);
		}
		//Move on to next message in fmsgMap
		++it_fmsg;
	}
	//Debug_Printf(DBG_CORE_DATAFLOW, "Exhausted all waveform, so safe to clearCheck if all infromation for msgId %d on pid %d is received\n",msgId, patternId);
}

void PacketHandler::ClearMap(WaveformId_t wfid, FMessageId_t msgId)
{
	//Debug_Printf(DBG_CORE_DATAFLOW, "MessageId is %d\n",messageList[wfid].GetItem(0));
	Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::About to clear Map entry. Make sure the corresponding messageID exists in map\n");
	BSTMapT <FMessageId_t , WF_MessageQElement*>:: Iterator it = msgToQElement[wfid].Find(msgId);
	if(it == msgToQElement[wfid].End()){
		Debug_Error("PacketHandler::ClearMap: Illegal. MessageID not found in msgToQElement map\n");
	}else{
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::ClearMap messageId found\n");
	}
	WF_MessageQElement* qElement = msgToQElement[wfid][msgId];
	//Debug_Printf(DBG_CORE_DATAFLOW, "Deleting messageList[%d] %d \n",wfid, messageList[wfid].GetItem(0));
	//Debug_Printf(DBG_CORE_DATAFLOW, "Remove oldest message Id from list\n");
	//messageList[wfid].Delete(0);
	//BSTMapT <FMessageId_t , WF_MessageQElement*>:: Iterator it;
	for(it = msgToQElement[wfid].Begin(); it != msgToQElement[wfid].End(); ++it){
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::ClearMap: message ID associated to this qElement is %d\n", it->First());
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::ClearMap: message ID associated to this qElement is %d\n", it->Second()->frameworkMsgId);
	}
	bool debug_flag = false;
	for(it = msgToQElement[wfid].Begin(); it != msgToQElement[wfid].End(); ++it){
		if(it->First() == msgId){
			Debug_Printf(DBG_CORE_DATAFLOW ,"PacketHandler::ClearMap: Found matching messageId %d. Erase this entry\n",it->First());
			debug_flag = true;
			break; 
		} 
	}
	if(debug_flag == false){
		Debug_Error("PacketHandler::ClearMap: No matching messageId found! Not possible to get messageID for message not sent\n");
	}
	msgToQElement[wfid].Erase(it); 
	//delete(qElement->msg);
	//qElement->msg = NULL;
	delete(qElement);
	qElement = NULL;
	deletecount++;
}              

//--------------------------------------------------------

void PacketHandler::CleanMC_Handler(uint32_t flag){
	//Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::CleanMC_Handler timer expired.\n");
	//get first exlement in framworkMsgMap
	//Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::CleanMC_Handler: Show Size of framworkMsgMap: %d\n",mc->framworkMsgMap->Size());
	FMsgMap::Iterator it_fmsg = mc->framworkMsgMap->Begin();
	while(it_fmsg != mc->framworkMsgMap->End()){
		FMessageId_t msgId = it_fmsg->First();
		FMessageQElement * fmsg_ptr = it_fmsg->Second();
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: Checking msgId %d\n",msgId);
		if(fmsg_ptr->dnRecvByWF_remaining != 0){
			Debug_Printf(DBG_CORE_DATAFLOW, "dnRecvByWF_remaining is not 0\n");
			++it_fmsg;
			continue;
		}
		if(fmsg_ptr->dnSentByWF_remaining != 0){
			Debug_Printf(DBG_CORE_DATAFLOW, "dnSentByWF_remaining is not 0\n");
			++it_fmsg;
			continue;
		}
		if(fmsg_ptr->dnFromDest_remaining != 0){
			Debug_Printf(DBG_CORE_DATAFLOW, "dnRecvByWF_remaining is not 0\n");
			++it_fmsg;
			continue;
		}
		Debug_Printf(DBG_CORE_DATAFLOW, "CleanMC_Handler::msgId %d is not waiting for anything. Delete it\n",msgId);
		mc->framworkMsgMap->Erase(it_fmsg);
		delete(fmsg_ptr->msg);
		fmsg_ptr->msg = NULL;
		Debug_Printf(DBG_CORE_DATAFLOW, "CleanMC_Handler::Before Erasing entry from msgIdToPidMap, make sure the entry exist for this msgId\n");
		BSTMapT <FMessageId_t , PatternId_t> ::Iterator it_msg = msgIdToPidMap.Find(msgId);
		if(it_msg == msgIdToPidMap.End()){
			Debug_Printf(DBG_CORE_DATAFLOW, "CleanMC_Handler::Corresponding pid does not exist. Maybe this is not Pattern message\n");
		}else{
			Debug_Printf(DBG_CORE_DATAFLOW, "CleanMC_Handler::Deleting entry (%d, %d) from msgIdToPidMap\n",msgId,msgIdToPidMap[msgId]);
			msgIdToPidMap.Erase(msgId);
		}
		++it_fmsg;
		deletecount++;
	}
	//Debug_Printf(DBG_CORE_DATAFLOW, "Finished cleaning...")
}



void PacketHandler::CleanUpMsg( FMessageId_t  msgId)
{
	mc->CleanUp(msgId);
	for (uint16_t i=0; i<MAX_WAVEFORMS; i++){
		msgToQElement[i].Erase(msgId);
	}

	Debug_Printf(DBG_CORE_DATAFLOW, "CleanUpMsg::Before Erasing entry from msgIdToPidMap, make sure the entry exist for this msgId\n");
	BSTMapT <FMessageId_t , PatternId_t> ::Iterator it_msg = msgIdToPidMap.Find(msgId);
	if(it_msg == msgIdToPidMap.End()){
		Debug_Printf(DBG_CORE_DATAFLOW, "CleanUpMsg:: Corresponding pid does not exist. Maybe this is not Pattern message\n");
	}else{
		Debug_Printf(DBG_CORE_DATAFLOW, "CleanUpMsg:: Deleting entry 8 (%d, %d) from msgIdToPidMap\n",msgId,msgIdToPidMap[msgId]);
		msgIdToPidMap.Erase(msgId);
	}
}


void PacketHandler::Add_Node(PatternId_t patternId, FMessageId_t  msgId, NodeId_t* destArray, uint16_t noOfNbrs, LinkComparatorTypeE lcType, AddDestinationResponse_Data& data )
{
	//find if msgId exist or not
	Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: Trying to add destinations to msgId %d\n",msgId);

	//Get hold of Framework Message.
	FMsgMap::Iterator it_fmsg = mc->framworkMsgMap->Find(msgId);
	if(it_fmsg == mc->framworkMsgMap->End()){
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::messageId does not exist on framework.\n");
		data.msgId = msgId;
		data.status = false;            //indicates payload was replaced sccessfully or not
		data.noOfDest = 0;      //no of node in destArray
		return;
	}
	FMessageQElement* fmsg_ptr = it_fmsg->Second();
	Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: msg ptr %p,  payload ptr %p\n",fmsg_ptr->msg, fmsg_ptr->msg->GetPayload());
	uint16_t current_noOfDest = fmsg_ptr->noOfDest;
	//stores valid node for each destination
	ListT <uint64_t , false, EqualTo<uint64_t> > destList[MAX_WAVEFORMS];
	bool found = false;
	//first check if given node is valid destination or not
	for(uint16_t i=0; i< noOfNbrs; i++){
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: First check this node %d is not already in the list\n",destArray[i]);
		for(uint16_t index =0; index < fmsg_ptr->noOfDest; index++){
			if(fmsg_ptr->linkArray[index]->linkId.nodeId == destArray[i]){
				Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: This node %d is already exits\n",destArray[i]);
				found = true;
				break;
			}
		}
		if(found){
			found = false;
			continue;
		}
		//fmsg_ptr->linkArray[current_noOfDest]= coreNbrTable->GetBestQualityLink(destArray[i]);
		//LinkComparatorI* comp = new QualityComparator();
		//TODO: BK: I think this choice should be policy based. This interface neeeds to be redesigned

		//		QualityComparator comp;
		//		fmsg_ptr->linkArray[current_noOfDest]= coreNbrTable->GetLinkwComp(destArray[i], comp);
		fmsg_ptr->linkArray[current_noOfDest] = fwBase->GetPolicyBasedLinkForNeighbor(destArray[i], lcType);

		if(fmsg_ptr->linkArray[current_noOfDest]){
			Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: Node %d is on wf %d. Adding node\n",destArray[i],fmsg_ptr->linkArray[current_noOfDest]->linkId.waveformId);
//			uint64_t node = destArray[i];
			string wfAddress = addressMap->LookUpWfAddress(fmsg_ptr->linkArray[current_noOfDest]->linkId);
			uint64_t node = NbrUtils::ConvertEthAddressToU64(wfAddress);
			destList[fmsg_ptr->linkArray[current_noOfDest]->linkId.waveformId].InsertBack(node);
			current_noOfDest++;
		}else{
			Debug_Printf(DBG_CORE_DATAFLOW,"PacketHandler:: Node %d not present in Neighbor Table pattern %d should check its logic\n", destArray[i], patternId); fflush(stdout);
			data.add_status[i] = false;
			data.destArray[i] = destArray[i];
			//nbrsNotFound[notFound] = destArray[i];
			//notFound++;
		}
	}
	Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: For each wavefrom send new destination to be added\n");
	//Now created per waveform destination list.
	for(uint16_t wf_index =0; wf_index < MAX_WAVEFORMS; wf_index++){
		if(destList[wf_index].Size() != 0){
			Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: destList[%d] size is %d\n",wf_index, destList[wf_index].Size());
			uint64_t dest[destList[wf_index].Size()];
			for(uint16_t index =0; index < destList[wf_index].Size(); index++){
                dest[index] = destList[wf_index].GetItem(index);
                Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: dest[%" PRIu16 "] is %" PRIu64 " \n",index, dest[index]);
			}
			//now created uint64_ array, call Add_node method of MC.
			MessageCabinet::AddNodeReturnValue_t rv =mc->Add_Node(fmsg_ptr,dest,destList[wf_index].Size(), wf_index, 1 ,msgId, patternId);
			if(rv == MessageCabinet::AddNodeReturnValue_t::ADDNODERV_ASK_PATTERN){
				Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: Send inquery to wf %d",wf_index);

				auto wfMsgToFMsgMap_itt = wfMsgToFMsgMap.Find(fmsg_ptr->wfQElement[wf_index].Begin()->GetData()->wfmsgId);
				if(wfMsgToFMsgMap_itt != wfMsgToFMsgMap.End()){
					FragmentList* fragmentlist_ptr = wfMsgToFMsgMap_itt->Second();
					for(auto fragmentmap_itt = fragmentlist_ptr->fragmentmap.RBegin(); fragmentmap_itt != fragmentlist_ptr->fragmentmap.REnd(); ++fragmentmap_itt){
						bool isFragReceivedbyWF = false;
						for(auto fragmentdelstatus_it = fragmentmap_itt->Second().Begin(); fragmentdelstatus_it != fragmentmap_itt->Second().End(); ++fragmentdelstatus_it){
							if(fragmentdelstatus_it->Second() >= FRAG_WF_RECV){
								isFragReceivedbyWF = true;
								break;
							}
						}
						if(isFragReceivedbyWF){
							Core::WFControl::wfControl->Send_AddDestinationRequest(wf_index, GetNewReqId(), fragmentmap_itt->First(), dest, destList[wf_index].Size());
						}
					}
				}else{
					Debug_Error("MessageCabinet::Add_Node ERROR! Cannot find the fragmentation list");
				}
			}

			else if(rv == MessageCabinet::AddNodeReturnValue_t::ADDNODERV_FAIL){
				Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: Too late to fulfill this request.\n");
				for(uint16_t index =0; index < destList[wf_index].Size();index++){
					data.add_status[data.noOfDest] = false;
					data.destArray[data.noOfDest] = dest[index];
					++data.noOfDest;
				}
			}

			else if(rv == MessageCabinet::AddNodeReturnValue_t::ADDNODERV_DEST_ADDED_TO_EXISTING_WF_MSG){
				Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler::Message was succesfully added. Updating noOfDest %d\n", data.noOfDest);
				for(uint16_t index =0; index < destList[wf_index].Size();index++){ //Add these to the outside AddDestinationResponse_Data to trigger an update after return
					data.add_status[data.noOfDest] = true;
					data.destArray[data.noOfDest] = dest[index];
					++data.noOfDest;
				}
			}



			else if(rv == MessageCabinet::AddNodeReturnValue_t::ADDNODERV_ADDED_AS_NEW_WF_MSG){
				Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: Added destination on wf whose had no destination. So start fetching on wf %d\n",wf_index);

//				for(uint16_t index =0; index < destList[wf_index].Size();index++){//Add these to the outside AddDestinationResponse_Data to trigger an update after return
//					data.add_status[data.noOfDest] = true;
//					data.destArray[data.noOfDest] = dest[index];
//					++data.noOfDest;
//				}

				AddDestinationResponse_Data data2;
				data2.msgId = fmsg_ptr->messageId;
				data2.noOfDest = 0;
				data2.status = true;
				for(uint16_t index =0; index < destList[wf_index].Size();index++){
					data2.add_status[data.noOfDest] = true;
					data2.destArray[index] = dest[index];
					++data2.noOfDest;
				}
				ControlResponseParam param;
				param.data = &data;
				param.type = PTN_AddDestinationResponse;
				PatternId_t patternId = msgIdToPidMap[fmsg_ptr->messageId];
				eventDispatcher->InvokeControlEvent(patternId, param);

				ProcessOutgoingMessages(wf_index);
			}

		}else{
			Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: wf %d has no additonal destinations\n",wf_index);
		}
	}


}




void PacketHandler::ReplacePayloadRequest(uint16_t sizeOfPayload, PatternId_t patternId, FMessageId_t  msgId, void* payload, ReplacePayloadResponse_Data& data){

	FMsgMap::Iterator it_fmsg = mc->framworkMsgMap->Find(msgId);
	if(it_fmsg == mc->framworkMsgMap->End()){
		Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet::messageId does not exist on framework.\n");
		data.msgId = msgId;
		data.status = false;
		data.noOfDest = 0;      //no of node in destArray
		data.SetAllStatusNegative();
		return;
	}
	FMessageQElement* fmsg_ptr = it_fmsg->Second();

	if(fmsg_ptr->n_pending_replace_payload_requests != 0){ //There is an ongoing request --> reject this one
		data.msgId = msgId;
		data.status = false;            //indicates payload was replaced sccessfully or not
		data.noOfDest = 0;      //no of node in destArray
		data.SetAllStatusNegative();

		return;
	}
	else{
		//create WaveformId_t array stores wf id which requires wf_inquery
		WaveformId_t wf_inquery[MAX_WAVEFORMS];
		uint16_t wf_index = 0;
		//call ReplacePayloadRequest;
		wf_index = mc->ReplacePayloadRequest(sizeOfPayload, patternId, msgId, payload, wf_inquery, data);

		if(wf_index == 0){
			Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: No wf inquery is requested by MC\n");
			return;
		}
		else{
			//Save the payload pointer and size to be used later
			fmsg_ptr->replace_payload_ptr = payload;
			fmsg_ptr->replace_payload_ptr_size = sizeOfPayload;

			//check unique wf in list.
			WaveformId_t unique_wf[wf_index];
			unique_wf[0] = wf_inquery[0];
			uint16_t unique_count =1;
			bool found;
			for(uint16_t i =1 ; i < wf_index; i ++){
				found = false;
				for(uint16_t j =0; j < unique_count; j++){
					if(unique_wf[j] ==wf_inquery[i]){
						found = true;
						break;
					}
				}
				if(found ==false){
					unique_wf[unique_count] = wf_inquery[i];
					unique_count++;
				}
			}





			for(uint16_t index = 0; index < unique_count ; index++){
				//For each unique waveform find the WFMessageIds that correspond to this FrameworkMessage check the fragments
				auto tempwfmsgid = fmsg_ptr->wfQElement[unique_wf[index]].Begin()->GetData()->wfmsgId;
				auto wfMsgToFMsgMap_itt = wfMsgToFMsgMap.Find(tempwfmsgid);
				if(wfMsgToFMsgMap_itt == wfMsgToFMsgMap.End()){
					Debug_Error("PacketHandler::ReplacePayloadRequest Cannot find the fragment list ");
				}
				else{//Find the fragment that needs to be replaced first
					FragmentList* fragmentlist_ptr = wfMsgToFMsgMap_itt->Second();
					auto fragmentmap_itt = fragmentlist_ptr->fragmentmap.RBegin();
					if(fragmentmap_itt != fragmentlist_ptr->fragmentmap.REnd()){
						//Prepare the first segment
						uint16_t prepared_segment_size;
						void * prepared_payload;
							//Find the attributes of the waveform
	  					WF_AttributeMap_t::Iterator wf_attribute_map_it = wfAttributeMap->Find(unique_wf[index]);
						if(wf_attribute_map_it == wfAttributeMap->End()){
							Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabiniet:: Replace_Ptr cannot find waveform\n");
							return;
						}
						uint16_t max_segment_size = wf_attribute_map_it->Second().maxPayloadSize;
						uint16_t numberOfMsgSegmentsRequired = sizeOfPayload / max_segment_size; if(sizeOfPayload % max_segment_size > 0 ) ++numberOfMsgSegmentsRequired;



						if(sizeOfPayload < max_segment_size) prepared_segment_size = sizeOfPayload;
						else prepared_segment_size = max_segment_size;
						prepared_payload = payload;

						Debug_Printf(DBG_CORE_DATAFLOW, "Send inquery to wf %d\n",unique_wf[index]);
						++fmsg_ptr->n_pending_replace_payload_requests;
						Core::WFControl::wfControl->Send_ReplacePayloadRequest(unique_wf[index], GetNewReqId(), fragmentmap_itt->First(), (uint8_t *)prepared_payload, prepared_segment_size);

					}
					else{
						Debug_Error("PacketHandler::ReplacePayloadRequest Fragment list is empty");
					}

				}

			}
		}

	}


	return;
}

void PacketHandler::CancelDataSendRequest(PatternId_t patternId, FMessageId_t  msgId, NodeId_t *destArray, uint16_t noOfDest, CancelDataResponse_Data& data ){
	Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: Entered CancelDataSendRequest, msgId %d\n",msgId);
	FMsgMap::Iterator it_fmsg = mc->framworkMsgMap->Find(msgId);
	if(it_fmsg == mc->framworkMsgMap->End()){
		Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet::messageId does not exist on framework.\n");
		data.msgId = msgId;
		data.status = false;            //indicates payload was replaced sccessfully or not
		data.noOfDest = 0;      //no of node in destArray
		return;
	}
	FMessageQElement* fmsg_ptr = it_fmsg->Second();
	//create wf array
	WaveformId_t wf_inquery[noOfDest];
	uint16_t wf_index =0;
	bool decrement_outstandinigpkt = false;
	wf_index = mc->CancelDataSendRequest(patternId, msgId, destArray, noOfDest, wf_inquery, data, decrement_outstandinigpkt);


	//check if all dest is removed or not
	it_fmsg = mc->framworkMsgMap->Find(msgId);
	if(it_fmsg == mc->framworkMsgMap->End()){
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: All destination has been removed. Adjust noOfOutstandingPkt %d\n",(*this->patternClients)[patternId]->noOfOutstandingPkts);
		(*this->patternClients)[patternId]->noOfOutstandingPkts--;
		return;
	}else{
		if(decrement_outstandinigpkt){
			Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: Removal of node caused dnRecvByWF to be zero. Adjust noOfOutstandingPkt %d\n",(*this->patternClients)[patternId]->noOfOutstandingPkts);
			(*this->patternClients)[patternId]->noOfOutstandingPkts--;
		}
	}

	if(wf_index == 0){
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: No wf inquery is posted by MC\n");
	}
	else{ //Ask the waveforms (that needs to be asked) to cancel the nodes
		//check unique wf in list.
		WaveformId_t unique_wf[MAX_WAVEFORMS];
		unique_wf[0] = wf_inquery[0];
		uint16_t unique_count =1;
		bool found;
		for(uint16_t i =1 ; i < wf_index; i ++){
			found = false;
			for(uint16_t j =0; j < unique_count; j++){
				if(unique_wf[j] ==wf_inquery[i]){
					found = true;
					break;
				}
			}
			if(found ==false){
				unique_wf[unique_count] = wf_inquery[i];
				unique_count++;

				//Prepare dest list for this unique wf
			}
		}
		for(uint16_t index = 0; index < unique_count ; index++){
			//For each unique waveform find the WFMessageIds that correspond to this FrameworkMessage check the fragments
			auto tempwfmsgid = fmsg_ptr->wfQElement[unique_wf[index]].Begin()->GetData()->wfmsgId;
			auto wfMsgToFMsgMap_itt = wfMsgToFMsgMap.Find(tempwfmsgid);
			if(wfMsgToFMsgMap_itt == wfMsgToFMsgMap.End()){
				Debug_Error("PacketHandler::CancelDataSendRequest Cannot find the fragment list ");
			}
			else{
				FragmentList* fragmentlist_ptr = wfMsgToFMsgMap_itt->Second();
				for(auto fragmentmap_itt = fragmentlist_ptr->fragmentmap.RBegin(); fragmentmap_itt != fragmentlist_ptr->fragmentmap.REnd(); ++fragmentmap_itt){ //BK: Traverse fragment list
					//For each fragment find the list of nodes that are ordered to be cancelled, in the Fmessage list, and that can be cancelled
					uint64_t* deswf = new uint64_t[noOfDest]; //store the wf_node_address of the nodes to be canncelled on this unique wf here
					uint ndestwf = 0;	// Store number of neighbors that will be canncelled on this waveform
					for(uint16_t index2 = 0; index2 < it_fmsg->Second()->noOfDest ; index2++){
						if( unique_wf[index] == it_fmsg->Second()->waveform[index2] ){
							for(uint16_t i = 0; i < noOfDest ; ++i){
								if(destArray[i] == it_fmsg->Second()->linkArray[index2]->linkId.nodeId){

									string wfAddress = addressMap->LookUpWfAddress(it_fmsg->Second()->linkArray[index2]->linkId);//Convert Destination Addresses to WF Addresses
									uint64_t node_wf_address = NbrUtils::ConvertEthAddressToU64(wfAddress);
									auto fragmentdelstatus_it = fragmentmap_itt->Second().Find( node_wf_address );
									if(fragmentdelstatus_it != fragmentmap_itt->Second().End()){
										if(fragmentdelstatus_it->Second() == FRAG_WF_RECV){
											deswf[ndestwf++] = node_wf_address;
										}
									}
								}
							}
						}
					}

					if(ndestwf > 0){ //If we have neighbors that we cancel on this waveform
						Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: %d wf_inquery is posted by MC",unique_count);
						Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: process inquery on wfid %d\n",unique_wf[index]);
						Core::WFControl::wfControl->Send_CancelDataRequest(unique_wf[index], GetNewReqId(), fragmentmap_itt->First(), deswf, ndestwf);
					}
				}

			}

		}
	}
}

bool PacketHandler::UpdatemsgToQElem( FMessageId_t  msgId, WaveformId_t wfid, NodeId_t remove_node){
	//check msgToQElement for give message exists;
	BSTMapT <FMessageId_t , WF_MessageQElement*> ::Iterator it_qElem = msgToQElement[wfid].Find(msgId);
	if(it_qElem == msgToQElement[wfid].End()){
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: Something strange. msgId %u has no corresponding WF_MessageQElem.\n",msgId);
		return false;
	}
	//Now safe to access MAp
	WF_MessageQElement* qElem_ptr = it_qElem->Second();
	uint16_t dest_count = qElem_ptr->noOfDest;
	//remove dest from destlist
	for(uint16_t index = 0; index < qElem_ptr->noOfDest ; index++){
		if(qElem_ptr->destArray[index] == remove_node){
			uint16_t shift_index = index;
			for( ; shift_index < dest_count ; shift_index++){
				qElem_ptr->destArray[shift_index] = qElem_ptr->destArray[shift_index+1];
			}
		}
	}
	for(uint16_t index = 0; index < qElem_ptr->noOfDest ; index++){
	    Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: destArray[%d] is %" PRIu64 " \n",index, qElem_ptr->destArray[index]);
	}

	//Update no of dest
	qElem_ptr->noOfDest -= 1;
	if(qElem_ptr->noOfDest ==0){
		Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: remaining no of dest is 0. erase this entry/n");
		BSTMapT <FMessageId_t , WF_MessageQElement* >::Iterator it_qElem = msgToQElement[wfid].Find(msgId);
		if(it_qElem == msgToQElement[wfid].End()){
			Debug_Printf(DBG_CORE_DATAFLOW,"PacketHandler:: Something is wrong. entry does nto exist?");
			return false;
		}
		msgToQElement[wfid].Erase(msgId);
		delete(qElem_ptr);
	}
	return true;
}


void PacketHandler::CalculatePacketWaitTime(WF_MessageQElement* qElement, double* delayMilli)
{
	uint64_t now= SysTime::GetEpochTimeInMicroSec();
	*delayMilli = (now - qElement->metric->submitTimeMicro)/1000;
}




}//End of namespace
}
