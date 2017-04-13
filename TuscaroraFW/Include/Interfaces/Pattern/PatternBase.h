////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef PATTERN_BASE_H_
#define PATTERN_BASE_H_

#include <Base/BasicTypes.h>
#include <Base/FrameworkTypes.h>
#include <Base/Delegate.h>
#include <Interfaces/PWI/Framework_I.h>
#include <Interfaces/Core/PatternNamingI.h>

//#include <Interfaces/PWI/PatternNeighborTableI.h>
#include "Lib/DS/AVLBinarySearchTreeT.h"
#include "Lib/Pattern/NeighborTable/PatternNeighborTable.h"

using namespace PAL;
using namespace PWI;
using namespace PWI::Neighborhood;

namespace Patterns {
typedef AVLBSTElement<NodeId_t> NeigborContainerElementType;
typedef AVLBST_T<NodeId_t> NeigborContainerType;

//A enum to keep track of Pattern Framework interaction state;
enum PatternStateE {
	NO_PID,
	GOT_PID,
	REGISTERED,
	EXECUTING,
	ERROR,
};

enum PatternRequestStateE {
	NONE_PENDING,
	WAITING_FOR_CONTROL_RESPONSE,
	WAITING_FOR_DATA_RESPONSE
};
///Defines the abstract base class for patterns
class PatternBase {
protected:
	/*
	 * @brief Returns a reference to the Pattern's custom neighbor table
	 *
	 * @param patternId Pattern's instance ID.
	 * @return PWI::Neighborhood::PatternNeighborTableI&. Reference to the patterns neighborhood table.

  virtual PatternNeighborTableI& GetNeighborTable(PatternId_t patternId)  = 0;
	 */

	char uniqueName[128];

protected:
	PatternId_t PID;
	Framework_I* FRAMEWORK;
	bool registered;
	PatternRequestStateE requestState;
	PatternStateE patternState;
	uint32_t n_ExpectedFrameworkResponses;
	
	UniformRandomInt *randInt;

	virtual void ReceiveMessageEvent (FMessage_t &msg)=0;
	virtual void NeighborUpdateEvent (NeighborUpdateParam nbrUpdate) =0;
	virtual void DataStatusEvent (DataStatusParam notification) =0;
	virtual void ControlResponseEvent (ControlResponseParam response) = 0;

	void RegisterPatternDelegates(PatternId_t pid, PatternTypeE _type);

	void Handle_RegisterResponse(ControlResponseParam response);


	// RandomLocalSpray is a helper function emulates broadcasting by calling Send command with a subset of neighbor IDs that are selected based on the spraytype.
	void RandomLocalSpray (PatternId_t pid, FMessage_t& msg, SprayTypeE spraytype, bool israndomselection, Patterns::PatternNeighborTableI& ptnNbrTable,  uint16_t nonce);

	// RandomlySelectNeighbor is a helper function that randomly selects one neighbor (that does not exists in selectedNeighborList) from ptnNbrTable and adds it to the selectedNeighborList. On success returns a true, and on fail it returns a false when such an operation is not
	bool RandomlySelectNeighbor(NeigborContainerType& selectedNeighborList, Patterns::PatternNeighborTableI& ptnNbrTable, UniformRandomInt *rand = NULL);

	//This is helper function that invokes send command to the list of neighbors in the selectedNeighborList
	void Send2SelectedNeighbors(PatternId_t pid, NeigborContainerType& selectedNeighborList, FMessage_t& msg,  uint16_t nonce);


public:
	RecvMessageDelegate_t *recvDelegate;
	NeighborDelegate *nbrDelegate;
	DataflowDelegate_t *dataflowDelegate;
	ControlResponseDelegate_t *controlDelegate;
	/////////////////// Registration and Instantiation Service ////////////////////
	//PatternBase(PatternTypeE type);
	PatternBase(PatternTypeE type, char _uniqueName[128]);
	///Starts the pattern, must be implemented by each pattern.
	virtual bool Start()=0;
	virtual bool Stop()=0;

	//  void RandomLocalSpray (PatternId_t pid, FMessage_t& msg, SprayTypeE spraytype, PatternNeighorTable* ptnNbrTableptr, NeighborLinkThreshold* nbrThresholdptr, bool applyThreshold);

	///Virtual destructor
	virtual ~PatternBase(){}




};




//	bool RandomlySelectNeighbor(NodeId_t& selnode, PatternNeighorTable& ptnNbrTable, UniformRandomInt *rand = NULL){
//		UniformRNGState _state;
//		uint16_t nbrCount = ptnNbrTable.GetNumberOfNeighbors();
//		if (nbrCount == 0) return false;
//		if(rand == NULL){
//			_state.min = 0;
//			_state.max = nbrCount;
//			rand = new UniformRandomInt(_state);
//		}
//		uint64_t pickrandom = rand->GetNext();
//		pickrandom = pickrandom % nbrCount;
//		PatternNeighborIterator it = ptnNbrTable.Begin();
//		while(pickrandom > 0){
//			--pickrandom;
//			++it;
//			if(it != myNbrHood->End()){
//				it = myNbrHood->Begin();
//			}
//			--pickrandom;
//		}
//		selnode = (it->linkId.nodeId);
//		return true;
//	}
//	bool RandomlySelectNeighbor(NodeId_t* selnode, PatternNeighorTable& ptnNbrTable, uint16_t num2select = 1 , UniformRandomInt *rand = NULL){
//		UniformRNGState _state;
//		uint16_t nbrCount = ptnNbrTable.GetNumberOfNeighbors();
//		if (nbrCount < num2select) return false;
//		if(rand == NULL){
//			_state.min = 0;
//			_state.max = nbrCount;
//			rand = new UniformRandomInt(_state);
//		}
//		uint64_t pickrandom = rand->GetNext();
//		pickrandom = pickrandom % nbrCount;
//		PatternNeighborIterator it = ptnNbrTable.Begin();
//		while(pickrandom > 0){
//			--pickrandom;
//			++it;
//			if(it != myNbrHood->End()){
//				it = myNbrHood->Begin();
//			}
//			--pickrandom;
//		}
//		selnode = (it->linkId.nodeId);
//		return true;
//	}

}//End of namespace

#endif /* PATTERN_BASE_H_ */



