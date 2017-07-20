/*
 * FragmentationTest.cpp
 *
 *  Created on: Jun 15, 2017
 *      Author: bora
 */
#include "FragmentationTest.h"

#include <string>

#include "Tests/Patterns/FWP/FWP_Init.h"

using namespace std;

FragTest::FragmentationTest::FragmentationTest() : isMsgReceived(false), isMsgIdAssigned(false)  {
	startTesttimerDel = new PAL::TimerDelegate (this, &FragmentationTest::Send);
	startTestTimer = new PAL::Timer(10000000, ONE_SHOT, *startTesttimerDel);

	addDesttimerDel = new PAL::TimerDelegate (this, &FragmentationTest::AddDest);
	addDestTimer = new PAL::Timer(100, ONE_SHOT, *addDesttimerDel);

	removeDesttimerDel = new PAL::TimerDelegate (this, &FragmentationTest::RemoveDest);
	removeDestTimer = new PAL::Timer(100, ONE_SHOT, *removeDesttimerDel);

	replacePayloadtimerDel = new PAL::TimerDelegate (this, &FragmentationTest::ReplacePayload);
	replacePayloadTimer = new PAL::Timer(100, ONE_SHOT, *replacePayloadtimerDel);


	endTesttimerDel = new PAL::TimerDelegate (this, &FragmentationTest::CheckTestResults);
	endTestTimer = new PAL::Timer(20000000, ONE_SHOT, *endTesttimerDel);

	assignedMsgId = 0;

	numsend = 0;

	string s = "FragmentationTestResults";
//	s = s + std::to_string(MY_NODE_ID) + ".txt";
	s += ".txt";

	outFile = fopen(s.c_str(), "w");

	printf("FragmentationTest:: Starting...\n");
	FPRINT_TEST_RESULT("FragmentationTest:: Starting...\n");


}

FragTest::FragmentationTest::~FragmentationTest() {
	// TODO Auto-generated destructor stub
}

void FragTest::FragmentationTest::CheckTestResults(uint32_t event){
	FPRINT_TEST_RESULT( "FragmentationTest::CheckTestResults MY_NODE_ID = %d \n", MY_NODE_ID );
	if(MY_NODE_ID == 0 ){
		bool isFailed = false;
		for(auto it = sel_dest_and_notifications.Begin(); it != sel_dest_and_notifications.End(); ++it){
			for(auto it2 = 0 ; it2 < 4; ++it2){
				if( it->Second().list[it2] == false ){
					FPRINT_TEST_RESULT( "FragmentationTest::FAIL Missing %d nd notification for NodeId = %d \n", it2, it->First() );
					isFailed = true;
				}
				else{
					FPRINT_TEST_RESULT( "FragmentationTest::CheckTestResults PASS %d nd notification for NodeId = %d \n", it2, it->First() );
				}
			}
		}
		if(!isFailed){
			FPRINT_TEST_RESULT( "FragmentationTest::PASS All notifications have been received \n" );

		}
	}
	if(MY_NODE_ID != 0 ){
		bool isFailed = false;
		if(isMsgReceived){
			FPRINT_TEST_RESULT( "FragmentationTest::PASS The Message was received \n" );
		}
		else{
			FPRINT_TEST_RESULT( "FragmentationTest::FAIL The Message was not received!! \n" );
		}
	}

	fclose(outFile);
}


void FragTest::FragmentationTest::Execute(RuntimeOpts *opts){

	Debug_Printf(DBG_PATTERN, "FragmentationTest::Start Starting FragmentationTest Pattern \n");
	Debug_Printf(DBG_PATTERN, "FragmentationTest:: Sending the RegisterPattern Request to framework...\n");
	FPRINT_TEST_RESULT( "FragmentationTest:: Sending the RegisterPattern Request to framework... \n" );


	this->FragmentationTest::Start();

	if(MY_NODE_ID ==0) startTestTimer->Start();
	endTestTimer->Start();
}


