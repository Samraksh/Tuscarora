////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 


#include <Base/AppTypes.h>
#include <Interfaces/AWI/Fwp_I.h>
#include <Lib/DS/BSTMapT.h>
#include "Platform/Shims/LinuxSocket/SocketCommunicatorServerBase.h"

#ifndef PLATFORM_LINUX_PATTERN_FWP2APPSHIM_H_
#define PLATFORM_LINUX_PATTERN_FWP2APPSHIM_H_

using namespace Lib;
using namespace Shims;

namespace AWI {



typedef BSTMapT<AppId_t, AppRecvMessageDelegate_t*> AppRecvMessageDelegatesMap_t;

class FWP2AppShim : public FWP2AppShim_I, SocketCommunicatorServerBase<AppId_t> {
//	Fwp_I *fi;
//	AppRecvMessageDelegatesMap_t m_recvDelegatesMap;
//	AppRecvMessageDelegate_t* m_recvDelegate;
	bool Deserialize (int32_t sockfd) ;
public:

	FWP2AppShim();
	~FWP2AppShim();
	void ReceiveMessage(void *data, uint16_t size);
};

} /* namespace AWI */

#endif /* PLATFORM_DCE_PATTERN_FWP2APPSHIM_H_ */
