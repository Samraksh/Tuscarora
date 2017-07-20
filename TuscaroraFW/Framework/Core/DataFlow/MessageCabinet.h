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


  WF_MessageSegmentsQElementSearchTree_t wfQElement[MAX_WAVEFORMS]; //points corresponding WF_MessageQElements in Heap.
  uint16_t wfCount;

  FMessage_t *msg;

  void* replace_payload_ptr; //Keep the pointer of replace payload when waiting for a response from the wfs
  uint16_t n_pending_replace_payload_requests;
  uint16_t replace_payload_ptr_size;

  uint16_t nonce;
  MsgPriorityMetric *metric;
  FMessageId_t  messageId;


  uint16_t noOfDest;   //Indicates number of destinations. Should be less than MAX_DEST
  Link* linkArray[MAX_DEST]; //Selected links for the destinations
  WaveformId_t waveform[MAX_DEST]; //Selected links for the destinations
  bool broadcast;



  //FMessageQElement(FMessage_t *_msg, MsgPriorityMetric *_metric, FMessageId_t  _id, NodeId_t *_nodeArray, Link **_linkArray, uint16_t _noOfDest,bool broadcast);
  FMessageQElement(FMessage_t *_msg, MsgPriorityMetric *_metric, FMessageId_t  _id, Link **_linkArray, uint16_t _noOfDest,bool broadcast, uint16_t nonce);
  FMessageQElement() = delete; //Preventing the default constuctor since the member variables have to be initialized
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
typedef BSTMapT <FMessageId_t , FMessageQElement*> FMsgMap;
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

	FMessageId_t frameworkMsgId;
	WF_MessageId_t waveformMsgId;
	WFMsgIdToFragmentListMap_t *wfMsgToFMsgMap;
	
	
	WF_PriorityMsgHeap *wfMsgHeap;


	FMessageId_t GetNewFrameworkMsgId() {
		//printf("Generating next Framework pkt id: current id (ptr: %p) is %d\n",&num1, num1);
		//printf("New Framework pkt id: current id is %d\n",num1);
		return ++frameworkMsgId;
	}
	
	WF_MessageId_t GetNewWaveformMsgId() {
		//printf("Generating next Waveform pkt id: current id (ptr: %p) is %d\n",&num2,num2);
		return ++waveformMsgId;
	}

	bool AddWaveformMessage(FMessageQElement *fmsgelementptr, WaveformId_t _wfid, bool broadcast, uint64_t* destArray, uint16_t noOfDest, PatternId_t pid);

public:
	FMsgMap *framworkMsgMap; //Making it public, Masahiro
	MessageCabinet(WF_AttributeMap_t *wfAttributes, WFMsgIdToFragmentListMap_t *wfMsgToFMsgMap, AddressMap *addressMap);
	void SetWaveformAdaptorMap(BSTMapT<WaveformId_t, PacketAdaptor<uint64_t>*> *_wfMsgAdaptorMap);
	bool AddMessageDestination( FMessageId_t  msgId, NodeId_t *nodeArray, Link **linkArray, uint16_t noOfDest);

	bool RemoveMessageDestination( FMessageId_t  msgId, NodeId_t *nodeArray, uint16_t noOfDest);
	//FMessage_t* GetNextMessage();
	WF_MessageQElement* GetNextMessage(WaveformId_t waveformId);
	WF_MessageBase* GetNextMessageForNeighbor(uint16_t neighborId);

	//FMessageId_t AddNewFrameworkMessage (PatternId_t pid, FMessage_t *msg, NodeId_t *nodeArray, Link** destArray, uint16_t noOfDest, bool broadcast, WaveformId_t _wfid);
	FMessageId_t AddNewFrameworkMessage (PatternId_t pid, FMessage_t *msg, NodeId_t *nodeArray, Link** destArray, uint16_t noOfDest, bool broadcast, WaveformId_t _wfid, FMessageId_t  messageId, uint16_t _nonce,bool softwarebroadcast);
	//FMessageId_t AddNewFrameworkMessage (FMessage_t *msg);
	bool RemoveFrameworkMessage( FMessageId_t  msgId);
	uint32_t GetNumberWaveFormOfMessages();
	uint32_t GetNumberWaveformOfMessages(WaveformId_t wfId);

	//  WF_MessageQElement* LookUpWaveformMessage( FMessageId_t  _wfMsgId, WaveformId_t _wid);
	FMessageQElement* LookUpFrameworkMessage(FMessageId_t  _msgId);

	FragmentStatusTypeE GetHighestFragmentStatusofFmessage(FMessageQElement* fmsg_ptr);
	FragmentStatusTypeE GetHighestFragmentStatusofFmessageDest(FMessageQElement* fmsg_ptr, const Link* link);
	FragmentStatusTypeE GetHighestFragmentStatusofFmessageonWaveform(FMessageQElement* fmsg_ptr, WaveformId_t wfid);


	void CleanUp( FMessageId_t  mid);
	virtual ~MessageCabinet();
	uint16_t GetdnFromDest( FMessageId_t  msgId);
	uint16_t GetdnSentByWF( FMessageId_t  msgId);
	uint16_t GetdnRecvByWF( FMessageId_t  msgId);
	bool IsmsgIdExists( FMessageId_t  msgId);
	bool Cancel_Data( FMessageId_t  msgId, NodeId_t* destArray, uint16_t noOfDest, WaveformId_t wfid);
	bool CancelDataReponse_Helper(FMessageQElement* fmsg_ptr, uint16_t noOfDest, NodeId_t remove_node, WaveformId_t wfid, FMessageId_t  msgId);
	bool Remove_Node(FMessageQElement* fmsg_ptr, uint16_t noOfDest, NodeId_t remove_node, WaveformId_t wfid, FMessageId_t  msgId,bool& flag);
	uint16_t CancelDataSendRequest(PatternId_t patternId, FMessageId_t  msgId, NodeId_t *destArray, uint16_t noOfDest, WaveformId_t* wf_inquery, CancelDataResponse_Data& data,bool& flag );
	bool Replace_Ptr(FMessageQElement * fmsg_ptr, void* payload ,uint16_t sizeOfPayload, WaveformId_t wfid);
	uint16_t ReplacePayloadRequest(uint16_t sizeOfPayload, PatternId_t patternId, FMessageId_t  msgId, void* payload ,WaveformId_t* wf_inquery, ReplacePayloadResponse_Data& data);
	/*bool peekHeap(WaveformId_t wfid){
		if(wfMsgHeap[wfid].PeekRoot()->msg->GetType() == Types::Pattern_Type){
			return true;
		}
		else{
			return false;
		}
	}*/



	enum AddNodeReturnValue_t{
		ADDNODERV_FAIL, //Failure to add the destination
		ADDNODERV_ASK_PATTERN,
		ADDNODERV_DEST_ADDED_TO_EXISTING_WF_MSG,
		ADDNODERV_ADDED_AS_NEW_WF_MSG,

	};

	AddNodeReturnValue_t Add_Node(FMessageQElement* fmsg_ptr, uint64_t* node_added, uint16_t noOfDest, WaveformId_t wfid,RequestId_t rId, FMessageId_t  msgId, PatternId_t pid);


	void AddToMC(WaveformId_t wfId,WF_MessageQElement* element){wfMsgHeap[wfId].Insert(element);}

};


}//end of namespace
}
#endif /* MESSAGECABINET_H_ */
