////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#include "FWPTest.h"




FWPTest::FWPTest(){

  //PatternId_t pid = GetNewPatternInstanceId(GOSSIP_PTN, (char*)"Fwp_1");
  fwp_variable = 0;
  if(MY_NODE_ID == 0){
	  fwp_variable = 5;
  }

  appid = 3;
  recvUpdateVarDelegate = new AppRecvMessageDelegate_t(this, &FWPTest::ReceiveFwpVariableUpdate);
  pattern2appshimptr = new FWP2AppShim ();
  pattern2appshimptr-> RegisterAppReceiveDelegate (appid, recvUpdateVarDelegate);
  fwp = new FWP(pattern2appshimptr);

  timerDel = new TimerDelegate (this, &FWPTest::TimerHandler);
  startTestTimer = new Timer(2000000, ONE_SHOT, *timerDel);

  if(MY_NODE_ID == 0){
	  updatevarDel = new TimerDelegate (this, &FWPTest::IncrementFwpVariable);
	  updateVariableTimer = new Timer(200000, PERIODIC, *updatevarDel);
  }
}

void FWPTest::TimerHandler(uint32_t event){
 Debug_Printf(DBG_PATTERN, "FwpTEST: Starting the Fwp Pattern...\n");
  fwp->Start();
  if(MY_NODE_ID == 0){
	  updateVariableTimer->Start();
  }
}

void FWPTest::IncrementFwpVariable(uint32_t event){
 Debug_Printf(DBG_PATTERN, "FwpTEST: Manually incrementing fwp variable! Previous Value =  %d...\n", fwp_variable);
  ++fwp_variable;
  //AppMessage_t* appMsgPtr = new AppMessage_t(sizeof(uint32_t));
  //appMsgPtr->CopyToPayload(&fwp_variable);
  uint32_t* fwp_var2send = new uint32_t(fwp_variable);
  fwp->Send(fwp_var2send, sizeof(uint32_t));
}

void FWPTest::ReceiveFwpVariableUpdate(void *data, uint16_t size){
 	 uint32_t received_Data = *(static_cast<uint32_t*>(data));
 	 //GenericDeSerializer<uint32_t> gs(&appMsg, received_Data );

 	 Debug_Printf(DBG_PATTERN, "FwpTEST: ReceiveFwpVariableUpdate fwp_variable = %d, updated_fwp_variable = %d ...\n", fwp_variable, received_Data);


 	 fwp_variable = received_Data;

 	 free(data);
}

void FWPTest::Execute(RuntimeOpts *opts){
  //Delay starting the actual pattern, to let the network stabilize
  //So start the timer now and when timer expires start the pattern
 Debug_Printf(DBG_PATTERN,"About to start timer on node 1\n");
  startTestTimer->Start();
}

FWPTest::~FWPTest() {
	// TODO Auto-generated destructor stub
//	delete (static_cast<FWP2AppShim*>(pattern2appshimptr));
//	delete fwp;
//	delete recvUpdateVarDelegate;
}

