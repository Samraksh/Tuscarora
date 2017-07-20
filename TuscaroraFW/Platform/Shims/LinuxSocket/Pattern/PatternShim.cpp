////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include "PatternShim.h"
#include "Platform/Shims/DirectBinding/Framework/FrameworkFasade.h"
#include <Base/FrameworkTypes.h>

#include <Sys/SocketShim.h>
#include "../SocketClient.h"
#include "../Serialization.h"

namespace Patterns {
/* 
Framework_I& GetFrameworkShim(){
  Framework_I *fi = new FrameworkFasade();
  return *fi;
}
  */

//PatternShim_I::PatternShim_I(){}

//PatternShim_I::~PatternShim_I(){}
extern IO_Signals ioSignals;


PatternShim::PatternShim() : SocketCommunicatorClientBase(FRAMEWORK_HOST, FRAMEWORK_PATTERN_PORT, FRAMEWORK_PATTERN_PORT_TYPE ){
    Debug_Printf(DBG_SHIM, "PatternShim::PatternShim initializing \n");

    m_pid = 0;
    m_recvDelegate = NULL;
    m_nbrDelegate = NULL;
    m_dataNotifierDelegate = NULL;
    m_controlDelegate = NULL;

}

bool PatternShim::Deserialize() {
    //Debug_Printf(DBG_SHIM, "PatternShim::Deserialize ... ReadBytes / TotalBytes = %lu / %lu \n", fw2ptnmsg.GetBytesRead(), fw2ptnmsg.GetPayloadSize() );
	Debug_Printf(DBG_SHIM, "PatternShim::Deserialize ...");
    int calltype = 0;
//    GenericDeSerializer<int> (&fw2ptnmsg, calltype);
    Peek<int>(calltype);

    Debug_Printf(DBG_SHIM, "PTN_ControllerShim::Deserialize: msg.GetType() = %d \n", calltype);

    if(calltype == FW2PTN_Event_Greet) {
        Debug_Printf(DBG_SHIM, "PTN_ControllerShim::Deserialize: msg.GetType() = %d \n", PTN2FW_Call_Greet);
    }
    else if (calltype == FW2PTN_Event_Control){
        ControlResponseParam _param0;
        _param0.data = NULL;
        GenericMsgPayloadSize_t totalsize;
//        GenericDeSerializer< uint32_t, ControlResponseTypeE&, GenericMsgPayloadSize_t, void* >
//                gds(&fw2ptnmsg,  _param0.sequenceNo,  _param0.type, totalsize, _param0.data);
        Read<int, uint32_t, ControlResponseTypeE&, GenericMsgPayloadSize_t, void* >
                        ( calltype,  _param0.sequenceNo,  _param0.type, totalsize, _param0.data);
        //_param0.type = (ControlResponseTypeE)dummytype;
        _param0.dataSize = totalsize;
        m_controlDelegate->operator()(_param0);
        //free(_param0.data); //Not our responsibility anymore should be destructed by the recipient
    }
    else if (calltype == FW2PTN_Event_RcvMsg){
        FMessage_t _param0;
        MessageTypeE type;
        WaveformId_t wfid;    //needed on receiving side
        //bool broadcast; //needed on receiving side
        NodeId_t src;   //needed on receiving side
        GenericMsgPayloadSize_t totalsize;
        void* payload = NULL;

        Read< int, MessageTypeE, WaveformId_t, NodeId_t, GenericMsgPayloadSize_t, void* >
                  ( calltype, type, wfid, src, totalsize,  payload);

        _param0.SetType(type);
        _param0.SetWaveform(wfid);
        _param0.SetSource(src);
        _param0.SetPayloadSize(totalsize/sizeof(uint8_t));
        _param0.SetPayload((uint8_t*)payload);


        m_recvDelegate->operator()(_param0);
        //free(payload); // Part of FMessage_t no need to destruct
    }
    else if (calltype == FW2PTN_Event_DataStasus){
        DataStatusParam _param0;

        GenericMsgPayloadSize_t totalsize1, totalsize2, totalsize3;
        void* st = (void*)_param0.statusType;
        void* stValue = (void*)_param0.statusValue;
        void* destArray = (void*)_param0.destArray;
        Read< int,  GenericMsgPayloadSize_t, void*, GenericMsgPayloadSize_t, void*, GenericMsgPayloadSize_t, void*, FMessageId_t , bool, uint16_t >
         ( calltype,          totalsize1,         st,         totalsize2,        stValue,  totalsize3,          destArray ,  _param0.messageId, _param0.readyToReceive, _param0.nonce   );
        _param0.noOfDest = totalsize1 / sizeof(NodeId_t);

        m_dataNotifierDelegate->operator()(_param0);
    }
    else if (calltype == FW2PTN_Event_NeighborUpdate){
        NeighborUpdateParam param0;

        Read< int, NeighborUpdateParam>
              ( calltype,  param0);

        m_nbrDelegate->operator()(param0);
    }
    else{
        return false;
    }
    return true;

}

void PatternShim::RegisterDelegates(PatternId_t _pid, RecvMessageDelegate_t* _recvDelegate, NeighborDelegate* _nbrDelegate, DataflowDelegate_t* _dataNotifierDelegate, ControlResponseDelegate_t* _controlDelegate)
{
    Debug_Printf(DBG_SHIM, "PatternShim::RegisterDelegates \n");
    // This internally stores the function pointers to be invoked for a reply message coming from the framework.
    // Does not send a message across process boundary
    m_pid = _pid ;
    m_recvDelegate = _recvDelegate ;
    m_nbrDelegate = _nbrDelegate ;
    m_dataNotifierDelegate = _dataNotifierDelegate ;
    m_controlDelegate = _controlDelegate ;

}



void PatternShim::SendData (PatternId_t pid, NodeId_t destArray[MAX_DEST], uint16_t noOfDest, FMessage_t& msg, uint16_t nonce, bool noAck)
{
    Debug_Printf(DBG_SHIM, "PatternShim::SendData \n");

  int calltype = PTN2FW_Call_MethodE::PTN2FW_Call_Send;

  GenericMsgPayloadSize_t serial_msg_size;
  void* serial_msg = Shim::Serialization::Serialize(&msg,serial_msg_size);
  Debug_Printf(DBG_SHIM, "PatternShim::SendData FMessage is serialized\n");

  SendMsg2Server<int, PatternId_t, GenericMsgPayloadSize_t, void*, GenericMsgPayloadSize_t, void*, uint16_t, bool >
  (calltype, pid, noOfDest*sizeof(NodeId_t), (void*) destArray, serial_msg_size, serial_msg, nonce, noAck);


  //Destruct Data
  FMessage_t* msg_ptr = &msg;
  delete msg_ptr;
}

void PatternShim::SendData (PatternId_t pid, NodeId_t destArray[MAX_DEST], uint16_t noOfDest, LinkComparatorTypeE lcType, FMessage_t& msg, uint16_t nonce, bool noAck)
{
    int calltype = PTN2FW_Call_MethodE::PTN2FW_Call_SendwComp;
    GenericMsgPayloadSize_t serial_msg_size;
    void* serial_msg = Shim::Serialization::Serialize(&msg, serial_msg_size);
    SendMsg2Server<int, PatternId_t, GenericMsgPayloadSize_t, void*, LinkComparatorTypeE, GenericMsgPayloadSize_t, void*, uint16_t, bool >
	(calltype, pid, noOfDest * sizeof(NodeId_t), (void*)destArray, lcType, serial_msg_size, serial_msg, nonce, noAck);

    //Destruct Data
    FMessage_t* msg_ptr = &msg;
    delete msg_ptr;
}


void PatternShim::BroadcastData(PatternId_t pid, FMessage_t& msg, WaveformId_t wid, uint16_t nonce)
{
    int calltype = PTN2FW_Call_MethodE::PTN2FW_Call_Broadcast;
    GenericMsgPayloadSize_t serial_msg_size;
    void* serial_msg = Shim::Serialization::Serialize(&msg,serial_msg_size);
    SendMsg2Server<int, PatternId_t, GenericMsgPayloadSize_t, void*, WaveformId_t, uint16_t >
	(calltype, pid,serial_msg_size, serial_msg,wid,nonce);

    //Destruct Data
    FMessage_t* msg_ptr = &msg;
    delete msg_ptr;
}

void PatternShim::ReplacePayloadRequest(PatternId_t patternId, FMessageId_t  msgId, void* payload, uint16_t sizeOfPayload)
{
    int calltype = PTN2FW_Call_MethodE::PTN2FW_Call_ReplacePayload;
	SendMsg2Server<int, PatternId_t, FMessageId_t , GenericMsgPayloadSize_t, void* >
	(calltype, patternId, msgId, (GenericMsgPayloadSize_t)sizeOfPayload, payload);

    //Destruct Data //BK: This data does not need to be distructed
//    for(int i = 0; i < sizeOfPayload; ++i){
//    	uint8_t* msg_ptr = static_cast<uint8_t*>(payload + i);
//    	delete msg_ptr;
//    }
}

void PatternShim::AddDestinationRequest(PatternId_t patternId, FMessageId_t  msgId, NodeId_t destArray[MAX_DEST], uint16_t noOfNbrs)
{
  int calltype = PTN2FW_Call_MethodE::PTN2FW_Call_AddDest;
	SendMsg2Server<int, PatternId_t, FMessageId_t , GenericMsgPayloadSize_t, void* >
	(calltype, patternId, msgId, ((GenericMsgPayloadSize_t)noOfNbrs) * sizeof(NodeId_t), (void*)destArray);
}

void PatternShim::AddDestinationRequest(PatternId_t patternId, FMessageId_t  msgId, NodeId_t destArray[MAX_DEST], uint16_t noOfNbrs, LinkComparatorTypeE lcType)
{
    int calltype = PTN2FW_Call_MethodE::PTN2FW_Call_AddDestwComp;
	SendMsg2Server<int, PatternId_t, FMessageId_t , GenericMsgPayloadSize_t, void*, LinkComparatorTypeE >
	(calltype, patternId, msgId, ((GenericMsgPayloadSize_t)noOfNbrs) * sizeof(NodeId_t), (void*)destArray, lcType);
}

void PatternShim::CancelDataRequest(PatternId_t patternId, FMessageId_t  msgId, NodeId_t destArray[MAX_DEST], uint16_t noOfDest)
{
    int calltype = PTN2FW_Call_MethodE::PTN2FW_Call_Cancel;
	SendMsg2Server<int, PatternId_t, FMessageId_t , GenericMsgPayloadSize_t, void* >
	(calltype, patternId, msgId, ((GenericMsgPayloadSize_t)noOfDest) * sizeof(NodeId_t), (void*)destArray);
}

void PatternShim::DataStatusRequest(PatternId_t patternId, FMessageId_t  msgId)
{
    int calltype = PTN2FW_Call_MethodE::PTN2FW_Call_DataStatus;
	SendMsg2Server<int, PatternId_t, FMessageId_t  >
	(calltype, patternId, msgId);
}

//void PatternShim::RegisterPatternRequest(PatternId_t patternId, PatternTypeE type)
//{
//    int calltype = PTN2FW_Call_MethodE::PTN2FW_Call_RegisterPattern;
//	GenericSerializer<int, PatternId_t, PatternTypeE >
//    gs(calltype, patternId, type);
//
//    Generic_VarSized_Msg* ptn2fw_msg = gs.Get_Generic_VarSized_Msg_Ptr();
//    this->SendMsg2Server(  ptn2fw_msg->GetPayload(), ptn2fw_msg->GetPayloadSize() );
//}

void PatternShim::RegisterPatternRequest(PatternId_t patternId, const char uniqueName[128], PatternTypeE type){
    Connect(0);

    int calltype = PTN2FW_Call_MethodE::PTN2FW_Call_RegisterPatternRequest;
    SendMsg2Server<int, PatternId_t, GenericMsgPayloadSize_t, const char*, PatternTypeE >
	(calltype, patternId, 128*sizeof(char), (char*)uniqueName, type);

}

void PatternShim::FrameworkAttributesRequest(PatternId_t patternId)
{
    int calltype = PTN2FW_Call_MethodE::PTN2FW_Call_FrameworkAttributes;
	SendMsg2Server<int, PatternId_t >
	(calltype, patternId);
}

void PatternShim::SelectDataNotificationRequest(PatternId_t patternId, uint8_t notifierMask)
{
    int calltype = PTN2FW_Call_MethodE::PTN2FW_Call_SelectDataNotification;
	SendMsg2Server<int, PatternId_t, uint8_t >
	(calltype, patternId, notifierMask);
}

void PatternShim::SelectLinkComparatorRequest(PatternId_t patternId, LinkComparatorTypeE lcType)
{
    int calltype = PTN2FW_Call_MethodE::PTN2FW_Call_SelectLinkComparator;
	SendMsg2Server<int, PatternId_t, LinkComparatorTypeE >
	(calltype, patternId, lcType);
}

void PatternShim::SetLinkThresholdRequest(PatternId_t patternId, LinkMetrics threshold)
{
    int calltype = PTN2FW_Call_MethodE::PTN2FW_Call_SetLinkThreshold;
	SendMsg2Server<int, PatternId_t, LinkMetrics >
	(calltype, patternId, threshold);
}

void PatternShim::NewPatternInstanceRequest(PatternTypeE type, char uniqueName[128])
{
    int calltype = PTN2FW_Call_MethodE::PTN2FW_Call_NewPatternInstance;
	SendMsg2Server<int, PatternTypeE, GenericMsgPayloadSize_t, void* >
	(calltype, type, 128*sizeof(char), (void*)uniqueName);
}



Framework_I& GetPatternShim(){
 Framework_I *pS = static_cast<Framework_I *> (new PatternShim());
 return *pS;
}

} //End of namespace
