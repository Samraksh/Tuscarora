////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#include "FanOutFWP.h"

#ifdef PLATFORM_DCE
#include "Platform/dce/App/App2FWPShim.h"
#else
#include "../../Platform/linux-multiprocess/App/App2FWPShim.h"
#endif

#define TESTDATASIZE 2000

namespace Apps {

FanOut_FWP::FanOut_FWP() {
	// TODO Auto-generated constructor stub
	//FWP = &(GetApp2FWPShim());
	FWP = &( AWI::GetApp2FWPShim());
	recvDelegate = new AppRecvMessageDelegate_t(this, &FanOut_FWP::Receive);
	//static_cast<App2FWPShim*>(FWP)->RegisterAppReceiveDelegate(appid, recvDelegate);
	FWP->RegisterAppReceiveDelegate(appid, recvDelegate);

	timerDel = new TimerDelegate (this, &FanOut_FWP::TimerHandler);
	startTestTimer = new Timer(2000000, ONE_SHOT, *timerDel);
}

FanOut_FWP::~FanOut_FWP() {
	// TODO Auto-generated destructor stub
}

void FanOut_FWP::TimerHandler(uint32_t event){
  Debug::PrintTimeMilli();
  printf("FWPTEST: Starting the flooding\n"); fflush(stdout);
  data = new uint16_t[TESTDATASIZE];

  for (uint16_t i=0; i< TESTDATASIZE; i++ ){
    data[i]=(TESTDATASIZE-i)*2;
  }
  //Debug::PrintTimeMilli();
  Debug_Printf(DBG_TEST,"FWPTEST: Created data of size %d \n", TESTDATASIZE);fflush(stdout);
  Send((void *)data, sizeof(uint16_t)*TESTDATASIZE);
}

void FanOut_FWP::StartApp(){
  //Start the flood, only on node 1, let others be receivers
  if(MY_NODE_ID == 0){
    //create data so be sent;
    startTestTimer->Start();
    Debug_Printf(DBG_TEST, "FwP_TEST: Started the timer....\n");fflush(stdout);
  }
}

void FanOut_FWP::Receive(void *data, uint16_t size){
	Debug_Printf(DBG_PATTERN,"FWP_App:: Received a message of size = %u \n", size);
	free(data);
}

void FanOut_FWP::Send(void *data, uint16_t size){
	//AppMessage_t appMsg(size, (uint8_t*)data);
	FWP->Send(data, size);
}

} /* namespace Apps */
