////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef WF_ALWAYSON_NOACK_H_
#define WF_ALWAYSON_NOACK_H_

#include "Interfaces/Waveform/Waveform_64b_I.h"
#include "Interfaces/Core/EstimationCallbackI.h"
#include "Lib/Waveform/Common.h"
//#include "Lib/Math/Rand.h"
#include <PAL/PseudoRandom/UniformRandomInt.h>

#include <PAL/Timer.h>
#include "net/if.h"
#include "Lib/DS/BSTMapT.h"
#include "Lib/DS/ListT.h"
#include "Lib/Misc/MemMap.h"
#include "Interfaces/Core/LinkEstimatorI.h"

#include "Lib/KernalSignals/IO_Signals.h"

#ifdef PLATFORM_LINUX
#include "Platform/linux/Waveform/WaveformFasade.h"
#endif

#include "Lib/Waveform/AlwaysOn/WFTestStats.h"

using namespace Core::Estimation;


namespace Waveform { 
  
class WF_AlwaysOn_NoAck : public Waveform_64b_I, EstimatorCallback_I<uint64_t> {
protected:
	WF_MessageId_t rcvFromFwNo;
	uint16_t MAX_AO_PAYLOAD_SIZE;
	unsigned char sendbuf[BUF_SIZE];
	unsigned char rcvbuf[BUF_SIZE];
	WF_MessageId_t wrn;
	
	//debugging
	uint64_t current_destination_;


	char ifName[IFNAMSIZ];
	//struct ifreq if_idx;
	char myIfAddress[IFNAMSIZ];
	struct ifreq if_mac;
	uint32_t protocol;
	int32_t sockID;
		
	WF_MessageId_t newwfMsgId;

	//validation
	AckUnicastSummary u_summary;
	AckBroadcastSummary b_summary;

	//used to check if the recieved message is broadcast or not in ProcessIncomingMessage. 
	//For broadcast, I use Ack_type key of 0 (setting node id to be zero) but this is not ture in Recieve function. 
	//So I need to check flag and if it is broadcast, force to use key of 0.
	bool IsBroadcast; 
	
	WF_Attributes wfAttrib;
	TimerDelegate *ackTimerDelegate;  //acknowledgement
	Timer *ackTimer;
	TimerDelegate *recvTimerDelegate;
	Timer *recvTimer;
	TimerDelegate *validationTimerDelegate;
	Timer *validationTimer;
	TimerDelegate *debugTimerDelegate;
	Timer *debugTimer;
	SocketSignalDelegate_t *socketDelegate;

	WF_DataStatusEvent_t *dataNotifierEvent;  //used to return ACK_WF_RECV, WDN_WF_SENT for now
	Core::Estimation::LinkEstimatorI<uint64_t> *estimator;
	//Core::Estimation::SendEstimationMessageDelegate_t *leSendDel; ///This will be called by estimation to send a estimation message out.
	WF_LinkChangeDelegate_n64_t  *leDel; //this be called by estimator to report changes.
	
	
	WF_LinkEstimateEvent_t *linkEstimateEvent;  //
	WF_ControlResponseEvent_t *controlResponseEvent; //
	uint64_t last_sent_msg_dest_ptr[MAX_DEST];
	//bool lastMsgStatus;
	WF_MessageStatusE lastMsgStatus; 
	uint64_t current_destination; //stores destination of current unicast
	//Add local variable which holds state 
	WF_ControlP_StatusE stat;
	WF_ControlResponseE cont;
	double lastSnr;
	double lastRss;
	WF_Message_n64_t *last_sent_msg_ptr, *send_msg_ptr, *rcv_msg_ptr;
	WF_RcvMessageEvent_t *recvMsgEvent;
	
	//hold messageId which I am handling right now.
	ListT<WF_MessageId_t,false, EqualTo<WF_MessageId_t> > current_messageId; 
	BSTMapT<NodeId_t, WF_DataStatusTypeE> Ack_Type;  //Used to map messageId and Ack_Type(WF_ACK_RECV,WF_ACK_SENT and so on)
	BSTMapT<string, WF_DataStatusTypeE> Ack_Type_SendData; // This maps string "messageId+destination" to encoded Ack_Type(Read comment in the end)
	//BSTMapT<WF_MessageId_t, int> Ack_Type;  //cannot delete map so use interger as value for now.
	BSTMapT<NodeId_t, WF_MessageStatusE> MessageStatus;  //Used to map messageId and MessageStatus(Success, PktTooBig, and so on)
	BSTMapT<WF_MessageId_t, WF_Message_n64_t*> MessageIdToPointer; //used to map messageId and pointer to message
	BSTMapT<WF_MessageId_t, timeval> MessageIdToTime; //used to map messageId and pointer to message
	BSTMapT<WF_MessageId_t, uint64_t* > MessageIdTodest; //WF_MessageT has *destArray pointer but not method to access it. So I will use map to keep track of it.
	ListT<WF_MessageId_t,false, EqualTo<WF_MessageId_t> > messageList; //Store messageId. This list maintains chronological order.
	ListT<uint64_t,false, EqualTo<uint64_t> > destList; //Stores list of destinations 
	ListT<uint8_t,true, EqualTo<uint8_t> > ackTypeList; //Stores ackType for each destinations.  

