////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include "WaveformFasade.h"

#include "EventSocket.h"

using namespace Waveform;

extern IO_Signals ioSignals;

namespace Waveform {

extern bool EventInitialized;
extern EventSocket eventSocket;

	
WaveformFasade::WaveformFasade(WaveformId_t wid, Waveform_I<uint64_t> *_wi){
	WID = wid;
	wi = _wi;
	del = new SocketSignalDelegate_t(this, &WaveformFasade::OnSignal); 
	//master = new SocketServer("localhost", WF_SOCKET, TCP_SOCK);
	//wfMasterSocket = master->GetSocketID();
	//ioSig = new IO_Signals();
	if(!EventInitialized){
		eventSocket.Connect(wid);
	}
	calllSocketID = eventSocket.GetSocketID();
	if(calllSocketID){
		EventInitialized=true;
		ioSignals.Register(calllSocketID, del);
	}
	
}


void WaveformFasade::OnSignal(int32_t sockfd)
{
	//You should read the socket here

	//A call from framework for me.
	//printf("WaveformFasade::OnSignal: Received a signal for socket %d \n", sockfd);
	if (sockfd == calllSocketID) {
		printf("WaveformFasade::OnSignal: Received a signal on the Command socket %d \n", sockfd);
		OnRecv(sockfd);
		
	}else {
		printf("WaveformFasade::OnSignal: Received a signal for unknown socketsocket %d \n", sockfd);
		
	}
	
}

void WaveformFasade::OnRecv(int sockfd)
{
	Debug_Printf(DBG_SHIM, "WaveformFasade::OnRecv: Reading from socket %d \n", sockfd);
	int32_t readBytes = recv(sockfd,readBuf, MAX_SOC_BUF_SIZE, 0);
	Debug_Printf(DBG_SHIM, "WaveformFasade::OnRecv: Read %d bytes from socket %d \n", readBytes, sockfd);

	if (readBytes > 0){
		Deserialize (readBuf,readBytes, sockfd);
	}else {
		if(readBytes ==0){
			Debug_Printf(DBG_SHIM, "WaveformFasade::Deserialize: Got a signal on socket, but couldnt read. Probably the waveform terminated \n");
			//Cleanup(sockfd);
		}
		
	}
	
	
}


void WaveformFasade::Deserialize(char* buf, uint32_t size, int32_t sockfd)
{
	Debug_Printf(DBG_SHIM, "WaveformFasade::Deserialize: Got a message of size %u, looking it up..\n", size);
	//Debug_Printf(DBG_SHIM, "WaveformFasade::Deserialize: Message %s\n", buf);
	PrintMsg(buf,size);
	WFMethod *msg = (WFMethod*) buf;
	switch (msg->callType) {
		case WF_Call_Greet:
		{
			Debug_Printf(DBG_SHIM, "WaveformFasade::Deserialize: Greetings from Framework for waveform \n");
			break;
		}
		case WF_Call_Send:{
			SendCallMsg *c = (SendCallMsg* )msg;
			Debug_Printf(DBG_SHIM, "WaveformFasade::Deserialize: Received SendData message wit id %d and size %d \n", c->msgId, c->payloadSize);
			uint8_t *payload = new uint8_t[c->payloadSize];
			memcpy(payload, buf+sizeof(SendCallMsg), c->payloadSize);
			c->wfMsg.SetPayload(payload);
			if(wi){
				wi->SendData(c->wfMsg, c->payloadSize, c->destArray, c->noOfDest, c->msgId, c->noAck);
			}
			break;
		}
		case WF_Call_Broadcast: {
			BroadcastCallMsg *c = (BroadcastCallMsg* )msg;
			Debug_Printf(DBG_SHIM, "WaveformFasade::Deserialize: Received Broadcast message wit id %d and size %d \n", c->msgId, c->payloadSize);
			uint8_t *payload = new uint8_t[c->payloadSize];
			memcpy(payload, buf+sizeof(BroadcastCallMsg), c->payloadSize);
			c->wfMsg.SetPayload(payload);
			if(wi){
				wi->BroadcastData(c->wfMsg, c->payloadSize, c->msgId);
			}
			break;
		}
		case WF_Call_AddDest:{
			AddDestinationRequestCallMsg *c = (AddDestinationRequestCallMsg* )msg;
			if(wi){
				wi->AddDestinationRequest(c->rId, c->msgId, c->destArray, c->noOfDestinations);
			}
			break;
		}
		case WF_Call_Cancel:{
			CancelDataCallMsg *c = (CancelDataCallMsg* )msg;
			wi->CancelDataRequest(c->rId, c->msgId, c->destArray, c->noOfDestinations);
			break;
		}
		case WF_Call_ReplacePayload:{
			ReplacePayloadCallMsg *c = (ReplacePayloadCallMsg* )msg;
			wi->ReplacePayloadRequest(c->rId, c->msgId, c->payload, c->payloadSize );
			break;
		}
		case WF_Call_Attributes:{
			AttributeRequestCallMsg *c = (AttributeRequestCallMsg* )msg;
			wi->AttributesRequest(c->rId);
			break;
		}
		case WF_Call_ControlStatus:{
			ControlStatusRequestCallMsg *c = (ControlStatusRequestCallMsg* )msg;
			wi->ControlStatusRequest(c->rId);
			break;			
		}
		case WF_Call_DataStatus:{
			DataStatusRequestCallMsg *c = (DataStatusRequestCallMsg* )msg;
			wi->DataStatusRequest(c->rId, c->msgId);
			break;	
		}
		default:
			Debug_Printf(DBG_SHIM, "WaveformFasade::Deserialize: Unknown message \n");
			PrintMsg(buf, size);
	};
}

void WaveformFasade::PrintMsg(char* buf, uint32_t size)
{
	for(uint32_t i=0; i<size; i++){
		printf("%02X ", buf[i]);
	}
	printf("\n");
}


} //End namespace
