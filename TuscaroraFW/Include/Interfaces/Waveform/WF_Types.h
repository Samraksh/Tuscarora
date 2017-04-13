////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef WF_TYPES_H_
#define WF_TYPES_H_

#include <Base/FrameworkTypes.h>
#include <Base/TimeI.h>
#include <Interfaces/Core/DataFlowI.h>
#include <Interfaces/Core/Link.h>
#include "Lib/PAL/PAL_Lib.h"
//#include <PAL/AsyncDelegate.h>

#include "WF_MessageT.h"

using namespace Types;

namespace Waveform {

  
  ///Enum defining the supported waveform address types
  enum WF_AddressTypeE {
    WFA_UINT64,
    WFA_ETH_MAC,
  };
  
  ///Enum for supported Antenna types
  enum WF_AntennaTypeE {
    OMNI_ANT = 1,
    DIRECTIONAL_ANT = 2,
  };
    
  ///Enum for supported Waveform types
  enum WF_TypeE {
    ALWAYS_ON_LOCAL_WF = 2,
    TDMA_LOCAL_WF = 3,
    LONG_LINK_WF = 1
  };


  ///Enum for the waveform modes supported by framework
  enum WF_ModeE {
    WF_MODE_NORMAL=1,
    WF_MODE_AJ=2,
    WF_MODE_LPD=4,
    WF_MODE_LOWPOWER=8,
    WF_MODE_RADIO_SILENCE=16,
  };
    
  ///Enum for type of waveform estimator
  enum WF_EstimatorTypeE {
    WF_FULL_EST, 	///Waveform provides the complete estimation
    //WF_COOP_EST,	///Waveform cooperatively works framework to provide estimates
    WF_NO_EST 	///Waveform does not provide estimates
  };


  ///Structure for information about the waveform. The waveform will create an instance of it and populate it with information
  ///which can be querried by framework
  struct WF_Attributes{
    WaveformId_t wfId;		///Waveform Ids given by the framework
    WF_TypeE type;		///Type of waveform	
    WF_EstimatorTypeE estType;	///Type of estimator provided by the waveform
    WF_AntennaTypeE antType;	///Type of antenna used by the waveform
    WF_ModeE mode;		///Current mode of the waveform
    bool broadcastSupport;	///Does waveform support broadcastSupport
    bool destReceiveAckSupport;   ///Does waveform support acknowlegements from receiver
    bool scheduleInfoSupport; 	///Does waveform expose its scheduling information to framework
    bool senderTimeStampSupport;
	bool receiverTimeStampSupport;
    uint16_t ifindex;		///Waveform OS/Platform interfce index number
    char name[32];		///Name of waveform
    uint16_t ackTimerPeriod;
    uint16_t headerSize;		///Size of the waveform header in bytes
    uint16_t maxPayloadSize;	///Maximum payload that can be sent on the waveform in bytes
    uint16_t maxPacketSize;	///Maximum packet size that can be sent on the waveform (in bytes), including the waveform header
    uint16_t minPacketSize;	///Minimum packet size that can be sent on the waveform (in bytes), including the waveform header
    uint32_t minInterPacketDelay; ///Minimum interpacket rate in micro seconds
    uint32_t channelRate; 	///The channel reate in bytes per second
    uint32_t maxBurstRate; 	///Maximum data burst rate in bytes per second
    UFixed_2_8_t energy;		///Energy in joules per bit  
    Fixed_14_16_t cost;		//Cost of using the WF for one packet
  };


  //Enum defining the valid responses to the status request
  enum WF_ControlP_StatusE {
    WF_NORMAL,
    WF_BUSY,
    WF_ERROR,
    WF_BUFFER_LOW,
    WF_BUFFER_FULL,
  };

  enum WF_ScheduleTypeE {WF_RecvSchd, WF_SendSchd, WF_LinkEstSchd};

  //A enum for the waveform control callbacks.
  enum WF_ControlResponseE {
    AttributeResponse,
   // SetScheduleResponse,
   //GetScheduleResponse,
    AddDestinationResponse,
    CancelPacketResponse,
    ReplacePayloadResponse,
    //StatusResponse, This is not in the documentation. [Masahiro]
    ControlStatusResponse, //Adding one in the documentation [Masahiro]
    DataStatusResponse,    //Adding one in the documentation [Masahiro]
    SetScheduleResponse
  };
  
  struct WF_ControlResponseParam {
    WaveformId_t wfid;
	RequestId_t id;
    WF_ControlResponseE type;
    uint32_t dataSize;
	void *data;
  }__attribute__((packed, aligned(1)));
  
  struct WF_ScheduleUpdateParam {
    NodeId_t nodeid;
    ScheduleTypeE type;
    ScheduleBaseI *data;
    uint32_t size;
  }__attribute__((packed, aligned(1)));

  template <typename WF_AddressType>
  struct WF_RecvMsgParam{
    MessageId_t wrn; //Waveform Receive Nonce
    WF_MessageT<WF_AddressType> *msg;
  }__attribute__((packed, aligned(1)));

} //End of namespace

#endif /* WF_TYPES_H_ */
