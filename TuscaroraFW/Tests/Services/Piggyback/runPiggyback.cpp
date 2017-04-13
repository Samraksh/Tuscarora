////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#include "Tests/FW_Init.h"
#include <Sys/Run.h>
#include "runPiggyback.h"
#include <Interfaces/Waveform/EventTimeStamp.h>
#define TESTDATASIZE 10

using namespace PAL;
using namespace Core;

/*void PBTest::TimerHandler(uint32_t event){
  Debug::PrintTimeMilli();
  printf("PBTEST: Starting the flooding\n"); fflush(stdout);
  data = new uint16_t[TESTDATASIZE];

  for (uint16_t i=0; i< TESTDATASIZE; i++ ){
    data[i]=(TESTDATASIZE-i)*2;
  }
  //Debug::PrintTimeMilli();
  Debug_Printf(DBG_TEST,"PBTEST: Created data of size %d \n", TESTDATASIZE);fflush(stdout);

}
*/


uint16_t TestPiggyBackee::CreateData(uint64_t dest, bool unaddressed, uint8_t*& data, uint8_t dataSize){

	if(unaddressed){
		Debug_Printf(DBG_TEST, "PB_TEST: Got a call from framework to create %d bytes of data  for broadcast....\n", dataSize);fflush(stdout);
	}
	else {
		Debug_Printf(DBG_TEST, "PB_TEST: Got a call from framework to create  %d bytes of data for unicast....\n", dataSize);fflush(stdout);
	}
	
	if(!overSizedPacket){
		Debug_Printf(DBG_TEST, "PB_TEST: Creating normal sized data for piggybacking....\n");fflush(stdout);
		if (dataSize > TESTDATASIZE){
			printf("PBTEST: Creating piggy data ...\n"); fflush(stdout);
			data = new uint8_t[TESTDATASIZE];

			for (uint16_t i=0; i< TESTDATASIZE; i++ ){
				data[i]=(TESTDATASIZE-i)*2;
			}
		}
	}
	else {

	}
	
	return true;
}


void TestPiggyBackee::ReceiveData(uint64_t src, uint8_t *data, uint8_t dataSize, U64NanoTime receiveTimestamp){

	Debug_Printf(DBG_TEST, "PB_TEST: REceived piggybacked data of size %d from framework with timestamp %lu \n", dataSize, receiveTimestamp.GetTime());fflush(stdout);
}


PBTest::PBTest(){
  //timerDel = new TimerDelegate (this, &PBTest::TimerHandler);
  //startTestTimer = new Timer(2000000, ONE_SHOT, *timerDel);

  Debug_Printf(DBG_TEST, "PB_TEST: Constructor initialized...\n");
}


void PBTest::Execute(int argc, char *argv[]){
    //create data so be sent;
	//OverSizedPacketTest();
	BasicTest();
	Debug_Printf(DBG_TEST, "PB_TEST: Starting Basic test...\n");fflush(stdout);

}




int main(int argc, char* argv[]) {
	//Make sure RuntimeOpts construction is the very first line of main
	RuntimeOpts opts ( argc-1, argv+1 );
	InitPlatform(&opts);	//This does a platform specific initialization

	FW_Init fwInit;
	FrameworkBase* fb= static_cast <FrameworkBase*>(fwInit.Execute(&opts));


	PBTest *pbTest = new PBTest();
	pbTest->Execute(argc, argv);
	WaveformId_t wfId=2;
	Debug_Printf(DBG_TEST, "PB_TEST: Calling framework for registering piggybacker.\n");
	bool registration = fb->RegisterPiggyBackee(Core::PB_TEST, Waveform::WFA_UINT64, TESTDATASIZE , "PBS_TEST_Piggy_10B", pbTest->pBackee, wfId) ;

	//RunMasterLoopNeverExit(); //never exists on dce
	if (!registration) {
		Debug_Printf(DBG_TEST, "PB_TEST: No valid piggybacker registered. Exiting\n");
		exit(1);
	}


	RunTimedMasterLoop();
	return 1;
}

