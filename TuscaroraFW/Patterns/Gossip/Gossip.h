////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef GOSSIP_H_
#define GOSSIP_H_

#include <Base/BasicTypes.h>
#include <Interfaces/Pattern/PatternBase.h>
//#include <Lib/Math/Rand.h>
#include <PAL/PseudoRandom/UniformRandomInt.h>

#include "Lib/PAL/PAL_Lib.h"//include the PAL layer
#include <Interfaces/AWI/Gossip_I.h>
#include "Lib/DS/AVLBinarySearchTreeT.h"

//#include "Lib/PAL/PAL_Lib.h"
#include "Framework/Core/Naming/StaticNaming.h"
#include "Lib/Pattern/NeighborTable/PatternNeighborTable.h"
#include <stdio.h>
#include <vector>

#ifdef PLATFORM_DCE
#include <Platform/dce/Pattern/Gossip2AppShim.h>
#elif PLATFORM_LINUX
#include <Platform/linux/Pattern/Gossip2AppShim.h>
#endif


extern uint16_t MY_NODE_ID;

using namespace Core::Dataflow;
using namespace PWI;
using namespace AWI;


#define MeanUpdateInterval 200000
#define RangeUpdateInterval 100000
#define ReSendTimeOutInterval 2000


template<class T>
struct GossipMsg {
	//whatever you want to send
	T currentStatusId;
};

namespace Patterns {


typedef AVLBSTElement<NodeId_t> GossipNeigborContainerTypeElement;
typedef AVLBST_T<NodeId_t> GossipNeigborContainerType;

class GossipLinkComparator: public LinkComparatorI {
public:
	//GossipLinkComparator();
	bool BetterThan (Core::LinkMetrics& A, Core::LinkMetrics& B){
		return (A.quality > B.quality);
	}
};

template<typename GOSSIPVARIABLE, typename GOSSIPCOMPARATOR = SimpleGenericComparator<GOSSIPVARIABLE> >
class Gossip : public PatternBase, public Gossip_I<GOSSIPVARIABLE, GOSSIPCOMPARATOR>{
private:
	typedef typename Gossip_I<GOSSIPVARIABLE, GOSSIPCOMPARATOR>::GossipVariableUpdateDelegate_t GossipVariableUpdateDelegate_t_l;
	//ListT<GossipVariableUpdateDelegate_t*, true, SimpleGenericComparator<GossipVariableUpdateDelegate_t>> list_gvu_delegates;
	GossipVariableUpdateDelegate_t_l* gvu_delegate;

	//	std::vector<Gossip2AppShim_I<GOSSIPVARIABLE>*> eventdispatcher_ptrs_vector;
	Gossip2AppShim_I<GOSSIPVARIABLE>* eventdispatcher;


	GOSSIPCOMPARATOR gossip_comparator;
	GOSSIPVARIABLE currentStatusId; //Status of the gossip protocol
	FrameworkAttributes fwAttributes;
	uint32_t nonce;
	//Pattern Specific
	UniformRandomInt *rand; //A random number generator
	Event *randEvent_SendUpdate;	//A pointer for the event class used to send random updates
	Timer *randEvent_ReSendUpdate;	//A pointer for the event class used to reSendPreviousPacket when no response is received after a timeout period
	EventDelegate *eventDel_SendUpdate;//delegate for the random data send event.
	TimerDelegate *eventDel_ReSendUpdate;//reSendPreviousPacket when no response is received after a timeout period

	void RandomSendHandler(EventDelegateParam param); //Handler for the event generator used to send random updates
	void ReSendHandler(uint32_t event); //Handler for the event generator used to send random updates

	PatternNeighborTableI *myNbrHood;
	//GossipLinkComparator *myLinkComparator;

	GossipNeigborContainerType SelectedNeighborList; //Set of selected neighbors to send a status update

