////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 


#include <Base/AppTypes.h>
#include <Interfaces/AWI/Fwp_I.h>
#include <Lib/DS/BSTMapT.h>

#ifndef PLATFORM_DCE_PATTERN_FWP2APPSHIM_H_
#define PLATFORM_DCE_PATTERN_FWP2APPSHIM_H_

namespace AWI {


typedef BSTMapT<AppId_t, AppRecvMessageDelegate_t*> AppRecvMessageDelegatesMap_t;

class FWP2AppShim : public FWP2AppShim_I {
	//AppRecvMessageDelegatesMap_t m_recvDelegatesMap;
	AppRecvMessageDelegate_t* m_recvDelegate;
public:
	FWP2AppShim();
	~FWP2AppShim();
	void ReceiveMessage(void *data, uint16_t size);
	void RegisterAppReceiveDelegate(AppId_t _app_id, AppRecvMessageDelegate_t* _gvu_del);
};

} /* namespace AWI */

#endif /* PLATFORM_DCE_PATTERN_FWP2APPSHIM_H_ */
