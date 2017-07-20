////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#include "MessageCabinet.h"
#include "Framework/Core/Neighborhood/Nbr_Utils.h"
#include "Lib/DS/BSTMapT.h"

extern uint16_t deletecount;
extern uint16_t newcount;
extern NodeId_t MY_NODE_ID;

namespace Core {
namespace Dataflow {


void CreateFMessageQElementArray(FMessageQElement* ar_ptr, std::size_t ar_size, FMessage_t *_msg, MsgPriorityMetric *_metric, FMessageId_t  _id, Link **_linkArray, uint16_t _noOfDest,bool _broadcast, uint16_t _nonce){

}

FMessageQElement::FMessageQElement(FMessage_t *_msg, MsgPriorityMetric *_metric, FMessageId_t  _id, Link **_linkArray, uint16_t _noOfDest,bool _broadcast, uint16_t _nonce){
  dnFromDest_remaining = _noOfDest;
  dnSentByWF_remaining = _noOfDest;
  dnRecvByWF_remaining = _noOfDest;
  operationComplete=false;
  //msg = _msg;
  //creating copy of msg
  msg = new FMessage_t(_msg->GetPayloadSize());
  msg->SetPayload(_msg->GetPayload());
  msg->SetPayloadSize(_msg->GetPayloadSize());
  msg->SetSource(_msg->GetSource());
  msg->SetType(_msg->GetType());
  msg->SetWaveform(_msg->GetWaveform());
  nonce = _nonce;
  metric = _metric;
  messageId=_id;
  noOfDest = _noOfDest;
  wfCount=0;
  broadcast = _broadcast;

  replace_payload_ptr = NULL;
  n_pending_replace_payload_requests = 0;
  replace_payload_ptr_size = 0;

  //old_payload_ptr = NULL;
  //memcpy(destArray, _nodeArray, noOfDest*sizeof(NodeId_t));
  memcpy(linkArray, _linkArray, noOfDest*sizeof(Link*));
}

FMessageQElement::~FMessageQElement(){
	//destructor
    //delete(old_payload_ptr);
    delete(metric);
    //delete(msg); //delete copy of message
}

MessageCabinet::MessageCabinet(WF_AttributeMap_t *_wfAttributes, WFMsgIdToFragmentListMap_t *_wfMsgToFMsgMap, AddressMap *_addressMap) {
	wfAttributes= _wfAttributes;
	wfMsgToFMsgMap = _wfMsgToFMsgMap;
	addressMap = _addressMap;
	frameworkMsgId=0;
	waveformMsgId=0;
	Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: Constructor, wf attribute prt :%p \n",wfAttributes);
	wfMsgHeap= new WF_PriorityMsgHeap[MAX_WAVEFORMS];
	framworkMsgMap = new FMsgMap();
}

void MessageCabinet::SetWaveformAdaptorMap(BSTMapT< WaveformId_t, PacketAdaptor< uint64_t >* >* _wfMsgAdaptorMap)
{
  wfMsgAdaptorMap = _wfMsgAdaptorMap;
}

MessageCabinet::~MessageCabinet() {
  // TODO Auto-generated destructor stub
}

///TODO::This is a dummy function to make code compile, remove this latter
bool MessageCabinet::RemoveFrameworkMessage( FMessageId_t  msgId)
{

  return false;
}

///TODO::This is a dummy function to make code compile, remove this latter.
/*FMessageId_t MessageCabinet::AddNewFrameworkMessage(FMessage_t *msg){

  return false;
}
*/

bool MessageCabinet::RemoveMessageDestination( FMessageId_t  msgId, NodeId_t *destArray, uint16_t noOfDest){

  return false;
}

///Implement this.
bool MessageCabinet::AddMessageDestination( FMessageId_t  msgId, NodeId_t* nodeArray, Link **linkArray,  uint16_t noOfDest)
{

  return false;
}


bool MessageCabinet::AddWaveformMessage(FMessageQElement *fmsgelementptr, WaveformId_t _wfid, bool broadcast, uint64_t* destArray, uint16_t noOfDest, PatternId_t pid){

//	FMsgMap::Iterator it_fmsg = framworkMsgMap->Find(fmsgelementptr->messageId);


//#if ENABLE_FRAGMENTATION == 1

	WF_AttributeMap_t::Iterator it = wfAttributes->Find(_wfid);
	if(it == wfAttributes->End()){
		return false;
	}
	uint16_t numberOfMsgSegmentsRequired = 1;
	uint16_t max_segment_size = it->Second().maxPayloadSize ;
	if(fmsgelementptr->msg->GetPayloadSize() > it->Second().maxPayloadSize){ //Fragmentation is required
		max_segment_size = max_segment_size - sizeof(FragmentationHeader);
		numberOfMsgSegmentsRequired = fmsgelementptr->msg->GetPayloadSize() / max_segment_size;
		if(fmsgelementptr->msg->GetPayloadSize() % max_segment_size > 0 ) ++numberOfMsgSegmentsRequired;
	}

	FragmentList* fragmentlist_ptr = new FragmentList;
	fragmentlist_ptr->frameworkMsgID = fmsgelementptr->messageId;

	for(uint16_t segment =0 ; segment < numberOfMsgSegmentsRequired; ++segment){
		WF_MessageId_t wMsgId = GetNewWaveformMsgId();
		WF_Message_n64_t *wfMsg = new WF_Message_n64_t();

		if(noOfDest == 0){
			fragmentlist_ptr->fragmentmap[wMsgId].Insert(destArray[0], FRAG_FRAG_CREATED);
		}
		else{
			for(uint16_t i = 0; i < noOfDest; ++i){
				fragmentlist_ptr->fragmentmap[wMsgId].Insert(destArray[i],FRAG_FRAG_CREATED);
			}
		}

//		fragmentstatusptr->fragmentStatus.Insert(wMsgId,FRAG_FRAG_CREATED);

		wfMsgToFMsgMap->Insert(wMsgId,fragmentlist_ptr);

//
//		auto fragmentstatusptr = wfMsgToFMsgMap->Find()
//		if(wfMsgToFMsgMap->)
//
//		if(segment == 0) { //For first segment
//			auto tempFragmentationIDListPtr = new WF_Message_FragmentIdList_t();
//			tempFragmentationIDListPtr->Insert(wMsgId, PDN_FW_RECV);
//			wfMsgToFMsgMap->Insert(tempFragmentationIDListPtr,PDN_FW_RECV);
//		}
//		else{
//			wfMsgToFMsgMap->
//		}
//		wfMsgToFMsgMap->Insert()
//
//		if(segment == numberOfMsgSegmentsRequired-1) {
//			if(wfMsgToFMsgMap->Insert(wMsgId, fmsgelementptr->messageId)){
//				Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet: AddWaveformMessage: Adding wfmsg id: %lu, fmMsg id: %u, to wfMsgToFMsgMap\n", wMsgId, fmsgelementptr->messageId);
//			}else {
//				Debug_Warning("MessageCabinet: AddNewFrameworkMessage: Grave Error: Map insert failed! wfMsgToFMsgMap\n");
//			}
//		}


		if(numberOfMsgSegmentsRequired > 1){


			wfMsg->CopyIthSegmentFrom(*(fmsgelementptr->msg), segment, max_segment_size); //This is an actual copy //TODO: Must check the destruction
			wfMsg->SetFlag(WF_FLG_FRAGMENTED);
		}
		else{
			wfMsg->CopyFrom(*(fmsgelementptr->msg));
			//Clear flag
			wfMsg->ClearFlag(WF_FLG_FRAGMENTED);
		}

		wfMsg->SetWaveform(_wfid);
		wfMsg->SetSource(MY_NODE_ID);
		wfMsg->SetNumberOfDest(noOfDest);
		wfMsg->SetWaveformMessageID(wMsgId); //This Id shold be assigned when WF recieved message//Mukundan: Not true;
		//wfMsg->SetType(msg->GetType()); it seems this is done in CopyFrom method
		wfMsg->SetInstance(pid);


		//Debug_Printf(DBG_CORE, "Adding new wf %d to wfList\n",wf);
		if(!fmsgelementptr->broadcast && fmsgelementptr->msg->GetType()==Types::PATTERN_MSG){
			if((*wfAttributes)[_wfid].destReceiveAckSupport == false){
				//if(1){
				Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet::AddNewFrameworkMessage: This waveform %d does not support acknowledgment. Changing waveform packet type to ask for acknowledgement\n",_wfid)
				wfMsg->SetType(Types::ACK_REQUEST_MSG);

			}
		}
		Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: AddNewFrameworkMessage: message type via qelement is %d\n",wfMsg->GetType());
		WF_MessageQElement *element = new WF_MessageQElement(wfMsg, fmsgelementptr->metric, wMsgId, fmsgelementptr->messageId, destArray, noOfDest, broadcast);
		Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet::WF_MessageQElement has mssage at %p, payload at %p\n",wfMsg,wfMsg->GetPayload());

		//Need to set wfQElement pointer array. This pointer is used to get waveform message in Heap.
		fmsgelementptr->wfQElement[_wfid].Insert(element);
//		Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet::Adding WF_MessageQElement ptr %p for fmid %d to wfQElement[%d]\n",it_fmsg->Second()->wfQElement[_wfid],newFMsgId, _wfid);
		Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet::Adding WF_MessageQElement ptr %p for fmid %d to wfQElement[%d]\n",element, fmsgelementptr->messageId, _wfid);
		//FMessageQElement* temp_ptr;
		//temp_ptr = (*framworkMsgMap)[newFMsgId];
		//Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: 4message type via qelement is %d\n",element->msg->GetType());
		//Debug_Printf(DBG_CORE_DATAFLOW, "Address of FMessageQElement is %p\n", temp_ptr);
		//Debug_Printf(DBG_CORE_DATAFLOW,"Going to inserted a qelement: Waveform %d heaps current size is: %d \n", _wfid, wfMsgHeap[ii->First()].Size());fflush(stdout);
		wfMsgHeap[_wfid].Insert(element);

		//Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: 5message type via qelement is %d\n",element->msg->GetType());
		//wfMsgBST[_wfid].Insert(element);
		//Debug_Printf(DBG_CORE_DATAFLOW,"Inserted a qelement(ptr %p), %d elements in heap of waveform %d \n", element, wfMsgHeap[ii->First()].Size(), _wfid);fflush(stdout);

		//debugging
		if(element->msg->GetType() == Types::ACK_REQUEST_MSG){
			SoftwareAcknowledgement* swack1 = (SoftwareAcknowledgement*) element->msg->GetPayload();
			Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet::AddNewFrameworkMessage: Show payload %d %d %lu\n",swack1->src, swack1->wfId, swack1->wfMsgId);
			Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet::AddNewFrameworkMessage: Show payload ptr %p\n", swack1);
		}

	}

	return true;
//#else
//
//	return false;
//#endif
}



FMessageId_t MessageCabinet::AddNewFrameworkMessage( PatternId_t pid, FMessage_t *msg, NodeId_t *nodeArray, Link** linkArray, uint16_t noOfDest, bool broadcast, WaveformId_t wfid, FMessageId_t newFMsgId, uint16_t _nonce, bool softwarebroadcast )
{
	Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:Address of linkArray is %p\n",linkArray);
	Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:Address of FMessage is %p,  payload is %p\n",msg, msg->GetPayload());
	Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: messageId %d, noOfDest %d\n",newFMsgId,noOfDest);
	for(uint16_t index =0; index < noOfDest; index++){
		//Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: destinations are %d\n",nodeArray[index]);
		Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: destinations are %d\n",linkArray[index]->linkId.nodeId);
	}
		///need to check if message exist before adding
	MsgPriorityMetric *_metric = new MsgPriorityMetric();
	_metric->submitTimeMicro = SysTime::GetEpochTimeInMicroSec();
	_metric->priority = PolicyManager::GetAsnpPriority(pid);
	//FMessageId_t newFMsgId = 0;// This is generated in FrameworkBase. Masahiro GetNewFrameworkMsgId();

	//Debug_Printf(DBG_CORE_DATAFLOW,"Show addresss of payload %p\n",msg->GetPayload());

	Debug_Printf(DBG_CORE_DATAFLOW,"MessageCabinet::Inserting message %d, message Type %d,  with priority %u \n",newFMsgId,msg->GetType(), _metric->priority);fflush(stdout);
	//FMessageQElement *element = new FMessageQElement(msg, _metric, newFMsgId, nodeArray, linkArray, noOfDest, broadcast);

	FMessageQElement *element = new FMessageQElement(msg, _metric, newFMsgId, linkArray, noOfDest, broadcast || softwarebroadcast,_nonce);
	

	Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet::Debug: It seems FMessageQElement broadcast value is not correctly set. Read value now to check. %d",element->broadcast);

	for(uint16_t index =0; index < noOfDest; index++){
		Debug_Printf(DBG_CORE_DATAFLOW,"MessageCabinet::dnFromDest[index] initialized to %d\n",element->dnFromDest_remaining);
		Debug_Printf(DBG_CORE_DATAFLOW,"MessageCabinet::dnSentByWF[index] initialized to %d\n",element->dnSentByWF_remaining);
		Debug_Printf(DBG_CORE_DATAFLOW,"MessageCabinet::dnRecvByWF[index] initialized to %d\n",element->dnRecvByWF_remaining);
	}

	Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet: Show address of dnFromDest map %p\n",&element->dnFromDest);
	Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet: Show address of dnSentBYWF map %p\n",&element->dnSentByWF);
	Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet: Show address of dnRecvBYWF map %p\n",&element->dnRecvByWF);

	if(noOfDest == 0){
		//element->dnFromDest.Insert((NodeId_t)0 ,false);
		//element->dnRecvByWF.Insert((NodeId_t)0 ,false);
		//element->dnRecvByWF.Insert((NodeId_t)0 ,false);
		element->dnFromDest_remaining = 1;
		element->dnRecvByWF_remaining = 1;
		element->dnSentByWF_remaining = 1;
	}

	if(noOfDest == 0){
		Debug_Printf(DBG_CORE_DATAFLOW,"MessageCabinet::dnFromDest_remaining_node is initialized to  %d\n",element->dnFromDest_remaining);
		Debug_Printf(DBG_CORE_DATAFLOW,"MessageCabinet::dnSentByWF_remaining_node is initialized to %d\n",element->dnSentByWF_remaining);
		Debug_Printf(DBG_CORE_DATAFLOW,"MessageCabinet::dnRecvByWF_remaining_node is initialized to %d\n",element->dnRecvByWF_remaining);
	}

	//uint16_t message_type = msg->GetType();
	//if msg pattern is Types::Ack_Reply_Type, do not add FMessageQElement in map.
	if(framworkMsgMap->Insert(newFMsgId,element)){
		//printf("MessageCabinet:: after adding element to framworkMsgMap %p for fw message %u\n", element, newFMsgId);
	}else {
		Debug_Error("MessageCabinet:: Insert to message map failed. Why?");
	}	

	Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: after adding entry to framworkMsgMap, checking broadcast value\n");
	FMsgMap::Iterator it_fmsg = framworkMsgMap->Find(newFMsgId);
	if( (it_fmsg == framworkMsgMap->End()) && (msg->GetType() != Types::ACK_REPLY_MSG) ){
		Debug_Error("MessageCabinet::entry not found!?");
		//abort();
	}
	Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet::Broadcast is %d\n",it_fmsg->Second()->broadcast);

	//Find out the waveform ids for each neighbor in the array, seperate them out by waveform id and create seperate messages for each of the waveforms.
	DestMap_t msgWfMap;
	if(broadcast){
		//msgWfMap[wfid].destArray = NULL;
		it_fmsg->Second()->waveform[0] = wfid;
		msgWfMap[wfid].noOfDest =0;
		Debug_Printf(DBG_CORE_DATAFLOW,"MessageCabinet::AddNewFrameworkMessage:: Processing a broadcast message %d, with priority %u \n",newFMsgId, _metric->priority);fflush(stdout);
	}else{
		uint16_t loop_count = noOfDest; 
		uint16_t destArray_index[MAX_WAVEFORMS];
		for(uint16_t i =0; i < MAX_WAVEFORMS; i++) {destArray_index[i]=0;}
		for(uint16_t i=0; i< loop_count; i++){  
			WaveformId_t tempWF = linkArray[i]->linkId.waveformId;
			//Debug_Printf(DBG_CORE_DATAFLOW, "Address of linkArray[%d] is %p\n",i,&linkArray[i]);
			//Debug_Printf(DBG_CORE_DATAFLOW, "Print wf of deestinaton %d, nodeid %d :%p\n",tempWF, linkArray[i]->linkId.nodeId, linkArray[i]);
			Debug_Printf(DBG_CORE_DATAFLOW,"tempWF is %d\n",linkArray[i]->linkId.waveformId);
			//V1.1: AddressMap look up.
			//msgWfMap[tempWF].destArray[destArray_index[tempWF]++] = linkArray[i]->linkId.nodeId;
			string wfAddress = addressMap->LookUpWfAddress(linkArray[i]->linkId);
			msgWfMap[tempWF].destArray[destArray_index[tempWF]++] = NbrUtils::ConvertEthAddressToU64(wfAddress);
			//Debug_Printf(DBG_CORE_DATAFLOW, "Destination %d is %lu, dest ptr is %p\n", msgWfMap[tempWF].noOfDest,  msgWfMap[tempWF].destArray[i], msgWfMap[tempWF].destArray);
			msgWfMap[tempWF].noOfDest++;
			it_fmsg->Second()->waveform[i] = tempWF;
		}
	}

	DestMap_t::Iterator ii = msgWfMap.Begin();
	element->wfCount=msgWfMap.Size();
	WaveformId_t _wfid;
	Debug_Printf(DBG_CORE_DATAFLOW,"MessageCabinet::Pattern message is to be sent across %d waveforms \n",msgWfMap.Size());fflush(stdout);

	for (; ii != msgWfMap.End(); ++ii)
	{
		_wfid = ii->First();
		//printf("MessageCabinet:: Searching through %d number of waveforms in system \n", wfAttributes->Size()); fflush(stdout);
		WF_AttributeMap_t::Iterator it = wfAttributes->Find(_wfid);
		if(it == wfAttributes->End()){
			//printf("MessageCabinet::AddNewFrameworkMessage:: ERROR: Something wrong, waveform %d, does not exist or framework doesnt know about it\n", _wfid);
			if(broadcast){
				//printf("MessageCabinet::AddNewFrameworkMessage:: ERROR: Bad broadcast message on non existent waveform %d\n", _wfid);
				it_fmsg->Second()->dnFromDest_remaining =0;
				it_fmsg->Second()->dnRecvByWF_remaining =0;
				it_fmsg->Second()->dnSentByWF_remaining =0;
			}else {
				it_fmsg->Second()->dnFromDest_remaining -= ii.Second().noOfDest;
				it_fmsg->Second()->dnRecvByWF_remaining -= ii.Second().noOfDest;
				it_fmsg->Second()->dnSentByWF_remaining -= ii.Second().noOfDest;
			}
			continue;
		}
		//TODO: BK: Create waveform messages and Insert into the map

		if(AddWaveformMessage(element, _wfid,  broadcast, ii->Second().destArray, ii->Second().noOfDest, pid)){
			Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: AddWaveformMessage added all segments successfully.");
		}
		else{
			Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: AddWaveformMessage ERROR! Some segments have failed to be added to queue. Why?");
		}

	}
	
	/*if(element->msg->GetType() == Types::ACK_REPLY_MSG){
		Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet::AddNewFrameworkMessage:This message is software acknowledgement. Erase it from map\n");
		//Erase entry from map
		FMsgMap::Iterator fmsg_delete = framworkMsgMap->Find(newFMsgId);
		if(fmsg_delete == framworkMsgMap->End()){
			Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet::AddNewFrameworkMessage: Something strange.... framework entry not found\n");
			abort();
		}else{
			Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: Erase entry as this is Ack_Reply_Type...\n");
			//(*framworkMsgMap).Erase(newFMsgId);
			fmsg_delete = framworkMsgMap->Find(newFMsgId);
			if(fmsg_delete == framworkMsgMap->End()){
				Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: Erased entry.\n");
			}else{
				Debug_Printf(DBG_CORE_DATAFLOW, "MessageCainet::  could not erase?\n");
			}
		}
	}else{
		Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: This message is not software acknowledgement. Add it to map\n");

	}*/

	//noOfMessages++;
	//FMsgMap::Iterator it_fmsg1 = framworkMsgMap->Find(newFMsgId);
	if(it_fmsg != framworkMsgMap->End()){
		Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: FMsgMap entry is at %p, msg_ptr is %p, payload_ptr is %p, msg Type is %d\n",
			it_fmsg->Second(),it_fmsg->Second()->msg, it_fmsg->Second()->msg->GetPayload(), it_fmsg->Second()->msg->GetType());
	}
	return newFMsgId;
}

WF_MessageQElement* MessageCabinet::GetNextMessage(uint16_t waveformId){
  if(wfMsgHeap[waveformId].Size() > 0){
    //WF_MessageQElement *qElement= wfMsgHeap[waveformId].ExtractTop();
    WF_MessageQElement *qElement= wfMsgHeap[waveformId].ExtractTop();
	//noOfMessages--;
    //Debug_Printf(DBG_CORE_DATAFLOW, "Found the message\n");fflush(stdout);
    return qElement;
  }
  else {
   Debug_Printf(DBG_CORE_DATAFLOW,"MessageCabinet::GetNextMessage:: No packets waiting to be sent out on waveform %d\n", waveformId);
  }
  return NULL;
}

FMessageQElement* MessageCabinet::LookUpFrameworkMessage( FMessageId_t  _msgId)
{
	//FMessageQElement* ret =framworkMsgMap->operator[](_msgId);
	FMsgMap::Iterator itfmsg = framworkMsgMap->Find(_msgId);
	if(itfmsg != framworkMsgMap->End()){
		return itfmsg->Second();
	}
	return 0;
}

/*WF_MessageQElement* MessageCabinet::LookUpWaveformMessage( FMessageId_t  _wfMsgId, WaveformId_t wid)
{
  WF_MessageQElement* ret=0;
  //wfMsgHeap[wid].Search(_wfMsgId);
  return ret;
}
*/

/*
WF_MessageBase* MessageCabinet::GetNextMessage(uint16_t waveformId){
  if((*wfMsgHeap)[waveformId].NoOfElements() > 0){
    WF_MessageQElement *qElement= (*wfMsgHeap)[waveformId].ExtractTop();
    noOfMessages--;
    //Debug_Printf(DBG_CORE_DATAFLOW, "Found the message\n");fflush(stdout);
    return qElement->msg;
  }
  else {
   Debug_Printf(DBG_CORE_DATAFLOW, "No packets waiting to be sent out on waveform %d\n", waveformId);
  }
  return NULL;
}
*/

WF_MessageBase* MessageCabinet::GetNextMessageForNeighbor(uint16_t neighborId){
  return NULL;
}


uint32_t MessageCabinet::GetNumberWaveFormOfMessages(){

return 0;
}

uint32_t MessageCabinet::GetNumberWaveformOfMessages(WaveformId_t wfId) {
  uint32_t ret=0;
  //for (uint16_t i=0; i < MAX_WAVEFORMS; i++){
    //Debug_Printf(DBG_CORE_DATAFLOW, "Waveform %d heap ptr(%p): Pending packets: %d\n", i, &wfMsgHeap[i], wfMsgHeap[i].Size());fflush(stdout);
    ret = wfMsgHeap[wfId].Size();
    //ret = wfMsgBST[wfId].NoOfElements();
  //}
  return ret;
}

void MessageCabinet::CleanUp( FMessageId_t  mid)
{
  framworkMsgMap->Erase(mid);
}


/*uint32_t MessageCabinet::RemoveFrameworkMessage(){
  return noOfMessages;
}
*/

FragmentStatusTypeE MessageCabinet::GetHighestFragmentStatusofFmessageDest(FMessageQElement* fmsg_ptr, const Link* link ){
	Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet::GetHighestFragmentStatusofFmessage.\n");
	WaveformId_t wfid = link->linkId.waveformId;

//		Case 1: at least one of the fragments  FRAG_DST_RECV
//		Case 2: None of the fragments are FRAG_DST_RECV
//			 Case 2-1: at least one of the fragments  FRAG_WF_SENT
//			 Case 2-2: none of the fragments FRAG_WF_SENT
//				Case 2-2-1: at least one of the fragments FRAG_WF_RECV
//				CaSe 2-2-2: none of the fragments for none of the destinations FRAG_WF_RECV
//	if not found

	if(fmsg_ptr){
		//Find the waveformMessageQElements corresponding to this destination
		auto wfMsgToFMsgMap_itt = wfMsgToFMsgMap->Find(fmsg_ptr->wfQElement[wfid].Begin()->GetData()->wfmsgId);
		if(wfMsgToFMsgMap_itt != wfMsgToFMsgMap->End()){
			FragmentList* fragmentlist_ptr = wfMsgToFMsgMap_itt->Second();
			bool isCase2_1 = false;
			bool isCase2_2_1 = false;
			for(auto fragmentmap_itt = fragmentlist_ptr->fragmentmap.RBegin(); fragmentmap_itt != fragmentlist_ptr->fragmentmap.REnd(); ++fragmentmap_itt){ //BK: Traverse fragment list
				string wfAddress = addressMap->LookUpWfAddress(link->linkId);
				auto fragmentdelstatus_it = fragmentmap_itt->Second().Find( NbrUtils::ConvertEthAddressToU64(wfAddress) );
				if(fragmentdelstatus_it == fragmentmap_itt->Second().End()){
					return FRAG_DOES_NOT_EXIST;
				}
				else{
					if(fragmentdelstatus_it->Second() >= FRAG_DST_RECV){
						return FRAG_DST_RECV;
					}
					else if(fragmentdelstatus_it->Second() >= FRAG_WF_SENT){
						isCase2_1 = true;
					}
					else if(fragmentdelstatus_it->Second() >= FRAG_WF_RECV){
						isCase2_2_1 = true;
					}
				}
			}

			if(isCase2_1) return FRAG_WF_SENT;
			else if(isCase2_2_1) return FRAG_WF_RECV;
			else return FRAG_FRAG_CREATED;
		}
		else{
			Debug_Error("MessageCabinet::GetFragmentStatusofFMessage ERROR! Cannot find the fragmentation list");
			return FRAG_DOES_NOT_EXIST;
		}
	}
	else{
		return FRAG_DOES_NOT_EXIST;
	}

}

FragmentStatusTypeE MessageCabinet::GetHighestFragmentStatusofFmessageonWaveform(FMessageQElement* fmsg_ptr, WaveformId_t wfid){
	Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet::GetHighestFragmentStatusofFmessage.\n");
	FragmentStatusTypeE rs = FRAG_DOES_NOT_EXIST;
	if(fmsg_ptr){

		for(uint16_t index = 0; index < fmsg_ptr->noOfDest ; index++){
			if(fmsg_ptr->waveform[index] == wfid){
				FragmentStatusTypeE ns = GetHighestFragmentStatusofFmessageDest(fmsg_ptr, fmsg_ptr->linkArray[index]);
				if(ns > rs) rs = ns;
			}
		}

	}
	return rs;

}

FragmentStatusTypeE MessageCabinet::GetHighestFragmentStatusofFmessage(FMessageQElement* fmsg_ptr){
	Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet::GetHighestFragmentStatusofFmessage.\n");
	FragmentStatusTypeE rs = FRAG_DOES_NOT_EXIST;
	if(fmsg_ptr){
		WaveformId_t processed_wfs[MAX_WAVEFORMS];
		uint8_t num_processed_wfs = 0;
		for(uint16_t index = 0; index < fmsg_ptr->noOfDest ; index++){
//			NodeId_t node = fmsg_ptr->linkArray[index]->linkId.nodeId;
			WaveformId_t wfid = fmsg_ptr->linkArray[index]->linkId.waveformId;
			bool already_processed = false;
			for(uint8_t i = 0; i < num_processed_wfs; ++i){
				if(processed_wfs[i] == wfid){
					already_processed = true;
				}
			}
			if(!already_processed){
				FragmentStatusTypeE ns = GetHighestFragmentStatusofFmessageonWaveform(fmsg_ptr, wfid );
				if(ns > rs) rs = ns;
			}
		}
	}
	return rs;
//
////		Case 1: at least one of the fragments for at least one of the destinations FRAG_DST_RECV
////		Case 2: None of the fragments are FRAG_DST_RECV
////			 Case 2-1: at least one of the fragments for at least one of the destinations FRAG_WF_SENT
////			 Case 2-2: none of the fragments for none of the destinations FRAG_WF_SENT
////				Case 2-2-1: at least one of the fragments for at least one of the destinations FRAG_WF_RECV
////				CaSe 2-2-2: none of the fragments for none of the destinations FRAG_WF_RECV
////	if not found --> add as a new message
//
//	if(found){
//		auto wfMsgToFMsgMap_itt = wfMsgToFMsgMap->Find(fmsg_ptr->wfQElement[wfid].Begin()->GetData()->wfmsgId);
//		if(wfMsgToFMsgMap_itt != wfMsgToFMsgMap->End()){
//			FragmentList* fragmentlist_ptr = wfMsgToFMsgMap_itt->Second();
//			bool isCase2_1 = false;
//			bool isCase2_2_1 = false;
//			uint8_t n_FRAG_WF_RECV = 0;
//			for(auto fragmentmap_itt = fragmentlist_ptr->fragmentmap.RBegin(); fragmentmap_itt != fragmentlist_ptr->fragmentmap.REnd(); ++fragmentmap_itt){ //BK: Traverse fragment list
//				uint8_t n_SentToDest = 0;
//				uint8_t n_WFRecv  = 0;
//				for(auto fragmentdelstatus_it = fragmentmap_itt->Second().Begin(); fragmentdelstatus_it != fragmentmap_itt->Second().End(); ++fragmentdelstatus_it){
//					if(fragmentdelstatus_it->Second() >= FRAG_DST_RECV){
//						return FRAG_DST_RECV;
//					}
//					if(fragmentdelstatus_it->Second() >= FRAG_WF_SENT){
//						++n_SentToDest;
//					}
//					if(fragmentdelstatus_it->Second() >= FRAG_WF_RECV){
//						++n_WFRecv;
//					}
//					if(n_SentToDest > 0){ //Case 2_1
//						isCase2_1 = true;
//					}
//					if(n_WFRecv > 0){ //Case 2_1_1
//						isCase2_2_1 = true;
//					}
//				}
//			}
//
//			if(isCase2_1) return FRAG_WF_SENT;
//			else if(isCase2_2_1) return FRAG_WF_RECV;
//			else return FRAG_FRAG_CREATED;
//		}
//		else{
//			Debug_Error("MessageCabinet::GetFragmentStatusofFMessage ERROR! Cannot find the fragmentation list");
//			return FRAG_DOES_NOT_EXIST;
//		}
//	}
//	else{
//		return FRAG_DOES_NOT_EXIST;
//	}

}

DataStatusTypeE FMessageQElement::GetStatusOfNode(NodeId_t node_id){

  //Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet: Show address of dnFromDest map %p\n",&dnFromDest);
  //Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet: Show address of dnSentBYWF map %p\n",&dnSentByWF);
  //Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet: Show address of dnRecvBYWF map %p\n",&dnRecvByWF);
  //first check dnFromDest
  if(Check_Item_Exists_dnFromDest(node_id)){
    if(dnFromDest.operator [](node_id) == true){
     Debug_Printf(DBG_CORE_DATAFLOW,"MessageCabinet:node_id %d  received WDN_DST_RECV\n",node_id);
      return PDN_DST_RECV;
    }
  }
  //then check dnSentByWF
  if(Check_Item_Exists_dnSentByWF(node_id)){
    if(dnSentByWF.operator [](node_id) == true){
	 Debug_Printf(DBG_CORE_DATAFLOW,"MessageCabinet:node_id %d  received PDN_WF_SENT\n",node_id);\
	  return PDN_WF_SENT;
    }
  }
  //Finally check dsRecvByWF
  if(Check_Item_Exists_dnRecvByWF(node_id)){
    if(dnRecvByWF.operator [](node_id) == true){
	 Debug_Printf(DBG_CORE_DATAFLOW,"MessageCabinet:node_id %d  received PDN_WF_RECV\n",node_id);
	  return PDN_WF_RECV;
    }
  }
 Debug_Printf(DBG_CORE_DATAFLOW,"MessageCabinet:node_id %d is PDN_FW_RECV\n",node_id);
  return PDN_FW_RECV;
}
uint16_t MessageCabinet::GetdnFromDest( FMessageId_t  msgId){
// return remaining number
	BSTMapT <FMessageId_t , FMessageQElement* > ::Iterator it_fmsg = framworkMsgMap->Find(msgId);
	if(it_fmsg == framworkMsgMap->End()){
		Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet::MessageId %d does not exist!\n",msgId);
	    return 0;
	}
	return it_fmsg->Second()->dnFromDest_remaining;
}
uint16_t MessageCabinet::GetdnSentByWF( FMessageId_t  msgId){
	BSTMapT <FMessageId_t , FMessageQElement* > ::Iterator it_fmsg = framworkMsgMap->Find(msgId);
	if(it_fmsg == framworkMsgMap->End()){
		Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet::MessageId %d does not exist!\n",msgId);
	    return 0;
	}
	return it_fmsg->Second()->dnSentByWF_remaining;
}
uint16_t MessageCabinet::GetdnRecvByWF( FMessageId_t  msgId){
	BSTMapT <FMessageId_t , FMessageQElement* > ::Iterator it_fmsg = framworkMsgMap->Find(msgId);
	if(it_fmsg == framworkMsgMap->End()){
		Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet::MessageId %d does not exist!\n",msgId);
	    return 0;
	}
	return it_fmsg->Second()->dnRecvByWF_remaining;
}


bool MessageCabinet::IsmsgIdExists( FMessageId_t  msgId){
	BSTMapT <FMessageId_t , FMessageQElement* > ::Iterator it_fmsg = framworkMsgMap->Find(msgId);
	if(it_fmsg == framworkMsgMap->End()){
		return false;
	}
	return true;
}
bool MessageCabinet::Cancel_Data( FMessageId_t  msgId, NodeId_t* destArray, uint16_t noOfDest, WaveformId_t wfid){
	BSTMapT <FMessageId_t , FMessageQElement* > ::Iterator it_fmsg = framworkMsgMap->Find(msgId);
	if(it_fmsg == framworkMsgMap->End()){
		Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet: msgId %d not found cannot cancel\n",msgId);
		return false;
	}
	//msgId found.
	if(it_fmsg->Second()->noOfDest == 0){
		Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet::This is broadcast. Cancel everything\n");
		//remove frameworkMsgMap;
		FMessageQElement* fmsg_ptr = it_fmsg->Second();
		Debug_Printf(DBG_CORE_DATAFLOW,"MessageCabinet:: Erase frameworkMsgMap entry\n");
		framworkMsgMap->Erase(msgId);
//		wfMsgHeap[wfid].Delete(fmsg_ptr->wfQElement[0]);
		for(auto it = fmsg_ptr->wfQElement[0].Begin(); it != NULL; it = fmsg_ptr->wfQElement[0].Next(it)){
			wfMsgToFMsgMap->Erase(it->GetData()->wfmsgId);
			wfMsgHeap[wfid].DeleteElement(it);
		}
		delete(fmsg_ptr);
		return true;
	}
	uint16_t current_node = it_fmsg->Second()->noOfDest;
	FMessageQElement* fmsg_ptr = it_fmsg->Second();

	uint16_t current_location =0;
	if(current_node != 0){
		Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: Remove destinations]\n")
		for(uint16_t index =0; index < noOfDest; index++){
			Debug_Printf(DBG_CORE_DATAFLOW, "Remove Node %d \n",destArray[index]);
			//Go through Link* array and find one to be removed
			for(uint16_t i =0; i < current_node; i++){
				if(fmsg_ptr->linkArray[i]->linkId.nodeId == destArray[index])
				{
					Debug_Printf(DBG_CORE_DATAFLOW, "Link[%d] has node %d\n",i ,destArray[index]);
					current_location = i;
				}
				for(uint16_t j = current_location; j < current_node -1; j++){
					fmsg_ptr->linkArray[j] = fmsg_ptr->linkArray[j+1];
				}
			}
		}
		//Update number of remaining node
		fmsg_ptr->noOfDest = current_node - noOfDest;

		for(auto it = fmsg_ptr->wfQElement[0].Begin(); it != NULL; it = fmsg_ptr->wfQElement[0].Next(it)){
			WF_MessageQElement* qelement_ptr = it->GetData();
			//Update Heap element.
			qelement_ptr->noOfDest -= noOfDest;
			for(uint16_t i =0; i < noOfDest; i++){
				for(uint16_t j =0; j < current_node; j++){
					if(qelement_ptr->destArray[j] == destArray[i]){
						current_location = j;
					}
					for(uint16_t k =current_location; k < current_node -1 ; k++){
						qelement_ptr->destArray[k] = qelement_ptr->destArray[k+1];
					}
				}
			}
			for(uint16_t index=0; index < qelement_ptr->noOfDest; index++){
				Debug_Printf(DBG_CORE_DATAFLOW,"MessageCabinet:: destArray[%d] is %ld\n",index, qelement_ptr->destArray[index]);
			}
		}
//		WF_MessageQElement* qelement_ptr = fmsg_ptr->wfQElement[wfid];
//		//Update Heap element.
//		qelement_ptr->noOfDest -= noOfDest;
//		for(uint16_t i =0; i < noOfDest; i++){
//			for(uint16_t j =0; j < current_node; j++){
//				if(qelement_ptr->destArray[j] == destArray[i]){
//					current_location = j;
//				}
//				for(uint16_t k =current_location; k < current_node -1 ; k++){
//					qelement_ptr->destArray[k] = qelement_ptr->destArray[k+1];
//				}
//			}
//		}
//		for(uint16_t index=0; index < qelement_ptr->noOfDest; index++){
//			Debug_Printf(DBG_CORE_DATAFLOW,"MessageCabinet:: destArray[%d] is %ld\n",index, qelement_ptr->destArray[index]);
//		}
		return true;
	}
	return false;
}


