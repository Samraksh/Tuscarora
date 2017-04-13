////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include <unistd.h>

#include "Fw2Wf_SocketShim.h"
#include "Sys/SocketShim.h"
#include "Lib/Shims/WF_API_Calls.h"

using namespace Lib::Shims;

int32_t Fw2WF_SocketShim::Write(char* buf, uint16_t size)
{
	 int32_t n = write(socketID, buf, size);
	 Debug_Printf(DBG_SHIM, "FW2WF_SocketShim:: Write : Sent %d bytes on socket %d \n",n, socketID);
	 PrintMsg(buf,size);
	 return n;
}

Fw2WF_SocketShim::Fw2WF_SocketShim(WaveformId_t waveformID){
 WID = waveformID;
}

/*Fw2WF_SocketShim::Fw2WF_SocketShim(WaveformId_t waveformID, WF_TypeE _type, WF_EstimatorTypeE _estType, char* deviceName)
{
}
*/


void Fw2WF_SocketShim::AddDestinationRequest(RequestId_t _rId, WF_MessageId_t _msgId, uint64_t* _destArray, uint16_t _noOfDestinations)
{
	Debug_Printf(DBG_SHIM, "FW2WF_SocketShim::AddDestinationRequest: Recived a call from framework. Serializing msg to socket %d for waveform %d\n",socketID, WID);
	AddDestinationRequestCallMsg msg;
	msg.callType = Shims::WF_Call_AddDest;
	msg.rId = _rId;
	msg.msgId = _msgId;
	msg.noOfDestinations = _noOfDestinations;
	
	memcpy(msg.destArray,_destArray, _noOfDestinations*sizeof(uint64_t));
	
	Write((char*)&msg,sizeof(AddDestinationRequestCallMsg));
}

void Fw2WF_SocketShim::AttributesRequest(RequestId_t _rId)
{
	Debug_Printf(DBG_SHIM, "FW2WF_SocketShim::AttributesRequest: Recived a call from framework. Serializing msg to socket %d for waveform %d\n",socketID, WID);
	AttributeRequestCallMsg msg;
	msg.callType = Shims::WF_Call_Attributes;
	msg.rId = _rId;
	
	Write((char*)&msg,sizeof(AttributeRequestCallMsg));
}

void Fw2WF_SocketShim::BroadcastData(WF_Message_n64_t& _msg, uint16_t _payloadSize, WF_MessageId_t _msgId)
{
	Debug_Printf(DBG_SHIM, "FW2WF_SocketShim::BroadcastData: Recived a call from framework for message %lu. Serializing msg to socket %d for waveform %d\n",_msgId, socketID, WID);
	uint16_t totalSize = sizeof(BroadcastCallMsg) + _payloadSize;
	uint8_t buf[MAX_SOC_BUF_SIZE];
	BroadcastCallMsg *msg = (BroadcastCallMsg *)buf;
	msg->callType = Shims::WF_Call_Broadcast;
	msg->msgId = _msgId;
	msg->payloadSize = _payloadSize;
	msg->wfMsg= _msg;
	
	memcpy(buf+sizeof(BroadcastCallMsg),_msg.GetPayload(),_payloadSize);
	Write((char*)buf,totalSize);
}


void Fw2WF_SocketShim::CancelDataRequest(RequestId_t _rId, WF_MessageId_t _msgId, uint64_t* _destArray, uint16_t _noOfDestinations)
{
	Debug_Printf(DBG_SHIM, "FW2WF_SocketShim::CancelDataRequest: Recived a call from framework. Serializing msg to socket %d for waveform %d\n",socketID, WID);
	//uint16_t totalSize = sizeof(CancelDataCallMsg);
	//uint8_t buf[MAX_SOC_BUF_SIZE];
	CancelDataCallMsg msg;
	msg.callType = Shims::WF_Call_Cancel;
	msg.msgId = _msgId;
	msg.rId = _rId;
	msg.noOfDestinations = _noOfDestinations;
	memcpy(msg.destArray,_destArray, sizeof(uint64_t)*_noOfDestinations);
	Write((char*)&msg,sizeof(CancelDataCallMsg));
	
}


void Fw2WF_SocketShim::ControlStatusRequest(RequestId_t _rId)
{
	Debug_Printf(DBG_SHIM, "FW2WF_SocketShim::ControlStatusRequest: Recived a call from framework. Serializing msg to socket %d for waveform %d\n",socketID, WID);

	ControlStatusRequestCallMsg msg;
	msg.callType = Shims::WF_Call_ControlStatus;
	msg.rId = _rId;
	
	Write((char*)&msg,sizeof(ControlStatusRequestCallMsg));
}

void Fw2WF_SocketShim::DataStatusRequest(RequestId_t _rId, WF_MessageId_t _mId)
{
	Debug_Printf(DBG_SHIM, "FW2WF_SocketShim::DataStatusRequest: Recived a call from framework. Serializing msg to socket %d for waveform %d\n",socketID, WID);
	DataStatusRequestCallMsg msg;
	msg.callType = Shims::WF_Call_DataStatus;
	msg.msgId = _mId;
	msg.rId = _rId;

	Write((char*)&msg,sizeof(DataStatusRequestCallMsg));
}


void Fw2WF_SocketShim::ReplacePayloadRequest(RequestId_t _rId, WF_MessageId_t _msgId, uint8_t* _payload, uint16_t _payloadSize)
{
	Debug_Printf(DBG_SHIM, "FW2WF_SocketShim::ReplacePayloadRequest: Recived a call from framework. Serializing msg to socket %d for waveform %d\n",socketID, WID);
	uint8_t buf[MAX_SOC_BUF_SIZE];
	ReplacePayloadCallMsg *msg = (ReplacePayloadCallMsg *)buf;
	msg->callType = Shims::WF_Call_ReplacePayload;
	msg->msgId = _msgId;
	msg->rId = _rId;
	msg->payloadSize = _payloadSize;
	
	memcpy(buf+sizeof(ReplacePayloadCallMsg), _payload, _payloadSize);
	Write((char*)&msg,sizeof(ReplacePayloadCallMsg)+_payloadSize);
}


void Fw2WF_SocketShim::SendData(WF_Message_n64_t& _msg, uint16_t _payloadSize, uint64_t* _destArray, uint16_t _noOfDest, WF_MessageId_t _msgId, bool _noAck)
{
	Debug_Printf(DBG_SHIM, "FW2WF_SocketShim::SendData: Recived a call from framework. Serializing msg to socket %d for waveform %d\n",socketID, WID);
	uint16_t totalSize = sizeof(SendCallMsg) + _payloadSize;
	uint8_t buf[MAX_SOC_BUF_SIZE];
	SendCallMsg *msg = (SendCallMsg *)buf;
	msg->callType = Shims::WF_Call_Broadcast;
	msg->msgId = _msgId;
	msg->payloadSize = _payloadSize;
	msg->wfMsg = _msg;
	msg->noAck = _noAck;
	msg->noOfDest = _noOfDest;
	memcpy(&msg->destArray, _destArray, sizeof(uint64_t)*_noOfDest);
	memcpy(buf+sizeof(SendCallMsg),_msg.GetPayload(),_payloadSize);
	Write((char*)buf,totalSize);
}


void Fw2WF_SocketShim::SetSocket(int32_t sockfd)
{
 socketID = sockfd;
}


void Fw2WF_SocketShim::PrintMsg(char* buf, uint32_t size)
{
	for(uint32_t i=0; i<size; i++){
		//printf("%u: %c, ", i, buf[i]);
		printf("%02X ", buf[i]);
	}
	printf("\n");
}
