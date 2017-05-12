////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef PATTERN_EVENT_DISPATCH_H_
#define PATTERN_EVENT_DISPATCH_H_

#include <Interfaces/Pattern/PatternEvent.h>
#include <Interfaces/PWI/Framework_I.h>
#include <Interfaces/Pattern/PatternBase.h>
#include <Lib/DS/BSTMapT.h>
#include "PatternDelegates.h"
#include <Interfaces/PWI/PatternEventDispatch_I.h>
#include "PTN_ControllerShim.h"

namespace PWI {

 class PatternEventDispatch : public PatternEventDispatch_I {
    


//    PatternEvent<PTN_CONT_RES_EVT, ControlResponseParam> controlEvent;
//    PatternEvent<PTN_NBR_CHG_EVT, NeighborUpdateParam> nbrEvent;
//    PatternEvent<PTN_RECV_MSG_EVT, FMessage_t&> rcvMsgEvent;
//    PatternEvent<PTN_DATA_NTY_EVT, DataStatusParam> dataNotifierEvent;
    
    
  public:
    PTN_ControllerShim * ptn_controller_ptr;
    PatternEventDispatch(){
        Debug_Printf(DBG_SHIM, "PatternEventDispatch Initializing for linux-multiprocess \n");
        printf("PatternEventDispatch Initializing for linux-multiprocess  \n");
        ptn_controller_ptr = new PTN_ControllerShim();
    }
    
    bool InvokeControlEvent (PatternId_t pid, ControlResponseParam _param0) {


        int calltype = FW2PTN_Event_MethodE::FW2PTN_Event_Control;

//       // GenericSerializer<int, ControlResponseParam >
//        //gs(calltype, _param0);
//        GenericSerializer<int, uint32_t, ControlResponseTypeE&, GenericMsgPayloadSize_t, void* >
//        gs(calltype, _param0.sequenceNo, _param0.type, _param0.dataSize, _param0.data);
//
//
//        Generic_VarSized_Msg* ptn2fw_msg = gs.Get_Generic_VarSized_Msg_Ptr();
//
//        bool rv = ptn_controller_ptr->SendMsg2Client( pid,  ptn2fw_msg->GetPayload(),ptn2fw_msg->GetPayloadSize() );

        bool rv = ptn_controller_ptr->SendMsg2Client<int, uint32_t, ControlResponseTypeE&, GenericMsgPayloadSize_t, void* >
        (pid, calltype, _param0.sequenceNo, _param0.type, _param0.dataSize, _param0.data );

        return rv;

      //return controlEvent.Invoke(pid, _param0);
    }
    bool InvokeRecvMsgEvent (PatternId_t pid, FMessage_t& _param0) {

        int calltype = FW2PTN_Event_MethodE::FW2PTN_Event_RcvMsg;
//
//        GenericSerializer<int, MessageTypeE, WaveformId_t, NodeId_t, GenericMsgPayloadSize_t, uint8_t* >
//               gs(calltype, _param0.GetType(), _param0.GetWaveform(), _param0.GetSource(), _param0.GetPayloadSize(),  _param0.GetPayload());
//
//
//        Generic_VarSized_Msg* ptn2fw_msg = gs.Get_Generic_VarSized_Msg_Ptr();
//
//        bool rv = ptn_controller_ptr->SendMsg2Client( pid, ptn2fw_msg->GetPayload(), ptn2fw_msg->GetPayloadSize() );

        bool rv = ptn_controller_ptr->SendMsg2Client<int, MessageTypeE, WaveformId_t, NodeId_t, GenericMsgPayloadSize_t, uint8_t* >
( pid, calltype, _param0.GetType(), _param0.GetWaveform(), _param0.GetSource(), _param0.GetPayloadSize(),  _param0.GetPayload());

        return rv;
      //return rcvMsgEvent.Invoke(pid, _param0);
    }
    bool InvokeDataStatusEvent (PatternId_t pid, DataStatusParam _param0) {
        int calltype = FW2PTN_Event_MethodE::FW2PTN_Event_DataStasus;
//        GenericSerializer<int,  GenericMsgPayloadSize_t, void*, GenericMsgPayloadSize_t, void*, GenericMsgPayloadSize_t, void*, MessageId_t, bool, uint16_t >
//        gs(calltype, _param0.noOfDest*sizeof(DataStatusTypeE), (void*)_param0.statusType,  _param0.noOfDest*sizeof(bool), (void*)_param0.statusValue, _param0.noOfDest*sizeof(NodeId_t), (void*)_param0.destArray,_param0.messageId, _param0.readyToReceive, _param0.nonce   );
//        Generic_VarSized_Msg* ptn2fw_msg = gs.Get_Generic_VarSized_Msg_Ptr();
//
//        bool rv = ptn_controller_ptr->SendMsg2Client(  pid, ptn2fw_msg->GetPayload(), ptn2fw_msg->GetPayloadSize() );

        bool rv = ptn_controller_ptr->SendMsg2Client<int,  GenericMsgPayloadSize_t, void*, GenericMsgPayloadSize_t, void*, GenericMsgPayloadSize_t, void*, MessageId_t, bool, uint16_t >
(  pid, calltype, _param0.noOfDest*sizeof(DataStatusTypeE), (void*)_param0.statusType,  _param0.noOfDest*sizeof(bool), (void*)_param0.statusValue, _param0.noOfDest*sizeof(NodeId_t), (void*)_param0.destArray,_param0.messageId, _param0.readyToReceive, _param0.nonce   );

        return rv;

      //return dataNotifierEvent.Invoke(pid, _param0);
    }
    bool InvokeNeighborUpdateEvent (PatternId_t pid, NeighborUpdateParam _param0) {
        int calltype = FW2PTN_Event_MethodE::FW2PTN_Event_NeighborUpdate;
//        GenericSerializer<int, NeighborUpdateParam >
//        gs(calltype, _param0);
//        Generic_VarSized_Msg* ptn2fw_msg = gs.Get_Generic_VarSized_Msg_Ptr();
//
//        bool rv = ptn_controller_ptr->SendMsg2Client(  pid, ptn2fw_msg->GetPayload(), ptn2fw_msg->GetPayloadSize() );
        bool rv = ptn_controller_ptr->SendMsg2Client<int, NeighborUpdateParam >
(  pid, calltype, _param0);
        return rv;

      //return nbrEvent.Invoke(pid, _param0);
    }
    
  };
  
} //End of namespace

#endif //PATTERN_EVENT_DISPATCH_H_
