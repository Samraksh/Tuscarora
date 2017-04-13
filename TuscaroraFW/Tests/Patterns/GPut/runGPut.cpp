////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#include <Sys/Run.h>
#include "Tests/FW_Init.h"
#include "runGPut.h"

//extern bool DBG_WAVEFORM;
//extern bool DBG_CORE;
//extern bool DBG_CORE_ESTIMATION;
//extern bool DBG_TEST;
//extern bool DBG_CORE_DATAFLOW;

GPut::GPut()
: PatternBase(Core::Naming::NON_STD_PTN, (char*)"GPutTest_1")
{ 
	Debug_Printf(DBG_TEST, "GPut:: Constructor..\n");
	MY_NODE_ID = atoi(getenv("NODEID"));
	seqNumber=0;
	nonce = 0;
	myNeighborTable = new PatternNeighborTable(QUAL_XMIT_DELAY_LC);
	Debug_Printf(DBG_TEST, "GPut:: Constructor done. My node id is %d, my pattern id is :%d \n", MY_NODE_ID, PID);
}


void GPut::Execute(RuntimeOpts *opts){

	///////////////////Do special things for this test////////////////

	//Register the pattern instance
	FRAMEWORK->RegisterPatternRequest (PID, uniqueName, Core::Naming::NON_STD_PTN);
	//Start();
}


//Pick a neighbor, send generate a random message, send to neighbor, wait for knowledgement
bool GPut::Start()
{
	//timerDelegate = new TimerDelegate (this, &GPut::LocalMsgTimer_Handler);
	//msgTimer = new Timer(1000000, PERIODIC, *timerDelegate);
	//msgTimer->Start();

	Debug_Printf(DBG_TEST,"GPut:: Starting protocol operation.. \n");
	FRAMEWORK->SelectLinkComparatorRequest(PID,PWI::Neighborhood::QUAL_XMIT_DELAY_LC);

	//LinkMetrics myThreshold;
	//myThreshold.quality = 0.10;
	//FRAMEWORK->SetLinkThresholdRequest(PID,myThreshold);

	SendBroadcast();
	return true;
}

bool GPut::Stop()
{
	return true;
}

void GPut::ControlResponseEvent(ControlResponseParam response)
{
	Debug_Printf(DBG_TEST,"GPut:: Control response event for message \n");

	switch (response.type){
	case PTN_RegisterResponse:
		Handle_RegisterResponse(response);
		if (patternState == REGISTERED) FRAMEWORK->FrameworkAttributesRequest(PID);
		else {
			Debug_Printf(DBG_TEST,"GPut:: Framework DENIED REGISTRATION \n");
		}
		break;
	case PTN_AddDestinationResponse:
		break;
	case PTN_ReplacePayloadResponse:
		break;
	case PTN_CancelDataResponse:
		break;
	case PTN_SelectDataNotificationResponse:
		break;
	case PTN_SetLinkThresholdResponse:

		break;
	case PTN_AttributeResponse:
		Handle_AttributeResponse(response);
		Start();
		break;
	default:
		break;
	}
}


void GPut::NeighborUpdateEvent(NeighborUpdateParam nbrUpdate)
{
	myNeighborTable->UpdateTable(nbrUpdate);
}

