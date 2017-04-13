////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include <Interfaces/Pattern/PatternBase.h>
#ifdef PLATFORM_DCE
#include "Platform/dce/Pattern/PatternShim.h"
#else
#include "Platform/linux/Pattern/PatternShim.h"
#endif 

using namespace PWI;


namespace Patterns {


  
PatternBase::PatternBase(PatternTypeE _type, char _uniqueName[128]){
	FRAMEWORK = &GetPatternShim();
	requestState=NONE_PENDING;

	patternState=NO_PID;
	PID = 0;
	randInt = new UniformRandomInt(0,256);

	memcpy(uniqueName,_uniqueName, 128*sizeof(char) );

	Debug_Printf(DBG_PATTERN, "PatternBase:: Constructor, got FremworkShim reference\n");
	RegisterPatternDelegates(PID, _type);
}


void PatternBase::RegisterPatternDelegates(PatternId_t pid, PatternTypeE _type)
{
	Debug_Printf(DBG_PATTERN, "PatternBase:: Registering the Pattern Shim Delegates with FremworkShim\n");

	recvDelegate = new RecvMessageDelegate_t(this, &PatternBase::ReceiveMessageEvent);
	nbrDelegate = new NeighborDelegate (this, &PatternBase::NeighborUpdateEvent);
	dataflowDelegate = new DataflowDelegate_t(this, &PatternBase::DataStatusEvent);
	controlDelegate = new ControlResponseDelegate_t (this, &PatternBase::ControlResponseEvent);
	static_cast<PatternShim*>(FRAMEWORK)->RegisterDelegates(pid, recvDelegate, nbrDelegate, dataflowDelegate, controlDelegate );
	}

/*void PatternBase::Handle_PatternIDResponse(ControlResponseParam response)
{
  if(patternState == Patterns::NO_PID){
    
    if(response.type == PTN_NewPatternInstanceResponse){
      
      Debug_Printf(DBG_PATTERN,"Neighborhood Test: Received my pattern instance number. I can start now.\n");
    }else {
      printf("I am not yet registered, waiting for my patternId");
    }  
  }
}
*/

void PatternBase::Handle_RegisterResponse(ControlResponseParam response)
{
	RegistrationResponse_Data *res = (RegistrationResponse_Data*) response.data;

	if(res->status){
		if (patternState == NO_PID) {
			PID = res->patternId;
			patternState = GOT_PID;
		}
	patternState = REGISTERED;
	Debug_Printf(DBG_PATTERN, "PatternBase::Handle_RegisterResponse: Cool. I Registered with framework. I am good to go.\n");
	}else {
	Debug_Printf(DBG_PATTERN, "PatternBase::Handle_RegisterResponse: I am doomed. I count register. Why?????\n");
	}
}


//void PatternBase::RandomLocalSpray (PatternId_t pid, FMessage_t& msg, SprayTypeE spraytype,  PatternNeighorTable& ptnNbrTable, LinkMetrics& myThreshold, LinkComparatorTypeE lcType){
//	FRAMEWORK->SendData(PID, selnodes, (uint16_t) SelectedNeighborList.Size(), PrepareStatusUpdate(), nonce,false);
//}


void PatternBase::RandomLocalSpray (PatternId_t pid, FMessage_t& msg, SprayTypeE spraytype, bool israndomselection, Patterns::PatternNeighborTableI& ptnNbrTable,  uint16_t nonce)
{
	NeigborContainerType selectedNeighborList;
	uint16_t numneighbor2beselected = 0;
	uint16_t numselected = 0;
	bool rv = true;
	if(spraytype == SPRAY_ALL_NBR) {
		for(PatternNeighborIterator it = ptnNbrTable.Begin(); it != ptnNbrTable.End() && rv; ++it ) {
			rv = selectedNeighborList.Insert(it->linkId.nodeId);
		}
	}
	else{
		if(spraytype == SPRAY_ONE_NBR){
			numneighbor2beselected = 1;
		}
		else if(spraytype == SPRAY_THREE_NBR){
			numneighbor2beselected = 3;
		}
		else if(spraytype == SPRAY_FOUR_NBR){
			numneighbor2beselected = 4;
		}
		else if(spraytype == SPRAY_FIFTYPERCENT_NBR){
			numneighbor2beselected = (ptnNbrTable.GetNumberOfNeighbors())/2;
		}
		if(israndomselection){
			for(numselected = 0; numselected < numneighbor2beselected && rv; ++numselected ) {
				rv = this->RandomlySelectNeighbor( selectedNeighborList, ptnNbrTable,randInt);
			}
		}
		else{
			for(PatternNeighborIterator it = ptnNbrTable.Begin(); it != ptnNbrTable.End() && numselected < numneighbor2beselected && rv; ++it,++numselected){
				 rv = selectedNeighborList.Insert(it->linkId.nodeId);
			}
		}
	}
	this->Send2SelectedNeighbors( pid, selectedNeighborList, msg, nonce);
}


bool PatternBase::RandomlySelectNeighbor(NeigborContainerType& selectedNeighborList, Patterns::PatternNeighborTableI& ptnNbrTable, UniformRandomInt *_rand){
	//UniformRNGState _state;
	uint16_t nbrCount = ptnNbrTable.GetNumberOfNeighbors();
	if(nbrCount == 0 || nbrCount - selectedNeighborList.Size() == 0 ) return false;

	uint64_t pickrandom=0;
	if(_rand == NULL){
	/*	UniformIntDistParameter dist;
		dist.min = 0;
		dist.max = nbrCount;
		_state.SetDistributionParameters(dist);*/
		UniformRandomInt tempRand(0,nbrCount);
		pickrandom = tempRand.GetNext();
	}
	else {
		pickrandom = _rand->GetNext();
	}
	pickrandom = pickrandom % nbrCount;
	PatternNeighborIterator it = ptnNbrTable.Begin();

	while(it != ptnNbrTable.End() && selectedNeighborList.Search(it->linkId.nodeId) != NULL) ++it;
	while(pickrandom > 0 && it != ptnNbrTable.End()){
		--pickrandom;
		while(it != ptnNbrTable.End() && selectedNeighborList.Search(it->linkId.nodeId) != NULL) ++it;
	}
	if(it != ptnNbrTable.End()){
		return selectedNeighborList.Insert(it->linkId.nodeId);
	}
	else{
		return false;
	}
}

void PatternBase::Send2SelectedNeighbors( PatternId_t pid, NeigborContainerType& selectedNeighborList, FMessage_t& msg,  uint16_t nonce){
	if(selectedNeighborList.Size() == 0 ) return;
	NodeId_t* selnodes = new NodeId_t[selectedNeighborList.Size()];
	NeigborContainerElementType * elptr = selectedNeighborList.Begin();
	int i=0;
	while(elptr){
		selnodes[i] = elptr->GetData();
		elptr = selectedNeighborList.Next(elptr);
		++i;
	}
	Debug_Printf(DBG_PATTERN,"PatternBase:: Calling Framework SendData with %d destinations\n", selectedNeighborList.Size());
	FRAMEWORK->SendData(PID, selnodes, (uint16_t) selectedNeighborList.Size(), msg, nonce, false);
}


} //End of namespace