uint16_t MessageCabinet::ReplacePayloadRequest(uint16_t sizeOfPayload, PatternId_t patternId, FMessageId_t  msgId, void* payload ,WaveformId_t* wf_inquery, ReplacePayloadResponse_Data& data)
{
	//Get hold of Framework Message.
	FMsgMap::Iterator it_fmsg = framworkMsgMap->Find(msgId);
	if(it_fmsg == framworkMsgMap->End()){
		Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet::messageId does not exist on framework.\n");
		data.msgId = msgId;
		data.status = false;            //indicates payload was replaced sccessfully or not
		data.noOfDest = 0;      //no of node in destArray
		return 0;
	}

	// Store pointer to FMessageQELement;
	FMessageQElement* fmsg_ptr = it_fmsg->Second();
	uint16_t wf_index = 0;
	//Get hold of old ptr
	//fmsg_ptr->old_payload_ptr = fmsg_ptr->msg->GetPayload();//store old message ptr.
	DataStatusTypeE status;
	if(fmsg_ptr->broadcast == true){
		//This message is broadcast. Find status
		status = fmsg_ptr->FindBroadcastStatus();
		switch(status){
			case PDN_ERROR_PKT_TOOBIG:
			case PDN_ERROR:
				Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: Status is PDN_ERROR, too late\n");
					break;
			case PDN_DST_RECV:
					Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: Status is PDN_DST_RECV, too late\n");
					break;
			case PDN_WF_SENT:
					Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: Status is PDN_WF_SENT, too late\n");
					break;
			case PDN_WF_RECV:
					wf_inquery[wf_index++] = fmsg_ptr->waveform[0];
					Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: Status is PDN_WF_RECV, inquery wf\n");
							break;
			case PDN_FW_RECV:
					Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: Status is PDN_FW_RECV, successful\n");
					// How to retreave wfid?
					this->Replace_Ptr(fmsg_ptr, payload, sizeOfPayload, fmsg_ptr->waveform[0]);
					data.status = true;
			case PDN_BROADCAST_NOT_SUPPORTED:
				break;
		}
		if(data.status)
		{
			Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet::ReplacePayloadRequest payload was replaced. delete old payload\n");
			uint8_t* old_payload = fmsg_ptr->msg->GetPayload();
			delete(old_payload);
		}
		data.msgId = msgId;
		data.noOfDest = 0;      //no of node in destArray
		return wf_index;
	}
	else{
		data.status = true;
		data.msgId = msgId;
		data.noOfDest = 0; //fmsg_ptr->noOfDest;
		Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: This is Unicast with %d destinations\n",fmsg_ptr->noOfDest);


		WaveformId_t processed_wfs[MAX_WAVEFORMS];
		uint8_t num_processed_wfs = 0;


		for(uint16_t index = 0; index < fmsg_ptr->noOfDest ; index++){
			NodeId_t node = fmsg_ptr->linkArray[index]->linkId.nodeId;
			WaveformId_t wfid = fmsg_ptr->linkArray[index]->linkId.waveformId;
			bool already_processed = false;
			for(uint8_t i = 0; i < num_processed_wfs; ++i){
				if(processed_wfs[i] == wfid){
					already_processed = true;
				}
			}
			if(!already_processed){
				processed_wfs[num_processed_wfs++] = wfid;

				Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet::ReplacePayloadRequest Consider nodeid %d on wfid %d  \n", node, wfid);
				auto s = GetHighestFragmentStatusofFmessageonWaveform(fmsg_ptr, wfid);
				switch(s){
				case FRAG_FRAG_CREATED: //Replace
					if(this->Replace_Ptr(fmsg_ptr, payload, sizeOfPayload, wfid)){
						for(uint16_t index2 = 0; index2 < fmsg_ptr->noOfDest ; index2++){
							if(fmsg_ptr->waveform[index2] == wfid){
								data.destArray[data.noOfDest] = fmsg_ptr->linkArray[index2]->linkId.nodeId;
								data.replace_status[data.noOfDest] = true;
								++data.noOfDest;
							}
						}
					}
					else{
						for(uint16_t index2 = 0; index2 < fmsg_ptr->noOfDest ; index2++){
							if(fmsg_ptr->waveform[index2] == wfid){
								data.destArray[data.noOfDest] = fmsg_ptr->linkArray[index2]->linkId.nodeId;
								data.replace_status[data.noOfDest] = false;
								++data.noOfDest;
							}
						}
					}
	//				return wf_index;
					break;
				case FRAG_WF_RECV: //TODO: BK: Ask waveform
					wf_inquery[wf_index++] = wfid;
					break;
				case FRAG_DST_RECV:
				case FRAG_WF_SENT:

					for(uint16_t index2 = 0; index2 < fmsg_ptr->noOfDest ; index2++){
						if(fmsg_ptr->waveform[index2] == wfid){
							data.destArray[data.noOfDest] = fmsg_ptr->linkArray[index2]->linkId.nodeId;
							data.replace_status[data.noOfDest] = false;
							++data.noOfDest;
						}
					}
					break;
				case FRAG_DOES_NOT_EXIST:
				default:
					Debug_Error("ReplacePayloadRequest unknown fragment status of message");


					data.destArray[data.noOfDest] = node;
					data.replace_status[data.noOfDest] = false;

					data.status = false;
					data.msgId = msgId;
					++data.noOfDest;
	//				return wf_index;
					break;
				}
			}

		}
//		auto s = GetHighestFragmentStatusofFmessage(fmsg_ptr);
//		switch(s){
//		case FRAG_DST_RECV:
//		case FRAG_WF_SENT:
//			for(uint16_t index = 0; index < fmsg_ptr->noOfDest ; index++){
//				data.destArray[index] = fmsg_ptr->linkArray[index]->linkId.nodeId;
//				data.replace_status[index] = false;
//			}
//			data.status = false;
//			data.msgId = msgId;
//			data.noOfDest = fmsg_ptr->noOfDest;
//			return wf_index;
//		case FRAG_WF_RECV: //Ask waveform
//
//		case FRAG_FRAG_CREATED: //Replace
//			this->Replace_Ptr(fmsg_ptr, payload, sizeOfPayload, wfid);
//			data.status = true;
//			data.msgId = msgId;
//			data.noOfDest = fmsg_ptr->noOfDest;
//			return wf_index;
//		case FRAG_DOES_NOT_EXIST:
//		default:
//			Debug_Error("ReplacePayloadRequest unknown fragment status of message");
//			return 0;
//		}
		return wf_index;
	}

//	Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: This is Unicast with %d destinations\n",fmsg_ptr->noOfDest);
//	//if unicase for each Node check status
//	uint16_t count = 0;
//	//bool flag = false;
//	bool success = false;
//	bool fail = false;
//	for(uint16_t index = 0; index < fmsg_ptr->noOfDest ; index++){
//		Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: This is Unicast with %d destinations\n",fmsg_ptr->noOfDest);
//		//Get Node id.
//		NodeId_t node = fmsg_ptr->linkArray[index]->linkId.nodeId;
//		WaveformId_t waveform = fmsg_ptr->linkArray[index]->linkId.waveformId;
//		Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: Find status of node %d\n",node);
//		switch(fmsg_ptr->GetStatusOfNode(node)){
//		case PDN_DST_RECV:{
//			Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: Status is PDN_DST_RECV, too late\n");
//			data.destArray[count] = node;
//			data.replace_status[count] = false;
//			count++;
//			fail = true;
//			break;
//		}
//		case PDN_WF_SENT:{
//			Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: Status is PDN_WF_SENT, too late\n");
//			data.destArray[count] = node;
//			data.replace_status[count] = false;
//			count++;
//			fail=true;
//			break;
//		}
//		case PDN_WF_RECV:{
//			Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: Status is PDN_WF_RECV, inquery wf\n");
//			wf_inquery[wf_index++] = waveform;
//			//go ahead and replay payload?
//			this->Replace_Ptr(fmsg_ptr, payload, sizeOfPayload, waveform);
//
//			/*if(wf_index == 0){ If I do this, somehow switch statement will not be executed?
//				wf_inquery[wf_index++] = waveform;
//			}else{
//				Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: Check this waveform %d is in inquery list or not\n",waveform);
//				for(uint16_t index_list =0; index_list < wf_index; index_list++){
//					if(wf_inquery[index_list] == waveform){
//						Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: waveform %d is already in the list\n",waveform);
//						flag = true;
//						break;
//					}
//				}
//				if(flag == false){
//					Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: waveform %d is now added to list\n",waveform);
//				}else{
//					flag = false;
//				}
//			}*/
//			fail = true;
//			break;
//		}
//		case PDN_FW_RECV:{
//			Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: Status is PDN_FW_RECV, successful\n");
//			// How to retreave wfid?
//			this->Replace_Ptr(fmsg_ptr, payload, sizeOfPayload, waveform);
//			data.destArray[count] = node;
//			data.replace_status[count] = true;
//			count++;
//			success = true;
//			///debugging
//			/* uint8_t* data_ptr =  fmsg_ptr->wfQElement[waveform]->msg->GetPayload();
//
//			for(uint16_t index =0; index < 256; index++){
//				printf("%x",data_ptr[index]);
//			}*/
//
//			break;
//		}
//		default:
//			Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: Unknown status value! \n");
//		}
//	}
//	if(!fail && success){
//		//uint8_t* old_payload = fmsg_ptr->msg->GetPayload();
//		//delete(old_payload);
//		Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: All destination successfully replace payload.\n");
//	}
//	data.msgId = msgId;
//	data.status = false;            //indicates payload was replaced sccessfully or not
//	data.noOfDest = count;
//	return wf_index;
}

