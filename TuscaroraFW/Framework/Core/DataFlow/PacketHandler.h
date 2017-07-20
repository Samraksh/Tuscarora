////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef PACKET_HANDLER_H_
#define PACKET_HANDLER_H_

#include "CoreTypes.h"
#include "MessageCabinet.h"
#include "Lib/DS/BSTMapT.h"
#include "string.h"
#include "Lib/DS/ListT.h" // for messageList
#include <Interfaces/PWI/FrameworkResponces.h>
#include "PiggyBacker.h"
//#include <Interfaces/Waveform/WF_Types.h>

//#if defined(PLATFORM_DCE_DC)
#include <Interfaces/PWI/PatternEventDispatch_I.h>
#include "Framework/Core/Neighborhood/AddressMap.h"
//#include "Platform/dce/Framework/PatternEventDispatch.h"
//#elif defined(PLATFORM_LINUX)
//#endif


using namespace PWI;
using namespace Waveform;
using namespace Core;

namespace PWI{
	class FrameworkBase;
}

namespace Core {
namespace Dataflow {

enum WFNotifierTypeE{
    ACKFW_WF_RECV,	///Acknowlegment that Waveform received the messsage
    ACKFW_WF_SENT,	///Acknowlegment that Waveform sent the messsage out on channel
    ACKFW_DST_RECV,	///Acknowlegment that (some module) on destination received the message
    READYFW_TO_RECV,	///Notification from framework that it is ready to receive more packets from pattern
    ACKFW_NONE,       ///Masahiro adding type means no ack  
  }; 
const uint16_t ackWFDSTRECV_Ack = 1000; //ms
const uint16_t ackWFSent_Ack = 2000; // ms;
const uint16_t ackWFRecvTimer_Ack = 5000; //ms
const uint16_t msgCleanupTimer = 10000; //ms


class FragmentedMessageQueueElement{
public:
	AVLBST_T<uint8_t> received_segment_numbers;
	WF_MessageBase* assembledMsg;
};


class PacketHandler {
	//FrameworkBase* parentFI; //Need pointer to FrameworkBase to start timer.
	//WaveformMessageDelegate_t *linkEstimationDelegate;
	//WaveformMessageDelegate_t *discoveryDelegate;
	PiggyBacker<uint64_t> *piggyBacker;
	WaveformMap_t *wfMap; //this can be used only inside ns3-dce
	WF_AttributeMap_t *wfAttributeMap;
	AddressMap *addressMap; 
	PatternClientMap_t  *patternClients;
	//BSTMapT<WF_MessageId_t, FMessageId_t > wfMsgToFMsgMap[MAX_WAVEFORMS];
	WFMsgIdToFragmentListMap_t wfMsgToFMsgMap;
	//LinkEstimationStore *LESptr;
	FrameworkBase *fwBase;

	//TODO:Check if we need this
	//ListT <FMessageId_t ,false, EqualTo <FMessageId_t > >* messageList;

	uint32_t WDN_WF_RECV_Timeout_Period[MAX_WAVEFORMS];
	uint32_t WDN_WF_SENT_Timeout_Period[MAX_WAVEFORMS];
	uint32_t WDN_DST_RECV_Timeout_Period[MAX_WAVEFORMS];
	FMessageId_t last_wfMsg_Seen[MAX_WAVEFORMS];

	uint32_t newReqnumber;

	PatternEventDispatch_I *eventDispatcher;

	void Process_WF_DataStatus(WF_DataStatusParam_n64_t& ack);
	NeighborTable *coreNbrTable;

	//New timers for WDN_WF_RECV.
	TimerDelegate *ackWFRECVTimerDelegate;
	Timer *ackWFRECVTimer;

	TimerDelegate *ackWFSENTTimerDelegate;
	Timer *ackWFSENTTimer;

	TimerDelegate *ackDSTRECVTimerDelegate;
	Timer *ackDSTRECVTimer;


	TimerDelegate *ackGarbageCollectorTimerDelegate;
	Timer *ackGarbageCollectorTimer;

	bool *attribute_request_pending; //stores attributes request pending falg.

	UniformRandomInt *rnd;
	WF_MessageQElement * current_qElement;


