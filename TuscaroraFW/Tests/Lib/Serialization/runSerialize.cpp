////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include "runSerialize.h"

#include <string.h>


bool cancel;
bool add;
bool replace;
bool normal;
bool broadcast;
uint16_t total_valid_replaced;
uint16_t destCount =0;


void SerializeTest::Execute(RuntimeOpts* opts)
{
  	
}


int main(int argc, char* argv[]) {
	//Make sure RuntimeOpts construction is the very first line of main
	RuntimeOpts opts ( argc-1, argv+1 );
	InitPlatform(&opts);
	
	Debug_Printf(DBG_TEST, "FI_Test:: Creating FI_Test object I am here. RunTime:: %d\n",runTime);fflush(stdout);
	SerializeTest stest;
	stest.Execute ( &opts );
	
	//RunMasterLoopNeverExit(); //never exists on dce
	RunTimedMasterLoop();
	
	return 0;
}