void FragTest::FragmentationTest::DataStatusEvent(DataStatusParam ntfy)
{
	///You have received ack for previous message.
	//check what happened to the previous message
	Debug_Printf(DBG_PATTERN,"FragmentationTest:: Received a data notification from framework\n");

	for (uint16_t i=0; i< ntfy.noOfDest; i++){
		if(sel_dest_and_notifications.Find(ntfy.destArray[i]) == sel_dest_and_notifications.End()){
			FPRINT_TEST_RESULT( "FragmentationTest::FAIL Received notification for unselected destination  \n" );
			continue;
		}
		if(ntfy.statusValue[i] ) {
			switch (ntfy.statusType[i]){
			case PDN_FW_RECV:
				Debug_Printf(DBG_PATTERN,"FragmentationTest:: Message with nonce = %d is assigned a message id = %d and is accepted by the framework. ntfy.destArray[%d] = %d \n", ntfy.nonce, ntfy.messageId, i, ntfy.destArray[i]);
				if(sel_dest_and_notifications.Find(ntfy.destArray[i])->Second().list[0] == false){
					FPRINT_TEST_RESULT("FragmentationTest::PASS Message with nonce = %d is assigned a message id = %d and is accepted by the framework for destArray[%d] = %d \n", ntfy.nonce, ntfy.messageId, i, ntfy.destArray[i] );
					assignedMsgId = ntfy.messageId;
					isMsgIdAssigned = true;
					sel_dest_and_notifications.Find(ntfy.destArray[i])->Second().list[0] = true;
					replacePayloadTimer->Start();
				}
				break;
			case PDN_WF_RECV:
				if(isMsgIdAssigned && ntfy.messageId == assignedMsgId
						&& sel_dest_and_notifications.Find(ntfy.destArray[i])->Second().list[0] == true
					){
					if(sel_dest_and_notifications.Find(ntfy.destArray[i])->Second().list[1] == false){
						FPRINT_TEST_RESULT( "FragmentationTest::PASS PDN_WF_RECV for MsgId = %d ntfy.messageId = %d ntfy.destArray[%d] = %d nonce = %d \n" , assignedMsgId, ntfy.messageId, i, ntfy.destArray[i], ntfy.nonce  );
						sel_dest_and_notifications.Find(ntfy.destArray[i])->Second().list[1] = true;
					}

				}
				else{
					FPRINT_TEST_RESULT( "FragmentationTest::FAIL PDN_WF_RECV MsgId = %d ntfy.messageId = %d ntfy.destArray[%d] = %d nonce = %d\n", assignedMsgId, ntfy.messageId, i, ntfy.destArray[i], ntfy.nonce );
				}
				break;
			case PDN_WF_SENT:
				if(isMsgIdAssigned && ntfy.messageId == assignedMsgId
						&& sel_dest_and_notifications.Find(ntfy.destArray[i])->Second().list[0] == true
						&& sel_dest_and_notifications.Find(ntfy.destArray[i])->Second().list[1] == true
					){
					if(sel_dest_and_notifications.Find(ntfy.destArray[i])->Second().list[2] == false){
						FPRINT_TEST_RESULT( "FragmentationTest::PASS PDN_WF_SENT for MMsgId = %d ntfy.messageId = %d ntfy.destArray[%d] = %d nonce = %d\n", assignedMsgId, ntfy.messageId, i, ntfy.destArray[i], ntfy.nonce  );
						sel_dest_and_notifications.Find(ntfy.destArray[i])->Second().list[2] = true;
					}

				}
				else{
					FPRINT_TEST_RESULT( "FragmentationTest::FAIL PDN_WF_SENT MsgId = %d ntfy.messageId = %d ntfy.destArray[%d] = %d nonce = %d\n", assignedMsgId, ntfy.messageId, i, ntfy.destArray[i], ntfy.nonce  );
				}
				Debug_Printf(DBG_PATTERN,"FragmentationTest:: Waveform has sent the message id: %d\n", ntfy.messageId);
				break;
			case PDN_DST_RECV:
				if(isMsgIdAssigned && ntfy.messageId == assignedMsgId
						&& sel_dest_and_notifications.Find(ntfy.destArray[i])->Second().list[0] == true
						&& sel_dest_and_notifications.Find(ntfy.destArray[i])->Second().list[1] == true
						&& sel_dest_and_notifications.Find(ntfy.destArray[i])->Second().list[2] == true
					){
					if(sel_dest_and_notifications.Find(ntfy.destArray[i])->Second().list[3] == false){
						FPRINT_TEST_RESULT( "FragmentationTest::PASS PDN_DST_RECV MsgId = %d ntfy.messageId = %d ntfy.destArray[%d] = %d nonce = %d\n", assignedMsgId, ntfy.messageId, i, ntfy.destArray[i], ntfy.nonce  );
						sel_dest_and_notifications.Find(ntfy.destArray[i])->Second().list[3] = true;
					}
				}
				else{
					FPRINT_TEST_RESULT( "FragmentationTest::FAIL PDN_DST_RECV MsgId = %d ntfy.messageId = %d ntfy.destArray[%d] = %d nonce = %d \n", assignedMsgId, ntfy.messageId, i, ntfy.destArray[i], ntfy.nonce  );
				}
				break;
			default:
				FPRINT_TEST_RESULT( "FragmentationTest::FAIL Unknown Status Type MsgId = %d ntfy.messageId = %d ntfy.destArray[%d] = %d nonce = %d \n", assignedMsgId, ntfy.messageId, i, ntfy.destArray[i], ntfy.nonce  );
				break;
			}
		}
	}
	nonce++;
}

