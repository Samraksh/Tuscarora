////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include "WF_ControllerShim.h"
#include "Lib/Shims/WF_API_Calls.h"
#include "Fw2Wf_SocketShim.h"
#include "Framework/Core/WF_Events/RecvWFEvents.h"

using namespace PAL;
using namespace Core::Events;
using namespace Lib::Shims;

extern IO_Signals ioSignals;

namespace Core{
namespace Events {
  extern RecvWFEvents<uint64_t>* waveformEventReceiver;
}
}




WF_ControllerShim::WF_ControllerShim(WaveformMap_t* wfMap){
	del = new SocketSignalDelegate_t(this, &WF_ControllerShim::OnSignal); 
	master = new SocketServer("localhost", WF_SOCKET, TCP_SOCK);
	wfMasterSocket = master->GetSocketID();
	//ioSig = new IO_Signals();
	ioSignals.Register(wfMasterSocket, del);

	wfIdMap = wfMap;
}


uint32_t WF_ControllerShim::NewConnection()
{
	int32_t newsock=0;
	newsock = master->Accept();

	if (newsock > 0){
		//Register this socket with the IO_Signal handler and store this in the list of waveforms
		//First read from socket and figure out the Waveform ID;
		//WaveformId_t nwfd =0;
		Debug_Printf(DBG_SHIM, "WF_ControllerShim::NewConnection: Master socket has received a new connection on socket %d \n", newsock);
		
	}else {
		Debug_Printf(DBG_SHIM, "Error: Waveform Master socket Error: Something wrong with accepting connections\n");

	}
	return newsock;
}


void WF_ControllerShim::OnSignal(int32_t sockfd){
	//You should read the socket here

	//this is the master socket, a waveform is (re)connecting
	//printf("WF_ControllerShim::OnSignal: Received a signal for socket %d \n", sockfd);
	if (sockfd == wfMasterSocket) {
		printf("WF_ControllerShim::OnSignal: Received a signal for Master socket %d \n", sockfd);
		int32_t nwWF = NewConnection();
		if (nwWF > 0) ioSignals.Register(nwWF, del);
	}else {
		//This is one of the client sockets, read message and process
		printf("WF_ControllerShim::OnSignal: Received a signal for a waveform socket %d \n", sockfd);
		OnRecv(sockfd);
	}
}

void WF_ControllerShim::OnRecv (int sockfd) {
	//int32_t readBytes = read(sockfd,readBuf, MAX_SOC_READ_SIZE);
	Debug_Printf(DBG_SHIM, "WF_ControllerShim::OnRecv: Reading from socket %d \n", sockfd);
	int32_t readBytes = recv(sockfd,readBuf, MAX_FW_SOC_READ_SIZE, 0);
	Debug_Printf(DBG_SHIM, "WF_ControllerShim::OnRecv: Read %d bytes from socket %d \n", readBytes, sockfd);

	if (readBytes > 0){
		PrintMsg(readBuf,readBytes);
		Deserialize (readBuf,readBytes, sockfd);
	}else {
		if(readBytes ==0){
			Debug_Printf(DBG_SHIM, "WF_ControllerShim::Deserialize: Got a signal on socket, but couldnt read. Probably the waveform terminated \n");
			Cleanup(sockfd);
		}
		
	}
}

