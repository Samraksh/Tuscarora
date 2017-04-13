////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#include "runFifo.h"
#include <Sys/Run.h>
#include <unistd.h>

#define NUMITEMS 10
// extern bool DBG_MAIN; // set in Lib/Misc/Debug.cpp

FifoTest::FifoTest() {
// 	DBG_MAIN=true; // set in Lib/Misc/Debug.cpp

	sendBuffer = new FIFOList();
	rcvBuffer = new FIFOList();

	rnd = new UniformRandomInt(1750,2250);

}

void FifoTest:: Execute(int argc, char *argv[]){

	Enqueue(4);
	sleep(1);
	Dequeue(3);
	sleep(1);
	Enqueue(3);
	sleep(1);
	Dequeue(4);
	//sleep(2);
	Enqueue(1);
	//sleep(2);
	Dequeue(2);
	//sleep(2);
	Enqueue(3);
	//sleep(2);
	Dequeue(3);
}

void FifoTest:: Enqueue(int32_t x){
	for (int i=0; i<x; i++){
		uint32_t rndval = rnd->GetNext();
		FMessage_t *msg = new FMessage_t(rndval);
		//msg->SetWaveform(88);
		//msg->dest = FRAMEWORK_LOCAL_BCAST_ADDRESS;
		sendBuffer->InsertMessage(msg);
		Debug_Printf(DBG_MAIN, "Enqueued a packet of size %u to sendBuffer\n",rndval); fflush(stdout);
	}

}


void FifoTest:: Dequeue(int32_t x){
	for(int i=0; i< x; i++ ){
		if(sendBuffer->Size()){
			FMessage_t *msg = sendBuffer->RemoveMessage();
			Debug_Printf(DBG_MAIN, "Denqueued a packet of size %u from sendBuffer\n",msg->GetPayloadSize()); fflush(stdout);
			delete(msg);
			Debug_Printf(DBG_MAIN, "Deleted the packet\n");fflush(stdout);
		}
	}
}

//int testFIFO(int argc, char* argv[]){
int main(int argc, char* argv[]){
	//Make sure RuntimeOpts construction is the very first line of main
	RuntimeOpts opts ( argc-1, argv+1 );
	InitPlatform(&opts);	//This does a platform specific initialization
	
	Debug::SetTextOutput(atoi(argv[13]));
	
	FifoTest fTest;
	fTest.Execute(argc,argv);

	//RunMasterLoopNeverExit(); //never exists on dce
	RunTimedMasterLoop();
	
	return 0;
}