void GPut::DataStatusEvent(DataStatusParam notify)
{
	Debug_Printf(DBG_TEST,"GPut:: Data Notification for nonce = %d, message %d\n", notify.nonce, notify.messageId);
	switch(notify.statusType[0]){
		case PDN_ERROR:
			break;
		case PDN_FW_RECV:
			if(notify.statusValue[0]) {
				Debug_Printf(DBG_TEST,"GPut:: Framework accepted packet, gave me new  message Id %d\n",notify.messageId);
			}else {
				Debug_Printf(DBG_TEST,"GPut:: Framework rejected the message %d\n", notify.messageId);
			}
			break;
		case PDN_WF_RECV:
			if(notify.statusValue[0]) {
				Debug_Printf(DBG_TEST,"GPut:: Framework sent the message Id %d to waveform\n",notify.messageId);
			}else {
				Debug_Printf(DBG_TEST,"GPut:: Framework has failed to send the message %d, neighbor %d probabaly doesnt exist\n", notify.messageId, notify.destArray[0]);
			}
			break;
		case PDN_WF_SENT:
			if(notify.statusValue[0]) {
				Debug_Printf(DBG_TEST,"GPut:: Waveform has sent out the message Id %d to dest %d\n",notify.messageId, notify.destArray[0]);
			}else {
				Debug_Printf(DBG_TEST,"GPut:: Waveform has failed to send the message %d\n", notify.messageId);
			}
			break;
		case PDN_DST_RECV:
			for (int i=0; i < notify.noOfDest; i++){
				if(notify.statusValue[0]){
					Debug_Printf(DBG_TEST,"GPut:: Received Successful DEST Ack for nbr %d and message id %d\n",notify.destArray[i], notify.messageId);
				}else {
					Debug_Printf(DBG_TEST,"GPut:: Received Failed DEST Ack for nbr %d and message id %d\n",notify.destArray[i], notify.messageId);
				}
			}
			break;
		case PDN_BROADCAST_NOT_SUPPORTED:
			break;
		default:
			break;
	}
	if(notify.readyToReceive ) {SendBroadcast();}
}

void GPut::ReceiveMessageEvent(FMessage_t& msg)
{
	RandMsg *rcvmsg = (RandMsg *) msg.GetPayload();
	Debug_Printf(DBG_TEST,"GPut:: Received a Message %d from node %d on Waveform %d\n", rcvmsg->seqNo, msg.GetSource(), msg.GetWaveform());
	//delete (&msg);

	//PrintMsg(msg);
}

void GPut::LocalMsgTimer_Handler(uint32_t event)
{
	Debug_Printf(DBG_TEST,"GPut:: Timer handler\n");

}


void GPut::Handle_AttributeResponse(ControlResponseParam response){
	FrameworkAttributes *atr = (FrameworkAttributes*) response.data;
	fwAttributes = *atr;
	requestState = NONE_PENDING;
	patternState = EXECUTING;
	active=true;
	Debug_Printf(DBG_TEST,"GPut:: Handle_AttributeResponse: Framework supports %d waveforms, with max pkt size is %d .\n", fwAttributes.numberOfWaveforms, fwAttributes.maxFrameworkPacketSize);
	for(uint i=0; i<fwAttributes.numberOfWaveforms; i++){
		Debug_Printf(DBG_TEST, "GPut::Handle_AttributeResponse: Waveform %d Id is %d\n", i, fwAttributes.waveformIds[i]);
	}
}


void GPut::SendBroadcast (){

	if(patternState == EXECUTING){

		uint16_t maxPayload = fwAttributes.maxFrameworkPacketSize;
		FMessage_t *sendMsg;
		sendMsg = new FMessage_t(maxPayload);
		sendMsg->SetType(Types::PATTERN_MSG);
		//uint8_t *dataPtr = (uint8_t *) sendMsg->GetPayload();
		sendMsg->SetPayloadSize(maxPayload);
		sendMsg->SetWaveform(fwAttributes.waveformIds[0]);

		//Send
		Debug_Printf(DBG_TEST,"GPut:: Sending broadcast data of payload size %d, on waveform %d with nonce %d\n",sendMsg->GetPayloadSize(), sendMsg->GetWaveform(), ++nonce);
#if ENABLE_FW_BROADCAST==1
		FRAMEWORK->BroadcastData(PID, *sendMsg, sendMsg->GetWaveform(), nonce);
#else
		this->RandomLocalSpray(PID, *sendMsg, SPRAY_THREE_NBR, true, *myNeighborTable, nonce);
#endif

	}else {
		Debug_Printf(DBG_TEST,"GPut:: Sorry!! Pattern Initialization is not complete. \n");
	}
	fflush(stdout);
}

