////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef MESSAGECABINET_H_
#define MESSAGECABINET_H_

#include "CoreTypes.h"
#include "Framework/PWI/PatternClient.h"
#include <Interfaces/Core/MessageT.h>
#include <Interfaces/Waveform/WF_MessageT.h>

#include "Framework/PolicyManager/PolicyManager.h"
#include "Framework/Core/Neighborhood/AddressMap.h"
#include <Lib/DS/BSTMapT.h>
#include "PriorityHeap.h"


using namespace PAL;
using namespace Core::Policy;
using namespace Waveform;


namespace Core {
namespace Dataflow {

class FMessageQElement {
public:
  BSTMapT<NodeId_t,bool> dnFromDest;
  BSTMapT<NodeId_t,bool> dnRecvByWF;
  BSTMapT<NodeId_t,bool> dnSentByWF;
  
  uint16_t dnRecvByWF_remaining;
  uint16_t dnSentByWF_remaining;
  uint16_t dnFromDest_remaining;
  
  bool operationComplete;
  //NodeId_t destArray[MAX_DEST];
  Link* linkArray[MAX_DEST];
  WF_MessageQElement* wfQElement[MAX_WAVEFORMS]; //points corresponding WF_MessageQElement in Heap.
  uint16_t wfCount;
  //uint64_t dummy;
  //DataStatusTypeE ackStatus[MAX_WAVEFORMS];
  FMessage_t *msg;
  uint16_t nonce;
  MsgPriorityMetric *metric;
  MessageId_t messageId;
  uint16_t noOfDest;
  bool broadcast;
  WaveformId_t waveform[MAX_WAVEFORMS];
  //uint8_t* old_payload_ptr;
  
  //FMessageQElement(FMessage_t *_msg, MsgPriorityMetric *_metric, MessageId_t _id, NodeId_t *_nodeArray, Link **_linkArray, uint16_t _noOfDest,bool broadcast);
  FMessageQElement(FMessage_t *_msg, MsgPriorityMetric *_metric, MessageId_t _id, Link **_linkArray, uint16_t _noOfDest,bool broadcast, uint16_t nonce);
  ~FMessageQElement(); //destructor
  bool Check_Item_Exists_dnFromDest(NodeId_t node_id){
    if (dnFromDest.Size() >0){
	  //Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: CHeck dnFromDest[%d] exists\n",node_id);
	  BSTMapT<NodeId_t, bool> ::Iterator it = dnFromDest.Find(node_id);
	  if(it == dnFromDest.End())
		  return false;
	  else
	    return true;
    }else {
     return false; 
    }
  }
  bool Check_Item_Exists_dnSentByWF(NodeId_t node_id){
    if (dnSentByWF.Size() >0){
	  //Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: CHeck dnSentByWF[%d] exists\n",node_id);
  	  BSTMapT<NodeId_t, bool> ::Iterator it = dnSentByWF.Find(node_id);
  	  if(it == dnSentByWF.End())
  		  return false;
  	  else
  		  return true;
    }else {
     return false; 
    }
  }
  bool Check_Item_Exists_dnRecvByWF(NodeId_t node_id){
    if (dnRecvByWF.Size() >0){
	 Debug_Printf(DBG_CORE_DATAFLOW, "MessageCabinet:: show address os dnRecvByWF Map %p, finding node %d\n",&dnRecvByWF, node_id);
  	  BSTMapT<NodeId_t, bool> ::Iterator it = dnRecvByWF.Find(node_id);
  	  if(it == dnRecvByWF.End())
  		  return false;
  	  else
  		  return true;
    }else {
     return false; 
    }
  }
  DataStatusTypeE FindBroadcastStatus(){
	  if(this->dnFromDest_remaining == 0){
		  return PDN_DST_RECV;
	  }
	  if(this->dnSentByWF_remaining == 0){
		  return PDN_WF_SENT;
	  }
	  if(this->dnRecvByWF_remaining == 0){
		  return PDN_WF_RECV;
	  }
	  return PDN_FW_RECV;
  }
  DataStatusTypeE GetStatusOfNode(NodeId_t node_id);
};

//typedef HeapT<WF_MessageQElement*,PriorityFCFSCompare> WFPriorityMsgHeap;
//typedef WF_MessageQElement<PriorityFCFSCompare> WF_PriorityHeapElement_t;
//typedef AVLBST_T<WF_MessageQElement*, PriorityFCFSCompare> WFPriorityMsgHeap;
typedef BSTMapT<MessageId_t, FMessageQElement*> FMsgMap;
//typedef BST_T<WF_MessageQElement*, PriorityFCFSCompare> WFPriorityMsgBST;

struct DestArray{
public:
  uint64_t destArray[MAX_DEST];
  uint16_t noOfDest;
  DestArray(){
   noOfDest=0;
   memset(destArray, 0, sizeof(uint64_t)*MAX_DEST);
  }
};

typedef BSTMapT<WaveformId_t, DestArray> DestMap_t;

class MessageCabinet {
	BSTMapT<WaveformId_t, PacketAdaptor<uint64_t>*> *wfMsgAdaptorMap;
	WF_AttributeMap_t *wfAttributes;
	AddressMap *addressMap;

