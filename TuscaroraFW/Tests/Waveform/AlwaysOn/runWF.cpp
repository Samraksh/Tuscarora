////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#include "runWF.h"
#include <string.h>
#include <math.h>
#include <Sys/Run.h>
#include "Framework/Core/WF_Events/RecvWFEvents.h"

// extern bool DBG_WAVEFORM; // set in Lib/Misc/Debug.cpp
// extern bool DBG_WF_EVENT; // set in Lib/Misc/Debug.cpp
// extern bool DBG_CORE; // set in Lib/Misc/Debug.cpp

#define DBG_WAVEFORM_ 1
bool ACK=1;

namespace Core{
namespace Events {
 extern RecvWFEvents<uint64_t> *waveformEventReceiver;
}
}

struct ifaddrs *addrs;
char *GetFirstInterface(){
	 if (addrs->ifa_addr && addrs->ifa_addr->sa_family == AF_PACKET) {
        printf("WF_TEST: First Network Interface is : %s\n", addrs->ifa_name);
	 }
	 return addrs->ifa_name;
}


WF_Test::WF_Test(){

	Core::Events::waveformEventReceiver = new Core::Events::RecvWFEvents<uint64_t>();

	timer=NULL;
	sendMsg= new WF_Message_n64_t(sizeof(struct MyMsg));
	ackDelegate = new Delegate<void, WF_DataStatusParam_n64_t>(this, &WF_Test::SendAck_Handler); //assign DataNotifier signal
	recvDelegate = new Delegate<void, WF_Message_n64_t&>(this, &WF_Test::Recv_Handler); //assign recv signal
	timerDelegate = new TimerDelegate(this, &WF_Test::Timer_Handler); 

	rndDouble = new UniformRandomValue (0,1);

	//MY_NODE_ID = atoi(getenv("NODEID"));

	//(WaveformId_t waveformID, WF_TypeE _type, char* deviceName
	//wf = new WF_AlwaysOn_DCE("wlan0",*recvDelegate,*ackDelegate);
 
#if defined(PLATFORM_LINUX)
	//char localWflink_ [] = "eth0";
	char localWflink_ [] = "enp0s3";
	//char *localWflink_ = GetFirstInterface();
	Debug_Printf(DBG_TEST, "Creating waveform for linux platform on device %s\n", localWflink_);
#elif defined(PLATFORM_DCE)
	char localWflink_ [] = "eth1";
#endif 
		
	if(ACK){
		wf = new WF_AlwaysOn_Ack((WaveformId_t) (Waveform::ALWAYS_ON_LOCAL_WF), Waveform::ALWAYS_ON_LOCAL_WF, Waveform::WF_NO_EST, localWflink_, 2, 3);
	}else {
		wf = new WF_AlwaysOn_NoAck((WaveformId_t) (Waveform::ALWAYS_ON_LOCAL_WF), Waveform::ALWAYS_ON_LOCAL_WF, Waveform::WF_NO_EST, localWflink_, 2, 3);
	}
	

	/*char localWflink_ [] = "eth1";
  if(ACK){
    wf = new WF_AlwaysOn_DCE_Ack((WaveformId_t) (Waveform::ALWAYS_ON_LOCAL_WF), Waveform::ALWAYS_ON_LOCAL_WF, Waveform::WF_NO_EST, localWflink_);
  }else {
    wf = new WF_AlwaysOn_DCE_NoAck((WaveformId_t) (Waveform::ALWAYS_ON_LOCAL_WF), Waveform::ALWAYS_ON_LOCAL_WF, Waveform::WF_NO_EST, localWflink_);
  }

*/

  //sendMsg.payload = new uint8_t[sizeof(struct MyMsg)];
  //sendMsg.payloadSize = sizeof(struct MyMsg);
  //wfAttrib = wf->GetWaveformAttributes();
  //maxPayloadSize = wfAttrib.maxPayloadSize;
  seqNumber=0;
  //Debug_Printf(DBG_TEST, "Initialized test: Maximum payload that can be sent is %d \n", maxPayloadSize);
}


void PrintMsg(WF_Message_n64_t& msg) {
  MyMsg* rcvPtr = (MyMsg*) msg.payload;
  Debug_Printf(DBG_TEST, "Message %d from node %lu :: ", rcvPtr->seqNo, rcvPtr->senderID);
  for (int i = 0; i< PayloadSize; i++){
    //Debug_Printf(DBG_TEST, "%d ",rcvPtr->data[i]);
  }
  printf( "%d\n", rcvPtr->seqNo);
}

