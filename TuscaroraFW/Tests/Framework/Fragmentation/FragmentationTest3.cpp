/*
 * FragmentationTest3.cpp
 */

#include "FragmentationTest3.h"
//#include <Platform/dce/Pattern/Gossip2AppShim.h>

FragmentationTest_Number3::FragmentationTest3::FragmentationTest3(){

	isTested = false;

	string s = "FragmentationTest3_";
	s = s + std::to_string(MY_NODE_ID) + ".txt";
	outFile.open(s.c_str());
	printf("FragmentationTest3:: Starting...\n");
	outFile << "FragmentationTest3:: Starting..."<< std::endl; outFile.flush();
	//PatternId_t pid = GetNewPatternInstanceId(GOSSIP_PTN, (char*)"Gossip_1");
	//AppId_t appid = 0;
	gossip_variable.SetMsg(4);
	if(MY_NODE_ID == 0){
		gossip_variable.SetMsg(5);
	}




	recvUpdateVarDelegate = new (TestGossipT::GossipVariableUpdateDelegate_t)(this, &FragmentationTest3::ReceiveGossipVariableUpdate);

	gossip = new TestGossipT(gossip_variable);
	gossip->RegisterGossipVariableUpdateDelegate(recvUpdateVarDelegate);

	timerDel = new TimerDelegate (this, &FragmentationTest3::TimerHandler);
	startTestTimer = new Timer(2000000, ONE_SHOT, *timerDel);


}

void FragmentationTest_Number3::FragmentationTest3::TimerHandler(uint32_t event){
	Debug_Printf(DBG_PATTERN, "GossipTEST: Starting the FragmentationTest3 Test...\n");
	outFile << "DBG_PATTERN:: Starting the FragmentationTest3 Test..." << std::endl; outFile.flush();
	gossip->Start();
	//  if(MY_NODE_ID == 0){
	//	  updateVariableTimer->Start();
	//  }
}

bool FragmentationTest_Number3::FragmentationTest3::IsRxMsgValid(void *data, uint16_t size ){
	printf ("FragmentationTest3::IsRxMsgValid Issued \n");
	outFile << "FragmentationTest3::IsRxMsgValid Issued "<< std::endl; outFile.flush();
	RepeatedBasicMsgType3 expected_message_embed = 4;
	if(MY_NODE_ID != 0){
		expected_message_embed = 5;
	}
	if(size != MSGSIZE) {
		printf ("FragmentationTest3::FAIL Received message size is INVALID. There might be an error in assembly \n");
		outFile << "FragmentationTest3::FAIL Received message size is INVALID. There might be an error in assembly "<< std::endl; outFile.flush();
		return false;
	}

	auto rxdata = static_cast<RepeatedBasicMsgType3*> (data);
	for(uint16_t i = 0; i < MSGSIZE; ++i){
		if(rxdata[i] != expected_message_embed) {
			printf ("FragmentationTest3::FAIL Received message does not match. There might be an error in assembly \n");
			outFile << "FragmentationTest3::FAIL Received message does not match. There might be an error in assembly "<< std::endl; outFile.flush();
			return false;
		}
	}

	printf ("FragmentationTest3::PASS Received message is valid \n");
	outFile << "FragmentationTest3::PASS Received message is valid "<< std::endl; outFile.flush();
	return true;
}



void FragmentationTest_Number3::FragmentationTest3::ReceiveGossipVariableUpdate(LongMessage3& updated_gossip_variable){
	isTested = true;
	IsRxMsgValid(updated_gossip_variable.array, MSGSIZE);
}

void FragmentationTest_Number3::FragmentationTest3::Execute(RuntimeOpts *opts){
	//Delay starting the actual pattern, to let the network stabilize
	//So start the timer now and when timer expires start the pattern
	Debug_Printf(DBG_PATTERN,"About to start timer on node 1\n");
	startTestTimer->Start();
}

FragmentationTest_Number3::FragmentationTest3::~FragmentationTest3() {
	// TODO Auto-generated destructor stub
	if(!isTested){
		printf ("FragmentationTest3::Test was not performed on this node  \n");
		outFile << "FragmentationTest3::Test was not performed on this node"<< std::endl; outFile.flush();
	}
	else{
		printf ("FragmentationTest3::Test Finished with at least one fragmentation \n");
		outFile << "FragmentationTest3::Test Finished with at least one fragmentation"<< std::endl; outFile.flush();
	}

}

