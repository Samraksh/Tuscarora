////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef WAVEFORM_64B_I_H_
#define WAVEFORM_64B_I_H_

#include <Interfaces/Waveform/WF_I.h>

namespace Waveform { 
  class Waveform_64b_I : public Waveform_I <uint64_t>{
  public:   
    //Methods
    Waveform_64b_I(){}
    Waveform_64b_I(WaveformId_t _wId, WF_TypeE _type, WF_EstimatorTypeE _estType, char* _deviceName);
    virtual void SendData (WF_Message_n64_t& _msg, uint16_t _payloadSize, uint64_t *_destArray, uint16_t _noOfDest, WF_MessageId_t _msgId, bool _noAck=false) =0;
    virtual void BroadcastData (WF_Message_n64_t& _msg, uint16_t _payloadSize, WF_MessageId_t _msgId) =0;
    virtual void AddDestinationRequest (RequestId_t _rId, WF_MessageId_t _msgId, uint64_t *_destArray, uint16_t _noOfDestinations)=0;
    virtual void CancelDataRequest (RequestId_t _rId, WF_MessageId_t _msgId, uint64_t *_destArray, uint16_t _noOfDestinations) =0;
    virtual void ReplacePayloadRequest (RequestId_t rId, WF_MessageId_t msgId, uint8_t *payload, uint16_t payloadSize) =0;
    virtual void AttributesRequest (RequestId_t rId) =0;
    virtual void ControlStatusRequest (RequestId_t rId) =0;
    virtual void DataStatusRequest (RequestId_t rId, WF_MessageId_t mId) =0;
    virtual void DataNotification(WF_DataStatusParam_n64_t) =0;

    //Added to make a common api for all incoming messages.
    virtual void ProcessIncomingMessage(WaveformId_t wfid, uint16_t msglen, unsigned char* buf) =0;
    
    virtual ~Waveform_64b_I(){}
  };
}

#endif /* WAVEFORM_64B_I_H_ */