	bool IsTimerSet; //Set if acknowledgement timer is running now
	bool last_packet; //Set if send type is broadcast or unicast sending to last destination in destArray. Used to trigger acknowoledgement timer
	//bool WF_Buffer_FULL; //Set if AlwaysOn_Dce is currently handling incoming message.
	bool readyToRecv; 
	WF_MessageId_t current_Id;
	uint16_t message_size;
	uint16_t destination_index;
	WF_MessageStatusE msgStat; 

#ifdef PLATFORM_LINUX
	WaveformFasade *fasade;
#endif

	///////////////Methods 
	WF_MessageStatusE Unicast(uint64_t dest, WF_Message_n64_t&  msg, uint16_t size);
	WF_MessageStatusE Broadcast(WF_Message_n64_t&  msg, uint16_t size);  
	void PrintMAC(uint8_t *mac);
	WF_MessageStatusE  SendToDevice(uint8_t* dest, WF_Message_n64_t&  msg, uint16_t size);
	void AckTimer_Handler(uint32_t flag);
	void RecvTimer_Handler(uint32_t flag);
	void validationTimer_Handler(uint32_t flag);
	void DebugTimer_Handler(uint32_t flag);
	//This method is not needed starting v1.1
	//uint64_t MapNodeIdToWaveformAddress(uint64_t nodeId);
	//Methods used in recieve function to access
	ListT<uint64_t,false, EqualTo<uint64_t> >* GetdestListAddress() {return &destList;}   
	BSTMapT<NodeId_t, WF_DataStatusTypeE>* GetAck_TypeAddress() {return &Ack_Type;}

	WF_Message_n64_t* getLastSentMessagePtr() { return last_sent_msg_ptr;}
	WF_MessageStatusE getMessageStatus() { return msgStat;}
	void SetAck_Type(NodeId_t node, WF_DataStatusTypeE type);
	uint8_t GetdestListElement(int index) { return destList[index];}
	void DeleteElement() { destList.Delete(0);}
	uint64_t GetDistination() {return destList[0];}
//	void SuspendAckTimer() {ackTimer->Suspend();}
//	void RestartAckTimer() {ackTimer->Change(20000, ONE_SHOT);}
	WF_MessageStatusE Unicast_Wrap(uint64_t dest, WF_Message_n64_t&  msg, uint16_t size) {return Unicast(dest,msg,size);} 
	ListT<uint64_t,false, EqualTo<uint64_t> >* GetDestList() {return &destList;} //Stores list of destinations 
	ListT<uint8_t,true, EqualTo<uint8_t> >* GetAckTypeList() { return &ackTypeList;} //Stores ackType for each destinations.  
	uint16_t GetMessageSize() {return message_size;}
	void IncrementDestinationIndex() {destination_index++;}
	uint16_t GetDestinationIndex(){ return destination_index;}
	//void ClearWFBusy() {WF_Buffer_FULL = false;}
	void ClearIsTimerSet() { IsTimerSet = false;}
	WF_MessageId_t GetNewWaveformMsgId() { return ++newwfMsgId;}
	void OnSocketSignal(int32_t sockfd);
	void ProcessAckMessages(WaveformId_t wfid, uint16_t msglen, unsigned char* buf);
	void InstantiateEstimation(uint32_t period);
	uint64_t NodeIdToNS3MacAddress(NodeId_t nodeId);
	
public:
	//Methods
	WF_AlwaysOn_NoAck(WaveformId_t waveformID, WF_TypeE _type, WF_EstimatorTypeE _estType, char* deviceName, float cost , float energy);
	void SendData (WF_Message_n64_t& _msg, uint16_t _payloadSize, uint64_t *_destArray, uint16_t _noOfDest, WF_MessageId_t _msgId, bool _noAck=false);
	void BroadcastData (WF_Message_n64_t& _msg, uint16_t _payloadSize, WF_MessageId_t _msgId);
	void AddDestinationRequest (RequestId_t _rId, WF_MessageId_t _msgId, uint64_t *_destArray, uint16_t _noOfDestinations);
	void CancelDataRequest (RequestId_t _rId, WF_MessageId_t _msgId, uint64_t *_destArray, uint16_t _noOfDestinations);
	void ReplacePayloadRequest (RequestId_t rId, WF_MessageId_t msgId, uint8_t *payload, uint16_t payloadSize);
	void AttributesRequest (RequestId_t rId);
	void ControlStatusRequest (RequestId_t rId);
	void DataStatusRequest (RequestId_t rId, WF_MessageId_t mId);
	void DataNotification(WF_DataStatusParam_n64_t);
	void CopyToDN( WF_DataStatusParam<uint64_t>& ackParam);
	void ProcessIncomingMessage(WaveformId_t wfid, uint16_t msglen, unsigned char* buf);
	WF_Message_n64_t* ExtractWFMessage (WaveformId_t wfid, uint16_t msglen, unsigned char* buf);
	
	///EstimationCallbackI methods
	bool SendEstimationMessage (WaveformId_t wfId, 	FMessage_t *msg);
	virtual WF_Attributes GetWaveformAttributes (WaveformId_t wfId);
	void LinkEstimatorChangeHandler(WF_LinkEstimationParam_n64_t  _param);
	WF_MessageStatusE Success;
	
};
 
}
#endif /* WF_ALWAYSON_NOACK_H_ */
