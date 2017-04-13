////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef PACKET_T_H
#define PACKET_T_H

#include <Base/BasicTypes.h>
#include "Platform/PlatformSelector.h"
#include <Base/TimeI.h>
#include <Interfaces/Core/DataFlowI.h>
#include <string.h>
#include <Interfaces/Core/MessageT.h>
#include "AddressMapper.h"

using namespace Types;
using namespace PAL;
using namespace Core;

namespace Waveform {

enum WF_MessageFlagsE {
	WF_FLG_PIGGYBACKING = 0x01, 
    WF_FLG_TIMESTAMP = 0x02, 
  };
	
  enum WF_MessageStatusE {
    WF_ST_BUSY, //ModuleBusy,
    WF_ST_PKT_TOO_BIG,
    WF_ST_SUCCESS,
    WF_ST_BCAST_NOT_SUPPTD,
    WF_ST_BUFFER_FULL, //Failed,
    WF_ST_CHANNEL_BUSY,
    WF_ST_ACK_NOT_RECV, ///WF_MessageTimeout,
    WF_ST_MAX_RETRIES_ERR,
    WF_ST_DEADNBR, //DeadLink,
  };
  
  ///Structure for packet metadata that should be supported by all waveforms
struct PacketMetadata{
  uint8_t rss;			///Receiver Signal Strength
  uint8_t sinr;			///Signal to noise and interference ratio
  U64NanoTime recvTimeStamp;	///Receive time stamp of the packet in local clock time
}__attribute__((packed, aligned(1)));
 
/// Base class for all waveform messages  
class WF_MessageBase {
private:
	//uint8_t startMarker[8];
	//MessageId_t fmID;
	WF_MessageId_t wmID;
public:
	//U64NanoTime expiryTime; ///System time when the packet will expire in nanoseconds
	uint16_t inst;
	uint8_t flags;
	NodeId_t nodeID;
	uint16_t payloadSize;
	WaveformId_t waveformId;
	MessageTypeE type;
	PacketMetadata metadata;
	uint8_t* payload;
#ifdef ENABLE_PIGGYBACKING
	uint8_t* piggyPayload;
	uint8_t piggyPayloadSize;
#endif

public:

	WF_MessageBase () {
			//memset(startMarker, 0xAA, 8);
		nodeID=0xffff;
		//fmID=0;
		wmID=0;
		//expiryTime=0;
		inst=0;
		payloadSize=0;
		waveformId=0;
		type=Types::NULL_MSG;
		payload=0;
		flags=0;
		metadata = PacketMetadata{};
#if ENABLE_PIGGYBACKING==1
		piggyPayload=0;
		piggyPayloadSize=0;
#endif
	}
	
	WF_MessageBase (uint16_t payload_size){
		WF_MessageBase();
		payload = new uint8_t[payload_size];
		payloadSize=payload_size;
		//size = payload_size + CAL_HEADER_SIZE;
	}

	//inline uint32_t GetFrameworkMessageID() {return fmID;}
	//inline void SetFrameworkMessageID(MessageId_t _id){fmID=_id;}

	inline WF_MessageId_t GetWaveformMessageID() {return wmID;}
	inline void SetWaveformMessageID(WF_MessageId_t _id){wmID=_id;}


	void SetType(MessageTypeE _type){
		type= _type;
	}

	MessageTypeE GetType(){
		return type;
	}

	/*void SetExpiryTime(U64NanoTime _time){
		expiryTime = _time;
	}

	U64NanoTime GetExpiryTime(){
		return expiryTime;
	}*/

	void SetSrcNodeID(NodeId_t _node){
		nodeID= _node;
	}

	NodeId_t GetSrcNodeID(){
		return nodeID;
	}

	void SetInstance(uint16_t _inst){
		inst= _inst;
	}

	uint16_t GetInstance(){
		return inst;
	}

	bool IsFlagSet(WF_MessageFlagsE _flag){
		return (flags & _flag);
	}

	void SetFlag(WF_MessageFlagsE _flag){
		flags = flags | _flag;
	}

	
	void SetWaveform(WaveformId_t wfId){
		waveformId = wfId;
	}

	WaveformId_t GetWaveform(){
		return waveformId;
	}

	uint16_t GetPayloadSize(){
		return payloadSize;
	}

	void SetPayloadSize(uint16_t _size){
		payloadSize=_size;
	}
	
	
	uint8_t* GetPayload(){
		return payload;
	}

