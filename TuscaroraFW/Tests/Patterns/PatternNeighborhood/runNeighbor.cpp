////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#include <Sys/Run.h>
#include "Tests/FW_Init.h"
#include "runNeighbor.h"

#define TEST_NBR_COUNT 10

// extern bool DBG_PATTERN; // set in Lib/Misc/Debug.cpp
// extern bool DBG_WAVEFORM; // set in Lib/Misc/Debug.cpp
// extern bool DBG_CORE; // set in Lib/Misc/Debug.cpp
// extern bool DBG_CORE_ESTIMATION; // set in Lib/Misc/Debug.cpp
// extern bool DBG_TEST; // set in Lib/Misc/Debug.cpp
// extern bool DBG_CORE_DATAFLOW; // set in Lib/Misc/Debug.cpp

NeighborTest::NeighborTest()
: PatternBase(Core::Naming::NON_STD_PTN, (char*)"NbrTest_1")
{ 
  Debug_Printf(DBG_TEST, "NeighborTest:: Constructor..\n");
  //MY_NODE_ID = atoi(getenv("NODEID"));
  seqNumber=0;
  
  Debug_Printf(DBG_TEST, "NeighborTest:: Constructor done. My node id is %d, my pattern id is :%d \n", MY_NODE_ID, PID);
}


void NeighborTest::Execute(RuntimeOpts *opts){

  ///////////////////Do special things for this test////////////////
  fi = static_cast<StandardFI *> (&GetFrameworkInterface());
  PolicyManager::SetLinkSelectionPolicy(Core::Policy::PREFFERED_WAVEFORM, 2);
  
  Debug_Printf(DBG_TEST, "Neighborhood TEST:: Using framework instance %p\n",fi);

  /*fi->SetLinkEstimationPeriod(opts->linkEstimationPeriod);
  fi->SetNeighborInactivityPeriod( opts->deadNeighborPeriod);
  fi->SetRange(opts->range );
  fi->runTime = opts->runTime;
  fi->density = opts->density;
  fi->mobilityModel = opts->mobilityModelInt();
  fi->speed = opts->averageSpeed();
  */
  
  //create a dummy estimator to publish some neighborhood.
  dEstimator = new DummyEstimator();
 
  //Register the dummy link estimator, for waveform id 10
  fi->RegisterLinkEstimator(*dEstimator,Core::Estimation::FW_TEST_EST, dEstimator->wid);

  //Do standard things a pattern would do.
  fi = static_cast<StandardFI *> (FRAMEWORK);
  
  Debug_Printf(DBG_TEST, "Initialized framework and link estimation. Staring the neihborhood test... \n");
  //Start the neighborhood test
 
  char pname[] ="NeighborTest"; 
  FRAMEWORK->RegisterPatternRequest(PID, pname ,Core::Naming::NON_STD_PTN);
  Start();
}


//Pick a neighbor, send generate a random message, send to neighbor, wait for knowledgement
bool NeighborTest::Start()
{
  timerDelegate = new TimerDelegate (this, &NeighborTest::LocalMsgTimer_Handler);
  msgTimer = new Timer(1000000, PERIODIC, *timerDelegate);
  msgTimer->Start();
    
  myNeighborTable = new PatternNeighborTable(QUAL_XMIT_DELAY_LC);
  
  FRAMEWORK->SelectLinkComparatorRequest(PID,PWI::Neighborhood::QUAL_XMIT_DELAY_LC);
  LinkMetrics myThreshold;
  myThreshold.quality = 0.10;
  FRAMEWORK->SetLinkThresholdRequest(PID,myThreshold);
  return true;
}

bool NeighborTest::Stop()
{
 return true;
}

void NeighborTest::ControlResponseEvent(ControlResponseParam response)
{
  Debug_Printf(DBG_TEST,"NeighborTest:: Control response event for message \n");
  
 switch (response.type){ 
  case PTN_RegisterResponse:
      Handle_RegisterResponse(response);
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
     break;
  default:
    break;
 }
}


void NeighborTest::NeighborUpdateEvent(NeighborUpdateParam nbrUpdate)
{
  //first update your local copy., then do whatever you want...
  
  myNeighborTable->UpdateTable(nbrUpdate);
  
  switch (nbrUpdate.changeType){
    case NBR_NEW:
      Debug_Printf(DBG_TEST,"NeighborTest:: Received new nbr notification for node %d\n", nbrUpdate.nodeId);
      break;
    case NBR_UPDATE:
      Debug_Printf(DBG_TEST,"NeighborTest:: Received nbr update notification for node %d\n", nbrUpdate.nodeId);
      break;
    case NBR_DEAD:
      Debug_Printf(DBG_TEST,"NeighborTest:: Received dead nbr notification for node %d\n", nbrUpdate.nodeId);
      break;
  } 
}

