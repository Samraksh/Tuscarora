////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef PLATFORM_LINUX_APP2FWP_SHIM_H_
#define PLATFORM_LINUX_APP2FWP_SHIM_H_

#include <Interfaces/AWI/Fwp_I.h>
#include "../SocketCommunicatorClientBase.h"

using namespace AWI;
using namespace Lib;
using namespace Shims;

namespace Apps {
  class App2FWPShim : public Fwp_I, SocketCommunicatorClientBase {
  
    //Fwp_I *fi;
    
    AppId_t m_appid;

    AppRecvMessageDelegate_t* m_recvDelegate;

    bool Deserialize(); // Inherited form SocketCommunicatorClientBase


  public:
    
    App2FWPShim();
//    bool ConnectToFwp(int32_t commId);
    void RegisterAppReceiveDelegate(AppId_t _app_id, AppRecvMessageDelegate_t* _recvDelegate);


    //Inherited from Fwp_I
    void Send(void *data, uint16_t size);


    //Inherited from SocketCommunicatorBase


    ~App2FWPShim(){};
  };

  
}//End of namespace

namespace AWI{
Fwp_I& GetApp2FWPShim();
}


#endif //APP2FWP_SHIM_H_