bool MessageCabinet::Replace_Ptr(FMessageQElement * fmsg_ptr, void* payload ,uint16_t sizeOfPayload, WaveformId_t wfid){


   Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabiniet:: show fmsg_ptr %p, payload_ptr %p\n",fmsg_ptr, payload);
   // fmsg_ptr->msg->SetPayload((uint8_t*)payload);
	fmsg_ptr->msg->SetPayloadSize(sizeOfPayload);
	fmsg_ptr->msg->SetPayload((uint8_t*)payload);
	//TODO: BK: Replace payload in the waveform messages
	// Consider 3 cases new payload requiring more segments, less segments, same amount of segments
	//TODO: BK: Also consider the case in which half of the segments was sent out before replace_PTR command comes in


	//Find wf attributes
	WF_AttributeMap_t::Iterator it = wfAttributes->Find(wfid);
	if(it == wfAttributes->End()){
		Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabiniet:: Replace_Ptr cannot find waveform\n");
		return false;
	}

	//Find number of segments required
	uint16_t max_segment_size = it->Second().maxPayloadSize;
	uint16_t numberOfMsgSegmentsRequired = sizeOfPayload / max_segment_size;
	if(sizeOfPayload % it->Second().maxPayloadSize > 0 ) ++numberOfMsgSegmentsRequired;



//	FragmentList* fragmentlist_ptr = new FragmentList;
//	fragmentlist_ptr->frameworkMsgID = fmsgelementptr->messageId;
//
//	for(uint16_t segment =0 ; segment < numberOfMsgSegmentsRequired; ++segment){
//		WF_MessageId_t wMsgId = GetNewWaveformMsgId();
//		WF_Message_n64_t *wfMsg = new WF_Message_n64_t();
//
//		for(uint16_t i = 0; i<noOfDest; ++i){
//			fragmentlist_ptr->fragmentmap[wMsgId].Insert(destArray[i],FRAG_FRAG_CREATED);
//		}
////		fragmentstatusptr->fragmentStatus.Insert(wMsgId,FRAG_FRAG_CREATED);
//
//		wfMsgToFMsgMap->Insert(wMsgId,fragmentlist_ptr);
//
//	if(fmsg_ptr->wfQElement[wfid].Size() == numberOfMsgSegmentsRequired){
//
//	}
//	else if(fmsg_ptr->wfQElement[wfid].Size() < numberOfMsgSegmentsRequired){
//
//	}
//	else if(fmsg_ptr->wfQElement[wfid].Size() > numberOfMsgSegmentsRequired){
//
//	}
//	All 3 cases above are handled by the following
	uint16_t segment_number = 1;
	auto itt = fmsg_ptr->wfQElement[wfid].Begin();
	FragmentList* fragmentlist_ptr = NULL;
	if( itt != NULL) {
		auto t = wfMsgToFMsgMap->Find(itt->GetData()->msg->GetSrcNodeID());
		if(t != wfMsgToFMsgMap->End()) {
			fragmentlist_ptr = t->Second();
		}
	}

    for ( ; itt != NULL && segment_number <= numberOfMsgSegmentsRequired; itt = fmsg_ptr->wfQElement[wfid].Next(itt), ++segment_number){
    	//delete itt->GetData()->msg->GetPayload();//BK: Probably we need to delete. However there was no deletion before I modify this. so I am omittting this for now although I think this will result in a leak.
		uint16_t previously_embedded_payload_size = (segment_number-1)*max_segment_size;
		if(sizeOfPayload - previously_embedded_payload_size < max_segment_size)
			itt->GetData()->msg->payloadSize = sizeOfPayload - previously_embedded_payload_size;
		else itt->GetData()->msg->payloadSize = max_segment_size;


		itt->GetData()->msg->SetPayload(fmsg_ptr->msg->GetPayload() + previously_embedded_payload_size);
    }
    for (; itt != NULL ; ){ //Delete unused waveform messages and elements when the new size is smaller
    	//Delete waveform messages
    	auto itt2 = fmsg_ptr->wfQElement[wfid].Next(itt);
    	wfMsgToFMsgMap->Erase(itt->GetData()->wfmsgId);
		wfMsgHeap[wfid].Delete(itt->GetData());
		fmsg_ptr->wfQElement[wfid].DeleteElement(itt);
		itt = itt2;
    }

    for (; segment_number <= numberOfMsgSegmentsRequired ; ++segment_number){//Create waveform messages if the new size is larger
		WF_MessageId_t wMsgId = GetNewWaveformMsgId();
		//WF_MessageBase wfMsg = wfMsgAdaptorHash->operator[](_wfid)->Convert_FM_to_WM(*msg,true);
		WF_Message_n64_t *wfMsg = new WF_Message_n64_t();

		if(fragmentlist_ptr == NULL) {
			fragmentlist_ptr = new FragmentList;
			fragmentlist_ptr->frameworkMsgID = fmsg_ptr->messageId;
		}

		for(uint16_t i = 0; i < fmsg_ptr->wfQElement[wfid].Begin()->GetData()->noOfDest; ++i){
			fragmentlist_ptr->fragmentmap[wMsgId].Insert(fmsg_ptr->wfQElement[wfid].Begin()->GetData()->destArray[i],FRAG_FRAG_CREATED);
		}


		wfMsgToFMsgMap->Insert(wMsgId,fragmentlist_ptr); //Copy

		wfMsg->CopyIthSegmentFrom(*(fmsg_ptr->msg), segment_number, max_segment_size);
		wfMsg->SetWaveform(wMsgId);
		wfMsg->SetSource(MY_NODE_ID);
		wfMsg->SetNumberOfDest(fmsg_ptr->wfQElement[wfid].Begin()->GetData()->noOfDest); //Copy Number of destinations from the first segment
		wfMsg->SetWaveformMessageID(wMsgId);
		wfMsg->SetInstance(fmsg_ptr->wfQElement[wfid].Begin()->GetData()->msg->GetInstance());//Copy PID from the first segment

		Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: AddNewFrameworkMessage: message type via qelement is %d\n",wfMsg->GetType());


		WF_MessageQElement *element = new WF_MessageQElement(wfMsg, fmsg_ptr->wfQElement[wfid].Begin()->GetData()->metric, wMsgId, fmsg_ptr->messageId, fmsg_ptr->wfQElement[wfid].Begin()->GetData()->destArray, fmsg_ptr->wfQElement[wfid].Begin()->GetData()->noOfDest, fmsg_ptr->wfQElement[wfid].Begin()->GetData()->broadcast);
		Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet::WF_MessageQElement has mssage at %p, payload at %p\n",wfMsg,wfMsg->GetPayload());
		fmsg_ptr->wfQElement[wfid].Insert(element);
		Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet::Adding WF_MessageQElement ptr %p for fmid %d to wfQElement[%d]\n",element, fmsg_ptr->messageId, wfid);
		wfMsgHeap[wfid].Insert(element);
		//debugging
		if(element->msg->GetType() == Types::ACK_REQUEST_MSG){
			SoftwareAcknowledgement* swack1 = (SoftwareAcknowledgement*) element->msg->GetPayload();
			Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet::Replace_Ptr: Show payload %d %d %lu\n",swack1->src, swack1->wfId, swack1->wfMsgId);
			Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet::Replace_Ptr: Show payload ptr %p\n", swack1);
		}
    }



    return true;
}

