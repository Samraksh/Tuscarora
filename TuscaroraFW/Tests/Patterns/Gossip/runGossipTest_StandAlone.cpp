////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 





#include "Lib/PAL/PAL_Lib.h"
#include <Sys/Run.h>
#include "Test/GossipTest.h"


int main(int argc, char* argv[]) {
  //Make sure RuntimeOpts construction is the very first line of main
  RuntimeOpts opts ( argc-1, argv+1 );
  InitPlatform(&opts); //This does a platform specific initialization


  GossipTest *gossipTest = new GossipTest();
  gossipTest->Execute(&opts);

	RunTimedMasterLoop();
	return 0;
}


