////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#include "Tests/FW_Init.h"
#include <Sys/Run.h>
#include "runTimestamp.h"
#include <Interfaces/Waveform/EventTimeStamp.h>
#define TESTDATASIZE 10

using namespace PAL;
using namespace Core;

#define WFID 2

/*void TimeStampTest::TimerHandler(uint32_t event){
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

void TimeStampTest::ReceiveTimestampTest()
{

}


void TimeStampTest::SenderTimestampTest()
{

}



TimeStampTest::TimeStampTest(FrameworkBase *_fb){
  //timerDel = new TimerDelegate (this, &TimeStampTest::TimerHandler);
  //startTestTimer = new Timer(2000000, ONE_SHOT, *timerDel);
	fb = _fb;
	tss = new TimeStamping<uint64_t> ( *(static_cast<PiggyBackerI<uint64_t>*>(fb)), WFID);
	
	Debug_Printf(DBG_TEST, "TS_TEST: Constructor initialized...\n");
}


void TimeStampTest::Execute(int argc, char *argv[]){
    //create data so be sent;
	//OverSizedPacketTest();
	SenderTimestampTest();
	Debug_Printf(DBG_TEST, "TS_TEST: Starting a Sender TimeStamp test...\n");fflush(stdout);

}


int main(int argc, char* argv[]) {
	//Make sure RuntimeOpts construction is the very first line of main
	RuntimeOpts opts ( argc-1, argv+1 );
	InitPlatform(&opts);	//This does a platform specific initialization

	FW_Init fwInit;
	FrameworkBase* fb= static_cast <FrameworkBase*>(fwInit.Execute(&opts));

	TimeStampTest *tsTest = new TimeStampTest(fb);
	tsTest->Execute(argc, argv);
	

	RunTimedMasterLoop();
	return 1;
}

