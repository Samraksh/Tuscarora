////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include "PatternShim.h"
#include "Platform/Shims/DirectBinding/Framework/FrameworkFasade.h"

namespace Patterns {
/* 
Framework_I& GetFrameworkShim(){
  Framework_I *fi = new FrameworkFasade();
  return *fi;
}
  */

//PatternShim_I::PatternShim_I(){}

//PatternShim_I::~PatternShim_I(){}



PatternShim::PatternShim(){
  fi = &(PWI::GetFrameworkShim());
}



void PatternShim::RegisterDelegates(PatternId_t _pid, RecvMessageDelegate_t* _recvDelegate, NeighborDelegate* _nbrDelegate, DataflowDelegate_t* _dataNotifierDelegate, ControlResponseDelegate_t* _controlDelegate)
{
    Debug_Printf(DBG_SHIM, "PatternShim::RegisterDelegates \n");
    // This internally stores the function pointers to be invoked for a reply message coming from the framework.
    // Does not send a message across process boundary
//    m_pid = _pid ;
//    m_recvDelegate = _recvDelegate ;
//    m_nbrDelegate = _nbrDelegate ;
//    m_dataNotifierDelegate = _dataNotifierDelegate ;
//    m_controlDelegate = _controlDelegate ;

  static_cast<FrameworkFasade*>(fi) ->RegisterDelegates(_pid, _recvDelegate,_nbrDelegate,_dataNotifierDelegate, _controlDelegate);
}

void PatternShim::SendData(PatternId_t pid, NodeId_t destArray[MAX_DEST], uint16_t noOfDest, FMessage_t& msg, uint16_t nonce, bool noAck)
{
  fi->SendData(pid, destArray, noOfDest, msg, nonce, noAck);
}

void PatternShim::SendData(PatternId_t pid, NodeId_t destArray[MAX_DEST], uint16_t noOfDest,  LinkComparatorTypeE lcType, FMessage_t& msg, uint16_t nonce, bool noAck)
{
  fi->SendData(pid, destArray, noOfDest,lcType, msg, nonce, noAck);
}



#if ENABLE_FW_BROADCAST==1
void PatternShim::BroadcastData(PatternId_t pid, FMessage_t& msg, WaveformId_t wid, uint16_t nonce)
{
  fi->BroadcastData(pid,msg,wid,nonce);
}
#endif

void PatternShim::ReplacePayloadRequest(PatternId_t patternId, FMessageId_t  msgId, void* payload, uint16_t sizeOfPayload)
{
  fi->ReplacePayloadRequest(patternId, msgId, payload,sizeOfPayload);
}

void PatternShim::AddDestinationRequest(PatternId_t patternId, FMessageId_t  msgId, NodeId_t destArray[MAX_DEST], uint16_t noOfNbrs)
{
  fi->AddDestinationRequest(patternId, msgId, destArray, noOfNbrs);
}
void PatternShim::AddDestinationRequest(PatternId_t patternId, FMessageId_t  msgId, NodeId_t destArray[MAX_DEST], uint16_t noOfNbrs, LinkComparatorTypeE lcType)
{
  fi->AddDestinationRequest(patternId, msgId, destArray, noOfNbrs, lcType);
}

void PatternShim::CancelDataRequest(PatternId_t patternId, FMessageId_t  msgId, NodeId_t destArray[MAX_DEST], uint16_t noOfDest)
{
  fi->CancelDataRequest(patternId, msgId, destArray, noOfDest);
}

void PatternShim::DataStatusRequest(PatternId_t patternId, FMessageId_t  msgId)
{
  fi->DataStatusRequest(patternId, msgId);
}


void PatternShim::RegisterPatternRequest(PatternId_t patternId,const char uniqueName[128], PatternTypeE type){
    fi->RegisterPatternRequest(patternId, uniqueName, type);

}
void PatternShim::FrameworkAttributesRequest(PatternId_t patternId)
{
  fi->FrameworkAttributesRequest(patternId);
}

void PatternShim::SelectDataNotificationRequest(PatternId_t patternId, uint8_t notifierMask)
{
  fi->SelectDataNotificationRequest(patternId, notifierMask);
}

void PatternShim::SelectLinkComparatorRequest(PatternId_t patternId, LinkComparatorTypeE lcType)
{
  fi->SelectLinkComparatorRequest(patternId, lcType);
}

void PatternShim::SetLinkThresholdRequest(PatternId_t patternId, LinkMetrics threshold)
{
  fi->SetLinkThresholdRequest(patternId, threshold);
}

/*void PatternShim::NewPatternInstanceRequest(PatternTypeE type, char uniqueName[128])
{
  //fi->NewPatternInstanceRequest(type,uniqueName);
}*/

Framework_I& GetPatternShim(){
 Framework_I *pS = static_cast<Framework_I *> (new PatternShim());
 return *pS;
}

} //End of namespace
