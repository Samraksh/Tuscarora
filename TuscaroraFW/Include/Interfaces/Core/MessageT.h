////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef MESSAGE_H
#define MESSAGE_H

#include <Base/FrameworkTypes.h>
#include <Base/Delegate.h>
//#include <Lib/DS/GenericMessages.h>

using namespace Types;
using namespace PAL;

namespace Core {

///A class that facilitates data exchange between Patterns and the framework.  
template <class NodeIDType>
class MessageT
{
  //uint8_t noOfDest;
  //uint16_t patternInst;
  MessageTypeE type;
  WaveformId_t wfid; 	//needed on receiving side
  //bool broadcast;	//needed on receiving side
  NodeIDType src; 	//needed on receiving side
  uint16_t payloadSize;
  uint8_t* payload;
  //NodeIDType *destArray;
public:
  
  MessageT (){
      payload = NULL;
      payloadSize=0;
      //size = payload_size + CAL_HEADER_SIZE;
  }
  
  MessageT (uint16_t payload_size){
      payload = new uint8_t[payload_size];
      payloadSize=payload_size;
      //size = payload_size + CAL_HEADER_SIZE;
  }
  ~MessageT () {
    if(payload){
      delete[] payload;
      payload = NULL;
    }
  }
  
  inline NodeIDType GetSource(){
    return src;
  }
  WaveformId_t GetWaveform(){
    return wfid;
  }
  inline void SetSource (NodeIDType _src) {
    src = _src;
  }

  void SetWaveform(WaveformId_t _wfid){
    wfid= _wfid;
  }

  /*inline uint16_t GetInstance(){
    return patternInst;
  }
  inline void SetInstance(uint16_t _inst){
    patternInst= _inst;
  }*/
  MessageTypeE GetType(){
    return type;
  }
  inline void SetType(MessageTypeE _type){
    type= _type;
  }
  inline uint16_t GetPayloadSize(){
    return payloadSize;
  }
  inline void SetPayloadSize(uint16_t _size){
    payloadSize=_size;
  }
  inline uint8_t* GetPayload(){
    return payload;
  }
  inline void SetPayload (uint8_t* _payload) {
    payload = _payload;
  }

  };


typedef MessageT<NodeId_t> FMessage_t;





 //typedef MessageT<uint64_t> Message_n64_t;
 
 //typedef AckDelegateParam<uint64_t> MessageAckParam_n64_t;
 
 //typedef Delegate<void, FMessage_t&> FMessageDelegate_t;
 
} //End of namespace

#endif // MESSAGE_H_H
