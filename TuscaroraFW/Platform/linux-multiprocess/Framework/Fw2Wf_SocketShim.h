////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef FW2WF_SOCKETSHIM_H_
#define FW2WF_SOCKETSHIM_H_

#include <Interfaces/Waveform/WF_I.h>

using namespace Waveform;

class Fw2WF_SocketShim :public Waveform_I<uint64_t> {
	int32_t socketID;
public:
	Fw2WF_SocketShim(){WID =0;}
	Fw2WF_SocketShim(WaveformId_t waveformID);
	//Fw2WF_SocketShim(WaveformId_t waveformID, WF_TypeE _type, WF_EstimatorTypeE _estType, char* deviceName);
	void SendData (WF_Message_n64_t& _msg, uint16_t _payloadSize, uint64_t *_destArray, uint16_t _noOfDest, WF_MessageId_t _msgId, bool _noAck=false);
	void BroadcastData (WF_Message_n64_t& _msg, uint16_t _payloadSize, WF_MessageId_t _msgId);
	void AddDestinationRequest (RequestId_t _rId, WF_MessageId_t _msgId, uint64_t *_destArray, uint16_t _noOfDestinations);
	void CancelDataRequest (RequestId_t _rId, WF_MessageId_t _msgId, uint64_t *_destArray, uint16_t _noOfDestinations);
	void ReplacePayloadRequest (RequestId_t rId, WF_MessageId_t msgId, uint8_t *payload, uint16_t payloadSize);
	void AttributesRequest (RequestId_t rId);
	void ControlStatusRequest (RequestId_t rId);
	void DataStatusRequest (RequestId_t rId, WF_MessageId_t mId);
	//void DataNotification(WF_DataStatusParam_n64_t);
	//void CopyToDN(WF_DataStatusParam<uint64_t>& ackParam);
	//void ProcessIncomingMessage(WaveformId_t wfid, uint16_t msglen, unsigned char* buf);

	//Additional methods
	void SetSocket(int32_t sockfd);

private:
	int32_t Write(char *buf, uint16_t size);
	void PrintMsg(char* buf, uint32_t size);
};

#endif //FW2WF_SOCKETSHIM_H_