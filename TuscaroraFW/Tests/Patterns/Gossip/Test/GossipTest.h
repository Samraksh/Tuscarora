////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef TESTS_PATTERNS_GOSSIP_GOSSIPTEST_H_
#define TESTS_PATTERNS_GOSSIP_GOSSIPTEST_H_

#include "Lib/PAL/PAL_Lib.h"

#include "Patterns/Gossip/Gossip.h"
#include <Sys/RuntimeOpts.h>

extern NodeId_t MY_NODE_ID;

using namespace Patterns;
using namespace PAL;

class GossipTest {
	uint32_t gossip_variable;
	Gossip<uint32_t> *gossip;
	Gossip<uint32_t>::GossipVariableUpdateDelegate_t* recvUpdateVarDelegate;

	TimerDelegate *timerDel;
	TimerDelegate *updatevarDel;
	Timer *startTestTimer;
	Timer *updateVariableTimer;
	uint16_t *data;
public:
	GossipTest();
	void TimerHandler(uint32_t event);
	void IncrementGossipVariable(uint32_t event);
	void ReceiveGossipVariableUpdate(uint32_t& updated_gossip_variable);
	void Execute(RuntimeOpts *opts);
	virtual ~GossipTest();
};


#endif /* TESTS_PATTERNS_GOSSIP_GOSSIPTEST_H_ */