uint16_t MessageCabinet::CancelDataSendRequest(PatternId_t patternId, FMessageId_t  msgId, NodeId_t *destArray, uint16_t noOfDest, WaveformId_t* wf_inquery, CancelDataResponse_Data& data, bool& flag ){
	  //Get hold of Framework Message.
		FMsgMap::Iterator it_fmsg = framworkMsgMap->Find(msgId);
	    if(it_fmsg == framworkMsgMap->End()){
	     Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet::messageId does not exist on framework.\n");
	      data.msgId = msgId;
	      data.status = false;            //indicates payload was replaced sccessfully or not
	      data.noOfDest = 0;      //no of node in destArray
		  return false;
	    }
	    //Get Fmessage ptr
	    FMessageQElement* fmsg_ptr = it_fmsg->Second();
	    uint16_t wf_index = 0;
	    if(fmsg_ptr->broadcast == true){
	    	FragmentStatusTypeE s = GetHighestFragmentStatusofFmessage(fmsg_ptr);
	    	switch(s){
				case  FRAG_FRAG_CREATED:
					data.status =  Remove_Node(fmsg_ptr, 0, 1, fmsg_ptr->waveform[0], msgId ,flag);
					break;
				case  FRAG_WF_RECV:
					wf_inquery[wf_index++] = fmsg_ptr->waveform[0];
					break;
				case  FRAG_WF_SENT:
					Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: Status is PDN_WF_SENT Too late to cancel\n");
					data.status = false;
					break;
				case  FRAG_DST_RECV:
					Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: Status is PDN_DST_RECV Too late to cancel\n");
					data.status = false;
					break;
				case  FRAG_DOES_NOT_EXIST:
				default:
					Debug_Error("CancelDataSendRequest unknown fragment status of message");
					return false;
	    	}
	    	data.msgId = msgId;
	    	data.noOfDest = 0;
	    	return wf_index;
	    }
	   Debug_Printf(DBG_CORE_DATAFLOW, "Show all destination to be removed\n");
	    for(uint16_t index =0;  index < noOfDest; index++){
	    	Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: Show dest to be removed %d\n",destArray[index]);
	    }
	   Debug_Printf(DBG_CORE_DATAFLOW, "This is Unicase with %d destinations.\n",fmsg_ptr->noOfDest);
	    uint16_t count = 0;
	    for(uint16_t remove_index =0 ; remove_index < noOfDest; remove_index++){
	    	//This is unicast.
	    	bool found = false;
	    	NodeId_t remove_node = destArray[remove_index];
	    	Debug_Printf(DBG_CORE_DATAFLOW, "Check if Node %d exists in msgId %d\n",remove_node, msgId);
	    	for(uint16_t link_index =0; link_index < fmsg_ptr->noOfDest; link_index++){
	    		Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: link[%d] is %d\n",link_index, fmsg_ptr->linkArray[link_index]->linkId.nodeId)
	    		if(remove_node == fmsg_ptr->linkArray[link_index]->linkId.nodeId){
	    			found = true;
	    			Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: Node %d found in msgId %d\n",remove_node,msgId);
	    			WaveformId_t wfid = fmsg_ptr->linkArray[link_index]->linkId.waveformId;
	    			Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: Node %d is on wfid %d\n",remove_node,wfid);
	    			//Get status
	    			FragmentStatusTypeE s = GetHighestFragmentStatusofFmessageDest(fmsg_ptr, fmsg_ptr->linkArray[link_index]);
	    	    	switch(s){
	    				case  FRAG_FRAG_CREATED:
							Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: Status is FRAG_FRAG_CREATED, success\n");
							data.cancel_status[count] = true;
							data.destArray[count] = remove_node;
							count++;
							Remove_Node(fmsg_ptr,(uint16_t)1, remove_node, wfid, msgId, flag);
							break;
	    				case  FRAG_WF_RECV:
	    					Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: Status is FRAG_WF_RECV, inquery wf\n");
							wf_inquery[wf_index++] = fmsg_ptr->linkArray[link_index]->linkId.waveformId;
							break;
	    				case  FRAG_WF_SENT:
	    					Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: Status is FRAG_WF_SENT Too late to cancel\n");
	    					data.cancel_status[count] = false;
							data.destArray[count] = remove_node;
							count++;
							break;
	    				case  FRAG_DST_RECV:
	    					Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: Status is FRAG_DST_RECV Too late to cancel\n");
							data.cancel_status[count] = false;
							data.destArray[count] = remove_node;
							count++;
							break;
	    				case  FRAG_DOES_NOT_EXIST:
	    				default:
	    					Debug_Error("CancelDataSendRequest unknown fragment status of message");
	    					return false;
	    	    	}
	    	    	break;
	    		}
	    	}
	        if(found == false){
	        	Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: Noode %d was NOT in msgId %d\n",remove_node, msgId);
	        	//failed
	        	data.cancel_status[count] = false;
	        	data.destArray[count] = remove_node;
	        	count++;
	        }
	    }
	    Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet: Return what I know now. noOfDest %d\n",count);
	    data.msgId = msgId;
	    data.status = false;            //indicates payload was replaced sccessfully or not
	    data.noOfDest = count;
	    return wf_index;
}
bool MessageCabinet::Remove_Node(FMessageQElement* fmsg_ptr, uint16_t noOfDest, NodeId_t remove_node, WaveformId_t wfid, FMessageId_t  msgId, bool& flag){
    //check if this is unicast of broadcast
	if(noOfDest == 0){
		Debug_Printf(DBG_CORE_DATAFLOW , "Cancel Broadcast msg\n");
        //simply delete FrameworkMsgMap entry and WF_MessageQElement entry
		framworkMsgMap->Erase(msgId);
//		this->wfMsgHeap[wfid].Delete(fmsg_ptr->wfQElement[wfid]);
		for(auto i_ptr = fmsg_ptr->wfQElement[wfid].Begin(); i_ptr != NULL; i_ptr = fmsg_ptr->wfQElement[wfid].Next(i_ptr)){
			wfMsgHeap[wfid].Delete(i_ptr->GetData());
		}
		delete fmsg_ptr;
		return true;
	}
	if(noOfDest != 0){
	    uint16_t current_noOfDest = fmsg_ptr->noOfDest;
	    //find index of node to be removed in Link* array
	    for(uint16_t link_index = 0; link_index < current_noOfDest; link_index++){
	    	if(fmsg_ptr->linkArray[link_index]->linkId.nodeId == remove_node){
	    		Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: Found index %d in LinkArray\n",link_index);
	    		uint16_t shift_index = link_index;
	    		for(;shift_index < current_noOfDest - 1; shift_index++){
	    			//shift element by one
	    			fmsg_ptr->linkArray[shift_index] = fmsg_ptr->linkArray[shift_index +1];
	    		}
	    		Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: Finished shifting.\n");
	    	}
	    	//break;
	    }
	    //Debug show linkarray
	    for(uint16_t link_index = 0; link_index < current_noOfDest; link_index++){
	    	Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet: liknArray[%d] is %d\n",link_index, fmsg_ptr->linkArray[link_index]->linkId.nodeId);
	    }
    }
	for(auto i_ptr = fmsg_ptr->wfQElement[wfid].Begin(); i_ptr != NULL; i_ptr = fmsg_ptr->wfQElement[wfid].Next(i_ptr)){
		Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: Try adjusting Heap informaton\n");
		//adjust information in Heap
		for(uint16_t remove_index =0; remove_index < noOfDest; remove_index++){
			Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: Remove node %d and Adjust destination in Heap\n",remove_node);
			for(uint16_t dest_index =0; dest_index < i_ptr->GetData()->noOfDest; dest_index++){
				if(i_ptr->GetData()->destArray[dest_index] == remove_node){
					Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet::Found Node %d at %d\n",remove_node, dest_index);
					uint16_t shift_index = dest_index;
					for(;shift_index < i_ptr->GetData()->noOfDest; shift_index++){
						i_ptr->GetData()->destArray[shift_index] = i_ptr->GetData()->destArray[shift_index+1];
					}
				}
			}
		}
	}
	//now modify noOfDest
    fmsg_ptr->noOfDest -= noOfDest;
    fmsg_ptr->dnFromDest_remaining -=noOfDest;
    fmsg_ptr->dnSentByWF_remaining -=noOfDest;
    fmsg_ptr->dnRecvByWF_remaining -=noOfDest;
//    fmsg_ptr->wfQElement[wfid]->noOfDest -= noOfDest;
    for(auto i_ptr = fmsg_ptr->wfQElement[wfid].Begin(); i_ptr != NULL; i_ptr = fmsg_ptr->wfQElement[wfid].Next(i_ptr)){
    	i_ptr->GetData()->noOfDest -= noOfDest;
    }

    if(fmsg_ptr->dnSentByWF_remaining == 0)
    {
    	flag = true;
    }
	Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: Adjust noOfDest")
    //if noOfdest in WF_MessageQElement is 0, remove entry from heap.
//    if(fmsg_ptr->wfQElement[wfid]->noOfDest == 0){
//    	Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet::Show pointer to WF_MessageQElement* %p\n",fmsg_ptr->wfQElement[wfid]);
//      	bool flag =this->wfMsgHeap[wfid].Delete(fmsg_ptr->wfQElement[wfid]);
//      	if(!flag){
//      	   	Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: Failed to remove entry\n");
//      	}else{
//      	   	Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: Entry in heap removed!\n");
//      	}
//      	fmsg_ptr->wfQElement[wfid] = NULL;
//    }
    for(auto i_ptr = fmsg_ptr->wfQElement[wfid].Begin(); i_ptr != NULL; i_ptr = fmsg_ptr->wfQElement[wfid].Next(i_ptr)){
		if(i_ptr->GetData()->noOfDest == 0){
			Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet::Show pointer to WF_MessageQElement* %p\n",i_ptr->GetData());
			bool flag =this->wfMsgHeap[wfid].Delete(i_ptr->GetData());
			if(!flag){
				Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: Failed to remove entry\n");
			}else{
				Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: Entry in heap removed!\n");
			}
		}
    }


	//if noOfdest in framworkMsgMap = 0 remove FMessageQElement
	if(fmsg_ptr->noOfDest == 0){
	   	Debug_Printf(DBG_CORE_DATAFLOW, "Now noOfDest is 0. Remove FMessageQElement from entry\n");
	   	framworkMsgMap->Erase(msgId);
	   	FMsgMap::Iterator it_fmsg = framworkMsgMap->Find(msgId);
	    if(it_fmsg == framworkMsgMap->End()){
	   				Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: All destination has been removed. Adjust outstandingpkt\n");
	 	}
	    delete fmsg_ptr;
  		return true;
	}else{
	   	Debug_Printf(DBG_CORE_DATAFLOW, "Now destination is removed");
	   	return true;
	}
return true;
}

