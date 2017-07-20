////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include "Lib/PAL/PAL_Lib.h"
#include "Framework/Core/Naming/StaticNaming.h"
#include "Lib/Pattern/NeighborTable/PatternNeighborTable.h"
#include "Platform/linux/PAL/Logs/FileLoggerImpl.h"
#include <stdio.h>

#include "Pplsc.h"

extern uint16_t MY_NODE_ID;

// extern bool DBG_PATTERN; // set in Lib/Misc/Debug.cpp

using namespace Core::Dataflow;

namespace Patterns {



Pplsc::Pplsc() : PatternBase(Core::Naming::NON_STD_PTN, (char*)"Pplsc_1"){
	Debug_Printf(DBG_PATTERN, "Pplsc::Initializing\n");

	m_numofMessagesSend = 0;
	//delegate handles to the events
	eventDel_SendUpdate = new EventDelegate(this, &Pplsc::RandomSendHandler);

	//Creates a uniform random number with mean 200,000 and with range +/_100,000
	//That is the values vary between 100,000 to 300,000
	RNStreamID cmrgState;
	cmrgState.stream = MY_NODE_ID;
	UniformRNGState _state(cmrgState);
	UniformIntDistParameter dist; dist.min = MeanUpdateInterval - RangeUpdateInterval; dist.max = MeanUpdateInterval + RangeUpdateInterval ;
	_state.SetDistributionParameters(dist);
	rand = new UniformRandomInt(_state);


	hasFWRejectedPacket = false;

	myNbrHood = NULL;
	myNbrHood = new PatternNeighborTable(QUALITY_LC);

	msg_logger_ptr_rx = new FileLogger<PplscRecvMsgLogElement>("PplscReceivedMessages", true, false);
	msg_logger_ptr_rx->addRecord(PplscRecvMsgLogElement(1,2,3));
	msg_logger_ptr_tx = new FileLogger<PplscSentMsgLogElement>("PplscSentMessages", true, false);
}

void Pplsc::ControlResponseEvent(ControlResponseParam response)
{
	Debug_Printf(DBG_PATTERN, "Pplsc:: Got a control response of type %d\n", response.type);

	switch (response.type){
	    case PWI::PTN_RegisterResponse:
	        Handle_RegisterResponse(response);
            FRAMEWORK->FrameworkAttributesRequest(PID);
            InitiateProtocol();
            break;
	    case PWI::PTN_AttributeResponse:
	        Handle_AttributeResponse(response);
	        break;
        case PTN_AddDestinationResponse:
            break;
        case PTN_ReplacePayloadResponse:
            break;
        case PTN_CancelDataResponse:
            break;
        case PTN_SelectDataNotificationResponse:
            Handle_SelectDataNofiticationResponse(response);
            break;
        case PTN_SetLinkThresholdResponse:
            Handle_LinkThresholdResponse(response);
            break;
        default:
        	Debug_Printf(DBG_PATTERN, "Pplsc::ControlResponseEvent !! UNKNWON RESPONSE = %d !! \n", response.type);
            break;
	    break;

	}


}

void Pplsc::DataStatusEvent(DataStatusParam ntfy)
{
	///You have received ack for previous message.
	//check what happened to the previous message
	Debug_Printf(DBG_PATTERN,"Pplsc:: Received a data notification from framework\n");
	for (uint16_t i=0; i< ntfy.noOfDest; i++){
	    switch (ntfy.statusType[i]){
	    case PDN_FW_RECV:
		    if(ntfy.statusValue[i] ) {
			   Debug_Printf(DBG_PATTERN,"Pplsc:: Message with nonce = %d is assigned a message id = %d and is accepted by the framework \n", ntfy.nonce, ntfy.messageId);
				    nonce++;
		    }
		    else {
			   Debug_Printf(DBG_PATTERN,"Pplsc:: Message with nonce = %d is rejected by the framework\n", ntfy.nonce);
			    if (ntfy.readyToReceive) {
				    SendMessage();
			    }
			    else {
				    hasFWRejectedPacket = true;
			    }
		    }
		    break;
	    case PDN_WF_RECV:
	    	Debug_Printf(DBG_PATTERN,"Pplsc:: Waveform has received the message id: %d\n", ntfy.messageId);
		    break;
	    case PDN_WF_SENT:
		   Debug_Printf(DBG_PATTERN,"Pplsc:: Waveform has sent the message id: %d\n", ntfy.messageId);
		    break;
	    case PDN_DST_RECV:
		    if(!ntfy.statusValue[i]){
			   Debug_Printf(DBG_PATTERN,"Pplsc:: Message id %d not received by destination\n", ntfy.messageId);

		    }
		    break;

	    default:
		    break;
	    }
	}
	if (ntfy.readyToReceive && hasFWRejectedPacket) { //If the previous message was successfully received by the framework
		SendMessage();
	}
}

void Pplsc::NeighborUpdateEvent(NeighborUpdateParam nbrUpdate)
{
    if(myNbrHood != NULL) myNbrHood->UpdateTable(nbrUpdate);
	/*
  switch (nbrUpdate.changeType) {
  case NBR_NEW:
	  SelectNeighbors(nbrUpdate.link.linkId.nodeId);
	  break;
  case NBR_DEAD:
	  SelectedNeighborList.Delete(nbrUpdate.link.linkId.nodeId);
	  break;
  case NBR_UPDATE:
	  break;
  default:
	 Debug_Printf(DBG_PATTERN,"Pplsc:: ReceiveNeighborUpdates unknown update\n");
	  break;
  }*/
}

void Pplsc::ReceiveMessageEvent(FMessage_t& msg)
{
	PplscMsg* gMsg = (PplscMsg*) msg.GetPayload();
	//PplscRecvMsgLogElement logel(msg.GetSource(), msg.GetWaveform());
	msg_logger_ptr_rx->addRecord(PplscRecvMsgLogElement(msg.GetSource(), msg.GetWaveform(),gMsg->numofMessagesSend));
	Debug_Printf(DBG_PATTERN, "Pplsc:: Received msg from %u on wfID = %d with status seq %u \n",msg.GetSource(), msg.GetWaveform(), gMsg->numofMessagesSend);
	//Process the recevied message
}

void Pplsc::Handle_AttributeResponse(ControlResponseParam response){
	FrameworkAttributes *atr = (FrameworkAttributes*) response.data;
	fwAttributes = *atr;
	requestState = NONE_PENDING;
	patternState = EXECUTING;

	Debug_Printf(DBG_PATTERN,"Pplsc:: Handle_AttributeResponse: Framework supports %d waveforms, with max pkt size is %d .\n", fwAttributes.numberOfWaveforms, fwAttributes.maxFrameworkPacketSize);
	for(uint i=0; i<fwAttributes.numberOfWaveforms; i++){
		Debug_Printf(DBG_PATTERN, "Pplsc::Handle_AttributeResponse: Waveform %d Id is %d\n", i, fwAttributes.waveformIds[i]);
	}
}

void Pplsc::Handle_LinkThresholdResponse(ControlResponseParam response){
	SetLinkThresholdResponse_Data *data = (SetLinkThresholdResponse_Data*) response.data;
	if(data->status){
		Debug_Printf(DBG_PATTERN,"Pplsc:: SetLinkThresholdResponse:  succeess \n");
	}else {
		Debug_Printf(DBG_PATTERN,"Pplsc:: SetLinkThresholdResponse: Failed???, shouldnt \n");
		SetNeighborhoodandLinkComparator();
	}
}

void Pplsc::Handle_SelectDataNofiticationResponse(ControlResponseParam response){
	SelectDataNotificationResponse_Data *data = (SelectDataNotificationResponse_Data*) response.data;

	if(data->status){
		Debug_Printf(DBG_PATTERN,"Pplsc:: SelectDataNotificationResponse:  succeess \n");
	}else {
		Debug_Printf(DBG_PATTERN,"Pplsc:: SelectDataNotificationResponse: Failed???, shouldnt \n");
		RequestDataNotifications();
	}
}

bool Pplsc::Start(){
	Debug_Printf(DBG_PATTERN, "Pplsc::Start Starting gossiping \n");
	Debug_Printf(DBG_PATTERN, "Pplsc:: Sending the RegisterPattern Request to framework...\n");

	FRAMEWORK->RegisterPatternRequest (PID, uniqueName, Core::Naming::NON_STD_PTN);
	return true;
}

void Pplsc::SetNeighborhoodandLinkComparator(){
	FRAMEWORK->SelectLinkComparatorRequest(PID,PWI::Neighborhood::COST_LC);
	LinkMetrics myThreshold;
	myThreshold.cost = 9999;

	FRAMEWORK->SetLinkThresholdRequest(PID,myThreshold);
}

void Pplsc::RequestDataNotifications(){
	uint8_t mask = PDN_RECV_DEST_MASK;
	FRAMEWORK->SelectDataNotificationRequest(PID, mask);
}

bool Pplsc::InitiateProtocol(){
	//Initialize neighborhood and set link comparator.
	SetNeighborhoodandLinkComparator();

	//tell framework which notifications you are interested in
	RequestDataNotifications();

	Debug_Printf(DBG_PATTERN, "Pplsc::Have configured everything succeessfully. Good to go. \n");
	uint64_t eventDelay = rand->GetNext();
	randEvent_SendUpdate = new Event(eventDelay, *eventDel_SendUpdate, (void *) 0);

	return true;
}

bool Pplsc::Stop()
{
	delete msg_logger_ptr_rx;
	delete msg_logger_ptr_tx;
	randEvent_SendUpdate->Cancel();
	return true;
}

//Handler for the RandomSend event
void Pplsc::RandomSendHandler(EventDelegateParam param){
	//Lets us first reshedule the event
	uint64_t eventDelay = rand->GetNext();
	uint64_t curTime = SysTime::GetEpochTimeInMicroSec();
	param.eventPtr->ReSchedule(eventDelay,(void *) (0));
	Debug_Printf(DBG_PATTERN,"Pplsc::RandomSend:: Rescheduled event to fire at %lu\n",curTime+eventDelay);fflush(stdout);
	//Now lets broadcast our status
	AdjustSelectedNeighbors();
	if(SelectedNeighborList.Size() > 0) SendMessage(); //If I have at least one destination send this message
}

void Pplsc::AdjustSelectedNeighbors() {
	if(SelectedNeighborList.Size() > 0) return; //Already selected neighbors based on other criteria
	uint16_t nbrCount = myNbrHood->GetNumberOfNeighbors();
	Debug_Printf(DBG_PATTERN,"Pplsc:: Got %d neighbors  \n", nbrCount);fflush(stdout);
	if (nbrCount == 0) return;//Return if there are no neighbors to choose from
	NodeId_t sNeighbor;
	uint16_t i=0;
	while(SelectedNeighborList.Size() == 0) {
		uint64_t pickrandom = rand->GetNext();
		pickrandom = pickrandom % nbrCount;
		Debug_Printf(DBG_PATTERN,"Pplsc:: Got %d neighbors and picking %lu neighbor from tabel  \n", nbrCount, pickrandom);fflush(stdout);
		PatternNeighborIterator it = myNbrHood->Begin();
		for(uint16_t ii=0; ii< nbrCount; ++ii) {
			if(ii != 0) it=it.GetNext();
			if(pickrandom == 0) {	
				sNeighbor = (it->linkId.nodeId);
			}
			--pickrandom;
			Debug_Printf(DBG_PATTERN,"Pplsc:: Iterating neighbor table (index, ID) =  (%d,%d) \n",ii, it->linkId.nodeId);fflush(stdout);


		}
		/*if(pickrandom == 0) {
			sNeighbor = (it->linkId.nodeId);
		}*/


		Debug_Printf(DBG_PATTERN,"Pplsc:: Selected neighbor %d \n", sNeighbor);fflush(stdout);
		if( SelectNeighbors(sNeighbor) ) {
			Debug_Printf(DBG_PATTERN,"Pplsc:: PickRandomNeighbor %d \n", sNeighbor);fflush(stdout);
		}
		i++;
		Debug_Printf(DBG_PATTERN,"Pplsc:: in while %d times  \n", i);fflush(stdout);
	}
}

bool Pplsc::SelectNeighbors(NodeId_t nbr){
	if( myNbrHood->GetNeighborLink(nbr) && !(SelectedNeighborList.Search(nbr)) ) { //If the node is my neighbor and not selected before
		Debug_Printf(DBG_PATTERN,"Pplsc:: SelectNeighbor: Ading neighbor %d neighbors  to SelectedNeighborList \n", nbr);fflush(stdout);
		return SelectedNeighborList.Insert(nbr);
	}
	return false;
}

// This function broadcast status
FMessage_t& Pplsc::PrepareStatusUpdate(){
	Debug_Printf(DBG_PATTERN, "Pplsc:: PrepareStatusUpdate\n");

	//Construct packet and send
	FMessage_t *sendMsg;
	//Make sure to use new() operator to create packet on heap.
	sendMsg = new FMessage_t(sizeof(struct PplscMsg));
	//sendMsg->SetSource(MY_NODE_ID);
	//sendMsg->SetInstance(PID);

	PplscMsg* gMsg = (PplscMsg*) sendMsg->GetPayload();
	gMsg->numofMessagesSend=m_numofMessagesSend;
	return (*sendMsg);
}


// This function sends a message to the list of selected neighbors
void Pplsc::SendMessage() {
	if(SelectedNeighborList.Size() == 0) {
		Debug_Printf(DBG_PATTERN, "Pplsc::SendMessage to %d destinations. Should not happen! Recheck algorithm! \n", SelectedNeighborList.Size()); fflush(stdout);
		return;
	}
	NodeId_t selnodes[SelectedNeighborList.Size()];
	PplscNeigborContainerTypeElement* elptr = SelectedNeighborList.Begin();
	int i=0;
	while(elptr){
		selnodes[i] = elptr->GetData();
		elptr = SelectedNeighborList.Next(elptr);
		++i;
	}
	myNbrHood->PrintTable();

	if(m_numofMessagesSend % 2 == 0){
			Debug_Printf(DBG_PATTERN, "Pplsc::SendMessage multicasting to %d destinations, first destination is %d, with COST_LC \n", SelectedNeighborList.Size(), selnodes[0]); fflush(stdout);
			FRAMEWORK->SendData(PID, selnodes, (uint16_t) SelectedNeighborList.Size(), COST_LC, PrepareStatusUpdate(), ++nonce,false);
			msg_logger_ptr_tx->addRecord(PplscSentMsgLogElement(selnodes[0], COST_LC, m_numofMessagesSend ));
	}
	else if(m_numofMessagesSend % 2 == 1){
			Debug_Printf(DBG_PATTERN, "Pplsc::SendMessage multicasting to %d destinations, first destination is %d, with NO_SELECTION \n", SelectedNeighborList.Size(), selnodes[0]); fflush(stdout);
			FRAMEWORK->SendData(PID, selnodes, (uint16_t) SelectedNeighborList.Size(), ENERGY_LC, PrepareStatusUpdate(), ++nonce,false);
			msg_logger_ptr_tx->addRecord(PplscSentMsgLogElement(selnodes[0], ENERGY_LC, m_numofMessagesSend));
	}
	else {
			Debug_Printf(DBG_PATTERN, "Pplsc::SendMessage multicasting to %d destinations, first destination is %d, with COST_LC \n", SelectedNeighborList.Size(), selnodes[0]); fflush(stdout);
			FRAMEWORK->SendData(PID, selnodes, (uint16_t) SelectedNeighborList.Size(), PrepareStatusUpdate(), ++nonce,false);
			msg_logger_ptr_tx->addRecord(PplscSentMsgLogElement(selnodes[0], 20, m_numofMessagesSend));
	}

	/*if(m_numofMessagesSend % 5 == 0){
		Debug_Printf(DBG_PATTERN, "Pplsc::SendMessage multicasting to %d destinations, first destination is %d, with QUALITY_LC \n", SelectedNeighborList.Size(), selnodes[0]); fflush(stdout);
		FRAMEWORK->SendData(PID, selnodes, (uint16_t) SelectedNeighborList.Size(), QUALITY_LC, PrepareStatusUpdate(), nonce,false);
		msg_logger_ptr_tx->addRecord(PplscSentMsgLogElement(selnodes[0], QUALITY_LC));
	}
	else if(m_numofMessagesSend % 5 == 1){
		Debug_Printf(DBG_PATTERN, "Pplsc::SendMessage multicasting to %d destinations, first destination is %d, with COST_LC \n", SelectedNeighborList.Size(), selnodes[0]); fflush(stdout);
		FRAMEWORK->SendData(PID, selnodes, (uint16_t) SelectedNeighborList.Size(), COST_LC, PrepareStatusUpdate(), nonce,false);
		msg_logger_ptr_tx->addRecord(PplscSentMsgLogElement(selnodes[0], COST_LC));
	}
	else if(m_numofMessagesSend % 5 == 2){
		Debug_Printf(DBG_PATTERN, "Pplsc::SendMessage multicasting to %d destinations, first destination is %d, with QUAL_XMIT_DELAY_LC \n", SelectedNeighborList.Size(), selnodes[0]); fflush(stdout);
		FRAMEWORK->SendData(PID, selnodes, (uint16_t) SelectedNeighborList.Size(), QUAL_XMIT_DELAY_LC, PrepareStatusUpdate(), nonce,false);
		msg_logger_ptr_tx->addRecord(PplscSentMsgLogElement(selnodes[0], QUAL_XMIT_DELAY_LC));
	}
	else if(m_numofMessagesSend % 5 == 3){
		Debug_Printf(DBG_PATTERN, "Pplsc::SendMessage multicasting to %d destinations, first destination is %d, with NO_SELECTION \n", SelectedNeighborList.Size(), selnodes[0]); fflush(stdout);
		FRAMEWORK->SendData(PID, selnodes, (uint16_t) SelectedNeighborList.Size(), ENERGY_LC, PrepareStatusUpdate(), nonce,false);
		msg_logger_ptr_tx->addRecord(PplscSentMsgLogElement(selnodes[0], ENERGY_LC));
	}
	else if(m_numofMessagesSend % 5 == 4){
		Debug_Printf(DBG_PATTERN, "Pplsc::SendMessage multicasting to %d destinations, first destination is %d, with COST_LC \n", SelectedNeighborList.Size(), selnodes[0]); fflush(stdout);
		FRAMEWORK->SendData(PID, selnodes, (uint16_t) SelectedNeighborList.Size(), PrepareStatusUpdate(), nonce,false);
		msg_logger_ptr_tx->addRecord(PplscSentMsgLogElement(selnodes[0], 20));
	}*/
	++m_numofMessagesSend;

	hasFWRejectedPacket = false;
}

void Pplsc::ClearSelectedNeighborList(){
	PplscNeigborContainerTypeElement *elptr,*elptr2;
	elptr = SelectedNeighborList.Begin();
	while(SelectedNeighborList.Size()>0){
		elptr2 = SelectedNeighborList.Next(elptr);
		SelectedNeighborList.DeleteElement(elptr);
		elptr = elptr2;
	}
}

}	//End of namespace