	FMessage_t& PrepareStatusUpdate();	//Broadcast your status
	void SendMessage();      //Sends a message to the list of selected neighbors
	bool hasFWRejectedPacket;

	bool SelectNeighbors(NodeId_t nbr); //Add a neighbor to the set of selected neighbors
	void AdjustSelectedNeighbors(); //Make sure at least one neighbor is selected
	void ClearSelectedNeighborList(); //Clear the list of selected neighbors

	NodeId_t IterateThroughNeighbors(uint16_t table_index);
	bool InitiateProtocol();
	void Handle_AttributeResponse(ControlResponseParam response);
	void Handle_LinkThresholdResponse(ControlResponseParam response);
	void Handle_SelectDataNofiticationResponse(ControlResponseParam response);

	void SetNeighborhoodandLinkComparator();
	void RequestDataNotifications();
	void InvokeAppsForGossipVariableUpdate(){
		//		for(typename AppDelegatesList_t_l::iterator it = list_gvu_delegates.begin(); it != list_gvu_delegates.end(); ++it){
		//			(*it)->operator()(currentStatusId);
		//		}
		eventdispatcher -> ReceiveUpdatedGossipVariable(currentStatusId);
	};
public:

	//Inherited from Gossip_I
	void RegisterGossipVariableUpdateDelegate(GossipVariableUpdateDelegate_t_l* _gvu_del);
	void UpdateGossipVariable(GOSSIPVARIABLE& initialGossipVariable);

	//Common to most patterns
	Gossip(const GOSSIPVARIABLE& initialGossipVariable);	//constructor

	bool Start();  //Lets the test code or network admin start the pattern
	bool Stop(); //Lets the test code or network admin stop the pattern