	void SetPayload(uint8_t* _payload){
		payload= _payload;
	}

	PacketMetadata GetMetaData(){
		return metadata;
	}
	
	void SetMetaData(PacketMetadata _metadata) {
		metadata = _metadata;
	}
	
	U64NanoTime GetRecvTimestamp() {
		return metadata.recvTimeStamp;
	}
	
	void SetRecvTimestamp(U64NanoTime _time) {
		metadata.recvTimeStamp = _time;
	}
	
#if ENABLE_PIGGYBACKING==1
	uint8_t* GetPiggyPayload(){
		return piggyPayload;
	}
	
	void SetPiggyPayload(uint8_t *_piggyPayload, uint8_t _piggyPayloadSize){
		piggyPayload=_piggyPayload;
		piggyPayloadSize = _piggyPayloadSize;
	}
	uint8_t GetPiggyPayloadSize(){
		return piggyPayloadSize;
	}
#endif

	
	
	~WF_MessageBase () {
		if(payload){
			delete[] payload;
			payload=0;
		}
#if ENABLE_PIGGYBACKING==1
		if(piggyPayload){
			delete[] piggyPayload;
			piggyPayload=0;
		}
#endif
	}

	
}__attribute__((packed, aligned(1)));
  
  //uint32_t WF_MessageBase::WF_MessageIDGenerator=0;
  
///A class that facilitates data exchange between the Waveform and the framework
template <class NodeIDType>
class WF_MessageT : public WF_MessageBase{
	NodeIDType src;
	uint8_t numberOfDest;
	NodeIDType *destArray;
	//uint8_t endMarker[8];
	
public:

	bool IsValid();
	bool IsLocked();
	bool Lock();
	bool Unlock();
	void SetValid(bool value);
	WF_MessageT () : WF_MessageBase()
	{
		//payload = new uint8_t[MAX_CAL_PACKET_SIZE-CAL_HEADER_SIZE];
		//size = MAX_CAL_PACKET_SIZE-CAL_HEADER_SIZE;
		src=0;
		destArray=0;
		//memset(endMarker, 0xEE, 8);
	};
	WF_MessageT (uint16_t _payloadSize) : WF_MessageBase(_payloadSize)
	{
		//payload = new uint8_t[MAX_CAL_PACKET_SIZE-CAL_HEADER_SIZE];
		//size = MAX_CAL_PACKET_SIZE-CAL_HEADER_SIZE;
		payloadSize = _payloadSize;
		src=0;
		destArray=0;
		//memset(endMarker, 0xEE, 8);
	};

	uint8_t GetNumberOfDest () {return numberOfDest;}
	void SetNumberOfDest (uint8_t _noOfDest) {numberOfDest = _noOfDest;}

	NodeIDType GetSource (){ return src;}
	void SetSource (NodeIDType _src){ src=_src;}

	static uint16_t GetHeaderSize(){
		return (sizeof(WF_MessageT<NodeIDType>));
	}
	

	/*static uint16_t MaxPayloadSize(uint8_t ){
		MAX_PAYLOAD_SIZE = MAX_WF_PACKET_SIZE - WF_HEADER_SIZE - sizeof(WF_MessageT<NodeIDType>);
		return MAX_PAYLOAD_SIZE;
	}*/

	/* template <class T2>
	void Copy(WF_MessageT<T2> &_msg){
		//printf("Copying from %lu to %lu, %d bytes \n",(uint64_t)_msg.payload, (uint64_t)payload, payloadSize ); fflush (stdout);
		memcpy(payload, _msg.payload, _msg.payloadSize);
		type =_msg.GetType();
		inst = _msg.GetInstance();
		//payloadSize = _msg.GetPayloadSize();
		waveformId=_msg.waveformId;
		payloadSize=_msg.payloadSize;
		expiryTime=_msg.expiryTime;
		src = (NodeIDType)_msg.src;
		//dest = (NodeIDType)_msg.dest;
	}*/

	void CopyFrom(FMessage_t &_msg){
		//printf("Copying from %lu to %lu, %d bytes \n",(uint64_t)_msg.payload, (uint64_t)payload, payloadSize ); fflush (stdout);
		//memcpy(payload, _msg.payload, _msg.payloadSize);
		type =_msg.GetType();

		Debug_Printf(DBG_CORE_DATAFLOW,"Message ptr is %p, Setting message type to %d msg->type is %d ",&_msg,type, _msg.GetType());
		//inst = _msg.GetInstance();
		payloadSize = _msg.GetPayloadSize();
		payload = _msg.GetPayload();
		//expiryTime=_msg.G expiryTime;
		//src = (NodeIDType)_msg.GetSource();
		//dest = (NodeIDType)_msg.dest;
	}

