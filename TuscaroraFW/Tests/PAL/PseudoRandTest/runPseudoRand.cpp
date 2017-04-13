////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 




#include "Lib/Misc.h"
#include "runPseudoRand.h"
#include <Sys/Run.h>

#include <string.h>


//int testRandomInt(int argc, char* argv[]) {
int main(int argc, char* argv[]) {
	//Make sure RuntimeOpts construction is the very first line of main
	RuntimeOpts opts ( argc-1, argv+1 );
	InitPlatform(&opts);	//This does a platform specific initialization

	PseudoRandTest rtest;
	rtest.Execute();

	//RunMasterLoopNeverExit(); //never exists on dce
	RunTimedMasterLoop();

	return 0;
	}