	MessageId_t frameworkMsgId;
	WF_MessageId_t waveformMsgId;
	WFMsgIdToFmsgIdMap_t *wfMsgToFMsgMap;
	
	
	WF_PriorityMsgHeap *wfMsgHeap;


	MessageId_t GetNewFrameworkMsgId() {
		//printf("Generating next Framework pkt id: current id (ptr: %p) is %d\n",&num1, num1);
		//printf("New Framework pkt id: current id is %d\n",num1);
		return ++frameworkMsgId;
	}
	
	WF_MessageId_t GetNewWaveformMsgId() {
		//printf("Generating next Waveform pkt id: current id (ptr: %p) is %d\n",&num2,num2);
		return ++waveformMsgId;
	}
public:
	FMsgMap *framworkMsgMap; //Making it public, Masahiro
	MessageCabinet(WF_AttributeMap_t *wfAttributes, WFMsgIdToFmsgIdMap_t *wfMsgToFMsgMap, AddressMap *addressMap);
	void SetWaveformAdaptorMap(BSTMapT<WaveformId_t, PacketAdaptor<uint64_t>*> *_wfMsgAdaptorMap);
	bool AddMessageDestination(MessageId_t msgId, NodeId_t *nodeArray, Link **linkArray, uint16_t noOfDest);

	bool RemoveMessageDestination(MessageId_t msgId, NodeId_t *nodeArray, uint16_t noOfDest);
	//FMessage_t* GetNextMessage();
	WF_MessageQElement* GetNextMessage(WaveformId_t waveformId);
	WF_MessageBase* GetNextMessageForNeighbor(uint16_t neighborId);

	//MessageId_t AddNewFrameworkMessage (PatternId_t pid, FMessage_t *msg, NodeId_t *nodeArray, Link** destArray, uint16_t noOfDest, bool broadcast, WaveformId_t _wfid);
	MessageId_t AddNewFrameworkMessage (PatternId_t pid, FMessage_t *msg, NodeId_t *nodeArray, Link** destArray, uint16_t noOfDest, bool broadcast, WaveformId_t _wfid, MessageId_t messageId, uint16_t _nonce,bool softwarebroadcast);
	//MessageId_t AddNewFrameworkMessage (FMessage_t *msg);
	bool RemoveFrameworkMessage(MessageId_t msgId);
	uint32_t GetNumberWaveFormOfMessages();
	uint32_t GetNumberWaveformOfMessages(WaveformId_t wfId);

	//  WF_MessageQElement* LookUpWaveformMessage(MessageId_t _wfMsgId, WaveformId_t _wid);
	FMessageQElement* LookUpFrameworkMessage(MessageId_t _msgId);

	void CleanUp(MessageId_t mid);
	virtual ~MessageCabinet();
	uint16_t GetdnFromDest(MessageId_t msgId);
	uint16_t GetdnSentByWF(MessageId_t msgId);
	uint16_t GetdnRecvByWF(MessageId_t msgId);
	bool IsmsgIdExists(MessageId_t msgId);
	bool Cancel_Data( MessageId_t msgId, NodeId_t* destArray, uint16_t noOfDest, WaveformId_t wfid);
	bool CancelDataReponse_Helper(FMessageQElement* fmsg_ptr, uint16_t noOfDest, NodeId_t remove_node, WaveformId_t wfid, MessageId_t msgId);
	bool Remove_Node(FMessageQElement* fmsg_ptr, uint16_t noOfDest, NodeId_t remove_node, WaveformId_t wfid, MessageId_t msgId,bool& flag);
	uint16_t CancelDataSendRequest(PatternId_t patternId, MessageId_t msgId, NodeId_t *destArray, uint16_t noOfDest, WaveformId_t* wf_inquery, CancelDataResponse_Data& data,bool& flag );
	bool Replace_Ptr(FMessageQElement * fmsg_ptr, void* payload ,uint16_t sizeOfPayload, WaveformId_t wfid);
	uint16_t ReplacePayloadRequest(uint16_t sizeOfPayload, PatternId_t patternId, MessageId_t msgId, void* payload ,WaveformId_t* wf_inquery, ReplacePayloadResponse_Data& data);
	/*bool peekHeap(WaveformId_t wfid){
		if(wfMsgHeap[wfid].PeekRoot()->msg->GetType() == Types::Pattern_Type){
			return true;
		}
		else{
			return false;
		}
	}*/
	uint16_t Add_Node(FMessageQElement* fmsg_ptr, uint64_t* node_added, uint16_t noOfDest, WaveformId_t wfid,RequestId_t rId, MessageId_t msgId, PatternId_t pid);
	void AddToMC(WaveformId_t wfId,WF_MessageQElement* element){wfMsgHeap[wfId].Insert(element);}
};


}//end of namespace
}
#endif /* MESSAGECABINET_H_ */
