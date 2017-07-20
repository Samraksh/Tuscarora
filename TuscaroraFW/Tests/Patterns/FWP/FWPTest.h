////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef TESTS_PATTERNS_FWP_FWPTEST_H_
#define TESTS_PATTERNS_FWP_FWPTEST_H_

#include <Sys/RuntimeOpts.h>
#include "Lib/PAL/PAL_Lib.h"
#include "Patterns/FloodingWithPruning/FWP.h"

#if  FWP_LOWER_SHIM==SOCKET_SHIM
#include <Platform/Shims/LinuxSocket/Pattern/FWP2AppShim.h>
#endif


extern NodeId_t MY_NODE_ID;

using namespace Patterns;
using namespace PAL;

class FWPTest {
	uint32_t fwp_variable;
	AppId_t appid;
	FWP2AppShim_I* pattern2appshimptr;
	FWP *fwp;
	AppRecvMessageDelegate_t* recvUpdateVarDelegate;

	TimerDelegate *timerDel;
	TimerDelegate *updatevarDel;
	Timer *startTestTimer;
	Timer *updateVariableTimer;

	uint16_t *data;
public:
	FWPTest();
	void TimerHandler(uint32_t event);
	void IncrementFwpVariable(uint32_t event);
	void ReceiveFwpVariableUpdate(void *data, uint16_t size);
	void Execute(RuntimeOpts *opts);
	virtual ~FWPTest();
};


#endif /* TESTS_PATTERNS_FWP_GOSSIPTEST_H_ */
