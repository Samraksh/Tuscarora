////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef PLATFORM_LINUX_MP_APP2Gossip_SHIM_H_
#define PLATFORM_LINUX_MP_APP2Gossip_SHIM_H_

#include <Interfaces/AWI/Gossip_I.h>
#include <Lib/Shims/Sockets/SocketCommunicatorClientBase.h>
#include <Include/Sys/SocketShim.h>

#include <Platform/linux/App/AppGossipShimMessages.h>


using namespace AWI;
using namespace Lib;
using namespace Shims;

namespace Apps {

template<typename GOSSIPVARIABLE, typename GOSSIPCOMPARATOR = SimpleGenericComparator<GOSSIPVARIABLE> >
class App2GossipShim : public Gossip_I<GOSSIPVARIABLE, GOSSIPCOMPARATOR>, SocketCommunicatorClientBase {

   // Gossip_I<GOSSIPVARIABLE, GOSSIPCOMPARATOR> *fi;

    AppId_t m_appid;
    typedef typename Gossip_I<GOSSIPVARIABLE, GOSSIPCOMPARATOR>::GossipVariableUpdateDelegate_t GossipVariableUpdateDelegate_t_l;

    GossipVariableUpdateDelegate_t_l* m_recvDelegate;

//    bool Deserialize(const GenericSocketMessages& fw2ptnmsg, int32_t sockfd) const;
    bool Deserialize(); // Inherited form SocketCommunicatorClientBase

public:

    App2GossipShim();

    //Inherited from Gossip_I
	void UpdateGossipVariable(GOSSIPVARIABLE& initialGossipVariable);
	void RegisterGossipVariableUpdateDelegate(GossipVariableUpdateDelegate_t_l* _gvu_del);

    ~App2GossipShim(){};
  };


template<typename GOSSIPVARIABLE, typename GOSSIPCOMPARATOR >
App2GossipShim<GOSSIPVARIABLE,GOSSIPCOMPARATOR>::App2GossipShim(): SocketCommunicatorClientBase(GOSSIP_HOST, GOSSIP_APP_PORT, GOSSIP_APP_PORT_TYPE ){
//  fi = &(AWI::GetGossipInterface<GOSSIPVARIABLE>());

}

template<typename GOSSIPVARIABLE, typename GOSSIPCOMPARATOR  >
void App2GossipShim<GOSSIPVARIABLE,GOSSIPCOMPARATOR>::RegisterGossipVariableUpdateDelegate(GossipVariableUpdateDelegate_t_l* _gvu_del){
//	fi -> RegisterGossipVariableUpdateDelegate(_gvu_del);
	m_recvDelegate = _gvu_del;
	Connect(0);



	int calltype = APP2GOSSIP_Call_MethodE::APP2GOSSIP_Call_RegisterApp;
	Debug_Printf(DBG_SHIM, "App2GOSSIPShim::RegisterGossipVariableUpdateDelegate msg is getting serialized\n");
	GenericSerializer<int, AppId_t>
				gs(calltype, m_appid);
	Generic_VarSized_Msg* ptn2fw_msg = gs.Get_Generic_VarSized_Msg_Ptr();

	Debug_Printf(DBG_SHIM, "App2GOSSIPShim::RegisterGossipVariableUpdateDelegate Entire message is serialized. Sending to FW\n");
	SendMsg2Server(  ptn2fw_msg->GetPayload(), ptn2fw_msg->GetPayloadSize() );

}

template<typename GOSSIPVARIABLE, typename GOSSIPCOMPARATOR>
void App2GossipShim<GOSSIPVARIABLE,GOSSIPCOMPARATOR>::UpdateGossipVariable(GOSSIPVARIABLE& initialGossipVariable){
	Debug_Printf(DBG_SHIM, "App2GOSSIPShim::SendData \n");

	int calltype = APP2GOSSIP_Call_MethodE::APP2GOSSIP_Call_Send;
	Debug_Printf(DBG_SHIM, "App2GOSSIPShim::SendData is getting serialized\n");
	GenericSerializer<int, AppId_t, GOSSIPVARIABLE >
				gs(calltype, m_appid, initialGossipVariable);
	Generic_VarSized_Msg* ptn2fw_msg = gs.Get_Generic_VarSized_Msg_Ptr();

	Debug_Printf(DBG_SHIM, "App2GOSSIPShim::SendData Entire message is serialized. Sending to FW\n");
	SendMsg2Server(  ptn2fw_msg->GetPayload(), ptn2fw_msg->GetPayloadSize() );


	//Destruct Data
//	AppMessage_t* msg_ptr = &msg;
//	delete msg_ptr;
}

//template<typename GOSSIPVARIABLE, typename GOSSIPCOMPARATOR >
//bool App2GossipShim<GOSSIPVARIABLE,GOSSIPCOMPARATOR>::Deserialize(const GenericSocketMessages& fw2ptnmsg, int32_t sockfd) const {
//	Debug_Printf(DBG_SHIM, "App2GossipShim::Deserialize ... ReadBytes / TotalBytes = %lu / %lu \n", fw2ptnmsg.GetBytesRead(), fw2ptnmsg.GetPayloadSize() );
//    int calltype = 0;
//    GenericDeSerializer<int> (&fw2ptnmsg, calltype);
//    Debug_Printf(DBG_SHIM, "App2GossipShim::Deserialize: msg.GetType() = %d \n", calltype);
//
//    if(calltype == GOSSIP2APP_Event_RcvMsg){
//    	GOSSIPVARIABLE gosVariable;
//        GenericDeSerializer< GOSSIPVARIABLE >
//                  gds(&fw2ptnmsg, gosVariable);
//
//        m_recvDelegate->operator()(gosVariable);
//    }
//    return true;
//}

template<typename GOSSIPVARIABLE, typename GOSSIPCOMPARATOR >
bool App2GossipShim<GOSSIPVARIABLE,GOSSIPCOMPARATOR>::Deserialize( ){
	Debug_Printf(DBG_SHIM, "App2GossipShim::Deserialize ...  \n");
    int calltype = 0;

    Peek<int>(calltype);

    Debug_Printf(DBG_SHIM, "App2GossipShim::Deserialize: msg.GetType() = %d \n", calltype);

    if(calltype == GOSSIP2APP_Event_RcvMsg){
    	GOSSIPVARIABLE gosVariable;
        Read< GOSSIPVARIABLE >(gosVariable);
        m_recvDelegate->operator()(gosVariable);
    }
    return true;
}


//template<typename GOSSIPVARIABLE, typename GOSSIPCOMPARATOR = SimpleGenericComparator<GOSSIPVARIABLE> >
//typedef Gossip_I<GOSSIPVARIABLE, GOSSIPCOMPARATOR> App2GossipShim;


template<typename GOSSIPVARIABLE, typename GOSSIPCOMPARATOR >
Gossip_I<GOSSIPVARIABLE, GOSSIPCOMPARATOR>* GetApp2GossipShimPtr(){



	 Gossip_I<GOSSIPVARIABLE, GOSSIPCOMPARATOR> *pS = static_cast<Gossip_I<GOSSIPVARIABLE, GOSSIPCOMPARATOR> *> (new App2GossipShim<GOSSIPVARIABLE>());
	 return pS;
}

}//End of namespace Apps



#endif //APP2Gossip_SHIM_H_
