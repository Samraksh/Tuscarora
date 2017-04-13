////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef DATAFLOWI_H
#define DATAFLOWI_H

#include <Base/BasicTypes.h>
#include <Base/TimeI.h>
#include <Base/FrameworkTypes.h>
#include <Base/Delegate.h>
#include <Interfaces/Core/MessageT.h>
#include "Framework/FrameworkConstants.h"


namespace Core {
 
  
///Types and classes related to the dataflow service
namespace Dataflow {
 
  //const uint8_t PDN_FW_RECV_MASK=0x01;
  const uint8_t PDN_WF_RECV_MASK=0x02;
  const uint8_t PDN_WF_SENT_MASK=0x04;
  const uint8_t PDN_RECV_DEST_MASK=0x08;
  
  enum DataStatusTypeE{
    PDN_ERROR,
	PDN_ERROR_PKT_TOOBIG,
    PDN_FW_RECV,	///Acknowledgement when framework reaceived message
    PDN_WF_RECV,	///Acknowlegment that Waveform received the messsage
    PDN_WF_SENT,	///Acknowlegment that Waveform sent the messsage out on channel
    PDN_DST_RECV,	///Acknowlegment that (some module on) destination received the message
    //FW_READY_TO_RECV,	///Notification from framework that it is ready to receive more packets from pattern
	PDN_BROADCAST_NOT_SUPPORTED,
  }; 
  
  enum WF_DataStatusTypeE{
    WDN_WF_RECV =1,	///Acknowlegment that Waveform received the messsage
    WDN_WF_SENT,	///Acknowlegment that Waveform sent the messsage out on channel
    WDN_DST_RECV,	///Acknowlegment that (some module) on destination received the message
  //  READY_TO_RECV,	///Notification from framework that it is ready to receive more packets from pattern
  };

  /// A structure that is passed as the parameter to the DataNotifier Delegate
  struct DataStatusParam{
  public:
    DataStatusTypeE statusType[MAX_DEST]; //1b
    bool statusValue[MAX_DEST];//1b
    NodeId_t  destArray[MAX_DEST]; //32b
    uint8_t noOfDest; //1b
    MessageId_t messageId;//4b
    bool readyToReceive; //1b
    Nonce_t nonce; //2b
    //U64NanoTime expCompTime;//8b
    DataStatusParam (){}
    //DataNotifierParam (DataNotifierTypeE _type) { ackType = _type; }
    DataStatusParam (uint32_t _messageId ){
      readyToReceive = true;
      //ackType = _type;
      messageId = _messageId;
      //status = _status;
    }
  };

  typedef Delegate<void, DataStatusParam> DataflowDelegate_t;
  
  ///Specifies a definition for the fragementation and reassembly service. 
  class FragmentAndReassembleI {
    
  public:
    ///Fragments a framework packet into a number of packets and returns them in an array pointed to by the 3rd parameter. 
    virtual bool Fragment (FMessage_t *org, uint16_t maxSize, FMessage_t *resultFragments, uint16_t *numberOfFragments) = 0;
    
    ///Reassembles an array of individual fragment packet 
    virtual bool Reassemble (FMessage_t *fragments, uint16_t *numberOfFragments, FMessage_t *result) = 0;
    
    ///Destructor for abstract virtual class
    virtual ~FragmentAndReassembleI () {}
  };
  
  
}//End namespace
}

#endif //DATAFLOWI_H
