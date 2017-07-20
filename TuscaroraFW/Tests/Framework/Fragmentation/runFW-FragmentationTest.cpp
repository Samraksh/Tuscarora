

#include "FragmentationTest.h"
#include "Lib/PAL/PAL_Lib.h"
#include "Sys/Run.h"
#include "Tests/FW_Init.h"


int main(int argc, char* argv[]) {

	//Make sure RuntimeOpts construction is the very first line of main
	RuntimeOpts opts ( argc-1, argv+1 );
	InitPlatform(&opts);	//This does a platform specific initialization
	
	FW_Init fwInit;
	fwInit.Execute(&opts);

	auto *t = new FragTest::FragmentationTest();
	t->Execute(&opts);

	RunTimedMasterLoop();
	return 0;

}


