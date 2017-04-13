////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#include "runNS3.h"
#include <Sys/Run.h>
#include <assert.h>
#include "Tests/FW_Init.h"

#include <string.h>


// extern bool DBG_PATTERN; // set in Lib/Misc/Debug.cpp
// extern bool DBG_WAVEFORM; // set in Lib/Misc/Debug.cpp
// extern bool DBG_CORE; // set in Lib/Misc/Debug.cpp
// extern bool DBG_CORE_ESTIMATION; // set in Lib/Misc/Debug.cpp
// extern bool DBG_WF_EVENT; // set in Lib/Misc/Debug.cpp
// extern bool DBG_CORE_DATAFLOW; // set in Lib/Misc/Debug.cpp
// extern bool DBG_CORE_WFCONTROL; // set in Lib/Misc/Debug.cpp
// extern bool DBG_TEST; // set in Lib/Misc/Debug.cpp

Core_Test::Core_Test(){
  printf(" Framework Test Constructor TEST ....\n");
   pid = 1;
  nonce = 1;
  fi = static_cast<StandardFI *> (&GetFrameworkInterface());
  //fi->RegisterDelegates(1,RecvDelegate, nbrDelegate,dataNotifierDelegate,controlDelegate);
 
  printf("Done. HOGE");fflush(stdout);
}



void Core_Test::Control_Handler(ControlResponseParam param){
}
void Core_Test::NeighborUpdateEvent(NeighborUpdateParam param){
   printf("Core_Test::NeighborUpdate message\n");
}
void Core_Test::Execute(RuntimeOpts *opts){
  dataNotifierDelegate = new DataflowDelegate_t(this, &Core_Test::DataStatus_Handler);
  recvDelegate = new RecvMessageDelegate_t(this, &Core_Test::Recv_Handler);
  controolResponceDelegate = new ControlResponseDelegate_t(this,&Core_Test::Control_Handler);
  nbrDelegate = new NeighborDelegate (this, &Core_Test::NeighborUpdateEvent);
 
  fi->RegisterDelegates(pid,recvDelegate, nbrDelegate,dataNotifierDelegate,controolResponceDelegate); 
  StartMessaging();
  printf("Core TEST:: Execute....fuga \n");
}


//Pick a neighbor, send generate a random message, send to neighbor, wait for knowledgement
void Core_Test::StartMessaging(){
  timerDelegate = new TimerDelegate(this, &Core_Test::LocalMsgTimer_Handler);
  msgTimer = new Timer(50000, PERIODIC, *timerDelegate);
  //uses small value for broadcast test. 20
  //unicast test  1000

  //msgTimer = new Timer(10000, ONE_SHOT, *timerDelegate);
  msgTimer->Start();
  printf("Core TEST:: started timer \n");
  
}


//void Core_Test::SendAck_Handler(MessageAckParam_n64_t& ack){
void Core_Test::DataStatus_Handler(DataStatusParam& ack){
   for(uint16_t index=0; index < ack.noOfDest; index++){
    switch(ack.statusType[index]){
      case PDN_ERROR:
	break;
       case PDN_FW_RECV:
       {
           break;
       } 
       case PDN_WF_RECV:{
	 
	if(ack.statusValue[index]){
	    Debug_Printf(DBG_TEST, "Positive WF_RECV ACK from %d\n",ack.destArray[index]);
	  }else{
	    Debug_Printf(DBG_TEST, "Negative WF_RECV ACK from %d\n",ack.destArray[index]);
	  }
	
	 break;
       }
       case PDN_WF_SENT:
       {
	  if(ack.statusValue[index]){
	    Debug_Printf(DBG_TEST, "Positive WF_SENT ACK from %d\n",ack.destArray[index]);
	  }else{
	    Debug_Printf(DBG_TEST, "Negative WF_SENT ACK from %d\n",ack.destArray[index]);
	  }
	  break;
       }
       case PDN_DST_RECV:
       {
	if(ack.statusValue[index]){
	  Debug_Printf(DBG_TEST, "Positive PDN_DST_RECV ACK from %d\n",ack.destArray[index]);
	}else{
	  Debug_Printf(DBG_TEST, "Negative PDN_DST_RECV ACK from %d\n",ack.destArray[index]);
	}
	break;
       }  
       case PDN_BROADCAST_NOT_SUPPORTED:
    	   break;
    }
  }
}

void Core_Test::Recv_Handler(FMessage_t& msg){
	Debug::PrintTimeMilli();
	Debug_Printf(DBG_TEST, "Core_Test::Received Message\n");
}

void Core_Test::LocalMsgTimer_Handler(uint32_t event)
{
  if(MY_NODE_ID == 0) {
    if(nonce == 1) {
      std::stringstream strs;
      strs << "LOCATION=1,1,1";
      std::string str = strs.str();
      putenv((char *)(str.c_str()));
    }

    Debug_Printf(DBG_TEST, "Sending message\n");
    Debug::PrintTimeMilli();
    FMessage_t sendMsg(256);
    
    memset(sendMsg.GetPayload(), 0, 256);
    RandMsg *storeMsg = new RandMsg();
    storeMsg->seqNo = seqNumber;
    RandMsg *sndPtr = (RandMsg*) sendMsg.GetPayload();
    sndPtr->src = MY_NODE_ID;
    sndPtr->seqNo=seqNumber;
    
    memcpy(sndPtr->msg,storeMsg->msg,256);
    sendMsg.SetPayloadSize(sizeof(class RandMsg));
    Debug_Printf(DBG_CORE,"RunFI TEST::Before transmit\n");
    int destsize=1;
    NodeId_t* destArray = new uint16_t[destsize];
    destArray[0] = 1;
    fi->SendData (pid, destArray, (uint16_t) destsize, sendMsg,nonce);
  }

  if(MY_NODE_ID == 1) {
    std::stringstream strs;
    strs << "LOCATION=" << nonce * 10 << "," << nonce * 10 << ",0";
    std::string str = strs.str();
    putenv((char *)(str.c_str()));
  }
  ExternalServices::Location2D location = ExternalServices::LocationService::GetLocation();
  Debug_Printf(DBG_TEST, "Location: %f, %f\n", location.x, location.y);
  nonce++;
}

int main(int argc, char* argv[]) {
	//Make sure RuntimeOpts construction is the very first line of main
	RuntimeOpts opts ( argc-1, argv+1 );
	InitPlatform(&opts);	//This does a platform specific initialization

	FW_Init fwInit;
	//fwInit.InitPI();
	fwInit.Execute(&opts);
	printf("Creating Core_Test object I am here\n");fflush(stdout);
	Core_Test ctest;
	ctest.Execute ( &opts );

	//RunMasterLoopNeverExit(); //never exists on dce
	RunTimedMasterLoop();
	return 0;
}
