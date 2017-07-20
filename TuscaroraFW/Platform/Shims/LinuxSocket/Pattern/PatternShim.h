////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef PATTERN_SHIM_H_
#define PATTERN_SHIM_H_

#include <Platform/Shims/LinuxSocket/Pattern/PatternShimMessages.h>

#include <Interfaces/Pattern/PatternShim_I.h>
#include "../SocketClient.h"
#include "../SocketCommunicatorClientBase.h"

#include "Lib/KernalSignals/IO_Signals.h"



using namespace Types;
using namespace Lib::Shims;

namespace Patterns {
  
  class PatternShim : public PatternShim_I, SocketCommunicatorClientBase {

  protected:
  
      PatternId_t m_pid;
      RecvMessageDelegate_t* m_recvDelegate;
      NeighborDelegate* m_nbrDelegate;
      DataflowDelegate_t* m_dataNotifierDelegate;
      ControlResponseDelegate_t* m_controlDelegate;

     // bool Deserialize(const GenericSocketMessages& fw2ptnmsg, int32_t sockfd) const; // Inherited form SocketCommunicatorClientBase
      bool Deserialize();

  public:
    
    PatternShim();

    void RegisterDelegates (PatternId_t _pid, RecvMessageDelegate_t *_recvDelegate, NeighborDelegate *_nbrDelegate, DataflowDelegate_t *_dataNotifierDelegate, ControlResponseDelegate_t *_controlDelegate);
  
    //Inherited from Framework_I
    void SendData (PatternId_t pid, NodeId_t destArray[MAX_DEST], uint16_t noOfDest, FMessage_t& msg, uint16_t nonce, bool noAck=false);
    void SendData (PatternId_t pid, NodeId_t destArray[MAX_DEST], uint16_t noOfDest, LinkComparatorTypeE lcType, FMessage_t& msg, uint16_t nonce, bool noAck=false);

    void BroadcastData(PatternId_t pid, FMessage_t& msg, WaveformId_t wid, uint16_t nonce );
    void ReplacePayloadRequest (PatternId_t patternId, FMessageId_t  msgId, void *payload, uint16_t sizeOfPayload);
    //void RegisterPatternRequest (PatternId_t patternId, PatternTypeE type);
    void RegisterPatternRequest(PatternId_t patternId, const char uniqueName[128], PatternTypeE type);
    void AddDestinationRequest (PatternId_t patternId, FMessageId_t  msgId, NodeId_t destArray[MAX_DEST], uint16_t noOfNbrs);
    void AddDestinationRequest (PatternId_t patternId, FMessageId_t  msgId, NodeId_t destArray[MAX_DEST], uint16_t noOfNbrs, LinkComparatorTypeE lcType);
    void CancelDataRequest (PatternId_t patternId, FMessageId_t  msgId, NodeId_t destArray[MAX_DEST], uint16_t noOfDest);
    void DataStatusRequest (PatternId_t patternId, FMessageId_t  msgId); 
    void SelectDataNotificationRequest (PatternId_t patternId, uint8_t notifierMask);
    void SetLinkThresholdRequest (PatternId_t patternId, LinkMetrics threshold);
    void FrameworkAttributesRequest(PatternId_t patternId);
    void SelectLinkComparatorRequest (PatternId_t patternId, LinkComparatorTypeE  lcType);
    void NewPatternInstanceRequest(PatternTypeE type, char uniqueName[128]);
    




    ~PatternShim(){}
  };
  
  
}//End of namespace



#endif //PATTERN_SHIM_H_
