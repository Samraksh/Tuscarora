////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef APPS_FWP_FANOUTFWP_H_
#define APPS_FWP_FANOUTFWP_H_

#include "Lib/PAL/PAL_Lib.h"
#include "Patterns/FloodingWithPruning/FWP.h"
#include <Sys/RuntimeOpts.h>

#include <Apps/AppBase.h>

extern NodeId_t MY_NODE_ID;

using namespace Patterns;
using namespace PAL;


namespace Apps {

class FanOut_FWP : public AppBase {
	Fwp_I* FWP;
	AppId_t appid;
	AppRecvMessageDelegate_t* recvDelegate;
	void Receive(void *data, uint16_t size );
	void Send(void *data, uint16_t size);

	TimerDelegate *timerDel;
	Timer *startTestTimer;

	uint16_t *data;

public:
	FanOut_FWP();
	virtual ~FanOut_FWP();
	void TimerHandler(uint32_t event);


	//Inherited from AppBase
	void StartApp();
};

} /* namespace Apps */

#endif /* APPS_FWP_FANOUTFWP_H_ */
