////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef FRAMEWORK_SHIM_H_
#define FRAMEWORK_SHIM_H_

//#include <Interfaces/Pattern/Pattern_I.h>
#include <Interfaces/PWI/Framework_I.h>

using namespace PWI;

namespace PWI {
  class FrameworkFasade : public Framework_I {
  
    Framework_I *fi;
    
    PatternId_t m_pid;
    RecvMessageDelegate_t* m_recvDelegate;
    NeighborDelegate* m_nbrDelegate;
    DataflowDelegate_t* m_dataNotifierDelegate;
    ControlResponseDelegate_t* m_controlDelegate;


  public:
    
    FrameworkFasade();
    void RegisterDelegates (PatternId_t _pid, RecvMessageDelegate_t *_recvDelegate, NeighborDelegate *_nbrDelegate, DataflowDelegate_t *_dataNotifierDelegate, ControlResponseDelegate_t *_controlDelegate);
  
    //Inherited from Framework_I
    void SendData (PatternId_t pid, NodeId_t destArray[MAX_DEST], uint16_t noOfDest, FMessage_t& msg, uint16_t nonce, bool noAck=false);
	void SendData (PatternId_t pid, NodeId_t destArray[MAX_DEST], uint16_t noOfDest, LinkComparatorTypeE lcType, FMessage_t& msg, uint16_t nonce, bool noAck=false);
#if ENABLE_FW_BROADCAST==1
	void BroadcastData(PatternId_t pid, FMessage_t& msg, WaveformId_t wid, uint16_t nonce );
#endif
    void ReplacePayloadRequest (PatternId_t patternId, MessageId_t msgId, void *payload, uint16_t sizeOfPayload);
    void RegisterPatternRequest (PatternId_t patternId, PatternTypeE type);
    void RegisterPatternRequest(PatternId_t patternId, const char uniqueName[128], PatternTypeE type);

    void AddDestinationRequest (PatternId_t patternId, MessageId_t msgId, NodeId_t destArray[MAX_DEST], uint16_t noOfNbrs);
    void AddDestinationRequest(PatternId_t patternId, MessageId_t msgId, NodeId_t destArray[MAX_DEST], uint16_t noOfNbrs, LinkComparatorTypeE lcType);
    void CancelDataRequest (PatternId_t patternId, MessageId_t msgId, NodeId_t destArray[MAX_DEST], uint16_t noOfDest);
    void DataStatusRequest (PatternId_t patternId, MessageId_t msgId); 
    void SelectDataNotificationRequest (PatternId_t patternId, uint8_t notifierMask);
    void SetLinkThresholdRequest (PatternId_t patternId, LinkMetrics threshold);
    void FrameworkAttributesRequest(PatternId_t patternId);
    void SelectLinkComparatorRequest (PatternId_t patternId, LinkComparatorTypeE  lcType);
    //void NewPatternInstanceRequest(PatternTypeE type, char uniqueName[128]);
  };
  
  
}//End of namespace


#endif // FRAMEWORK_SHIM_H_
