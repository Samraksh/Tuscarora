////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 


#ifndef PPLSC_H_
#define PPLSC_H_

#include <Base/BasicTypes.h>
#include <Interfaces/Pattern/PatternBase.h>
//#include <Lib/Math/Rand.h>
#include <PAL/PseudoRandom/UniformRandomInt.h>
#include "../../Include/PAL/FileLogger.h"

#include "Lib/PAL/PAL_Lib.h"//include the PAL layer
#include "Lib/DS/AVLBinarySearchTreeT.h"

using namespace PWI;

#define MeanUpdateInterval 20000
#define RangeUpdateInterval 10000

namespace Patterns {

typedef AVLBSTElement<NodeId_t> PplscNeigborContainerTypeElement;
typedef AVLBST_T<NodeId_t> PplscNeigborContainerType;
typedef AVLBST_T<uint32_t> PplscNonceContainerType;
typedef AVLBSTElement<NodeId_t> PplscNonceContainerTypeElement;

class PplscLinkComparator: public LinkComparatorI {
public:
	//PplscLinkComparator();
	bool BetterThan (Core::LinkMetrics& A, Core::LinkMetrics& B){
		return (A.quality > B.quality);
	}
};

class PplscRecvMsgLogElement  : public FileLogElementWTime{
	NodeId_t neighborID;
	WaveformId_t wfid;
	uint32_t msgID;
public:
	PplscRecvMsgLogElement (){};
	PplscRecvMsgLogElement( NodeId_t neighborID_, WaveformId_t wfid_, uint32_t msgID_):FileLogElementWTime(){
		neighborID = neighborID_;
		wfid = wfid_;
		msgID = msgID_;
	};
	std::string PrintHeader() const {
		std::string out("CurTime, NodeID, WaveformId, msgID ");
		return out;
	};
	std::string PrintContents() const {
	    std::ostringstream out;
	    out<< CurTime<< ", "<< neighborID<< ", "<<wfid<< ", "<< msgID;
	    return out.str();;
	};
};


class PplscSentMsgLogElement  : public FileLogElementWTime{
	NodeId_t neighborID;
	int lctype;
	uint32_t msgID;
public:
	PplscSentMsgLogElement(){};
	PplscSentMsgLogElement( NodeId_t neighborID_, int lctype_, uint32_t msgID_):FileLogElementWTime(){
		neighborID = neighborID_;
		lctype = lctype_;
		msgID = msgID_;
	};
	std::string PrintHeader() const {
		std::string out("CurTime, NodeID, LinkComparatorTypeE, msgID");
		return out;
	};
	std::string PrintContents() const{
	    std::ostringstream out;
	    out<< CurTime<< ", "<< neighborID<< ", "<<lctype<< ", "<<msgID;
	    return out.str();;
	};
};


class Pplsc : public PatternBase{
protected:
	FrameworkAttributes fwAttributes;
	uint32_t nonce;
	//Pattern Specific
	UniformRandomInt *rand; //A random number generator
	Event *randEvent_SendUpdate;	//A pointer for the event class used to send random updates
	EventDelegate *eventDel_SendUpdate;//delegate for the random data send event.

	FileLogger<PplscRecvMsgLogElement>* msg_logger_ptr_rx;//Record Received Packets in here
	FileLogger<PplscSentMsgLogElement>* msg_logger_ptr_tx;//Record Received Packets in here


	uint32_t m_numofMessagesSend;
	void RandomSendHandler(EventDelegateParam param); //Handler for the event generator used to send random updates
	void UpdateVariableHandler(EventDelegateParam param);  //Handler for the event generator used to update stored information

	PatternNeighborTableI *myNbrHood;
	//PplscLinkComparator *myLinkComparator;

	PplscNeigborContainerType SelectedNeighborList; //Set of selected neighbors to send a status update
	PplscNonceContainerType outstandingNonces; //Set of outstanding nonces

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
public:

	//Common to most patterns

	Pplsc();	//constructor
	bool Start();  //Lets the test code or network admin start the pattern
	bool Stop(); //Lets the test code or network admin stop the pattern

	void NeighborUpdateEvent (NeighborUpdateParam nbrUpdate);
	void ControlResponseEvent (ControlResponseParam response);
	void DataStatusEvent (DataStatusParam notification);
	void ReceiveMessageEvent (FMessage_t& msg);
};

struct PplscMsg {
	//whatever you want to send
	uint32_t numofMessagesSend;
};

} //end of namespace

#endif //PPLSC_H_
