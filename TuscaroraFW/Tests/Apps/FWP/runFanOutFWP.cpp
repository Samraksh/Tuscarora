////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#include <Sys/Run.h>
#include <Apps/FWP/FanOutFWP.h>




int main(int argc, char* argv[]) {
	//Make sure RuntimeOpts construction is the very first line of main
	RuntimeOpts opts ( argc-1, argv+1 );
	InitPlatform(&opts);	//This does a platform specific initialization

	Apps::FanOut_FWP fwp_init;
	fwp_init.StartApp();


	//RunMasterLoopNeverExit(); //never exists on dce
	RunTimedMasterLoop();
	return 1;
}

