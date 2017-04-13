////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#include "FWP.h"

extern NodeId_t MY_NODE_ID;

// extern bool DBG_PATTERN; // set in Lib/Misc/Debug.cpp
// extern bool DBG_CORE; // set in Lib/Misc/Debug.cpp

namespace Patterns {

FWP::FWP(FWP2AppShim_I* pattern2appshimptr)
: PatternBase(Core::Naming::FWP_PTN, (char*)"FWP_1")
{
//	eventdispatcher = new FWP2AppShim();
	eventdispatcher = pattern2appshimptr;
	//Initialize pattern neighbor table
	myNeighborTable = new PatternNeighborTable(QUAL_XMIT_DELAY_LC);

	//Initialize variables facilitating message passing
	nonce = 1;
	sendBuffer = new FIFOList();
	//rcvBuffer = new FIFOList();

	active=false;

	Debug_Printf(DBG_PATTERN,"FWP:: Construction done");
}

FWP::~FWP(){

}

bool FWP::Start()
{
	requestState = WAITING_FOR_CONTROL_RESPONSE;
	Debug_Printf(DBG_PATTERN, "FWP:: Sending the RegisterPattern Request to framework...\n");
	FRAMEWORK->RegisterPatternRequest (PID, (char*)"FWP_1", Core::Naming::FWP_PTN);


	return true;
}

bool FWP::Stop()
{
	active = false;
	return true;
}

//void FWP::SendData (AppId_t _app_id, AppMessage_t& msg, uint16_t nonce){
//	Send((void*)msg.GetPayload(), msg.GetPayloadSize());
//	AppMessage_t* msgptr = &msg;
//	delete msgptr;
//}

void FWP::Send (void *data, uint16_t size){
	if(patternState == EXECUTING && fwAttributes.maxFrameworkPacketSize > 0 && fwAttributes.numberOfWaveforms > 0){
		//Divide data into packet chunks and send
		uint8_t *dataPtr = (uint8_t *) data;
		uint16_t maxPayload = fwAttributes.maxFrameworkPacketSize;
		uint16_t bytes_rem = size % fwAttributes.maxFrameworkPacketSize;
		uint16_t no_of_pkts = size/fwAttributes.maxFrameworkPacketSize;
		if(bytes_rem > 0){
			no_of_pkts++;
		}

		FMessage_t *sendMsg;
		uint16_t payloadsize;
		Debug_Printf(DBG_PATTERN,"Flooding data of size %d , max payload per packet: %d\n", size, fwAttributes.maxFrameworkPacketSize);
		uint16_t cumSize=0;
		//First put everything in your local buffer
		for (uint16_t i=0; i < no_of_pkts; i++){
			for(uint16_t j=0; j< fwAttributes.numberOfWaveforms; j++){
				if(size-cumSize > maxPayload){
					//sendMsg->SetPayloadSize(maxPayload);
					payloadsize = maxPayload;
				}else {	//send what ever is left and exit loop
//					sendMsg->SetPayloadSize(size-cumSize);
					payloadsize = size-cumSize;
				}
				sendMsg = new FMessage_t(payloadsize);
				//sendMsg->src=MY_NODE_ID;
				sendMsg->SetType(Types::PATTERN_MSG);
				//sendMsg->SetInstance(PID);

//				sendMsg->SetPayload(&dataPtr[i*maxPayload]);
				memcpy( &dataPtr[i*maxPayload], sendMsg->GetPayload(), payloadsize);

				sendMsg->SetWaveform(fwAttributes.waveformIds[j]);
				printf("Enqueing packet %d (ptr %lu) with payload size %d\n", i, (uint64_t)sendMsg, sendMsg->GetPayloadSize());
				sendBuffer->InsertMessage(sendMsg);
			}
			cumSize+=sendMsg->GetPayloadSize();
		}
		Debug_Printf(DBG_PATTERN,"FWP:: Done enqueing packets to the collector \n");

		//take a message out and send it
		DequeAndSend();
	}else {
		Debug_Printf(DBG_PATTERN,"FWP:: Sorry!! Not ready. \n");
	}
	fflush(stdout);
}

void FWP::Handle_AttributeResponse(ControlResponseParam response){
	FrameworkAttributes *atr = (FrameworkAttributes*) response.data;
	fwAttributes = *atr;
	if (fwAttributes.maxFrameworkPacketSize == 0 ) fwAttributes.maxFrameworkPacketSize = 100;
	if (fwAttributes.numberOfWaveforms == 0 ) fwAttributes.numberOfWaveforms = 1;
	requestState = NONE_PENDING;
	patternState = EXECUTING;
	active=true;
	Debug_Printf(DBG_PATTERN,"FWP:: Handle_AttributeResponse: Framework supports %d waveforms, with max pkt size is %d .\n", fwAttributes.numberOfWaveforms, fwAttributes.maxFrameworkPacketSize);
	for(uint i=0; i<fwAttributes.numberOfWaveforms; i++){
		Debug_Printf(DBG_PATTERN, "FWP::Handle_AttributeResponse: Waveform %d Id is %d\n", i, fwAttributes.waveformIds[i]);
	}
}

void FWP::ControlResponseEvent(ControlResponseParam response)
{
	Debug_Printf(DBG_PATTERN, "PatternBase:: Got a control response of type %d\n", response.type);
	switch (patternState){
	case NO_PID:
	case GOT_PID:
		if(response.type == PWI::PTN_RegisterResponse){
			Handle_RegisterResponse(response);
			Debug_Printf(DBG_PATTERN,"FWP:: Sending the attribute request to the framework.\n");
			FRAMEWORK->FrameworkAttributesRequest(PID);
			requestState = WAITING_FOR_CONTROL_RESPONSE;
		}else {
			Debug_Printf(DBG_PATTERN, "FWP:: ControlResponseEvent: My state is %d I got wrong response type %d\n", patternState, response.type);
		}
		break;
	case REGISTERED:
		if(response.type == PWI::PTN_AttributeResponse){
			Handle_AttributeResponse(response);
			InitiateProtocol();
		}else {
			Debug_Printf(DBG_PATTERN, "FWP:: ControlResponseEvent: My state is %d, I got wrong response type %d\n", patternState,response.type);
		}
		break;
	case EXECUTING:
		switch(response.type){
		case PTN_AddDestinationResponse:
			break;
		case PTN_ReplacePayloadResponse:
			break;
		case PTN_CancelDataResponse:
			break;
		case PTN_SelectDataNotificationResponse:
			break;
		case PTN_SetLinkThresholdResponse:
			break;
		case PTN_AttributeResponse:
			Handle_AttributeResponse(response);
			InitiateProtocol();
			break;
		default:
			break;
		}
		break;
		default:
			break;
	}
}

bool FWP::InitiateProtocol(){
	active = true;
	return true;
}

void FWP::DataStatusEvent(DataStatusParam notify)
{
	///You have received ack for previous message.
	///Check if you have anymore messages in sendBuffer and send it out.
	for(uint16_t i=0; i<notify.noOfDest; i++){
		Debug_Printf(DBG_PATTERN,"FWP:: Recived an ack of type %d for dest %d my packet, for my packet %d \n", notify.statusType[i], notify.destArray[i], notify.messageId);

		switch(notify.statusType[i]){
			case PDN_FW_RECV:
				if(!notify.statusValue[i]){
					Debug_Printf(DBG_PATTERN,"FWP:: Framework failed to take my packet. Putting packet back in the send queue \n");
					sendBuffer->InsertFront(outgoingPacket);
				}else {
					Debug_Printf(DBG_PATTERN,"FWP:: Framework accepted my packet and gave a message id %d \n",notify.messageId);
					sentMap.Insert(notify.messageId,outgoingPacket);
				}
				break;
			case PDN_WF_RECV:
				if(!notify.statusValue[i]){
					Debug_Printf(DBG_PATTERN,"FWP:: Framework failed to send message to waveform. Put packet back in the send queue? \n");
				}else {
					Debug_Printf(DBG_PATTERN,"FWP:: Framework sent packet %d to waveform  \n",notify.messageId);
					CleanUpBuffers(notify.messageId);
				}
				break;
			case PDN_WF_SENT:
				if(!notify.statusValue[i]){
					Debug_Printf(DBG_PATTERN,"FWP:: Framework failed to send message to waveform. Put packet back in the send queue? \n");
				}else {
					Debug_Printf(DBG_PATTERN,"FWP:: Waveform sent packet %d out  \n",notify.messageId);
				}
				break;
			case PDN_DST_RECV:
				break;
			case PDN_ERROR_PKT_TOOBIG:
				Debug_Printf(DBG_PATTERN,"FWP:: Framework rejected message because its too big\n");
				break;
			case PDN_ERROR:
				break;
			case PDN_BROADCAST_NOT_SUPPORTED:
				break;
		}
	}

	if(notify.readyToReceive ) {DequeAndSend();}
}

void FWP::DequeAndSend(){
	if(sendBuffer->Size()>0){
		nonce++;
		outgoingPacket = sendBuffer->RemoveMessage ();

		Debug_Printf(DBG_PATTERN,"FWP:: Sending broadcast data of payload size %d, on waveform %d with nonce %d, packets left in buffer %d\n",outgoingPacket->GetPayloadSize(), outgoingPacket->GetWaveform(), nonce, sendBuffer->Size());
#if ENABLE_FW_BROADCAST==1
		FRAMEWORK->BroadcastData(PID, *outgoingPacket, outgoingPacket->GetWaveform(), nonce);
#else
		this->RandomLocalSpray(PID, *outgoingPacket, SPRAY_THREE_NBR, true, *myNeighborTable, nonce);
#endif
	}
	else {
		Debug_Printf(DBG_PATTERN,"FWP:: No more packets in send buffer\n");
	}
}

void FWP::ReceiveMessageEvent(FMessage_t& msg)
{
	Debug_Printf(DBG_PATTERN,"FWP:: Reveived a message %lu from %d of size %d\n", (uint64_t)&msg, msg.GetSource(), msg.GetPayloadSize() );

	//AppMessage_t appmsg(msg.GetPayloadSize(),msg.GetPayload());
	//TODO: BK: Destruct FMessage or better think about creating AppMessage_t without destructing FMessage_t
	eventdispatcher->ReceiveMessage(msg.GetPayload(), msg.GetPayloadSize());
}

void FWP::InserBackToSendBuffer(MessageId_t id){
	FMessage_t *msg = sentMap[id];
	sendBuffer->InsertFront(msg);
	sentMap.Erase(id);
}

void FWP::CleanUpBuffers(MessageId_t id){
	sentMap.Erase(id);
}


void FWP::NeighborUpdateEvent(NeighborUpdateParam nbrUpdate)
{
	myNeighborTable->UpdateTable(nbrUpdate);

	switch (nbrUpdate.changeType){
	case NBR_NEW:
		Debug_Printf(DBG_PATTERN,"FWP: Received new nbr notification for node %d\n", nbrUpdate.nodeId);
		break;
	case NBR_UPDATE:
		//Debug_Printf(DBG_PATTERN,"FWP: Received nbr update notification for node %d\n", nbrUpdate.nodeId);
		break;
	case NBR_DEAD:
		Debug_Printf(DBG_PATTERN,"FWP: Received dead nbr notification for node %d\n", nbrUpdate.nodeId);
		break;
	}
}

void FWP::RegisterAppReceiveDelegate(AppId_t _app_id, AppRecvMessageDelegate_t* _gvu_del){
	eventdispatcher->RegisterAppReceiveDelegate(_app_id, _gvu_del);
}

//void FWP::RegisterAppReceptionDelegate(string app_name, RecvMessageDelegate_t* _gvu_del){
//	app_delegates_list.Insert(app_name, _gvu_del);
//};

} // End namespace
