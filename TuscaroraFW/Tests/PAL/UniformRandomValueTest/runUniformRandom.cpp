////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#include <stdlib.h>
#include <string.h>

#include "Lib/Misc.h"
#include "runUniformRandom.h"
#include <Sys/Run.h>

//int testUniformRandom(int argc, char* argv[]) {
int main(int argc, char* argv[]) {
  //Make sure RuntimeOpts construction is the very first line of main
  RuntimeOpts opts ( argc-1, argv+1 );
  InitPlatform(&opts);	//This does a platform specific initialization
  
  UniformRandomTest rtest;
  rtest.Execute();
  
	//RunMasterLoopNeverExit(); //never exists on dce
	RunTimedMasterLoop();

  return 0;
}
