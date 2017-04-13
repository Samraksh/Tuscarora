////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

/*
 * PplscTest.cpp
 *
 */



#include "Lib/PAL/PAL_Lib.h"
#include "Patterns/Pplsc/Pplsc.h"
#include <Sys/Run.h>
#include "Tests/FW_Init.h"
#include "runPplsc.h"

using namespace Patterns;
using namespace PAL;
using namespace Core::Naming;

PplscTest::PplscTest(){

  //PatternId_t pid = GetNewPatternInstanceId(GOSSIP_PTN, (char*)"Pplsc_1");
  pplsc = new Pplsc();
  timerDel = new TimerDelegate (this, &PplscTest::TimerHandler);
  startTestTimer = new Timer(2000000, ONE_SHOT, *timerDel);
}

void PplscTest::TimerHandler(uint32_t event){
 Debug_Printf(DBG_PATTERN, "PplscTEST: Starting the Pplsc Pattern...\n");
  pplsc->Start();
}

void PplscTest::Execute(RuntimeOpts *opts){
  //Delay starting the actual pattern, to let the network stabilize
  //So start the timer now and when timer expires start the pattern
 Debug_Printf(DBG_PATTERN,"About to start timer on node 1\n");
  startTestTimer->Start();
}

int main(int argc, char* argv[]) {
  //Make sure RuntimeOpts construction is the very first line of main
  RuntimeOpts opts ( argc-1, argv+1 );
  InitPlatform(&opts);    //This does a platform specific initialization
  

  FW_Init fwInit;
  fwInit.Execute(&opts);

  PolicyManager::SetLinkSelectionPolicy(Core::Policy::PATTERN_DEFINED, 2);
  
  PplscTest *pplscTest = new PplscTest();
  pplscTest->Execute(&opts);

  RunTimedMasterLoop();
}


