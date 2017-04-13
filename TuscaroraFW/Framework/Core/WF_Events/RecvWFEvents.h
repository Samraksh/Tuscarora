////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef _RECV_WF_EVENTS_
#define _RECV_WF_EVENTS_

#include <Lib/Misc/Debug.h>
#include <Interfaces/Waveform/WF_I.h>
#include "Framework/Core/DataFlow/PacketHandler.h"
#include "Framework/Core/ControlPlane/WF_Controller.h"

using namespace Waveform;
using namespace Core::WFControl;

namespace Core {
namespace Events {
  
//This class receives signals from Waveforms
template <typename WF_AddressType>
class RecvWFEvents {
  PacketHandler *ph;
  WF_Controller *wfControl;
  
public:
  RecvWFEvents(PacketHandler *_ph, WF_Controller *_wfControl);
  RecvWFEvents(); //Use only for testing purposes
  void Recv_NewWaveformEvent(WaveformId_t wid);
  void Recv_RecvMsgEvent(WF_RecvMsgParam<WF_AddressType> param);
  void Recv_LinkEstimationEvent(WF_LinkEstimationParam<WF_AddressType>  param);
  void Recv_ScheduleUpdateEvent(WF_ScheduleUpdateParam param);
  void Recv_DataNotificationEvent(WF_DataStatusParam<WF_AddressType> param);
  void Recv_ControlResponseEvent(WF_ControlResponseParam param);
};

}//end of namespace
}

#endif //_RECV_WF_EVENTS_
