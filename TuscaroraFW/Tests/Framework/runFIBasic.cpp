////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 


#include "runFIBasic.h"
#include <Sys/Run.h>
#include "Tests/FW_Init.h"

#include <string.h>


// extern bool DBG_PATTERN; // set in Lib/Misc/Debug.cpp
// extern bool DBG_WAVEFORM; // set in Lib/Misc/Debug.cpp
// extern bool DBG_CORE; // set in Lib/Misc/Debug.cpp
// extern bool DBG_CORE_ESTIMATION; // set in Lib/Misc/Debug.cpp
// extern bool DBG_CORE_DISCOVERY; // set in Lib/Misc/Debug.cpp
// extern bool DBG_WF_EVENT; // set in Lib/Misc/Debug.cpp
// extern bool DBG_CORE_DATAFLOW; // set in Lib/Misc/Debug.cpp
// extern bool DBG_CORE_WFCONTROL; // set in Lib/Misc/Debug.cpp

Core_Test::Core_Test(){
  printf(" Framework Test Constructor TEST ....\n");
 
//   DBG_WAVEFORM=true; // set in Lib/Misc/Debug.cpp
}


void Core_Test::Execute(RuntimeOpts *opts){
  printf("Core TEST:: Execute....fuga \n");
}


int main(int argc, char* argv[]) {
  //Make sure RuntimeOpts construction is the very first line of main
  RuntimeOpts opts ( argc-1, argv+1 );

//   DBG_CORE = true; // set in Lib/Misc/Debug.cpp
//   DBG_CORE_ESTIMATION = true; // set in Lib/Misc/Debug.cpp
//   DBG_CORE_DISCOVERY = true; // set in Lib/Misc/Debug.cpp
//   DBG_WF_EVENT = true;  // set in Lib/Misc/Debug.cpp
//   DBG_CORE_DATAFLOW = true; // set in Lib/Misc/Debug.cpp
//   DBG_WAVEFORM = true; // set in Lib/Misc/Debug.cpp
//   DBG_CORE_WFCONTROL = true; // set in Lib/Misc/Debug.cpp


  InitPlatform(&opts);	//This does a platform specific initialization

  FW_Init fwInit;
  //fwInit.InitPI();
  fwInit.Execute(&opts);
 
  Core_Test ctest;
  ctest.Execute ( &opts );
  //RunMasterLoopNeverExit(); //never exists on dce
  RunTimedMasterLoop();

  return 0;
}