void FragTest::FragmentationTest::SelectAllDestinationNodes(){
	PatternNeighborIterator it = myNbrHood->Begin();
	uint16_t nbrCount = myNbrHood->GetNumberOfNeighbors();
	uint16_t numselectedNodes = 0;
	NotificationList initial_not_list;

	for(; numselectedNodes < nbrCount; ++numselectedNodes) {
		sel_dest_and_notifications.Insert(it->linkId.nodeId,initial_not_list);
		it=it.GetNext();
	}

}

void FragTest::FragmentationTest::ReplacePayload(uint32_t event){
	if(isMsgIdAssigned){
		auto embed_long_msg_ptr = new LongMessage;
		embed_long_msg_ptr->SetMsg(MSG_EMBED);

		FRAMEWORK->ReplacePayloadRequest(PID, assignedMsgId, embed_long_msg_ptr, sizeof(LongMessage)) ;
	}
}

void FragTest::FragmentationTest::AddDest(uint32_t event){
	if(isMsgIdAssigned){

		uint16_t destarray2[2];
		destarray2[0] = sel_dest_and_notifications.RBegin()->First();
		uint16_t numneighbors = 1;
		FRAMEWORK->AddDestinationRequest(PID,assignedMsgId,destarray2,numneighbors);
	}

}

void FragTest::FragmentationTest::RemoveDest(uint32_t event){
	if(isMsgIdAssigned){

		uint16_t destarray2[2];
		destarray2[0] = sel_dest_and_notifications.RBegin()->First();
		uint16_t numneighbors = 1;
		FRAMEWORK->CancelDataRequest(PID,assignedMsgId,destarray2,numneighbors);
	}

}

void FragTest::FragmentationTest::Handle_AddDestinationResponse(ControlResponseParam response){
	Debug_Printf(DBG_PATTERN,"FragmentationTest::Handle_AddDestinationResponse \n");
	AddDestinationResponse_Data* data_ptr = reinterpret_cast<AddDestinationResponse_Data*>(response.data);

	for(uint8_t index = 0; index < data_ptr->noOfDest; ++index){

		auto sel_dest_iter = sel_dest_and_notifications.Find(data_ptr->destArray[index]);

		if(sel_dest_iter != sel_dest_and_notifications.End()){
			if(sel_dest_iter->Second().list[0] == false){
				FPRINT_TEST_RESULT("FragmentationTest::Handle_AddDestinationResponse::PASS Add destination request for message id = %d is accepted by the framework for dest = %d \n",data_ptr->msgId, data_ptr->destArray[index]);
				sel_dest_iter->Second().list[0] = true;
			}
			else{
				FPRINT_TEST_RESULT("FragmentationTest::Handle_AddDestinationResponse::FAIL message id = %d, dest = %d was already accepted by the FW \n",data_ptr->msgId, data_ptr->destArray[index]);
			}
		}
		else{
			FPRINT_TEST_RESULT("FragmentationTest::Handle_AddDestinationResponse::FAIL Unknown destination for message id = %d , dest = %d does not exist in  sel_dest_and_notifications  \n",data_ptr->msgId, data_ptr->destArray[index]);

		}
	}


};

