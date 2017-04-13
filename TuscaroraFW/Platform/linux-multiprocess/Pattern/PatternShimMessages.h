////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef PTN_SHIM_MESSAGES_H
#define PTN_SHIM_MESSAGES_H

//#include <Interfaces/PWI/FrameworkResponces.h>
//#include <Interfaces/PWI/PatternEventDispatch_I.h>
#include <Lib/DS/GenericMessages.h>
#include <Base/FrameworkTypes.h>


#define MAX_PAYLOAD 2048
#define GREETINGMSGSIZE 22

enum PTN2FW_Call_MethodE {
	PTN2FW_Call_Greet = 1,
	PTN2F2_Call_RegisterDelegates,
	PTN2FW_Call_Send,
	PTN2FW_Call_SendwComp,
	PTN2FW_Call_Broadcast,
    PTN2FW_Call_ReplacePayload,
	PTN2FW_Call_AddDest,
	PTN2FW_Call_AddDestwComp,
	PTN2FW_Call_Cancel,
	PTN2FW_Call_DataStatus,
	PTN2FW_Call_RegisterPattern,
	PTN2FW_Call_RegisterPatternRequest,
	PTN2FW_Call_FrameworkAttributes,
	PTN2FW_Call_SelectDataNotification,
	PTN2FW_Call_SelectLinkComparator,
	PTN2FW_Call_SetLinkThreshold,
	PTN2FW_Call_NewPatternInstance
};

enum FW2PTN_Event_MethodE {
    FW2PTN_Event_Greet = 1,
    FW2PTN_Event_Control,
    FW2PTN_Event_RcvMsg,
    FW2PTN_Event_DataStasus,
    FW2PTN_Event_NeighborUpdate
};

class PTN2FWMessages : public Generic_VarSized_Msg {
public:
    PTN2FWMessages(GenericMsgPayloadSize_t  _payload_size, char* _payload)
    :Generic_VarSized_Msg(_payload_size, (uint8_t*)_payload){};
    ~PTN2FWMessages(){}
    int GetType(){
        int calltype;
        memcpy( &calltype, (void*)(this->GetPayload()), sizeof(int));
        return(calltype);
    }

};

class FW2PTNMessages : public Generic_VarSized_Msg {
public:
    FW2PTNMessages(GenericMsgPayloadSize_t  _payload_size, char* _payload)
    :Generic_VarSized_Msg(_payload_size, (uint8_t*)_payload){};
    ~FW2PTNMessages(){};
    int GetType(){
        int calltype;
        memcpy( &calltype, (void*)(this->GetPayload()), sizeof(int));
        return(calltype);
    }

};

//      FROM FRAMEWORK TO THE PATTERN


#endif //PTN_SHIM_MESSAGES_H //PTN_API_CALLS_H
