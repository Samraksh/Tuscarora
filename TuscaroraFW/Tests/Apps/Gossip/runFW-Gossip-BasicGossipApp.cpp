////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 




#include <Sys/Run.h>
#include "Tests/FW_Init.h"
#include "Tests/Patterns/Gossip/Gossip_Init.h"
#include <Apps/Gossip/BasicGossipApp.h>


int main(int argc, char* argv[]) {
	//Make sure RuntimeOpts construction is the very first line of main
	RuntimeOpts opts ( argc-1, argv+1 );
	InitPlatform(&opts); //This does a platform specific initialization

	FW_Init fwInit;
	fwInit.Execute(&opts);

	Gossip_Init<uint32_t> gInit(0);
	gInit.Execute(&opts);

	Apps::BasicGossipApp<uint32_t> bga;
	bga.Execute(&opts);


	RunTimedMasterLoop();
	return 0;
}


