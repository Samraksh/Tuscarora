////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef WF_API_CALLS_H
#define WF_API_CALLS_H

#include <Interfaces/Waveform/WF_MessageT.h>
#include <Interfaces/Waveform/WF_Event.h>
#include <Interfaces/Waveform/WF_Types.h>

using namespace Waveform;

namespace Lib {
namespace Shims {

#define MAX_PAYLOAD 2048

enum WF_MethodE {
	WF_Call_Greet = 1,
	WF_Call_Send,
	WF_Call_Broadcast,
	WF_Call_AddDest,
	WF_Call_Cancel,
	WF_Call_ReplacePayload,
	WF_Call_Attributes,
	WF_Call_ControlStatus,
	WF_Call_DataStatus,
};


struct WFMethod{
	WF_MethodE callType;
}__attribute__((packed, aligned(1)));


//void SendData (WF_Message_n64_t& _msg, uint16_t _payloadSize, uint64_t *_destArray, uint16_t _noOfDest, FMessageId_t  _msgId, bool _noAck=false);
struct SendCallMsg : public WFMethod {
	uint16_t payloadSize;
	uint16_t noOfDest;
	FMessageId_t msgId;
	bool noAck;
	uint64_t destArray[MAX_DEST]; 
	WF_Message_n64_t wfMsg;
} __attribute__((packed, aligned(1)));


//void BroadcastData (WF_Message_n64_t& _msg, uint16_t _payloadSize, FMessageId_t  _msgId);
struct BroadcastCallMsg : public WFMethod {
	FMessageId_t msgId;
	uint16_t payloadSize;
	WF_Message_n64_t wfMsg;
} __attribute__((packed, aligned(1)));

//void AddDestinationRequest (RequestId_t _rId, FMessageId_t  _msgId, uint64_t *_destArray, uint16_t _noOfDestinations);
struct AddDestinationRequestCallMsg : public WFMethod {
	RequestId_t rId;
	FMessageId_t msgId;
	uint64_t destArray[16];
	uint16_t noOfDestinations;
} __attribute__((packed, aligned(1)));


//void CancelDataRequest (RequestId_t _rId, FMessageId_t  _msgId, uint64_t *_destArray, uint16_t _noOfDestinations);
struct CancelDataCallMsg : public WFMethod {
	RequestId_t rId;
	FMessageId_t msgId;
	uint16_t noOfDestinations;
	uint64_t destArray[MAX_DEST];
} __attribute__((packed, aligned(1)));

//void ReplacePayloadRequest (RequestId_t rId, FMessageId_t  msgId, uint8_t *payload, uint16_t payloadSize);
struct ReplacePayloadCallMsg : public WFMethod {
	RequestId_t rId;
	FMessageId_t msgId;
	uint16_t payloadSize;
	uint8_t payload[MAX_PAYLOAD];
} __attribute__((packed, aligned(1)));

//void AttributesRequest (RequestId_t rId);
struct AttributeRequestCallMsg : public WFMethod {
	RequestId_t rId;
} __attribute__((packed, aligned(1)));

//void ControlStatusRequest (RequestId_t rId);
struct ControlStatusRequestCallMsg : public WFMethod {
	RequestId_t rId;
} __attribute__((packed, aligned(1)));


//void DataStatusRequest (RequestId_t rId, FMessageId_t  mId);
struct DataStatusRequestCallMsg : public WFMethod {
	RequestId_t rId;
	FMessageId_t msgId;
} __attribute__((packed, aligned(1)));


struct WFEventMsg{
	WF_EventNameE eventType;
}__attribute__((packed, aligned(1)));

struct WF_GreetEventMsg: public WFEventMsg{
	WaveformId_t wid0;
	char greet[22];
	WaveformId_t wid1;
}__attribute__((packed, aligned(1)));

struct WF_RecvEventMsg: public WFEventMsg {
	WF_RecvMsgParam<uint64_t> param;
}__attribute__((packed, aligned(1)));

struct WF_LinkUpdateEventMsg: public WFEventMsg {
	WF_LinkEstimationParam<uint64_t> param;
}__attribute__((packed, aligned(1)));

struct WF_DataStatusEventMsg: public WFEventMsg {
	WF_DataStatusParam_n64_t  param;
}__attribute__((packed, aligned(1)));

struct WF_ControlResponseEventMsg: public WFEventMsg {
	WF_ControlResponseParam param;
}__attribute__((packed, aligned(1)));

struct WF_ScheduleUpdateEventMsg: public WFEventMsg {
	WF_ScheduleUpdateParam param;
}__attribute__((packed, aligned(1)));


}//End namespace
}

#endif //WF_API_CALLS_H