bool WF_ControllerShim::Deserialize (char * buf, uint32_t size, int32_t sockfd){
	Debug_Printf(DBG_SHIM, "WF_ControllerShim::Deserialize: Got a message of size %u, looking it up..\n", size);
	//Debug_Printf(DBG_SHIM, "WF_ControllerShim::Deserialize: Message %s\n", buf);
	WFEventMsg *msg = (WFEventMsg*) buf;
	switch (msg->eventType) {
		case WF_GREET_EVT:
		{
			WF_GreetEventMsg *g = (WF_GreetEventMsg* )msg;
			//memcpy((char*)g, buf, size);
			//Debug_Printf(DBG_SHIM, "WF_ControllerShim::Deserialize: Got a Greetings %s from new waveform %d, %d\n",g->greet, g->wid0, g->wid1);
			PrintMsg(buf, size);
			if(g->wid0 == g->wid1){
				InitWaveform(g->wid0,sockfd);
				waveformEventReceiver->Recv_NewWaveformEvent(g->wid0);
			}
			else { 
				Debug_Printf(DBG_SHIM, "WF_ControllerShim::Deserialize: Something wrong with Greetings %s from new waveform %d, %d\n",g->greet, g->wid0, g->wid1);
			}
			break;
		}
		case WF_LINK_EST_EVT:{
			WF_LinkUpdateEventMsg *c = (WF_LinkUpdateEventMsg* )buf;
			WF_LinkEstimationParam<uint64_t> param = c->param;
			//memcpy(param, &(c->param), sizeof(WF_LinkEstimationParam<uint64_t>));
			waveformEventReceiver->Recv_LinkEstimationEvent(param);
			break;
		}
		case WF_RECV_MSG_EVT: {
			WF_RecvEventMsg *c = (WF_RecvEventMsg* )buf;
			
			//WF_RecvMsgParam<uint64_t> *param = new WF_RecvMsgParam<uint64_t>();
			//memcpy (param, c->param, sizeof(WF_RecvMsgParam<uint64_t>));
			WF_RecvMsgParam<uint64_t> param = c->param;
			uint16_t offset=sizeof(WF_RECV_MSG_EVT)+sizeof(MessageId_t);
			WF_Message_n64_t *msg = new WF_Message_n64_t();
			memcpy(msg, buf+offset, sizeof(WF_Message_n64_t)); //then copy the message header
			param.msg = msg;
			offset += sizeof(WF_Message_n64_t);
			uint16_t payloadSize = size-offset;
			uint8_t *payload = new uint8_t(payloadSize);
			memcpy(payload, buf+offset, payloadSize); //copy payload first
			param.msg->SetPayload(payload);
			param.msg->SetPayloadSize(payloadSize);
			Debug_Printf(DBG_SHIM, "WF_ControllerShim::Deserialize: Received a waveform message %d from node %lu, %d with payload size %d from waveform %d\n",
									 param.wrn, param.msg->GetSource(), param.msg->GetSrcNodeID(), param.msg->GetPayloadSize(), param.msg->GetWaveform());
			waveformEventReceiver->Recv_RecvMsgEvent(param);
			break;
		}
		case WF_DATA_NTY_EVT:{
			WF_DataStatusEventMsg *c = (WF_DataStatusEventMsg* )buf;
			//WF_DataStatusParam_n64_t *param = new WF_DataStatusParam_n64_t();
			//memcpy(param,c->param, sizeof(WF_DataStatusParam_n64_t));
			WF_DataStatusParam_n64_t param = c->param;
			printf("Received a data notification for message %lu\n", param.wfMsgId);
			waveformEventReceiver->Recv_DataNotificationEvent(param);
			break;
		}
		case WF_CONT_RES_EVT:{
			WF_ControlResponseEventMsg *c = (WF_ControlResponseEventMsg* )buf;
			WF_ControlResponseParam param = c->param;
			param.data = (void*)(buf+sizeof(WF_CONT_RES_EVT)+sizeof(param));
			printf("Received a control message from wf %d\n", param.wfid);
			waveformEventReceiver->Recv_ControlResponseEvent(param);
			break;
		}
		case WF_SCHD_UPD_EVT:{
			WF_ScheduleUpdateEventMsg *c = (WF_ScheduleUpdateEventMsg* )buf;
			WF_ScheduleUpdateParam  param = c->param;
			
			waveformEventReceiver->Recv_ScheduleUpdateEvent(param);
			break;
		}
		default:
			Debug_Printf(DBG_SHIM, "WF_ControllerShim::Deserialize: Unknown message \n");
			PrintMsg(buf, size);
	};
	
	return true;
}

void WF_ControllerShim::Cleanup(int sockfd)
{
	ioSignals.DeRegister(sockfd);
}

void WF_ControllerShim::InitWaveform(WaveformId_t wid, int32_t sock)
{
	Debug_Printf(DBG_SHIM, "WF_ControllerShim::InitWaveform: Initializing shim for waveform %d on sock %d\n", wid, sock);
	//Check if waveform shim already exist for this waveform id
	if(wfIdMap){
		Debug_Printf(DBG_SHIM, "WF_ControllerShim::InitWaveform: Waveform Map address is %p \n", wfIdMap);
		if(wfIdMap->Find(wid) != wfIdMap->End()) {
			Debug_Printf(DBG_SHIM, "WF_ControllerShim::InitWaveform: Initializing shim for waveform %d on sock %d\n", wid, sock);
			Fw2WF_SocketShim *shim = static_cast<Fw2WF_SocketShim *>(wfIdMap->operator[](wid));
			shim->SetSocket(sock);
		}else {
			Debug_Printf(DBG_SHIM, "WF_ControllerShim::InitWaveform: Initializing new shim for waveform %d on sock %d\n", wid, sock);
			Fw2WF_SocketShim *wfShim = new Fw2WF_SocketShim(wid);
			//wfIdMap->Insert(wid,static_cast<Waveform::Waveform_I<uint64_t>*>(wfShim));
			Debug_Printf(DBG_SHIM, "WF_ControllerShim::InitWaveform: Created shim \n");
			wfIdMap->Insert(wid,wfShim);
			Debug_Printf(DBG_SHIM, "WF_ControllerShim::InitWaveform: Insterted shim into Map \n");
			wfShim->SetSocket(sock);
			Debug_Printf(DBG_SHIM, "WF_ControllerShim::InitWaveform: Done. Fini. \n");
			noOfWaveforms++;
		}
	}else {
		Debug_Printf(DBG_SHIM, "WF_ControllerShim::InitWaveform: Something terrribly wrong, Waveform Map is not initialized. \n");
	}
}

void WF_ControllerShim::PrintMsg(char* buf, uint32_t size)
{
	for(uint32_t i=0; i<size; i++){
		//printf("%u: %c, ", i, buf[i]);
		printf("%02X ", buf[i]);
	}
	printf("\n");
}