void GPut::LookUpNeighborsAndSendUnicast(){
	//Get a the strongest neighbor.
	uint16_t noOfnbrs = myNeighborTable->GetNumberOfNeighbors() ;
	if (noOfnbrs > 0){
		NodeId_t destArray[noOfnbrs];
		//Debug_Printf(DBG_TEST,"I have %d neighbor\n", noOfnbrs );
		PatternNeighborIterator nbrItr = myNeighborTable->Begin();
		Debug_Printf(DBG_TEST,"GPut:: Looked up first neighbor Id is %d, on waveform %d, with quality %0.5f\n", nbrItr->linkId.nodeId, nbrItr->linkId.waveformId ,nbrItr->metrics.quality );fflush(stdout);
		uint16_t i=0;
		destArray[i]=nbrItr->linkId.nodeId;
		//printf("Neighbor copied is %d\n",destArray[i]);
		while (nbrItr != myNeighborTable->End()){
			nbrItr = nbrItr.GetNext();
			i++;
			Debug_Printf(DBG_TEST,"GPut:: Looked up next neighbor Id is %d, on waveform %d, with quality %0.5f\n", nbrItr->linkId.nodeId, nbrItr->linkId.waveformId, nbrItr->metrics.quality);
			destArray[i]=nbrItr->linkId.nodeId;
			//printf("Neighbor copied is %d\n",destArray[i]);
		}

		//Add a dummy neighbor, to see if you get negative acks
		i++;
		destArray[i]=2222;
		noOfnbrs++;

		//Create and send a message to neighbors
		sendMsg = new FMessage_t(sizeof(class RandMsg));
		RandMsg *randMsg = (RandMsg*) sendMsg->GetPayload();
		randMsg->src = MY_NODE_ID;
		randMsg->seqNo=seqNumber;
		//Debug_Printf(DBG_TEST,"Neighbood test timer: %d event: Sending Message %d to node %lu \n",event, seqNumber, sndPtr->dst);
		sendMsg->SetPayloadSize(sizeof(class RandMsg));
		//sendMsg->SetInstance(PID);

		//MessageId_t mid;
		//U64NanoTime expectedTxTime;
		Debug_Printf(DBG_TEST,"GPut:: Payload is at %p,  size if %d\n",sendMsg->GetPayload(), sendMsg->GetPayloadSize());
		Debug_Printf(DBG_TEST,"GPut:: I have %d neighbor, going to send a message of size %d to the neighbor(s) in array %p a message\n", noOfnbrs, sendMsg->GetPayloadSize(), destArray );fflush(stdout);
		FRAMEWORK->SendData(PID,destArray, noOfnbrs, *sendMsg, seqNumber);
		/*if(mid>0){
      Debug_Printf(DBG_TEST,"GPut:: Framework accepted my packet and gave me a token %d\n",mid); fflush(stdout);
    }else {
      Debug_Printf(DBG_TEST,"GPut:: Framework rejected my packet :-( \n"); fflush(stdout);
    }*/

	}else {
		Debug_Printf(DBG_TEST,"GPut:: No neighbors yet...\n");
	}

	seqNumber++;
}

//int testCore(int argc, char* argv[]) {
int main(int argc, char* argv[]) {
	//Make sure RuntimeOpts construction is the very first line of main
	RuntimeOpts opts ( argc-1, argv+1 );
	InitPlatform(&opts);	//This does a platform specific initialization  

	FW_Init fwInit;
	fwInit.Execute(&opts);

	//PatternId_t pid = GetNewPatternInstanceId(NON_STD_PTN,);
	GPut ntest;

	//start the test
	ntest.Execute(&opts);

	//RunMasterLoopNeverExit(); //never exists on dce
	RunTimedMasterLoop();

	return 0;
}
