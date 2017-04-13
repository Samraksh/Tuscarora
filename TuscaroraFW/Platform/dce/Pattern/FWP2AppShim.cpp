////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#include "FWP2AppShim.h"

namespace AWI {

FWP2AppShim::FWP2AppShim() {
	// TODO Auto-generated constructor stub

}

FWP2AppShim::~FWP2AppShim() {
	// TODO Auto-generated destructor stub
}
void FWP2AppShim::ReceiveMessage(void *data, uint16_t size){
//	AppRecvMessageDelegatesMap_t::Iterator it = m_recvDelegatesMap.Find(msg.GetAppId());
//	if(it != m_recvDelegatesMap.End()){
//		(*it)->operator()(msg);
//	}
	if(m_recvDelegate !=NULL ){
		Debug_Printf(DBG_PATTERN, "FWP2AppShim::ReceiveMessage Invo\n");
		m_recvDelegate->operator()(data, size);
	}
	else{
		Debug_Printf(DBG_PATTERN, "FWP2AppShim::ReceiveMessage AppDelegate not available \n");
	}

}

void FWP2AppShim::RegisterAppReceiveDelegate(AppId_t _app_id, AppRecvMessageDelegate_t* _gvu_del){
	Debug_Printf(DBG_PATTERN, "FWP2AppShim:: Storing delegates \n");
	//m_recvDelegatesMap.Insert(appId, _gvu_del);
	m_recvDelegate = _gvu_del;
}

} /* namespace AWI */