	//WaveformId_t current_wfid;
	bool Send_Busy[MAX_WAVEFORMS]; //Maximum number of supported wavefor is 16.
	bool ackTimer_flag[MAX_WAVEFORMS];
	FMessageId_t frameworkMsgId;
private:
	//	void ProcessLinkEstimationMsg (WF_MessageBase& msg);
	//	void CreateLinkEstimator(WaveformId_t wid);
	void ProcessWFAck(WF_DataStatusParam_n64_t& ack);
	void ProcessWFAckWithFragmentation(WF_DataStatusParam_n64_t& ack);
	void SendSoftwareAck(WF_Message_n64_t& msg);
	void ProcessSoftwareAck(WF_Message_n64_t& msg);
	void ProcessPiggyBackedData(WF_MessageBase& _wfMsg);
	//Method to set corresponding waveform attribute
	void AddEntryWFMap(WaveformId_t id, WF_Attributes attr);
	//acknowldgement timer for WDN_WF_RECV, WDN_WF_SENT, and WDN_DST_RECV
	void WDN_WF_RECV_Checker_Handler(uint32_t flag);
	void WDN_WF_SENT_Checker_Handler(uint32_t flag);
	void WDN_DST_RECV_Checker_Handler(uint32_t flag);
	void CleanMC_Handler(uint32_t flag);
	void CalculatePacketWaitTime (WF_MessageQElement *qElement, double *delay);

public:
	//map messageId and WF_MessageQElement*
	BSTMapT< FMessageId_t  , WF_MessageQElement* > msgToQElement[MAX_WAVEFORMS];
	//lookup pid with msgid
	BSTMapT< FMessageId_t  , PatternId_t > msgIdToPidMap; 
	//lookup MessageCabinet with waveform id
	//BSTMapT<WaveformId_t, MessageCabinet*> wfToMC;
	MessageCabinet *mc;
	BSTMapT<NodeId_t, FragmentedMessageQueueElement> incoming_fragmented_msg_queue;
	PacketHandler ();

	//PacketHandler (WaveformMap_t *_wfMap, WF_AttributeMap_t *_wfAtrribMap, WF_MsgAdaptorMap_t *_wfMsgAdaptorMap, PatternClientMap_t  *clients, 

	//		 WaveformMessageDelegate_t *_linkEstimationDelegate, WaveformMessageDelegate_t *_discoveryDelegate, NeighborTable& _coreNbrTable, PatternEventDispatch *_eventDispatcher);

	void Initialize (WaveformMap_t *_wfHash, WF_AttributeMap_t *_wfAtrribMap, AddressMap *_addressMap, PatternClientMap_t  *clients,
			FrameworkBase *fwBase, NeighborTable& _coreNbrTable, bool* attribute_request_pending, PatternEventDispatch_I *_eventDispatcher, PiggyBacker<uint64_t> *_pb);

	uint32_t GetRemainingPacket(WaveformId_t wfId);
	void Recv_DN_Handler_For_WF(WF_DataStatusParam_n64_t& ack);
	void RecvControlResponse_From_WF(WF_ControlResponseParam param);
	//void Recv_Handler_For_WF(Message_n64_t& msg);
	void Recv_Handler_For_WF_MSG_FRAGMENT(WF_MessageBase& msg, FMessageId_t  wrn);
	void ProcessRcvdPatternMsg (PatternId_t _pid, FMessage_t& msg);
	void Recv_Handler_For_WF(WF_MessageBase& msg, FMessageId_t  wrn);

	FMessageId_t EnqueueToMC(PatternId_t pid, FMessage_t *msg, NodeId_t *nodeArray, Link **linkPtrArray, uint16_t noOfDest, bool broadcast,  WaveformId_t wfid, FMessageId_t  newFWMsgId, uint16_t nonce, bool softwarebroadcast);
	uint16_t ProcessOutgoingMessages (WaveformId_t waveformId);
	//void GenerateNotification (PatternId_t pid,  FMessage_t& msg, FMessageId_t  fmid, Core::Dataflow::DataStatusTypeE ackType, bool status, NodeId_t *destArray, uint16_t noOfDest); 
	bool PrepareAndSendDataStatusResponse( FMessageId_t  msgId);
	//void ConvertU64ToEthAddress(uint64_t x, char *address);
	bool AddOrUpdateWFAddress(WaveformId_t  wid, uint64_t wfAddress, NodeId_t srcNodeId);

	FMessageId_t GetNewFrameworkMsgId() {
		//printf("Generating next Framework pkt id: current id (ptr: %p) is %d\n",&num1, num1);
		//printf("New Framework pkt id: current id is %d\n",num1);
		return ++frameworkMsgId;
	}
	uint32_t GetNewReqId(){
		return ++newReqnumber;
	}

	void Add_Node(PatternId_t patternId, FMessageId_t  msgId, NodeId_t* destArray, uint16_t noOfNbrs, LinkComparatorTypeE lcType, AddDestinationResponse_Data& data );
	bool UpdatemsgToQElem( FMessageId_t  msgId, WaveformId_t wfid, NodeId_t remove_node);
	void CancelDataSendRequest(PatternId_t patternId, FMessageId_t  msgId, NodeId_t *destArray, uint16_t noOfDest, CancelDataResponse_Data& data );
	void ReplacePayloadRequest(uint16_t sizeOfPayload, PatternId_t patternId, FMessageId_t  msgId, void* payload, ReplacePayloadResponse_Data& data);
	void ClearMap(WaveformId_t wfid, FMessageId_t  msgId); //remove entry from msgToQElement.
	void CleanUpMsg( FMessageId_t  msgId);
}; //End of PacketHandler class

}
}


#endif //PACKET_HANDLER_H_