void WF_Test::Timer_Handler(uint32_t event)
{
  if(MY_NODE_ID == 0 || MY_NODE_ID == 333){  
    //if(seqNumber % 2 == 1) {
		memset(sendMsg->payload, 10, sizeof(struct MyMsg));
		MyMsg* sndPtr = (MyMsg*) sendMsg->payload;
		sndPtr->senderID = MY_NODE_ID;
		sndPtr->seqNo=seqNumber;
		Debug_Printf(DBG_TEST, "Timer Handler: %d event: Sending Message %d\n",event, seqNumber);
		
		for (int i=0; i< PayloadSize ; i++){
			if((seqNumber % 2) ==0 ){
				sndPtr->data[i] = PayloadSize-i; //I do not understand. Seqnumber cannot be even.
			}else {
				sndPtr->data[i] = i;//if sequence number is odd,fill data to be integer.
			}
		}
		
		sendMsg->payloadSize = sizeof(struct MyMsg);
		
		//wf->BroadcastData((*sendMsg), (sendMsg->payloadSize), seqNumber);

		//Test Multiple Destination(9 destinations) SendData
		int starting_node = MY_NODE_ID+1;
		uint64_t* destArray = new uint64_t[9];
		for(int i = 0; i <9 ; i++){
				destArray[i] = (starting_node++) % 10;
		}
	
		//wf->SendData((*sendMsg), (sendMsg->payloadSize), destArray, 9, seqNumber,true);
		wf->BroadcastData((*sendMsg), sendMsg->payloadSize, seqNumber);
		Debug_Printf(DBG_TEST, "WF_TEST:: TimerHandler: Sent a broadcast message %d of size %d to waveform %d \n", seqNumber, sendMsg->payloadSize, Waveform::ALWAYS_ON_LOCAL_WF); fflush(stdout);
		  
	//}
		seqNumber++;
		ResetTimer(messagePeriod);

    //Make request... Why always first DataRequest fails?
    /*Debug_Printf(DBG_TEST, "Cancel send..\n");
    wf->CancelDataRequest (0,0,0,0);
    Debug_Printf(DBG_TEST, "Replace packet..\n");
    wf->ReplacePayloadRequest (1, 2,(uint8_t*)sendMsg->payload, sendMsg->payloadSize);
    Debug_Printf(DBG_TEST, "Get Attributes\n");
    wf->AttributesRequest (2);
    Debug_Printf(DBG_TEST, "Get Control Status\n");
    wf->ControlStatusRequest (3);
    Debug_Printf(DBG_TEST, "Get DataStatus\n");
    wf->DataStatusRequest (4,2);*/
  }
}

void WF_Test::SendAck_Handler(WF_DataStatusParam_n64_t& ack){
	//Debug::PrintTime();
	//Debug_Printf(DBG_TEST, "Inside Ack Handler: \n");
	
}

void WF_Test::Recv_Handler(WF_Message_n64_t& msg){
	Debug::PrintTimeMilli();
	Debug_Printf(DBG_TEST, "WF_Test:: Receive: Got a msg from %lu of size %d on waveform %d\n",msg.GetSource(), msg.GetPayloadSize(),msg.GetWaveform());
	PrintMsg(msg);
}

void WF_Test::ResetTimer(uint32_t _period)
{
  double r = rndDouble->GetNext();
  //uint32_t period = opts->linkEstimationPeriod;
  uint32_t sendPeriod = _period * 0.5 + r * _period;
  Debug_Printf (DBG_TEST, "WF_TEST:: ResetTimer: %d, Setting beaconing with period: %d \n",_period, sendPeriod);
  //timer = new Timer(sendPeriod, PERIODIC, *timerDelegate);
  if(MY_NODE_ID == 0 || MY_NODE_ID == 333){
		if(timer){
			timer->Change(sendPeriod,ONE_SHOT);
		}else {
			timer = new Timer(sendPeriod, ONE_SHOT, *timerDelegate, "WF Test Timer");
			timer->Start();
		}
	}
}



void WF_Test::Execute(RuntimeOpts *opts){
	messagePeriod = opts->linkEstimationPeriod;
	//ResetTimer(messagePeriod);
}

//int testWF(int argc, char* argv[]) {
int main(int argc, char* argv[]) {
	//Make sure RuntimeOpts construction is the very first line of main
	RuntimeOpts opts ( argc-1, argv+1 );


	InitPlatform(&opts);	//This does a platform specific initialization
	NETWORK_SIZE = opts.nodes;

	if(strcmp(opts.patternArgs.c_str(), "NOACK") == 0) {
		ACK=0;
		Debug_Printf (DBG_TEST, "WF_TEST:: NOACK Arg\n");
	}
	
	WF_Test wtest;
	wtest.Execute(&opts);

	RunTimedMasterLoop(); //never exists on dce
	return 0;
}

