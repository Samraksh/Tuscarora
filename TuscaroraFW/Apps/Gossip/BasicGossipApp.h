////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef APPS_GOSSIP_BASICGOSSIPAPP_H_
#define APPS_GOSSIP_BASICGOSSIPAPP_H_

#ifdef PLATFORM_DCE
#include "Platform/dce/App/App2GossipShim.h"
#else
#include "Platform/linux/App/App2GossipShim.h"
#endif

//#include "Platform/dce/App/App2GossipShim.h"

namespace Apps {

template<typename GOSSIPVARIABLE, typename GOSSIPCOMPARATOR = SimpleGenericComparator<GOSSIPVARIABLE> >
class BasicGossipApp {
	Gossip_I<GOSSIPVARIABLE, GOSSIPCOMPARATOR>* gossip;

	typedef typename Gossip_I<GOSSIPVARIABLE, GOSSIPCOMPARATOR>::GossipVariableUpdateDelegate_t GossipVariableUpdateDelegate_t_l;

	GossipVariableUpdateDelegate_t_l* recvUpdateVarDelegate;

	GOSSIPVARIABLE gossip_variable;
	TimerDelegate *timerDel;
	TimerDelegate *updatevarDel;
	Timer *startTestTimer;
	Timer *updateVariableTimer;


public:
	BasicGossipApp(){
		gossip_variable = 0;

		// TODO Auto-generated constructor stub
		timerDel = new TimerDelegate (this, &BasicGossipApp<GOSSIPVARIABLE,GOSSIPCOMPARATOR>::InitiateApp);
		startTestTimer = new Timer(2000000, ONE_SHOT, *timerDel);

		if(MY_NODE_ID == 0){
			updatevarDel = new TimerDelegate (this, &BasicGossipApp<GOSSIPVARIABLE,GOSSIPCOMPARATOR>::IncrementGossipVariable);
			updateVariableTimer = new Timer(2000000, PERIODIC, *updatevarDel);
		}
		gossip = GetApp2GossipShimPtr<GOSSIPVARIABLE, GOSSIPCOMPARATOR>();

		recvUpdateVarDelegate = new (GossipVariableUpdateDelegate_t_l)(this, &BasicGossipApp<GOSSIPVARIABLE,GOSSIPCOMPARATOR>::ReceiveGossipVariableUpdate);
	};

	virtual ~BasicGossipApp(){
		delete recvUpdateVarDelegate;
	}

	void InitiateApp(uint32_t event){
		Debug_Printf(DBG_PATTERN, "GossipTEST: Starting the Gossip Pattern...\n");
		gossip->RegisterGossipVariableUpdateDelegate(recvUpdateVarDelegate);
		if(MY_NODE_ID == 0){
			updateVariableTimer->Start();
		}
	};
	void IncrementGossipVariable(uint32_t event){
		Debug_Printf(DBG_PATTERN, "GossipTEST: Manually incrementing gossip variable! Previous Value =  %d...\n", gossip_variable);
		gossip->UpdateGossipVariable(++gossip_variable);
	};
	void ReceiveGossipVariableUpdate(GOSSIPVARIABLE& updated_gossip_variable){
		Debug_Printf(DBG_PATTERN, "GossipTEST: ReceiveGossipVariableUpdate gossip_variable = %d, updated_gossip_variable = %d ...\n", gossip_variable, updated_gossip_variable);
		gossip_variable = updated_gossip_variable;
	}
	void Execute(RuntimeOpts *opts){
		//Delay starting the actual application, to let the network stabilize
		//So start the timer now and when timer expires start the pattern
		Debug_Printf(DBG_PATTERN,"About to start timer on node 1\n");
		startTestTimer->Start();
	};
};

} /* namespace PAL */

#endif /* APPS_GOSSIP_BASICGOSSIPAPP_H_ */
