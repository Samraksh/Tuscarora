////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef PATTERNCLIENT_H_
#define PATTERNCLIENT_H_

#include "Framework/Core/Neighborhood/NeighborTable.h"
//#include "Framework/Core/Estimation/Periodic.h"
#include "Framework/PWI/Neighborhood/CustomPatternNeighborTable.h"

using namespace Core::Discovery;
using namespace Core::Estimation;
using namespace PWI::Neighborhood;


namespace PWI {
  


  class Accounting {
    uint16_t noOfPacketsSent;
    uint64_t totalChannelUsage;
    uint32_t bitsPerSec;
    
  public:
    Accounting(){
      noOfPacketsSent=0;
      totalChannelUsage=0;
      bitsPerSec=0;
    }
    void AddPacket(uint32_t size, uint16_t noOfDest){
      noOfPacketsSent++;
      totalChannelUsage += size * noOfDest;
    }
  };
  
  ///Keeps track of information about a pattern, which is currently instantiated
  class PatternClient{
  public:
    PatternId_t PID;
    uint32_t responseSeqNo;
    bool responsePending;
    CustomPatternNeighborTable* cusNbrTable;
    uint8_t priority;
    Accounting accounts;
    uint16_t noOfOutstandingPkts;
    bool ackRecvWFNotifier;
    bool ackSentWFNotifier;
    bool ackRecvDestNotifier;
    bool ackFWRecvNotifier;
    uint16_t nonce;  
    MessageId_t msgId;
    //PatternId_t pid;
    bool readyToReceive;
    bool softwareBroadcast;
    //create structure holds per message information.
    //BSTMapT< MessageId_t , perMessageInfo* > perMessageInfoMap; 
  public:
    PatternClient(){
      responseSeqNo=0;
      responsePending=false;
      priority=254;
      readyToReceive = true;
      ackRecvWFNotifier = true;
      ackFWRecvNotifier = true;
      ackSentWFNotifier = true;
      ackRecvDestNotifier = true;
      noOfOutstandingPkts =0;
      nonce = 1234; //Masahiro, just storing some random value as initial value.
    }  

    void SelectDataNotifications (uint8_t _notifierMask){
      ackRecvWFNotifier = _notifierMask & PDN_WF_RECV_MASK;
      ackSentWFNotifier = _notifierMask & PDN_WF_SENT_MASK;
      ackRecvDestNotifier = _notifierMask & PDN_RECV_DEST_MASK;
      //ackFWRecvNotifier = _notifierMask & PDN_FW_RECV_MASK;
    }

    bool IsNotifierSet(DataStatusTypeE type){
      switch(type) {
	/*case PDN_FW_RECV:
	  return ackFWRecvNotifier;
	  break;
	  */
	case PDN_WF_RECV:
	  return ackRecvWFNotifier;
	  break;
	case PDN_WF_SENT:
	  return ackSentWFNotifier;
	  break;
	case PDN_DST_RECV:
	  return ackRecvDestNotifier;
	  break;
	default:
	  break;
      }
      return false;
    }
    void SetPriority(uint8_t _priority){
	    priority= _priority;
    }
    uint8_t GetID(){
	    return PID;
    }
    void SetID(uint8_t id){PID=id;}
    void AddToAccounts(uint32_t size, uint16_t noOfDest){
      accounts.AddPacket(size, noOfDest);
    }
    //uint8_t GetASNPCount(){return asnpCount;}
    //uint8_t SetASNPCount(uint8_t count){asnpCount=count;}
  };

}

#endif /* PATTERNCLIENT_H_ */
