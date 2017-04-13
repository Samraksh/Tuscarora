////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#include "Tests/FW_Init.h"

#include "FWPTest.h"
#include <Sys/Run.h>



int main(int argc, char* argv[]) {
	//Make sure RuntimeOpts construction is the very first line of main
	RuntimeOpts opts ( argc-1, argv+1 );
	InitPlatform(&opts);	//This does a platform specific initialization

	FW_Init fwInit;
	fwInit.Execute(&opts);

//	FWP_Init fwpInit;
//	fwpInit.Execute(&opts);

	FWPTest *fwpTest = new FWPTest();
	fwpTest->Execute(&opts);


	//RunMasterLoopNeverExit(); //never exists on dce
	RunTimedMasterLoop();
	return 1;
}