	void NeighborUpdateEvent (NeighborUpdateParam nbrUpdate);
	void ControlResponseEvent (ControlResponseParam response);
	void DataStatusEvent (DataStatusParam notification);
	void ReceiveMessageEvent (FMessage_t& msg);

};


//Gossip<GOSSIPVARIABLE, GOSSIPCOMPARATOR>::Gossip(const GOSSIPVARIABLE& initialGossipVariable, Gossip2AppShim_I<GOSSIPVARIABLE>* _eventdispatcher) : PatternBase(Core::Naming::GOSSIP_PTN, (char*)"Gossip_1"){
template<typename GOSSIPVARIABLE, typename GOSSIPCOMPARATOR>
Gossip<GOSSIPVARIABLE, GOSSIPCOMPARATOR>::Gossip(const GOSSIPVARIABLE& initialGossipVariable) : PatternBase(Core::Naming::GOSSIP_PTN, (char*)"Gossip_1"){
	Debug_Printf(DBG_PATTERN, "Gossip::Initializing\n");


	//delegate handles to the events
	eventDel_SendUpdate = new EventDelegate(this, &Gossip::RandomSendHandler);
	eventDel_ReSendUpdate = new TimerDelegate(this, &Gossip::ReSendHandler);


	//Creates a uniform random number with mean 200,000 and with range +/_100,000
	//That is the values vary between 100,000 to 300,000
	UniformRNGState _state;
	UniformIntDistParameter dist;
	dist.min = MeanUpdateInterval - RangeUpdateInterval;
	dist.max = RangeUpdateInterval + RangeUpdateInterval;
	_state.SetDistributionParameters(dist);
	rand = new UniformRandomInt(_state);

	//Initialize pattern neighbor table
	myNbrHood = new PatternNeighborTable(QUALITY_LC);

	//Initialize variables facilitating message passing
	nonce=1;
	hasFWRejectedPacket = false;

	//Initialize Gossip status
	currentStatusId = initialGossipVariable;

	//eventdispatcher = _eventdispatcher;
	eventdispatcher = new Gossip2AppShim<GOSSIPVARIABLE,GOSSIPCOMPARATOR>(this);
}


template<typename GOSSIPVARIABLE, typename GOSSIPCOMPARATOR>
void Gossip<GOSSIPVARIABLE, GOSSIPCOMPARATOR>::UpdateGossipVariable(GOSSIPVARIABLE& initialGossipVariable){
	if(gossip_comparator.LessThan(currentStatusId, initialGossipVariable)) {
		currentStatusId = initialGossipVariable;
		InvokeAppsForGossipVariableUpdate();
	}
};

template<typename GOSSIPVARIABLE, typename GOSSIPCOMPARATOR>
void Gossip<GOSSIPVARIABLE, GOSSIPCOMPARATOR>::RegisterGossipVariableUpdateDelegate(GossipVariableUpdateDelegate_t_l* _gvu_del){
	 //list_gvu_delegates.push_back(_gvu_del);
	eventdispatcher->RegisterGossipVariableUpdateDelegate(_gvu_del);
};

template<typename GOSSIPVARIABLE, typename GOSSIPCOMPARATOR>
void Gossip<GOSSIPVARIABLE, GOSSIPCOMPARATOR>::ControlResponseEvent(ControlResponseParam response)
{
	Debug_Printf(DBG_PATTERN, "Gossip:: Got a control response of type %d\n", response.type);
	switch (patternState){
	case NO_PID:
	case GOT_PID:
		if(response.type == PWI::PTN_RegisterResponse){
			--n_ExpectedFrameworkResponses;
			Handle_RegisterResponse(response);
			FRAMEWORK->FrameworkAttributesRequest(PID);
			++n_ExpectedFrameworkResponses;
		}else {
			Debug_Printf(DBG_PATTERN, "Gossip:: ControlResponseEvent: My state is %d I got wrong response type %d\n", patternState, response.type);
		}
		break;
	case REGISTERED:
		if(response.type == PWI::PTN_AttributeResponse){
			--n_ExpectedFrameworkResponses;
			Handle_AttributeResponse(response);
			InitiateProtocol();

		}else {
			Debug_Printf(DBG_PATTERN, "Gossip:: ControlResponseEvent: My state is %d, I got wrong response type %d\n", patternState,response.type);
		}
		break;
	case EXECUTING:
		switch(response.type){
		case PTN_SelectDataNotificationResponse:
			Handle_SelectDataNofiticationResponse(response);
			--n_ExpectedFrameworkResponses;
			break;
		case PTN_SetLinkThresholdResponse:
			Handle_LinkThresholdResponse(response);
			--n_ExpectedFrameworkResponses;
			break;
		case PTN_AttributeResponse:
			--n_ExpectedFrameworkResponses;
			Handle_AttributeResponse(response);
			InitiateProtocol();
			break;
		case PTN_AddDestinationResponse:
		case PTN_ReplacePayloadResponse:
		case PTN_CancelDataResponse:
		default:
			Debug_Printf(DBG_PATTERN, "Gossip:: ControlResponseEvent: My state is %d, I got wrong response type %d\n", patternState,response.type);
			break;
		}
		break;
		default:
			break;
	}
}

template<typename GOSSIPVARIABLE, typename GOSSIPCOMPARATOR>
void Gossip<GOSSIPVARIABLE, GOSSIPCOMPARATOR>::DataStatusEvent(DataStatusParam ntfy)
{
	///You have received ack for previous message.
	//check what happened to the previous message
	Debug_Printf(DBG_PATTERN,"Gossip:: Received a data notification from framework\n");
	if (nonce == ntfy.nonce) randEvent_ReSendUpdate->Suspend();
	for (uint16_t i=0; i< ntfy.noOfDest; i++){
		switch (ntfy.statusType[i]){
		case PDN_FW_RECV:
			if(ntfy.statusValue[i] ) {
				Debug_Printf(DBG_PATTERN,"Gossip:: Message with nonce = %d is assigned a message id = %d and is accepted by the framework \n", ntfy.nonce, ntfy.messageId);
				ClearSelectedNeighborList();
				nonce++;
			}
			else {
				Debug_Printf(DBG_PATTERN,"Gossip:: Message with nonce = %d is rejected by the framework\n", ntfy.nonce);
				if (ntfy.readyToReceive) {
					SendMessage();
				}
				else {
					hasFWRejectedPacket = true;
				}
			}
			break;
		case PDN_WF_RECV:
			break;
		case PDN_WF_SENT:
			Debug_Printf(DBG_PATTERN,"Gossip:: Waveform has sent the message id: %d\n", ntfy.messageId);
			break;
		case PDN_DST_RECV:
			if(!ntfy.statusValue[i]){
				Debug_Printf(DBG_PATTERN,"Gossip:: Message id %d not received by destination\n", ntfy.messageId);
				SelectNeighbors(ntfy.destArray[i]);

			}
			break;

		default:
			break;
		}
	}
	if (ntfy.readyToReceive && hasFWRejectedPacket) { //If the previous message was successfully received by the framework
		SendMessage();
	}
}

template<typename GOSSIPVARIABLE, typename GOSSIPCOMPARATOR>
void Gossip<GOSSIPVARIABLE, GOSSIPCOMPARATOR>::NeighborUpdateEvent(NeighborUpdateParam nbrUpdate)
{
	if(myNbrHood != NULL) myNbrHood->UpdateTable(nbrUpdate);
	/*
  switch (nbrUpdate.changeType) {
  case NBR_NEW:
	  SelectNeighbors(nbrUpdate.link.linkId.nodeId);
	  break;
  case NBR_DEAD:
	  SelectedNeighborList.Delete(nbrUpdate.link.linkId.nodeId);
	  break;
  case NBR_UPDATE:
	  break;
  default:
	 Debug_Printf(DBG_PATTERN,"Gossip:: ReceiveNeighborUpdates unknown update\n");
	  break;
  }*/
}

template<typename GOSSIPVARIABLE, typename GOSSIPCOMPARATOR>
void Gossip<GOSSIPVARIABLE, GOSSIPCOMPARATOR>::ReceiveMessageEvent(FMessage_t& msg)
{
	GossipMsg<GOSSIPVARIABLE>* gMsg = (GossipMsg<GOSSIPVARIABLE>*) msg.GetPayload();
	Debug_Printf(DBG_PATTERN, "Gossip:: Received msg from %u with status seq %u \n",msg.GetSource(), gMsg->currentStatusId);
	//Process the recevied message
	if(gossip_comparator.LessThan(currentStatusId, gMsg->currentStatusId)) {
		currentStatusId = gMsg->currentStatusId;
		InvokeAppsForGossipVariableUpdate();
	}
	else if(!gossip_comparator.EqualTo(currentStatusId,gMsg->currentStatusId)) {
		SelectNeighbors(msg.GetSource());
	}
}

template<typename GOSSIPVARIABLE, typename GOSSIPCOMPARATOR>
void Gossip<GOSSIPVARIABLE, GOSSIPCOMPARATOR>::Handle_AttributeResponse(ControlResponseParam response){
	FrameworkAttributes *atr = (FrameworkAttributes*) response.data;
	fwAttributes = *atr;

	if( patternState == REGISTERED) {
		patternState = EXECUTING;
	}
	else {
		Debug_Printf(DBG_PATTERN, "PatternBase::Pattern getting initiated without registration.\n");
	}

	Debug_Printf(DBG_PATTERN,"Gossip:: Handle_AttributeResponse: Framework supports %d waveforms, with max pkt size is %d .\n", fwAttributes.numberOfWaveforms, fwAttributes.maxFrameworkPacketSize);
	for(uint i=0; i<fwAttributes.numberOfWaveforms; i++){
		Debug_Printf(DBG_PATTERN, "Gossip::Handle_AttributeResponse: Waveform %d Id is %d\n", i, fwAttributes.waveformIds[i]);
	}
}

template<typename GOSSIPVARIABLE, typename GOSSIPCOMPARATOR>
void Gossip<GOSSIPVARIABLE, GOSSIPCOMPARATOR>::Handle_LinkThresholdResponse(ControlResponseParam response){
	SetLinkThresholdResponse_Data *data = (SetLinkThresholdResponse_Data*) response.data;
	if(data->status){
		Debug_Printf(DBG_PATTERN,"Gossip:: SetLinkThresholdResponse:  succeess \n");
		requestState = NONE_PENDING;
		patternState = EXECUTING;
	}else {
		Debug_Printf(DBG_PATTERN,"Gossip:: SetLinkThresholdResponse: Failed???, shouldnt \n");
		SetNeighborhoodandLinkComparator();
	}
}

template<typename GOSSIPVARIABLE, typename GOSSIPCOMPARATOR>
void Gossip<GOSSIPVARIABLE, GOSSIPCOMPARATOR>::Handle_SelectDataNofiticationResponse(ControlResponseParam response){
	SelectDataNotificationResponse_Data *data = (SelectDataNotificationResponse_Data*) response.data;
	if(data->status){
		Debug_Printf(DBG_PATTERN,"Gossip:: SelectDataNotificationResponse:  succeess \n");
	}else {
		Debug_Printf(DBG_PATTERN,"Gossip:: SelectDataNotificationResponse: Failed???, shouldnt \n");
		RequestDataNotifications();
	}

}

template<typename GOSSIPVARIABLE, typename GOSSIPCOMPARATOR>
bool Gossip<GOSSIPVARIABLE, GOSSIPCOMPARATOR>::Start(){
	Debug_Printf(DBG_PATTERN, "Gossip::Start Starting Gossip Pattern \n");
	Debug_Printf(DBG_PATTERN, "Gossip:: Sending the RegisterPattern Request to framework...\n");

	FRAMEWORK->RegisterPatternRequest (PID, uniqueName, Core::Naming::GOSSIP_PTN);
	++n_ExpectedFrameworkResponses;
	return true;
}

template<typename GOSSIPVARIABLE, typename GOSSIPCOMPARATOR>
void Gossip<GOSSIPVARIABLE, GOSSIPCOMPARATOR>::SetNeighborhoodandLinkComparator(){
	FRAMEWORK->SelectLinkComparatorRequest(PID,PWI::Neighborhood::QUALITY_LC);
	++n_ExpectedFrameworkResponses;
	//FRAMEWORK->SelectLinkComparatorRequest (PID, QUALITY_LC);
	LinkMetrics myThreshold;
	myThreshold.quality = 0.10;


	FRAMEWORK->SetLinkThresholdRequest(PID,myThreshold);
	requestState = WAITING_FOR_CONTROL_RESPONSE;
	++n_ExpectedFrameworkResponses;
}

template<typename GOSSIPVARIABLE, typename GOSSIPCOMPARATOR>
void Gossip<GOSSIPVARIABLE, GOSSIPCOMPARATOR>::RequestDataNotifications(){
	uint8_t mask = PDN_RECV_DEST_MASK;
	FRAMEWORK->SelectDataNotificationRequest(PID, mask);
	requestState = WAITING_FOR_CONTROL_RESPONSE;
	++n_ExpectedFrameworkResponses;
}

template<typename GOSSIPVARIABLE, typename GOSSIPCOMPARATOR>
bool Gossip<GOSSIPVARIABLE, GOSSIPCOMPARATOR>::InitiateProtocol(){
	//Initialize neighborhood and set link comparator.
	SetNeighborhoodandLinkComparator();

	//tell framework which notifications you are interested in
	RequestDataNotifications();

	Debug_Printf(DBG_PATTERN, "Gossip::Have configured everything succeessfully. Good to go. \n");

	uint64_t eventDelay = rand->GetNext();
	//The event callback will have null parameter
	randEvent_SendUpdate = new Event(eventDelay, *eventDel_SendUpdate, (void *) 0);

	randEvent_ReSendUpdate = new Timer(ReSendTimeOutInterval, ONE_SHOT, *eventDel_ReSendUpdate);
	randEvent_ReSendUpdate->Suspend();
	return true;
}

template<typename GOSSIPVARIABLE, typename GOSSIPCOMPARATOR>
bool Gossip<GOSSIPVARIABLE, GOSSIPCOMPARATOR>::Stop()
{
	randEvent_SendUpdate->Cancel();
	randEvent_ReSendUpdate->Suspend();
	return true;
}



//Handler for the RandomSend event
template<typename GOSSIPVARIABLE, typename GOSSIPCOMPARATOR>
void Gossip<GOSSIPVARIABLE, GOSSIPCOMPARATOR>::RandomSendHandler(EventDelegateParam param){
	randEvent_ReSendUpdate->Suspend(); //If there is a  timer counting to resend the previous packet, Suspend it.
	//Lets us first reshedule the event
	uint64_t eventDelay = rand->GetNext();
	uint64_t curTime = SysTime::GetEpochTimeInMicroSec();
	param.eventPtr->ReSchedule(eventDelay,(void *) (0));
	Debug_Printf(DBG_PATTERN,"Gossip::RandomSend:: Rescheduled event to fire at %lu\n",curTime+eventDelay);fflush(stdout);
	//Now lets broadcast our status
	AdjustSelectedNeighbors();
	if(SelectedNeighborList.Size() > 0) SendMessage(); //If I have at least one destination send this message
}

template<typename GOSSIPVARIABLE, typename GOSSIPCOMPARATOR>
void Gossip<GOSSIPVARIABLE, GOSSIPCOMPARATOR>::AdjustSelectedNeighbors() {
	if(SelectedNeighborList.Size() > 0) return; //Already selected neighbors based on other criteria
	uint16_t nbrCount = myNbrHood->GetNumberOfNeighbors();
	Debug_Printf(DBG_PATTERN,"Gossip:: Got %d neighbors  \n", nbrCount);fflush(stdout);
	if (nbrCount == 0) return;//Return if there are no neighbors to choose from
	NodeId_t sNeighbor;
	uint16_t i=0;
	while(SelectedNeighborList.Size() == 0) {
		uint64_t pickrandom = rand->GetNext();
		pickrandom = pickrandom % nbrCount;
		Debug_Printf(DBG_PATTERN,"Gossip:: Got %d neighbors and picking %lu neighbor from tabel  \n", nbrCount, pickrandom);fflush(stdout);
		PatternNeighborIterator it = myNbrHood->Begin();
		for(uint16_t ii=0; ii< nbrCount; ii++) {
			if(pickrandom == 0) {
				sNeighbor = (it->linkId.nodeId);
			}
			--pickrandom;
			Debug_Printf(DBG_PATTERN,"Gossip:: Intering neighbortable inde %d neighbor is %d \n",ii, it->linkId.nodeId);fflush(stdout);
			it=it.GetNext();
		}
		/*if(pickrandom == 0) {
			sNeighbor = (it->linkId.nodeId);
		}*/


		Debug_Printf(DBG_PATTERN,"Gossip:: Selected %d neighbor \n", sNeighbor);fflush(stdout);
		if( SelectNeighbors(sNeighbor) ) {
			Debug_Printf(DBG_PATTERN,"Gossip:: PickRandomNeighbor %d \n", sNeighbor);fflush(stdout);
		}
		i++;
		Debug_Printf(DBG_PATTERN,"Gossip:: in while %d times  \n", i);fflush(stdout);
	}
}

template<typename GOSSIPVARIABLE, typename GOSSIPCOMPARATOR>
bool Gossip<GOSSIPVARIABLE, GOSSIPCOMPARATOR>::SelectNeighbors(NodeId_t nbr){
	if( myNbrHood->GetNeighborLink(nbr) && !(SelectedNeighborList.Search(nbr)) ) { //If the node is my neighbor and not selected before
		Debug_Printf(DBG_PATTERN,"Gossip:: SelectNeighbor: Ading neighbor %d neighbors  to SelectedNeighborList \n", nbr);fflush(stdout);
		return SelectedNeighborList.Insert(nbr);
	}
	return false;
}

// This function broadcast status
template<typename GOSSIPVARIABLE, typename GOSSIPCOMPARATOR>
FMessage_t& Gossip<GOSSIPVARIABLE, GOSSIPCOMPARATOR>::PrepareStatusUpdate(){
	Debug_Printf(DBG_PATTERN, "Gossip:: PrepareStatusUpdate\n");

	//Construct packet and send
	FMessage_t *sendMsg;
	//Make sure to use new() operator to create packet on heap.
	sendMsg = new FMessage_t(sizeof(struct GossipMsg<GOSSIPVARIABLE>));
	//sendMsg->SetSource(MY_NODE_ID);
	//sendMsg->SetInstance(PID);

	GossipMsg<GOSSIPVARIABLE>* gMsg = (GossipMsg<GOSSIPVARIABLE>*) sendMsg->GetPayload();
	gMsg->currentStatusId=currentStatusId;
	return (*sendMsg);
}

template<typename GOSSIPVARIABLE, typename GOSSIPCOMPARATOR>
void Gossip<GOSSIPVARIABLE, GOSSIPCOMPARATOR>::ReSendHandler(uint32_t event){
	if(SelectedNeighborList.Size() > 0) SendMessage();
	else Debug_Printf(DBG_PATTERN, "Gossip::ReSendHandler. SelectedNeighborList.Size() = %d . Should not happen! Recheck algorithm! \n", SelectedNeighborList.Size()); fflush(stdout);
}

// This function sends a message to the list of selected neighbors
template<typename GOSSIPVARIABLE, typename GOSSIPCOMPARATOR>
void Gossip<GOSSIPVARIABLE, GOSSIPCOMPARATOR>::SendMessage() {
	if(SelectedNeighborList.Size() == 0) {
		Debug_Printf(DBG_PATTERN, "Gossip::SendMessage to %d destinations. Should not happen! Recheck algorithm! \n", SelectedNeighborList.Size()); fflush(stdout);
		return;
	}
	NodeId_t selnodes[SelectedNeighborList.Size()];
	GossipNeigborContainerTypeElement* elptr = SelectedNeighborList.Begin();
	int i=0;
	while(elptr){
		selnodes[i] = elptr->GetData();
		elptr = SelectedNeighborList.Next(elptr);
		++i;
	}
	Debug_Printf(DBG_PATTERN, "Gossip::SendMessage multicasting to %d destinations \n", SelectedNeighborList.Size()); fflush(stdout);
	FRAMEWORK->SendData(PID, selnodes, (uint16_t) SelectedNeighborList.Size(), PrepareStatusUpdate(), nonce,false);
	randEvent_ReSendUpdate->Change(ReSendTimeOutInterval, ONE_SHOT);
	hasFWRejectedPacket = false;
}

template<typename GOSSIPVARIABLE, typename GOSSIPCOMPARATOR>
void Gossip<GOSSIPVARIABLE, GOSSIPCOMPARATOR>::ClearSelectedNeighborList(){
	GossipNeigborContainerTypeElement *elptr,*elptr2;
	elptr = SelectedNeighborList.Begin();
	while(SelectedNeighborList.Size()>0){
		elptr2 = SelectedNeighborList.Next(elptr);
		SelectedNeighborList.DeleteElement(elptr);
		elptr = elptr2;
	}
}

}	//End of namespace


#endif //GOSSIP_H_
