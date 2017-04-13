////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef FWP_I_H_
#define FWP_I_H_

#include <Base/AppTypes.h>
#include <Interfaces/Core/MessageT.h>


namespace AWI {

typedef Delegate<void, void* , uint16_t> AppRecvMessageDelegate_t;

class Fwp_I {
public:
//	virtual void SendData (AppId_t _app_id, AppMessage_t& msg, uint16_t nonce) = 0;
	virtual void Send (void *data, uint16_t size) = 0;
	virtual void RegisterAppReceiveDelegate(AppId_t _app_id, AppRecvMessageDelegate_t* _gvu_del) = 0;
  ///Virtual destructor
	virtual ~Fwp_I (){}
};

class FWP2AppShim_I {
public:
  virtual void ReceiveMessage(void *data, uint16_t size) = 0;
  virtual void RegisterAppReceiveDelegate(AppId_t _app_id, AppRecvMessageDelegate_t* _gvu_del){}; //This registration interface is reimplemented and used in some shims and not used in others.
};

///Returns a reference to an implentation of the pattern interface.
Fwp_I& GetFwpInterface();
Fwp_I* GetFwpInterfacePtr();
Fwp_I& GetFwpShim();




} //End of namespace

#endif /* FWP_I_H_ */
