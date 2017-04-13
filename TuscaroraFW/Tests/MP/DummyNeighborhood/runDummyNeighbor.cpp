////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#include "runDummyNeighbor.h"

#include <Sys/Run.h>
#include "Tests/FW_Init.h"
#include <Platform/linux/Framework/PatternEventDispatch.h>

#define TEST_NBR_COUNT 10

// extern bool DBG_PATTERN; // set in Lib/Misc/Debug.cpp
// extern bool DBG_WAVEFORM; // set in Lib/Misc/Debug.cpp
// extern bool DBG_CORE; // set in Lib/Misc/Debug.cpp
// extern bool DBG_CORE_ESTIMATION; // set in Lib/Misc/Debug.cpp
// extern bool DBG_TEST; // set in Lib/Misc/Debug.cpp
// extern bool DBG_CORE_DATAFLOW; // set in Lib/Misc/Debug.cpp

DummyNeighborTest::DummyNeighborTest()
{ 
  Debug_Printf(DBG_TEST, "DummyNeighborTest:: Constructor..\n");
  //MY_NODE_ID = atoi(getenv("NODEID"));
  seqNumber=0;
  
  //Debug_Printf(DBG_TEST, "DummyNeighborTest:: Constructor done. My node id is %d, my pattern id is :%d \n", MY_NODE_ID, PID);
}


void DummyNeighborTest::Execute(RuntimeOpts *opts){

  ///////////////////Do special things for this test////////////////
  fi = static_cast<StandardFI *> (&GetFrameworkInterface());
  PolicyManager::SetLinkSelectionPolicy(Core::Policy::PREFFERED_WAVEFORM, 2);
  
  Debug_Printf(DBG_TEST, "Neighborhood TEST:: Using framework instance %p\n",fi);

  //create a dummy estimator to publish some neighborhood.
  dEstimator = new DummyEstimator2();

  //Register the dummy link estimator, for waveform id 10
  fi->RegisterLinkEstimator(*dEstimator,Core::Estimation::FW_TEST_EST, dEstimator->wid);

  Start();

}


//Pick a neighbor, send generate a random message, send to neighbor, wait for knowledgement
bool DummyNeighborTest::Start()
{
  timerDelegate = new TimerDelegate (this, &DummyNeighborTest::LocalMsgTimer_Handler);
  msgTimer = new Timer(20000000, PERIODIC, *timerDelegate);
  msgTimer->Start();

  return true;
}

bool DummyNeighborTest::Stop()
{
 return true;
}

void DummyNeighborTest::LocalMsgTimer_Handler(uint32_t event)
{
  Debug_Printf(DBG_TEST,"DummyNeighborTest:: Timer handler\n");

  //Make dummy estimator add a new neighbor to framework
  if(seqNumber < TEST_NBR_COUNT){
    //Debug_Printf(DBG_TEST,"DummyNeighborTest:: Adding a new neighbor\n");
    dEstimator->AddNeighbor();
  }
  else {
    //Debug_Printf(DBG_TEST,"DummyNeighborTest:: Removing neighbo\nr");
    dEstimator->RemoveNeighbor();
  }
  seqNumber++;
  if(seqNumber == 2 * TEST_NBR_COUNT){
      seqNumber = 0;
  }

}

//int testCore(int argc, char* argv[]) {
int main(int argc, char* argv[]) {
  
  RuntimeOpts opts ( argc-1, argv+1 );
  
  InitPlatform(&opts);  //This does a platform specific initialization

  FW_Init fwInit;
  fwInit.Execute(&opts);

//  Debug_Printf(DBG_TEST, "Main:: Creating framework for node %d .....\n", MY_NODE_ID);
//  StandardFI *fi = new StandardFI();
//  PWI::SetFrameworkInterface(*fi);
//  (static_cast<PatternEventDispatch *>(fi->eventDispatcher))->ptn_controller_ptr->SetFrameworkPtr(fi);
//
//
//  fi->SetLinkEstimationPeriod(opts.linkEstimationPeriod);
//  fi->SetNeighborInactivityPeriod(opts.deadNeighborPeriod);
//  fi->SetDiscoveryType ( NULL_DISCOVERY );
//  fi->SetLinkEstimationType( FW_RANDOM_EST );


  //PatternId_t pid = GetNewPatternInstanceId(NON_STD_PTN,);
  DummyNeighborTest ntest;
  
  //start the test
  ntest.Execute(&opts);

  RunMasterLoopNeverExit(); //never exists on dce
  return 0;
}
