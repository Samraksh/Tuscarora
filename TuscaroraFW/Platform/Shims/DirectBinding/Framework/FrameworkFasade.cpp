////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include "FrameworkFasade.h"
#include "PatternEventDispatch.h"
#include "Framework/PWI/FrameworkBase.h"

namespace PWI {
 
Framework_I& GetFrameworkShim(){
  Framework_I *fi = new FrameworkFasade();
  return *fi;
}
  
FrameworkFasade::FrameworkFasade(){
  fi = &GetFrameworkInterface();
}


void FrameworkFasade::RegisterDelegates(PatternId_t _pid, RecvMessageDelegate_t* _recvDelegate, NeighborDelegate* _nbrDelegate, DataflowDelegate_t* _dataNotifierDelegate, ControlResponseDelegate_t* _controlDelegate)
{
  //static_cast<FrameworkBase*>(fi) ->RegisterDelegates(_pid, _recvDelegate,_nbrDelegate,_dataNotifierDelegate, _controlDelegate);
    Debug_Printf(DBG_CORE, "FrameworkFasade:: Storing delegates  %d\n",_pid);
    //static_cast<PatternEventDispatch*>(static_cast<FrameworkBase*>(fi) -> eventDispatcher)->RegisterDelegates(_pid, _recvDelegate,_nbrDelegate,_dataNotifierDelegate,_controlDelegate);
    m_pid = _pid ;
    m_recvDelegate = _recvDelegate ;
    m_nbrDelegate = _nbrDelegate ;
    m_dataNotifierDelegate = _dataNotifierDelegate ;
    m_controlDelegate = _controlDelegate ;
}

void FrameworkFasade::RegisterPatternRequest(PatternId_t patternId, const char uniqueName[128], PatternTypeE type){
    if(patternId == 0){
        patternId =   static_cast<FrameworkBase*>(fi)->NewPatternInstanceRequest(type, uniqueName );
        Debug_Printf(DBG_SHIM, "PTN_ControllerShim::Deserialize: PTN2FW_Call_RegisterPatternRequest got new PID patternId = %d type = %d \n", patternId, type);
    }
    static_cast<PatternEventDispatch*>(static_cast<FrameworkBase*>(fi) -> eventDispatcher)->RegisterDelegates(patternId, m_recvDelegate,m_nbrDelegate,m_dataNotifierDelegate,m_controlDelegate);
    static_cast<FrameworkBase*>(fi)->RegisterPatternRequest(patternId, type);

}

void FrameworkFasade::SendData(PatternId_t pid, NodeId_t destArray[MAX_DEST], uint16_t noOfDest, FMessage_t& msg, uint16_t nonce, bool noAck)
{
  fi->SendData(pid, destArray, noOfDest, msg, nonce, noAck);
}

void FrameworkFasade::SendData(PatternId_t pid, NodeId_t destArray[MAX_DEST], uint16_t noOfDest, LinkComparatorTypeE lcType, FMessage_t& msg, uint16_t nonce, bool noAck)
{
  fi->SendData(pid, destArray, noOfDest, lcType, msg, nonce, noAck);
}

#if ENABLE_FW_BROADCAST==1
void FrameworkFasade::BroadcastData(PatternId_t pid, FMessage_t& msg, WaveformId_t wid, uint16_t nonce)
{
  fi->BroadcastData(pid,msg,wid,nonce);
}
#endif

void FrameworkFasade::ReplacePayloadRequest(PatternId_t patternId, FMessageId_t  msgId, void* payload, uint16_t sizeOfPayload)
{
  fi->ReplacePayloadRequest(patternId, msgId, payload,sizeOfPayload);
}

void FrameworkFasade::AddDestinationRequest(PatternId_t patternId, FMessageId_t  msgId, NodeId_t destArray[MAX_DEST], uint16_t noOfNbrs)
{
  fi->AddDestinationRequest(patternId, msgId, destArray, noOfNbrs);
}
void FrameworkFasade::AddDestinationRequest(PatternId_t patternId, FMessageId_t  msgId, NodeId_t destArray[MAX_DEST], uint16_t noOfNbrs, LinkComparatorTypeE lcType)
{
  fi->AddDestinationRequest(patternId, msgId, destArray, noOfNbrs, lcType);
}

void FrameworkFasade::CancelDataRequest(PatternId_t patternId, FMessageId_t  msgId, NodeId_t destArray[MAX_DEST], uint16_t noOfDest)
{
  fi->CancelDataRequest(patternId, msgId, destArray, noOfDest);
}

void FrameworkFasade::DataStatusRequest(PatternId_t patternId, FMessageId_t  msgId)
{
  fi->DataStatusRequest(patternId, msgId);
}

void FrameworkFasade::FrameworkAttributesRequest(PatternId_t patternId)
{
  fi->FrameworkAttributesRequest(patternId);
}

void FrameworkFasade::SelectDataNotificationRequest(PatternId_t patternId, uint8_t notifierMask)
{
  fi->SelectDataNotificationRequest(patternId, notifierMask);
}

void FrameworkFasade::SelectLinkComparatorRequest(PatternId_t patternId, LinkComparatorTypeE lcType)
{
  fi->SelectLinkComparatorRequest(patternId, lcType);
}

void FrameworkFasade::SetLinkThresholdRequest(PatternId_t patternId, LinkMetrics threshold)
{
  fi->SetLinkThresholdRequest(patternId, threshold);
}

/*void FrameworkFasade::NewPatternInstanceRequest(PatternTypeE type, char uniqueName[128])
{
  static_cast<FrameworkBase*>(fi)->NewPatternInstanceRequest(type,uniqueName);
}*/

}
