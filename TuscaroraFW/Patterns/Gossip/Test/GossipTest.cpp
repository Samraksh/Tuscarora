////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#include "GossipTest.h"
//#include <Platform/dce/Pattern/Gossip2AppShim.h>



GossipTest::GossipTest(){

  //PatternId_t pid = GetNewPatternInstanceId(GOSSIP_PTN, (char*)"Gossip_1");
  //AppId_t appid = 0;
  gossip_variable = 0;
  if(MY_NODE_ID == 0){
	  gossip_variable = 5;
  }


  recvUpdateVarDelegate = new (Gossip<uint32_t>::GossipVariableUpdateDelegate_t)(this, &GossipTest::ReceiveGossipVariableUpdate);
  //Gossip2AppShim_I<uint32_t> *pattern2appshimptr;
  //Gossip2AppShim<uint32_t>* pattern2appshimptr = new Gossip2AppShim<uint32_t> ();
  //pattern2appshimptr-> RegisterGossipVariableUpdateDelegate (recvUpdateVarDelegate);
  gossip = new Gossip<uint32_t>(gossip_variable);
  gossip->RegisterGossipVariableUpdateDelegate(recvUpdateVarDelegate);

  timerDel = new TimerDelegate (this, &GossipTest::TimerHandler);
  startTestTimer = new Timer(2000000, ONE_SHOT, *timerDel);

  if(MY_NODE_ID == 0){
	  updatevarDel = new TimerDelegate (this, &GossipTest::IncrementGossipVariable);
	  updateVariableTimer = new Timer(2000000, PERIODIC, *updatevarDel);
  }
}

void GossipTest::TimerHandler(uint32_t event){
 Debug_Printf(DBG_PATTERN, "GossipTEST: Starting the Gossip Pattern...\n");
  gossip->Start();
  if(MY_NODE_ID == 0){
	  updateVariableTimer->Start();
  }
}

void GossipTest::IncrementGossipVariable(uint32_t event){
 Debug_Printf(DBG_PATTERN, "GossipTEST: Manually incrementing gossip variable! Previous Value =  %d...\n", gossip_variable);
  gossip->UpdateGossipVariable(++gossip_variable);
}

void GossipTest::ReceiveGossipVariableUpdate(uint32_t& updated_gossip_variable){
 Debug_Printf(DBG_PATTERN, "GossipTEST: ReceiveGossipVariableUpdate gossip_variable = %d, updated_gossip_variable = %d ...\n", gossip_variable, updated_gossip_variable);
 	 gossip_variable = updated_gossip_variable;
}

void GossipTest::Execute(RuntimeOpts *opts){
  //Delay starting the actual pattern, to let the network stabilize
  //So start the timer now and when timer expires start the pattern
 Debug_Printf(DBG_PATTERN,"About to start timer on node 1\n");
  startTestTimer->Start();
}

GossipTest::~GossipTest() {
	// TODO Auto-generated destructor stub
}