bool MessageCabinet::CancelDataReponse_Helper(FMessageQElement* fmsg_ptr, uint16_t noOfDest, NodeId_t remove_node, WaveformId_t wfid, FMessageId_t  msgId)
{
	if(noOfDest != 0){
	    uint16_t current_noOfDest = fmsg_ptr->noOfDest;
	    //find index of node to be removed in Link* array
	    for(uint16_t link_index = 0; link_index < current_noOfDest; link_index++){
	    	if(fmsg_ptr->linkArray[link_index]->linkId.nodeId == remove_node){
	    		Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: Found index %d in LinkArray\n",link_index);
	    		uint16_t shift_index = link_index;
	    		for(;shift_index < current_noOfDest - 1; shift_index++){
	    			//shift element by one
	    			fmsg_ptr->linkArray[shift_index] = fmsg_ptr->linkArray[shift_index +1];
	    		}
	    		Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: Finished shifting.\n");
	    	}
	    	//break;
	    }
	    //Debug show linkarray
	    for(uint16_t link_index = 0; link_index < current_noOfDest; link_index++){
	    	Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet: liknArray[%d] is %d\n",link_index, fmsg_ptr->linkArray[link_index]->linkId.nodeId);
	    }
	    //Update noOfdest
	    fmsg_ptr->noOfDest -=1;
	    fmsg_ptr->dnFromDest_remaining -=1;
	    fmsg_ptr->dnSentByWF_remaining -=1;
	    fmsg_ptr->dnRecvByWF_remaining -=1;
	   Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: Show remaining number of dest %d for msgId %d\n",fmsg_ptr->noOfDest,msgId);

	    return true;
    }
	return false;
}



