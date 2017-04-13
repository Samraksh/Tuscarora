////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include "runMobilityControl.h"
#include <Sys/Run.h>

// extern bool DBG_MAIN; // set in Lib/Misc/Debug.cpp
// extern bool DBG_TEST; // set in Lib/Misc/Debug.cpp

using namespace ExternalServices;

MobilityControlTest::MobilityControlTest() {
//   DBG_MAIN=true; // set in Lib/Misc/Debug.cpp
//   DBG_TEST=true;  // set in Lib/Misc/Debug.cpp
}

void MobilityControlTest:: Execute(int argc, char *argv[]){
  double myId = MY_NODE_ID;
 Debug_Printf(DBG_TEST, "Setting my location to %f, %f, %f \n", myId, myId, myId);
  Location3D loc;
  loc.x = myId;
  loc.y = myId;
  loc.z = myId;
  MobilityControl::SetLocation(loc);
}


int main(int argc, char* argv[]){
	//Make sure RuntimeOpts construction is the very first line of main
	RuntimeOpts opts ( argc-1, argv+1 );
	InitPlatform(&opts);	//This does a platform specific initialization

	MobilityControlTest mcTest;
	sleep(1);
	mcTest.Execute(argc,argv);
  
	//RunMasterLoopNeverExit(); //never exists on dce
	RunTimedMasterLoop();
	
	return 0;
}
