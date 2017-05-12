////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 


#include <Base/AppTypes.h>
#include <Lib/Shims/Sockets/SocketCommunicatorServerBase.h>
#include <Interfaces/AWI/Gossip_I.h>
#include <Patterns/Gossip/Gossip.h>
#include <Lib/DS/BSTMapT.h>

#include <Platform/linux/App/AppGossipShimMessages.h>
#include <Include/Sys/SocketShim.h>

#ifndef PLATFORM_LINUX_MP_PATTERN_Gossip2APPSHIM_H_
#define PLATFORM_LINUX_MP_PATTERN_Gossip2APPSHIM_H_

using namespace Lib;
using namespace Shims;

namespace AWI {



template<typename GOSSIPVARIABLE, typename GOSSIPCOMPARATOR>
class Gossip2AppShim : public Gossip2AppShim_I<GOSSIPVARIABLE>,  SocketCommunicatorServerBase<AppId_t> {
	Gossip_I<GOSSIPVARIABLE, GOSSIPCOMPARATOR> *fi;

	bool Deserialize (int32_t sockfd);
public:
	Gossip2AppShim(Gossip_I<GOSSIPVARIABLE, GOSSIPCOMPARATOR> *_fi);
	~Gossip2AppShim();
	void ReceiveUpdatedGossipVariable(GOSSIPVARIABLE& msg);
	//void RegisterGossipVariableUpdateDelegate(AppId_t _app_id, typename Gossip_I<GOSSIPVARIABLE, GOSSIPCOMPARATOR>::GossipVariableUpdateDelegate_t* _gvu_del);

};

template<typename GOSSIPVARIABLE, typename GOSSIPCOMPARATOR>
Gossip2AppShim<GOSSIPVARIABLE, GOSSIPCOMPARATOR>::Gossip2AppShim(Gossip_I<GOSSIPVARIABLE, GOSSIPCOMPARATOR> *_fi) : SocketCommunicatorServerBase<AppId_t>(GOSSIP_HOST, GOSSIP_APP_PORT, GOSSIP_APP_PORT_TYPE, true) {
	// TODO Auto-generated constructor stub
    Debug_Printf(DBG_SHIM, "Gossip2AppShim::Gossip2AppShim Initializing \n");
    //fi =  &GetGossipInterface<GOSSIPVARIABLE>();
    fi = _fi;
	Debug_Printf(DBG_SHIM, "Gossip2AppShim::Gossip2AppShim Initializing \n");

}
template<typename GOSSIPVARIABLE, typename GOSSIPCOMPARATOR>
Gossip2AppShim<GOSSIPVARIABLE, GOSSIPCOMPARATOR>::~Gossip2AppShim() {
	// TODO Auto-generated destructor stub
}
template<typename GOSSIPVARIABLE, typename GOSSIPCOMPARATOR>
void Gossip2AppShim<GOSSIPVARIABLE, GOSSIPCOMPARATOR>::ReceiveUpdatedGossipVariable(GOSSIPVARIABLE& msg){
	int calltype = GOSSIP2APP_Event_RcvMsg;

	for(SocketMap_t::Iterator it = ptnToSockIdMap.Begin(); it != ptnToSockIdMap.End(); ++it){
		SendMsg2Client<int,  GOSSIPVARIABLE >(it->First(), calltype, msg );
	}

}

//template<typename GOSSIPVARIABLE, typename GOSSIPCOMPARATOR>
//void Gossip2AppShim<GOSSIPVARIABLE, GOSSIPCOMPARATOR>::RegisterGossipVariableUpdateDelegate(AppId_t _app_id, typename Gossip_I<GOSSIPVARIABLE, GOSSIPCOMPARATOR>::GossipVariableUpdateDelegate_t* _gvu_del) {
//	Debug_Printf(DBG_PATTERN, "Gossip2AppShim:: Received UNEXPECTED Request to register variable. Is there an app in the same process with the pattern? \n");
//	//m_recvDelegatesMap.Insert(appId, _gvu_del);
//	//m_recvDelegate = _gvu_del;
//}

template<typename GOSSIPVARIABLE, typename GOSSIPCOMPARATOR>
bool Gossip2AppShim<GOSSIPVARIABLE, GOSSIPCOMPARATOR>::Deserialize (int32_t sockfd) {
	Debug_Printf(DBG_SHIM, "Gossip2AppShim::Deserialize ... sockfd = %d \n", sockfd);

	int calltype = 0;
    bool rv = Peek<int>(sockfd, calltype);
    if(!rv){
    	Debug_Printf(DBG_SHIM, "Gossip2AppShim::Deserialize No data to deserialize ... sockfd = %d \n", sockfd);
    	return false;
    }

	Debug_Printf(DBG_SHIM, "Gossip2AppShim::Deserialize: Got a message msg.GetType() = %d \n", calltype);

	if (calltype == APP2GOSSIP_Call_Send){
		AppId_t pid = 0;
//		uint16_t nonce;
		GOSSIPVARIABLE gossipvar;

		rv = Read<int,   AppId_t, GOSSIPVARIABLE>( calltype, pid, gossipvar);

		fi->UpdateGossipVariable(gossipvar);
//		free(serial_msg); Consumed by AppMessage_t
	}
	else if(calltype == APP2GOSSIP_Call_RegisterApp){
		AppId_t pid = 0;

		rv = Read<int,   AppId_t>(calltype, pid);

        client_init_ptr->InitClient(pid, sockfd);

        //GossipVariableUpdateDelegate_t_l* recvUpdateVarDelegate = new GossipVariableUpdateDelegate_t_l(this, &Gossip2AppShim<GOSSIPVARIABLE, GOSSIPCOMPARATOR>::ReceiveUpdatedGossipVariable);
        //fi->RegisterGossipVariableUpdateDelegate(pid, recvUpdateVarDelegate); //Register's shims method as the app receive delegate. The receive method is responsible to carry the mesage over.
	}
	else{
		Debug_Printf(DBG_SHIM, "GOSSIP2AppShim::Deserialize: Unknown message \n");
	}
	Debug_Printf(DBG_SHIM, "GOSSIP2AppShim::Deserialize:EXITING \n");
	return true;

}

} /* namespace AWI */


#endif /* PLATFORM_DCE_PATTERN_Gossip2APPSHIM_H_ */
