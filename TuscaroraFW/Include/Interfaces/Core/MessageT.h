////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef MESSAGE_H
#define MESSAGE_H

#include <Base/FrameworkTypes.h>
#include <Base/Delegate.h>
#include <Lib/DS/GenericMessages.h>

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
  inline MessageTypeE GetType(){
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

  inline void* Serialize(GenericMsgPayloadSize_t& serial_msg_size){
      GenericSerializer<MessageTypeE, WaveformId_t, NodeId_t, GenericMsgPayloadSize_t, void* >
        gs(type, wfid, src, (GenericMsgPayloadSize_t)(payloadSize*sizeof(uint8_t)), (void*)payload);

      //Copy serialized message since it will be destructed by GenericSerializer
      serial_msg_size = gs.Get_Generic_VarSized_Msg_Ptr()->GetPayloadSize();
      void* serial_msg =  malloc(serial_msg_size);
      memcpy( (void*)serial_msg, gs.Get_Generic_VarSized_Msg_Ptr()->GetPayload(), serial_msg_size);

      return static_cast<void*>(serial_msg);
  }
  void DeSerialize(const GenericMsgPayloadSize_t& serial_msg_size,  void * serial_msg){
      Generic_VarSized_Msg gen_msg(serial_msg_size, static_cast<uint8_t*>(serial_msg)); //This consumes serial_msg, the destructor will destruct serial_msg

      GenericMsgPayloadSize_t v_payload_size;
      void* v_payload = NULL;
      GenericDeSerializer<MessageTypeE, WaveformId_t, NodeId_t, GenericMsgPayloadSize_t, void* >
        gs(&gen_msg, type, wfid, src, v_payload_size, v_payload); //This copies serial_msg contents to the variables. For the case of pointers, it copies contents to the location pointed by the pointer. If the pointer is NULL, then malloc is called to allocate memory,

      payloadSize = v_payload_size;
      payload = static_cast<uint8_t*>(v_payload);

  }
};

 typedef MessageT<NodeId_t> FMessage_t;
 //typedef MessageT<uint64_t> Message_n64_t;
 
 //typedef AckDelegateParam<uint64_t> MessageAckParam_n64_t;
 
 //typedef Delegate<void, FMessage_t&> FMessageDelegate_t;
 
} //End of namespace

#endif // MESSAGE_H_H
