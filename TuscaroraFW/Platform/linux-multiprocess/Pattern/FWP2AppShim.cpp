////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#include "FWP2AppShim.h"
#include <Platform/linux/App/AppFWPShimMessages.h>
#include <Include/Sys/SocketShim.h>
namespace AWI {


FWP2AppShim::FWP2AppShim() : SocketCommunicatorServerBase<AppId_t>(FWP_HOST, FWP_APP_PORT, FWP_APP_PORT_TYPE, true){
    Debug_Printf(DBG_SHIM, "FWP2AppShim::FWP2AppShim Initializing \n");
    //fi =  &GetFwpInterface();

}

FWP2AppShim::~FWP2AppShim() {
	// TODO Auto-generated destructor stub
}

void FWP2AppShim::ReceiveMessage(void *data, uint16_t size){

	int calltype = FWP2APP_Event_RcvMsg;
	SendMsg2Client<int,  uint16_t, void* >(calltype, size, data);
}

//void FWP2AppShim::RegisterAppReceiveDelegate(AppId_t _app_id, AppRecvMessageDelegate_t* _gvu_del){
//	Debug_Printf(DBG_PATTERN, "FWP2AppShim:: RegisterAppReceiveDelegate MP_SHIM GOT UNEXPECTED RegisterAppReceiveDelegate invocation \n");
//	//m_recvDelegatesMap.Insert(appId, _gvu_del);
//	//m_recvDelegate = _gvu_del;
//}

bool FWP2AppShim::Deserialize (int32_t sockfd) {
	Debug_Printf(DBG_SHIM, "FWP2AppShim::Deserialize ... sockfd = %d \n", sockfd);

	int calltype = 0;
    bool rv = Peek<int>(sockfd, calltype);
    if(!rv){
    	Debug_Printf(DBG_SHIM, "FWP2AppShim::Deserialize No data to deserialize ... sockfd = %d \n", sockfd);
    	return false;
    }

	Debug_Printf(DBG_SHIM, "FWP2AppShim::Deserialize: Got a message msg.GetType() = %d \n", calltype);

	if (calltype == APP2FWP_Call_Send){
		AppId_t pid = 0;


		GenericMsgPayloadSize_t serial_msg_size;
		void* serial_msg = NULL;

		uint16_t nonce;

		rv = Read<int,  AppId_t, GenericMsgPayloadSize_t, void*, uint16_t>(calltype, pid, serial_msg_size, serial_msg, nonce);

	    if(!rv){
	    	Debug_Printf(DBG_SHIM, "FWP2AppShim::Deserialize Failed to Deserialize ... sockfd = %d \n", sockfd);
	    	return false;
	    }


		Fwp_I *fi = GetFwpInterfacePtr();
		if(fi != NULL) fi->Send (serial_msg, serial_msg_size);
//		free(serial_msg); Consumed by AppMessage_t
	}
	else if(calltype == APP2FWP_Call_RegisterApp){
		AppId_t pid = 0;

		rv = Read<int,   AppId_t>(calltype, pid);

	    if(!rv){
	    	Debug_Printf(DBG_SHIM, "FWP2AppShim::Deserialize Failed to Deserialize ... sockfd = %d \n", sockfd);
	    	return false;
	    }

		SocketCommunicatorServerBase::client_init_ptr->InitClient(pid, sockfd);
	}
	else{
		Debug_Printf(DBG_SHIM, "FWP2AppShim::Deserialize: Unknown message \n");
	}



	Debug_Printf(DBG_SHIM, "FWP2AppShim::Deserialize:EXITING \n");
	return true;

}


} /* namespace AWI */
