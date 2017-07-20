////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include "App2FWPShim.h"
#include "AppFWPShimMessages.h"

#include <Include/Sys/SocketShim.h>

using namespace Lib;
using namespace Shims;
namespace Apps {


App2FWPShim::App2FWPShim() : SocketCommunicatorClientBase(FWP_HOST, FWP_APP_PORT, FWP_APP_PORT_TYPE ){

}

void App2FWPShim::RegisterAppReceiveDelegate(AppId_t _app_id, AppRecvMessageDelegate_t* _recvDelegate){

	Debug_Printf(DBG_SHIM, "PatternShim::RegisterDelegates \n");
	m_appid = _app_id;
	m_recvDelegate = _recvDelegate;

	Connect(0);
	int calltype = APP2FWP_Call_MethodE::APP2FWP_Call_RegisterApp;

	SendMsg2Server<int, AppId_t >(calltype, m_appid);
}


void App2FWPShim::Send(void *data, uint16_t size){
	Debug_Printf(DBG_SHIM, "App2FWPShim::SendData \n");

	int calltype = APP2FWP_Call_MethodE::APP2FWP_Call_Send;
	SendMsg2Server<int, uint16_t, void* >(calltype, size, data);

	char* next_ptr = NULL;
	for(uint16_t i = 0; i < size; ++i){
		if(size - i > 1){
			next_ptr = (char*)data;
			++next_ptr;
		}
		free(data);
		data = (void*)next_ptr;
	}
}

bool App2FWPShim::Deserialize() {
	Debug_Printf(DBG_SHIM, "App2FWPShim::Deserialize ...  \n");
    int calltype = 0;

    Peek<int>(calltype);

    Debug_Printf(DBG_SHIM, "App2FWPShim::Deserialize: msg.GetType() = %d \n", calltype);

    if(calltype == FWP2APP_Event_RcvMsg){
        GenericMsgPayloadSize_t totalsize;
        void* payload = NULL;
        Read<int,   GenericMsgPayloadSize_t, void* >(calltype, totalsize,  payload);
        m_recvDelegate->operator()(payload, totalsize);
    }
    return true;
}



} //End of namespace

namespace AWI {
	Fwp_I& GetApp2FWPShim(){
	 Fwp_I *pS = static_cast<Fwp_I *> (new Apps::App2FWPShim());
	 return *pS;
	}
}