void NeighborTest::DataStatusEvent(DataStatusParam notify)
{
  Debug_Printf(DBG_TEST,"NeighborTest:: Data Notification for message %d\n",notify.messageId);
  for(uint16_t i=0; i< notify.noOfDest; i++){
    switch(notify.statusType[i]){
      case PDN_ERROR:
	break;
      case PDN_FW_RECV:
	if(notify.statusValue[i]) {
	  Debug_Printf(DBG_TEST,"NeighborTest:: Framework accepted packet, gave me new  message Id %d\n",notify.messageId);
	}else {
	  Debug_Printf(DBG_TEST,"NeighborTest:: Framework rejected the message %d\n", notify.messageId);
	}
      case PDN_WF_RECV:
	if(notify.statusValue[i]) {
	  Debug_Printf(DBG_TEST,"NeighborTest:: Framework send the message Id %d to waveform\n",notify.messageId);
	}else {
	  Debug_Printf(DBG_TEST,"NeighborTest:: Framework has failed to send the message %d, neighbor %d probabaly doesnt exist\n", notify.messageId, notify.destArray[0]);
	}
	break;
      case PDN_WF_SENT:
	if(notify.statusValue[i]) {
	  Debug_Printf(DBG_TEST,"NeighborTest:: Waveform has sent out the message Id %d to dest %d\n",notify.messageId, notify.destArray[0]);
	}else {
	  Debug_Printf(DBG_TEST,"NeighborTest:: Waveform has failed to send the message %d\n", notify.messageId);
	}
	break;
      case PDN_DST_RECV:
	for (int i=0; i < notify.noOfDest; i++){
	  if(notify.statusValue[i]){
	    Debug_Printf(DBG_TEST,"NeighborTest:: Received Successful DEST Ack for nbr %d and message id %d\n",notify.destArray[i], notify.messageId);
	  }else {
	    Debug_Printf(DBG_TEST,"NeighborTest:: Received Failed DEST Ack for nbr %d and message id %d\n",notify.destArray[i], notify.messageId);
	  }
	}
	break;
      case PDN_BROADCAST_NOT_SUPPORTED:
         {
         	break;
         }
    }
  }
}

void NeighborTest::ReceiveMessageEvent(FMessage_t& msg)
{
  RandMsg *rcvmsg = (RandMsg *) msg.GetPayload();
  Debug_Printf(DBG_TEST,"NeighborTest:: Received a Message %d from node %d on Waveform %d\n", rcvmsg->seqNo, msg.GetSource(), msg.GetWaveform());
  //delete (&msg);
  
  //PrintMsg(msg);
}

void NeighborTest::LocalMsgTimer_Handler(uint32_t event)
{
  Debug_Printf(DBG_TEST,"NeighborTest:: Timer handler\n");

  //Make dummy estimator add a new neighbor to framework
  /*if(seqNumber < TEST_NBR_COUNT){
    //Debug_Printf(DBG_TEST,"NeighborTest:: Adding a new neighbor\n");
    dEstimator->AddNeighbor();
  }else {
    //Debug_Printf(DBG_TEST,"NeighborTest:: Removing neighbo\nr");
    dEstimator->RemoveNeighbor();
  }*/
  
  //Get a the strongest neighbor.
  uint16_t noOfnbrs = myNeighborTable->GetNumberOfNeighbors() ;
  if (noOfnbrs > 0){
    NodeId_t destArray[noOfnbrs];
    //Debug_Printf(DBG_TEST,"I have %d neighbor\n", noOfnbrs );
    PatternNeighborIterator nbrItr = myNeighborTable->Begin();
    Debug_Printf(DBG_TEST,"NeighborTest:: Looked up first neighbor Id is %d, on waveform %d, with quality %0.5f\n", nbrItr->linkId.nodeId, nbrItr->linkId.waveformId ,nbrItr->metrics.quality );fflush(stdout);
    uint16_t i=0;
    destArray[i]=nbrItr->linkId.nodeId;
    //printf("Neighbor copied is %d\n",destArray[i]);
    while (nbrItr != myNeighborTable->End()){
      nbrItr = nbrItr.GetNext();
      i++;
      Debug_Printf(DBG_TEST,"NeighborTest:: Looked up next neighbor Id is %d, on waveform %d, with quality %0.5f\n", nbrItr->linkId.nodeId, nbrItr->linkId.waveformId, nbrItr->metrics.quality);
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
    Debug_Printf(DBG_TEST,"NeighborTest:: Payload is at %p,  size if %d\n",sendMsg->GetPayload(), sendMsg->GetPayloadSize());
    Debug_Printf(DBG_TEST,"NeighborTest:: I have %d neighbor, going to send a message of size %d to the neighbor(s) in array %p a message\n", noOfnbrs, sendMsg->GetPayloadSize(), destArray );fflush(stdout);
    FRAMEWORK->SendData(PID,destArray, noOfnbrs, *sendMsg, seqNumber);
    /*if(mid>0){
      Debug_Printf(DBG_TEST,"NeighborTest:: Framework accepted my packet and gave me a token %d\n",mid); fflush(stdout);
    }else {
      Debug_Printf(DBG_TEST,"NeighborTest:: Framework rejected my packet :-( \n"); fflush(stdout);
    }*/
    
  }else {
    Debug_Printf(DBG_TEST,"NeighborTest:: No neighbors yet...\n");
  }
  
  seqNumber++; 
}

//int testCore(int argc, char* argv[]) {
int main(int argc, char* argv[]) {
  
  RuntimeOpts opts ( argc-1, argv+1 );
  
  InitPlatform(&opts);  //This does a platform specific initialization

  //DBG_CORE_ESTIMATION = true;
  //DBG_TEST=true;
  //DBG_CORE=true;
  //DBG_PATTERN= false;//true;
  //DBG_WAVEFORM = true;
  //DBG_CORE_DATAFLOW = true;//true;
  //DBG_CORE = false;//true;

  FW_Init fwInit;
  fwInit.Execute(&opts);
//   //DBG_CORE = true; // set in Lib/Misc/Debug.cpp
  
  
  //PatternId_t pid = GetNewPatternInstanceId(NON_STD_PTN,);
  NeighborTest ntest;
  
  //start the test
  ntest.Execute(&opts);

  while(1){ sleep(1);}
  return 0;
}