MessageCabinet::AddNodeReturnValue_t MessageCabinet::Add_Node(FMessageQElement* fmsg_ptr, uint64_t* destArray, uint16_t noOfDest, WaveformId_t wfid, RequestId_t rId, FMessageId_t  msgId, PatternId_t pid){
	//check status of this destination
	Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet::Add_Node. Check and Add destination on wf %d\n",wfid);
	// this wfid may not exist for this msgId yet. Check if this wfid exists or not.
	bool found = false;
	Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet::Show fmsg_ptr->noOfDest %d\n",fmsg_ptr->noOfDest);
	for(uint16_t i =0; i < fmsg_ptr->noOfDest; i++){
		Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: waveform[%d] is %d\n",i , fmsg_ptr->waveform[i]);
		if(fmsg_ptr->waveform[i] == wfid){
			found = true;
			break;
		}
	}


//#if ENABLE_FRAGMENTATION == 1

	//BK: Check the status of the message to determine whether we can add this destination
	//If this waveform was already selected
		//Case 1: None of the fragments have been received by the waveform for no destination --> add
		//Case 2: Only one of fragments have been received by the waveform
			//Case 2-1: None of the fragments were sent by the waveform for none of the destinations -->Ask waveform
			//Case 2-2: At least one of fragments were sent by the waveform for at least one of the destinations but not all-->Ask waveform
			//Case 2-3: At least one of fragments were sent by the waveform for all the destinations --> reject
		//Case 3: More than one fragment is received by the waveform --> reject
	//if not found --> add as a new message

	if(found){
		auto wfMsgToFMsgMap_itt = wfMsgToFMsgMap->Find(fmsg_ptr->wfQElement[wfid].Begin()->GetData()->wfmsgId);
		if(wfMsgToFMsgMap_itt != wfMsgToFMsgMap->End()){
			FragmentList* fragmentlist_ptr = wfMsgToFMsgMap_itt->Second();
			if(fragmentlist_ptr->lastSentCombinedStatusMap.Size() == 0){ //Case 1 or Case 2
				bool isCase1 = true;

				if(fragmentlist_ptr->fragmentmap.Size() > 0){
					// We can consider looking at the first fragment since it is First come first served. However, otherwise we need to traverse
					for(auto fragmentmap_itt = fragmentlist_ptr->fragmentmap.RBegin(); fragmentmap_itt != fragmentlist_ptr->fragmentmap.REnd(); ++fragmentmap_itt){ //BK: Traverse fragment list
						for(auto fragmentdelstatus_it = fragmentmap_itt->Second().Begin(); fragmentdelstatus_it != fragmentmap_itt->Second().End(); ++fragmentdelstatus_it){
							if(fragmentdelstatus_it->Second() >= FRAG_WF_RECV){
								isCase1 = false;
								break;
							}
						}
						if(!isCase1) break;
					}

					if(isCase1){ // Case 1
						//If a waveform message exists in the queue for this fmsg_ptr (destined to other neighbors on this destination)
						// Traverse through all wfQElements and add the new destination to them
						// And find FragmentList object and traverse through all waveformMessages (fragments) and create fragment status for the new destination
						for(uint16_t index =0; index < noOfDest; index++){
							Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: adding node %ld to list\n",destArray[index]);
							fmsg_ptr->dnFromDest_remaining++;
							fmsg_ptr->dnSentByWF_remaining++;
							fmsg_ptr->dnRecvByWF_remaining++;
							fmsg_ptr->waveform[fmsg_ptr->noOfDest] = wfid;
							for(auto itt = fmsg_ptr->wfQElement[wfid].Begin(); itt != NULL; itt = fmsg_ptr->wfQElement[wfid].Next(itt)){
								itt->GetData()->destArray[itt->GetData()->noOfDest] = destArray[index];
								itt->GetData()->noOfDest++;
							}
							for(auto fragmentmap_itt = fragmentlist_ptr->fragmentmap.RBegin(); fragmentmap_itt != fragmentlist_ptr->fragmentmap.REnd(); ++fragmentmap_itt){
								FragmentDeliveryStatusMap_t* fragment_del_status_map_ptr = &(fragmentmap_itt->Second());
								fragment_del_status_map_ptr->Insert(destArray[index],FRAG_FRAG_CREATED);

							}
							fmsg_ptr->noOfDest++;
							Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:::: noOfDest is %d Node %ld is added\n",index, destArray[index]);
							Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: Show number of message on wfid %d is %d\n", wfid,this->GetNumberWaveformOfMessages(wfid));
						}
						return ADDNODERV_DEST_ADDED_TO_EXISTING_WF_MSG;
					}
					else{ //Case 2
						bool isCase2_3 = true;
						for(auto fragmentmap_itt = fragmentlist_ptr->fragmentmap.RBegin(); fragmentmap_itt != fragmentlist_ptr->fragmentmap.REnd(); ++fragmentmap_itt){ //BK: Traverse fragment list
							for(auto fragmentdelstatus_it = fragmentmap_itt->Second().Begin(); fragmentdelstatus_it != fragmentmap_itt->Second().End(); ++fragmentdelstatus_it){
								if(fragmentdelstatus_it->Second() <= FRAG_WF_RECV){
									isCase2_3 = false;
									break;
								}
							}
							if(!isCase2_3) break;
						}

						if(!isCase2_3){ //Case 2-1 or Case 2-2
							Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: One of the destinations is PDN_WF_RECV Ask waveform %d\n",wfid);
							//Core::WFControl::wfControl->Send_AddDestinationRequest(wfid, rId,msgId, destArray, noOfDest);

							//Add destination for now but return ADDNODERV_ASK_PATTERN for initating askinng to WF
							for(uint16_t index =0; index < noOfDest; index++){
								Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: adding node %ld to list\n",destArray[index]);
								fmsg_ptr->dnFromDest_remaining++;
								fmsg_ptr->dnSentByWF_remaining++;
								fmsg_ptr->dnRecvByWF_remaining++;
								fmsg_ptr->waveform[fmsg_ptr->noOfDest] = wfid;
								for(auto itt = fmsg_ptr->wfQElement[wfid].Begin(); itt != NULL; itt = fmsg_ptr->wfQElement[wfid].Next(itt)){
									itt->GetData()->destArray[itt->GetData()->noOfDest] = destArray[index];
									itt->GetData()->noOfDest++;
								}
								for(auto fragmentmap_itt = fragmentlist_ptr->fragmentmap.RBegin(); fragmentmap_itt != fragmentlist_ptr->fragmentmap.REnd(); ++fragmentmap_itt){
									FragmentDeliveryStatusMap_t* fragment_del_status_map_ptr = &(fragmentmap_itt->Second());
									fragment_del_status_map_ptr->Insert(destArray[index],FRAG_FRAG_CREATED);

								}

								fmsg_ptr->noOfDest++;
								Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:::: noOfDest is %d Node %ld is added\n",index, destArray[index]);
								Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: Show number of message on wfid %d is %d\n", wfid,this->GetNumberWaveformOfMessages(wfid));
							}

//							return ADDNODERV_FAIL;
							return ADDNODERV_ASK_PATTERN;
						}
						else{
							Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: All node has PDN_WF_SENT for one of the fragments. Too late! \n");
							return ADDNODERV_FAIL;
						}

					}
				}
				else{
					Debug_Error("MessageCabinet::Add_Node ERROR! Fragmentation list is empty");
					return ADDNODERV_FAIL;
				}
			}
			else { //Case 3
				Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: Some node has PDN_WF_RECV for all of the fragments. Too late! \n");
				return ADDNODERV_FAIL;
			}


		}
		else{
			Debug_Error("MessageCabinet::Add_Node ERROR! Cannot find the fragmentation list");
			return ADDNODERV_FAIL;
		}
	}
	else{
		Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: Create new WF_QElement on wfid %d\n",wfid);
		uint16_t old_noOfDest = fmsg_ptr->noOfDest;

		AddWaveformMessage(fmsg_ptr, wfid, false, destArray, noOfDest, pid);

		fmsg_ptr->dnFromDest_remaining += noOfDest;
		fmsg_ptr->dnSentByWF_remaining += noOfDest;
		fmsg_ptr->dnRecvByWF_remaining += noOfDest;
		fmsg_ptr->noOfDest += noOfDest;

		//update fmsg_ptr->waveform array
		for(uint16_t index = 0; index < noOfDest; index++){
			fmsg_ptr->waveform[old_noOfDest++] = wfid;
		}
		Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: Show number of message on wfid %d is %d\n", wfid,this->GetNumberWaveformOfMessages(wfid));

		return ADDNODERV_ADDED_AS_NEW_WF_MSG;
	}

//#else
//
//	//for(uint16_t index =0; index < noOfDest; index++){
//	if(fmsg_ptr->dnFromDest_remaining == 0 && found){
//		Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: All node has PDN_DST_RECV too late to add\n");
//		return ADDNODERV_FAIL;
//	}
//	if(fmsg_ptr->dnSentByWF_remaining == 0 && found){
//		Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: All node has PDN_WF_SENT too late to add\n");
//		return ADDNODERV_FAIL;
//	}
//	if(fmsg_ptr->dnRecvByWF_remaining == 0 && found){
//		Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: Status is PDN_WF_RECV Ask waveform %d\n",wfid);
//		for(uint16_t index =0; index < noOfDest; index++){
//			Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: adding node %ld to list\n",destArray[index]);
//			//Do not add entry here. It should be added when it received acknowledgement from wavefrom,
//			//fmsg_ptr->dnFromDest.Insert(destArray[index],false);
//			//fmsg_ptr->dnSentByWF.Insert(destArray[index],false);
//			//fmsg_ptr->dnRecvByWF.Insert(destArray[index],false);
//			fmsg_ptr->dnFromDest_remaining++;
//			fmsg_ptr->dnSentByWF_remaining++;
//			//fmsg_ptr->dnRecvByWF_remaining++; If success this is already 0
//			fmsg_ptr->waveform[fmsg_ptr->noOfDest] = wfid;
//			//fmsg_ptr->wfQElement[wfid]->destArray[fmsg_ptr->wfQElement[wfid]->noOfDest] = destArray[index];
//			//fmsg_ptr->wfQElement[wfid]->noOfDest++;
//			fmsg_ptr->noOfDest++;
//			Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: noOfDest is %d Node %ld is added\n",index, destArray[index]);
//		}
//
//		//Core::WFControl::wfControl->Send_AddDestinationRequest(wfid, rId,msgId, destArray, noOfDest);
//		return ADDNODERV_ASK_PATTERN;
//	}
//
//	Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: Status is PDN_FW_RECV: Adding node  to list.\n");
//	//create map entry
//
//
//	if(!found){
//		Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: Create new WF_QElement on wfid %d\n",wfid);
//		uint16_t old_noOfDest = fmsg_ptr->noOfDest;
//		//WF_MessageBase* message_ptr = fmsg_ptr->msg;
//
//		AddWaveformMessage(fmsg_ptr, wfid, false, destArray, noOfDest, pid);
//
//
//		//#if ENABLE_FRAGMENTATION == 1
//		//		AddWaveformMessage(fmsg_ptr, wfid, false, destArray, noOfDest, pid);
//		//
//		//#else
//		//		AddWaveformMessage(fmsg_ptr, wfid, false, destArray, noOfDest, pid);
//		//
//		////		WF_Message_n64_t *wfMsg = new WF_Message_n64_t();
//		////		WF_MessageId_t wfMsgId = GetNewWaveformMsgId();
//		////		wfMsgToFMsgMap->Insert(wfMsgId, msgId);
//		////		Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: Show fmsg_ptr->msg %p, payload points to %p, msg type is %d\n", fmsg_ptr->msg, fmsg_ptr->msg->GetPayload(), fmsg_ptr->msg->GetType());
//		////		wfMsg->CopyFrom(*fmsg_ptr->msg);
//		////		Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: Show payload points to %p\n", wfMsg->GetPayload());
//		////		wfMsg->SetWaveformMessageID(wfMsgId);
//		////		//wfMsg->SetFrameworkMessageID(msgId);
//		////		wfMsg->SetWaveform(wfid);
//		////		wfMsg->SetSource(MY_NODE_ID);
//		////		wfMsg->SetNumberOfDest(noOfDest);
//		////		wfMsg->SetInstance(pid);
//		////		Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: Show adapterhdrsize is %d\n",wfMsg->GetHeaderSize());
//		////		Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: Show message size is %ld\n", sizeof(WF_Message_n64_t));
//		////		MsgPriorityMetric* metric_ptr = fmsg_ptr->metric;
//		////		WF_MessageQElement *element = new WF_MessageQElement(wfMsg, metric_ptr, 0, msgId, destArray, noOfDest, false); //Masahirio. wfMsgId is assigned by wf.
//		////		this->wfMsgHeap[wfid].Insert(element);
//		////		Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: show created WF_MessageQElement ptr, msg %p %p\n",element, element->msg);
//		//#endif
//
//		fmsg_ptr->dnFromDest_remaining += noOfDest;
//		fmsg_ptr->dnSentByWF_remaining += noOfDest;
//		fmsg_ptr->dnRecvByWF_remaining += noOfDest;
//		fmsg_ptr->noOfDest += noOfDest;
//
//		//update fmsg_ptr->waveform array
//		for(uint16_t index = 0; index < noOfDest; index++){
//			fmsg_ptr->waveform[old_noOfDest++] = wfid;
//		}
//		Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: Show number of message on wfid %d is %d\n", wfid,this->GetNumberWaveformOfMessages(wfid));
//
//		return ADDNODERV_ADDED_AS_NEW_WF_MSG;
//	}
//	else{ //If a waveform message exists in the queue for this fmsg_ptr (destined to other neighbors on this destination)
//		// Traverse through all wfQElements and add the new destination to them
//		// And find FragmentList object and traverse through all waveformMessages (fragments) and create fragment status for the new destination
//		for(uint16_t index =0; index < noOfDest; index++){
//			Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: adding node %ld to list\n",destArray[index]);
//			//fmsg_ptr->dnFromDest.Insert(destArray[index],false); do not create entry here. I should be added when it got acknowledgement
//			//fmsg_ptr->dnSentByWF.Insert(destArray[index],false);
//			//fmsg_ptr->dnRecvByWF.Insert(destArray[index],false);
//			fmsg_ptr->dnFromDest_remaining++;
//			fmsg_ptr->dnSentByWF_remaining++;
//			fmsg_ptr->dnRecvByWF_remaining++;
//			fmsg_ptr->waveform[fmsg_ptr->noOfDest] = wfid;
//			for(auto itt = fmsg_ptr->wfQElement[wfid].Begin(); itt != NULL; itt = fmsg_ptr->wfQElement[wfid].Next(itt)){
//				itt->GetData()->destArray[itt->GetData()->noOfDest] = destArray[index];
//				itt->GetData()->noOfDest++;
//			}
//			auto itt2 = wfMsgToFMsgMap->Find(fmsg_ptr->wfQElement[wfid].Begin()->GetData()->wfmsgId);
//			if(itt2 != wfMsgToFMsgMap->End()){
//				FragmentList* fragmentlist_ptr = itt2->Second();
//				for(auto fragmentmap_itt = fragmentlist_ptr->fragmentmap.Begin(); fragmentmap_itt != fragmentlist_ptr->fragmentmap.End(); ++fragmentmap_itt){
//					fragmentmap_itt->Second().Insert(destArray[index],FRAG_FRAG_CREATED);
//				}
//			}else{
//				Debug_Error("MessageCabinet::Add_Node ERROR! Cannot find the fragmentation list");
//			}
//
//			//			fmsg_ptr->wfQElement[wfid]->destArray[fmsg_ptr->wfQElement[wfid]->noOfDest] = destArray[index];
//			//			fmsg_ptr->wfQElement[wfid]->noOfDest++;
//			fmsg_ptr->noOfDest++;
//			Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:::: noOfDest is %d Node %ld is added\n",index, destArray[index]);
//			Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: Show number of message on wfid %d is %d\n", wfid,this->GetNumberWaveformOfMessages(wfid));
//		}
//	}
//
//	return ADDNODERV_DEST_ADDED_TO_EXISTING_WF_MSG;
//#endif
}



}//end of namespace
}
