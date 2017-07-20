/*
 * BasicPattern.cpp
 *
 *  Created on: Jun 16, 2017
 *      Author: bora
 */

#include "BasicPattern.h"

namespace Patterns {


BasicPattern::BasicPattern() : PatternBase(Core::Naming::NON_STD_PTN, (char*)"NON_STD_PTN_1"){
	//delegate handles to the events
	eventDel_ReSendUpdate = new TimerDelegate(this, &BasicPattern::ReSendHandler);


	//Initialize pattern neighbor table
	myNbrHood = new PatternNeighborTable(QUALITY_LC);

	//Initialize variables facilitating message passing
	nonce=1;
	hasFWRejectedPacket = false;

}
bool BasicPattern::Start(){
	Debug_Printf(DBG_PATTERN, "BasicPattern::Start Starting BasicPattern Pattern \n");
	Debug_Printf(DBG_PATTERN, "BasicPattern:: Sending the RegisterPattern Request to framework...\n");

	FRAMEWORK->RegisterPatternRequest (PID, uniqueName, Core::Naming::GOSSIP_PTN);
	++n_ExpectedFrameworkResponses;

	return true;
}

bool BasicPattern::Stop(){
	if(!randEvent_ReSendUpdate->Suspend()) return false;
	return true;
}



void BasicPattern::Handle_AttributeResponse(ControlResponseParam response){
	FrameworkAttributes *atr = (FrameworkAttributes*) response.data;
	fwAttributes = *atr;

	if( patternState == REGISTERED) {
		patternState = EXECUTING;
	}
	else {
		Debug_Printf(DBG_PATTERN, "PatternBase::Pattern getting initiated without registration.\n");
	}

	Debug_Printf(DBG_PATTERN,"BasicPattern:: Handle_AttributeResponse: Framework supports %d waveforms, with max pkt size is %d .\n", fwAttributes.numberOfWaveforms, fwAttributes.maxFrameworkPacketSize);
	for(uint i=0; i<fwAttributes.numberOfWaveforms; i++){
		Debug_Printf(DBG_PATTERN, "BasicPattern::Handle_AttributeResponse: Waveform %d Id is %d\n", i, fwAttributes.waveformIds[i]);
	}
}

void BasicPattern::Handle_LinkThresholdResponse(ControlResponseParam response){
	SetLinkThresholdResponse_Data *data = (SetLinkThresholdResponse_Data*) response.data;
	if(data->status){
		Debug_Printf(DBG_PATTERN,"BasicPattern:: SetLinkThresholdResponse:  succeess \n");
		requestState = NONE_PENDING;
		patternState = EXECUTING;
	}else {
		Debug_Printf(DBG_PATTERN,"BasicPattern:: SetLinkThresholdResponse: Failed???, shouldnt \n");
		SetNeighborhoodandLinkComparator();
	}
}

void BasicPattern::Handle_SelectDataNofiticationResponse(ControlResponseParam response){
	SelectDataNotificationResponse_Data *data = (SelectDataNotificationResponse_Data*) response.data;
	if(data->status){
		Debug_Printf(DBG_PATTERN,"BasicPattern:: SelectDataNotificationResponse:  succeess \n");
	}else {
		Debug_Printf(DBG_PATTERN,"BasicPattern:: SelectDataNotificationResponse: Failed???, shouldnt \n");
		RequestDataNotifications();
	}

}



void BasicPattern::SetNeighborhoodandLinkComparator(){
	FRAMEWORK->SelectLinkComparatorRequest(PID,PWI::Neighborhood::QUALITY_LC);
	++n_ExpectedFrameworkResponses;
	//FRAMEWORK->SelectLinkComparatorRequest (PID, QUALITY_LC);
	LinkMetrics myThreshold;
//	myThreshold.quality = 0.0010;


	FRAMEWORK->SetLinkThresholdRequest(PID,myThreshold);
	requestState = WAITING_FOR_CONTROL_RESPONSE;
	++n_ExpectedFrameworkResponses;
}

void BasicPattern::RequestDataNotifications(){
	uint8_t mask = PDN_RECV_DEST_MASK;
	FRAMEWORK->SelectDataNotificationRequest(PID, mask);
	requestState = WAITING_FOR_CONTROL_RESPONSE;
	++n_ExpectedFrameworkResponses;
}

void BasicPattern::NeighborUpdateEvent(NeighborUpdateParam nbrUpdate)
{
	if(myNbrHood != NULL) myNbrHood->UpdateTable(nbrUpdate);
}

void BasicPattern::DataStatusEvent(DataStatusParam ntfy)
{
	///You have received ack for previous message.
	//check what happened to the previous message
	Debug_Printf(DBG_PATTERN,"BasicPattern:: Received a data notification from framework\n");
	if (nonce == ntfy.nonce) randEvent_ReSendUpdate->Suspend();
	for (uint16_t i=0; i< ntfy.noOfDest; i++){
		switch (ntfy.statusType[i]){
		case PDN_FW_RECV:
			if(ntfy.statusValue[i] ) {
				Debug_Printf(DBG_PATTERN,"BasicPattern:: Message with nonce = %d is assigned a message id = %d and is accepted by the framework \n", ntfy.nonce, ntfy.messageId);
//					ClearSelectedNeighborList();
				nonce++;
			}
			else {
				Debug_Printf(DBG_PATTERN,"BasicPattern:: Message with nonce = %d is rejected by the framework\n", ntfy.nonce);
				hasFWRejectedPacket = true;
//					if (ntfy.readyToReceive) {
//						SendMessage();
//					}
//					else {
//						hasFWRejectedPacket = true;
//					}
			}
			break;
		case PDN_WF_RECV:
			break;
		case PDN_WF_SENT:
			Debug_Printf(DBG_PATTERN,"BasicPattern:: Waveform has sent the message id: %d\n", ntfy.messageId);
			break;
		case PDN_DST_RECV:
			if(!ntfy.statusValue[i]){
				Debug_Printf(DBG_PATTERN,"BasicPattern:: Message id %d not received by destination\n", ntfy.messageId);
			}
			break;

		default:
			break;
		}
	}
}
void BasicPattern::ControlResponseEvent(ControlResponseParam response)
{
	Debug_Printf(DBG_PATTERN, "BasicPattern:: Got a control response of type %d\n", response.type);
	switch (patternState){
	case UNREGISTERED:
		if(response.type == PWI::PTN_RegisterResponse){
			--n_ExpectedFrameworkResponses;
			Handle_RegisterResponse(response);
			FRAMEWORK->FrameworkAttributesRequest(PID);
			++n_ExpectedFrameworkResponses;
		}else {
			Debug_Printf(DBG_PATTERN, "BasicPattern:: ControlResponseEvent: My state is %d I got wrong response type %d\n", patternState, response.type);
		}
		break;
	case REGISTERED:
		if(response.type == PWI::PTN_AttributeResponse){
			--n_ExpectedFrameworkResponses;
			Handle_AttributeResponse(response);
			InitiateProtocol();

		}else {
			Debug_Printf(DBG_PATTERN, "BasicPattern:: ControlResponseEvent: My state is %d, I got wrong response type %d\n", patternState,response.type);
		}
		break;
	case EXECUTING:
		switch(response.type){
		case PTN_SelectDataNotificationResponse:
			Handle_SelectDataNofiticationResponse(response);
			break;
		case PTN_SetLinkThresholdResponse:
			Handle_LinkThresholdResponse(response);
			break;
		case PTN_AttributeResponse:
			Handle_AttributeResponse(response);
			InitiateProtocol();
			break;
		case PTN_AddDestinationResponse:
			Handle_AddDestinationResponse(response);
			break;
		case PTN_ReplacePayloadResponse:
			Handle_ReplacePayloadResponse(response);
			break;
		case PTN_CancelDataResponse:
			Handle_CancelDataResponse(response);
			break;
		default:
			Debug_Printf(DBG_PATTERN, "BasicPattern:: ControlResponseEvent: My state is %d, I got wrong response type %d\n", patternState,response.type);
			break;
		}
		break;
		default:
			break;
	}
}

bool BasicPattern::InitiateProtocol(){
	//Initialize neighborhood and set link comparator.
	SetNeighborhoodandLinkComparator();

	//tell framework which notifications you are interested in
	RequestDataNotifications();


	randEvent_ReSendUpdate = new Timer(ReSendTimeOutInterval, ONE_SHOT, *eventDel_ReSendUpdate);
	randEvent_ReSendUpdate->Suspend();

	return true;
}



} /* namespace Patterns */
