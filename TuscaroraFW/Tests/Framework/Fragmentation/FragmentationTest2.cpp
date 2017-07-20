/*
 * FragmentationTest2.cpp
 *
 *  Created on: Jun 15, 2017
 *      Author: bora
 */

#include "FragmentationTest2.h"

//#include "Tests/Patterns/FWP/FWP_Init.h"

FragTest2::FragmentationTest2::FragmentationTest2(NodeId_t _otherNodesID) {
	  recvUpdateVarDelegate = new (GossipVariableUpdateDelegate_t_l)(this, &FragmentationTest2::ReceiveGossipVariableUpdate);
	  otherNodesID = _otherNodesID;

}

FragTest2::FragmentationTest2::~FragmentationTest2() {
	// TODO Auto-generated destructor stub
}

void FragTest2::FragmentationTest2::Execute(){

	Debug_Printf(DBG_PATTERN, "FragTest2::FragmentationTest2::Start Starting FragmentationTest2 Pattern \n");

////
////	otherNodesID = 0;
////	if(opts->my_node_id == otherNodesID) otherNodesID = 1;
//
//	LongMessage lm;
//	for(auto i = MSGSIZE; i > 0 ; --i){
//		lm.array[i] = mynodeID;
//	}
//	Gossip_Init<LongMessage, LongMessageComparator> gInit(lm);
//
//	gossip = GetApp2GossipShimPtr<GOSSIPVARIABLE, GOSSIPCOMPARATOR>();
//	gossip->UpdateGossipVariable(lm);
//
//	gossip->RegisterGossipVariableUpdateDelegate(recvUpdateVarDelegate);
//
//	Debug_Printf(DBG_PATTERN, "FragmentationTest2:: Sending the RegisterPattern Request to framework...\n");

//	startTestTimer->Start();
}

void FragTest2::FragmentationTest2::ReceiveGossipVariableUpdate(LongMessage& updated_gossip_variable){
	IsRxMsgValid(updated_gossip_variable.array, MSGSIZE);
}


bool FragTest2::FragmentationTest2::IsRxMsgValid(void *data, uint16_t size ){
	if(size != MSGSIZE) {
		printf ("FragTest2::FragmentationTest2::FAIL Received message size is INVALID. There might be an error in assembly ");
		return false;
	}

	auto rxdata = static_cast<uint8_t*> (data);
	for(uint64_t i = 0; i < MSGSIZE; ++i){
		if(rxdata[i] != otherNodesID) {
			printf ("FragTest2::FragmentationTest2::FAIL Received message does not match. There might be an error in assembly ");
			return false;
		}
	}

	printf ("PASS Received message is valid ");
	return true;
}