	void CopyTo(FMessage_t &_msg){
		//printf("Copying from %lu to %lu, %d bytes \n",(uint64_t)_msg.payload, (uint64_t)payload, payloadSize ); fflush (stdout);
		//memcpy(payload, _msg.payload, _msg.payloadSize);
		_msg.SetType(type);
		//_msg.SetInstance(inst);
		_msg.SetPayloadSize(payloadSize);
		_msg.SetPayload(payload);
		//expiryTime=_msg.G expiryTime;
		//_msg.SetSource(src);
		//dest = (NodeIDType)_msg.dest;
	}

}__attribute__((packed, aligned(1)));
  
  
  /*
  //Specialization
  template <>
  class WF_MessageT<uint16_t> {

	  WF_MessageT<uint16_t>(WF_MessageT<uint64_t> &msg){
		  flags =msg.GetFlag();
		  inst = msg.GetInstance();
		  size = msg.Size();
		  radioInterface=msg.radioInterface;
		  payloadSize=msg.payloadSize;
		  expiryTime=msg.expiryTime;
		  payload = msg.payload;
		  src = (uint16_t)msg.src;
		  dest = (uint16_t)msg.dest;
	  }
  };
  */

///Structure for the parameter passed to the acknowledgement of the packets. 
///The framework will implement this callback which will be called by waveform the acknowledge packets sent to it by framework
template <class WF_AddressType>
struct WF_DataStatusParam{
	WaveformId_t wfId;
	WF_MessageId_t wfMsgId;
	uint8_t noOfDest;
	bool readyToReceive;
	Core::Dataflow::WF_DataStatusTypeE statusType[MAX_DEST];
	WF_MessageStatusE statusValue[MAX_DEST]; //bool status;
	WF_AddressType destArray[MAX_DEST];    

	WF_DataStatusParam (MessageId_t _msgId, WaveformId_t _wfId){
		readyToReceive = true;
		//status = _status;
		wfMsgId= _msgId;
		wfId = _wfId;
	}
}__attribute__((packed, aligned(1)));
  
  struct SoftwareAcknowledgement{
	  WaveformId_t wfId;
	  WF_MessageId_t wfMsgId;
	  NodeId_t src;
  };

  template <class addressType>
  class PacketAdaptor {
    //Convert a waveform message to framework message
  public:
    //convert a framework message to waveform message
    WF_MessageT<addressType>& Convert_FM_to_WM (FMessage_t &msg, bool copyDest){
      WF_MessageT<addressType> *ret = new WF_MessageT<addressType>();
     
      //ret->SetInstance (msg.GetInstance());
      ret->SetPayload (msg.GetPayload());
      ret->SetPayloadSize(msg.GetPayloadSize());
      ret->SetType(msg.GetType());
      ret->SetSource(WaveformAddressMapper<addressType>::GetWaveformAdress(msg.GetSource()));
      
      if(copyDest){
      }
      return *ret;
    };
          
          
    FMessage_t& Convert_WM_to_FM (WF_MessageT<addressType> &wfMsg, bool copyDest){
      FMessage_t *ret = new FMessage_t();
      //ret->SetInstance (wfMsg.GetInstance());
      ret->SetPayload (wfMsg.GetPayload());
      ret->SetPayloadSize(wfMsg.GetPayloadSize());
      ret->SetType(wfMsg.GetType());
      ret->SetSource(WaveformAddressMapper<addressType>::GetFrameworkId(wfMsg.GetSource()));
      
      if(copyDest){
      }
      return *ret;
    };
    
    
  };
  
  typedef WF_MessageT<uint32_t> WF_Message_n32_t;
  typedef WF_MessageT<uint64_t> WF_Message_n64_t;
  //typedef AckDelegateParam<uint16_t> WF_MessageAckParam_n16_t;
  typedef WF_DataStatusParam<uint32_t> WF_DataStatusParam_n32_t;
  typedef WF_DataStatusParam<uint64_t> WF_DataStatusParam_n64_t;

} //End of namespace

#endif // PACKET_T_H
