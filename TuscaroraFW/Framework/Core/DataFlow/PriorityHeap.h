////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef PRIORITY_HEAP_H
#define PRIORITY_HEAP_H

#include <Interfaces/Waveform/WF_MessageT.h>
#include <Interfaces/Waveform/WF_I.h>

#include <Lib/DS/HeapT.h>
#include <Lib/DS/AVLBinarySearchTreeT.h>
#include <Lib/DS/BSTMapT.h>

using namespace Waveform;

namespace Core {
namespace Dataflow {

struct MsgPriorityMetric {
 uint16_t priority;
 uint64_t submitTimeMicro; 
};

class WF_MessageQElementComparator_t;

struct WF_MessageQElement {
  WF_MessageBase *msg;
  WF_MessageId_t  wfmsgId;
  FMessageId_t  frameworkMsgId;
  MsgPriorityMetric *metric;

  //bool ackFromDest[MAX_DEST];  //indicates if acknowledgement from destination is recieved
  //bool msgSentByWF[MAX_DEST];  //indicates if acknowledgement from WF saying it is send out is received
  //bool ackFromWF[MAX_DEST];    //indicates if acknowledgement from WF saying it got the data
  struct timeval waiting_in_framework; //stores time when this packet becomes ready to be send to framework
  struct timeval Waiting_ACK; //stores time when this ACK_WF_RECV is received for this packet.
  uint64_t destArray[Core::MAX_DEST];
  bool broadcast;
  uint16_t noOfDest;
  //Link* linkArray[MAX_DEST];
  
public:
  WF_MessageQElement(WF_MessageBase *_msg, MsgPriorityMetric *_metric, FMessageId_t  _wfMsgid, FMessageId_t  _fwsMsgid, uint64_t *_nodeArray, uint16_t _noOfDest, bool _broadcast){
    msg = _msg;
    wfmsgId=_wfMsgid;
    frameworkMsgId = _fwsMsgid;
    metric = _metric;
    noOfDest = _noOfDest;
    broadcast = _broadcast;
    //memset(destArray, 0,  _noOfDest*sizeof(uint64_t));
    memcpy(destArray, _nodeArray, _noOfDest*sizeof(uint64_t));
    //printf("WF_MessageQElement::Constructor:: this: %p, Src: %p, Dst: %p, size: %d \n", this, _nodeArray, destArray, (int)(_noOfDest*sizeof(uint64_t))); fflush(stdout);
    //for (int i=0; i< noOfDest; i++){
    //  printf("WF_MessageQElement:: Destination %d is %lu, %lu\n",i, _nodeArray[i], destArray[i]); fflush(stdout);      
    //}
  }
  /*bool operator < (WF_MessageQElement<Comparator> &B){
    return comp.LessThan(*this,B);
  }
  bool operator == (WF_MessageQElement<Comparator> &B){
    return comp.EqualTo(*this,B);
  }*/
  friend class WF_MessageQElementComparator_t;
};

class WF_MessageQElementComparator_t{
public:
	static bool LessThan ( WF_MessageQElement* const &A,  WF_MessageQElement* const&B) {
		return (A->wfmsgId < B->wfmsgId);
	}
	static bool EqualTo ( WF_MessageQElement* const &A,  WF_MessageQElement* const &B) {
		return (A->wfmsgId == B->wfmsgId);
	}
};

typedef AVLBST_T<WF_MessageQElement*,WF_MessageQElementComparator_t> WF_MessageSegmentsQElementSearchTree_t;


class PriorityFCFSCompare{
 public:
  static bool LessThan (WF_MessageQElement* A, WF_MessageQElement* B) {
    if(A->metric->priority < B->metric->priority){
      return true;
    }else if(A->metric->priority > B->metric->priority){
      return false;
    }else {
      if (A->metric->submitTimeMicro < B->metric->submitTimeMicro){
	return true;
      }else {
	return false;
      }
      return true;
    }
  }
  static bool EqualTo (WF_MessageQElement* A, WF_MessageQElement* B) {
    if(A->metric->priority != B->metric->priority){
      return false;
    }
    else {
      if (A->metric->submitTimeMicro == B->metric->submitTimeMicro){
	return true;
      }else {
	return false;
      }
    }
    return false;
  }
  
};

/**
 * @brief Keeps track of messages for a given waveform
 * 
 */


class WF_PriorityMsgHeap: public AVLBST_T<WF_MessageQElement*,PriorityFCFSCompare> {

public:
	WF_MessageQElement* ExtractTop(){
    AVLBSTElement<WF_MessageQElement*> *minNode = AVLBST_T<WF_MessageQElement*,PriorityFCFSCompare>::Begin();
    WF_MessageQElement* ret = minNode->GetData();
    AVLBST_T<WF_MessageQElement*,PriorityFCFSCompare>::DeleteElement(minNode);
    return ret;
  };
  /*
	WF_MessageQElement* ExtractTop(){
	    AVLBSTElement<WF_MessageQElement*> *minNode = AVLBST_T<WF_MessageQElement*,PriorityFCFSCompare>::Begin();
	    WF_MessageQElement* ret = minNode->GetData();
	    //I do not want to send message out if it is pattern type
	    if(ret->msg->GetType() == Types::Pattern_Type){
	    	return NULL;
	    }else{
	    	AVLBST_T<WF_MessageQElement*,PriorityFCFSCompare>::DeleteElement(minNode);
	    	return ret;
	    }
	  };*/

};

}//End namespace
}
#endif //PRIORITY_HEAP_H