void FragTest::FragmentationTest::Handle_CancelDataResponse(ControlResponseParam response){
	Debug_Printf(DBG_PATTERN,"FragmentationTest::Handle_CancelDataResponse \n");
	auto data_ptr = reinterpret_cast<CancelDataResponse_Data*>(response.data);
	addDestTimer->Start();
}

void FragTest::FragmentationTest::Handle_ReplacePayloadResponse(ControlResponseParam response){
	Debug_Printf(DBG_PATTERN,"FragmentationTest::Handle_ReplacePayloadResponse \n");
	auto data_ptr = reinterpret_cast<ReplacePayloadResponse_Data*>(response.data);
	removeDestTimer->Start();
};

void FragTest::FragmentationTest::Send(uint32_t event){
	//Select destinations
//	NodeId_t selnode = myNbrHood->Begin()->linkId.nodeId;
	SelectAllDestinationNodes();
	if(sel_dest_and_notifications.Size()){
		FPRINT_TEST_RESULT( "FragmentationTest::PASS Some neighbors have been selected as dest \n"  );

		NodeId_t selnode[sel_dest_and_notifications.Size()];
		uint8_t i = 0;
		for(auto it = sel_dest_and_notifications.Begin(); it != sel_dest_and_notifications.End(); ++it){
			selnode[i++] = it->First();
		}


		auto embed_long_msg_ptr = new LongMessage;
		embed_long_msg_ptr->SetMsg(MSG_EMBED-1);

		FMessage_t *sendMsg = new FMessage_t();
		sendMsg->SetPayloadSize(sizeof(LongMessage));
		sendMsg->SetPayload(reinterpret_cast<uint8_t*>(embed_long_msg_ptr));

		sendMsg->SetSource(MY_NODE_ID);
		sendMsg->SetType(PATTERN_MSG);

		FRAMEWORK->SendData(PID, selnode, i, *sendMsg, nonce, false); // Send Except the lst destination
		numsend++;

	}
	else{
		FPRINT_TEST_RESULT( "FragmentationTest::FAIL No neighbors to choose as dest  \n");
	}

}

void FragTest::FragmentationTest::ReceiveMessageEvent(FMessage_t& msg){
	isMsgReceived = true;

	if(msg.GetPayloadSize() == sizeof(LongMessage)) {
		FPRINT_TEST_RESULT( "FragmentationTest::PASS Received message size is CORRECT.  \n");
	}
	else {
		printf ("FragmentationTest::FAIL Received message size is INVALID. There might be an error in assembly \n");
		FPRINT_TEST_RESULT( "FragmentationTest::FAIL Received message size is INVALID. There might be an error in assembly  \n");
		return ;
	}

	LongMessage* rx_long_msg_ptr = reinterpret_cast<LongMessage*>(msg.GetPayload());
	IsRxMsgValid(rx_long_msg_ptr->array, MSGSIZE);
}

bool FragTest::FragmentationTest::IsRxMsgValid(void *data, uint16_t size ){

	printf ("FragmentationTest::IsRxMsgValid Issued \n");
	FPRINT_TEST_RESULT( "FragmentationTest::IsRxMsgValid Issued  \n");


	RepeatedBasicMsgType* rxdata = static_cast<RepeatedBasicMsgType*> (data);
	for(uint16_t i = 0; i < MSGSIZE; ++i){
		if(rxdata[i] != MSG_EMBED) {
			printf ("FragmentationTest::FAIL Received message does not match. There might be an error in assembly \n");
			FPRINT_TEST_RESULT( "FragmentationTest::FAIL Received message does not match. There might be an error in assembly  \n");
			return false;
		}
	}

	printf ("FragmentationTest::PASS Received message is valid \n");
	FPRINT_TEST_RESULT( "FragmentationTest::PASS Received message is valid  \n");
	return true;
}
