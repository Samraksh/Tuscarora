////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef FRAMEWORKBASE_H_
#define FRAMEWORKBASE_H_

#include <map>
#include "Platform/PlatformSelector.h"
#include <Base/FrameworkTypes.h>
#include "Lib/PAL/PAL_Lib.h"
#include <Interfaces/Waveform/WF_I.h>
#include <Interfaces/PWI/Framework_I.h>
#include <Interfaces/PWI/FrameworkResponces.h>

#include "Framework/Core/DataFlow/PiggyBacker.h"
#include "Framework/PWI/Neighborhood/CustomPatternNeighborTable.h"
#include "Framework/PWI/Neighborhood/PI_NeighborManager.h"
#include "Framework/Core/Neighborhood/NeighborTable.h"
#include "Framework/Core/Discovery/None.h"
#include "Framework/Core/Discovery/Global.h"
#include "Framework/Core/Discovery/LongLink.h"
#include "Framework/Core/Discovery/OracleLongLink.h"
#include "Framework/Core/Neighborhood/AddressMap.h"
#include "Framework/Core/Estimation/EstBase.h"
#include "Framework/Core/Estimation/Scheduled.h"
#include "Framework/Core/Estimation/ConflictAware.h"
#include "Framework/Core/DataFlow/PacketHandler.h"
#include "Framework/Core/Naming/StaticNaming.h"
#include "Lib/Misc/MemMap.h"
#include "PatternClient.h"

using namespace Core::Dataflow;
using namespace Waveform;
using namespace PWI::Neighborhood;

static Core::Naming::StaticNaming NameService;


namespace PWI {
  
  

	
class FrameworkBase : public Framework_I , EstimatorCallback_I<uint64_t>, public PiggyBackerI<uint64_t>
{
	friend class PacketHandler;///< Coordinates the delivery and transmission of the packets for the patterns, link estimation, neighborhood discovery, time synchronization, and data packets
	friend class LinkEstimatorI;///< The interface to the link estimation module
public:

	Core::Naming::StaticNaming *nameService;
	BSTMapT<MessageId_t, MessageId_t> fwToPattern;  //Store correspondance between pattern message Id and framewrok mesageId 
	BSTMapT<MessageId_t, ListT<WaveformId_t,false, EqualTo<WaveformId_t> >* > fwmsgTowfid; // stores framework messageId <=> wfid list
	LinkEstimationStore LES;
	U64AddressSendDataDelegate_t *u64SendDelegate ;
protected:
  
	uint16_t outstandingPacketsPetPattern;
	PotentialNeighborRegistry potential;
	PiggyBacker<uint64_t> *PB;
	
	WF_AttributeMap_t *wfAttributeMap;
	bool wfExists[MAX_WAVEFORMS];
	WaveformMap_t wfMap;
	//WF_MsgAdaptorMap_t wfMsgAdaptorMap;
	AddressMap addressMap;
	
		
	//ListT<NodeId_t,false, EqualTo<NodeId_t> >* validDest;  
	//ListT<NodeId_t,false, EqualTo<NodeId_t> >* invalidDest; // Stores node id did not appear on any neighbor table.
	ListT<WaveformId_t,false, EqualTo<WaveformId_t> >* wfList;  

	ListT<NodeId_t,false, EqualTo<NodeId_t> >* successDest;
	ListT<NodeId_t,false, EqualTo<NodeId_t> >* failDest;

	//MessageId_t frameworkMsgId;
	MessageId_t patternMsgId;

	uint8_t noOfWaveforms;

	//used to check inquery timer to decide to send request again or not.
	bool attribute_request_pending[MAX_WAVEFORMS];



	//RandomSchedule<UniformRandomInt,UniformRNGState,uint64_t,uint64_t> *coreEventSch;
	//Delegate<void, EventInfoU64&> *coreEventDel;
	
	Event *corePacketEvent;
	EventDelegate *corePacketDel;
	UniformRandomInt *rnd;

	Timer *printstate;
	//uint32_t id;
	uint32_t beaconPeriod;
	uint32_t deadNbrPeriod;
	uint32_t seq;
	int fd;
	Header header;
	MemMap<Store> data;
	int communicationRange;

	//PatternNeighborTable *defaultPatternTable;
	NeighborTable nbrTable;


	//LinkEstimatorI *linkEstimator;
	NetworkDiscovery *discovery;

	uint32_t numNeighbors;

	//ASNP variables
	PatternClientMap_t  patternClientsMap;	///< Array of asnp client instances
	//BSTMapT<PatternId_t, uint8_t> asnpPriority; ///< Hash table for asnp priorities
	uint8_t noOfPatterns;

	PacketHandler *pktHandler;  ///< Create a instance of the packet handler

	
	//Neighborhood related variabled
	PI_NeighborManager nbrManager;
	
	FrameworkAttributes fwAttribute;
	
	/* moving it to packethandler
	MessageId_t GetNewFrameworkMsgId() {
		//printf("Generating next Framework pkt id: current id (ptr: %p) is %d\n",&num1, num1);
		//printf("New Framework pkt id: current id is %d\n",num1);
		return ++frameworkMsgId;
	}
	*/
	/*MessageId_t GetNewPatternMsgId(){
		return ++patternMsgId;
	}*/

	//uint32_t GetNewReqId();
	//uint32_t newReqnumber;
	
  
public:
	PatternEventDispatch_I *eventDispatcher;

	WF_Attributes localWfInfo;
	PacketAdaptor<uint64_t> *pktAdaptor64;

	Delegate<void, WF_DataStatusParam_n64_t> *ackDel;
	Delegate<void, WF_Message_n64_t&> *wfRecvDel;

	WF_LinkChangeDelegate_n64_t *leDel;///< This delegate is informed when a link is created or destroyed
	PotentialNeighborDelegate * discoveryDel;///< This delegate is informed when a neighbor becomes a potential neighbor, or is no longer a potential neighbor
	//SendEstimationMessageDelegate_t *leSendDel; ///This will be called by estimation to send a estimation message out.
	
	uint16_t runTime;
	uint8_t density;
	uint8_t mobilityModel;
	uint16_t speed;
	uint16_t numNodes;
	uint32_t mask[32];///< Used for logging only
	uint16_t maxFrameworkPacketSize;
	
	//decreate inquerytimer to reschedule initial attribute request.
	TimerDelegate *inqueryTimerDelegate;
	Timer *inqueryTimer;

	
public://Non interface methods
	//Other public functions not part of spec
	Core::Link* GetCompBasedLinkForNeighbor(NodeId_t node_id,LinkComparatorTypeE lcType);
	Core::Link* GetPolicyBasedLinkForNeighbor(NodeId_t id,LinkComparatorTypeE lcType);
	void InitializeWaveform(WaveformId_t wid);
	void RemoveWaveform(WaveformId_t wid);
	void SetLongLink(Waveform::WaveformBase &ref, WaveformId_t wid, const char* name);
	void SetLocalLink(Waveform::WaveformBase &ref, WaveformId_t wid, const char* name);
	WaveformId_t SetLink(Waveform::WaveformBase &ref, WaveformId_t wid, const char* name);
	void SetLinkEstimationType(EstimatorTypeE type);///< Sets the type of link estimation to use
	void SetDiscoveryType(uint8_t type);///< Sets the type of neighborhood discovery to use
	void SetRange(int range);
	void SetNeighborInactivityPeriod(uint32_t period);
	void SetLinkEstimationPeriod(uint32_t _period);///< The mean period of time between link estimation beacons
	void Start();///< Called after the framework is configured to start link estimation, network discovery, packet queues, and begin interfacing with the radio.
	
	void GetWaveformIDs(WaveformId_t *idArray, uint8_t noOfWaveforms);
	uint8_t GetNumberOfWaveforms();

	//Allows simulations in dce to set delegates.
	void RegisterDelegates (PatternId_t pid, RecvMessageDelegate_t *_recvDelegate, NeighborDelegate *_nbrDelegate, DataflowDelegate_t *_dataNotifierDelegate, ControlResponseDelegate_t *_controlDelegate); 
	bool HasWaveform(WaveformId_t waveformId);
	void SendToWF(WaveformId_t wfId, bool bcast, MessageTypeE type, NodeId_t *destArray, uint16_t noOfDest, FMessage_t& msg);
	void CreateLinkEstimator(WaveformId_t wid);
	void DestroyLinkEstimator(WaveformId_t wid);
	//void LinkEstimatorChangeHandler(NeighborDelegateParam _param);///< Called when a link has been created or destroyed
	
	///EstimationCallback_I methods
	bool SendEstimationMessage (WaveformId_t wfId, 	FMessage_t *msg);
	virtual WF_Attributes GetWaveformAttributes (WaveformId_t wfId);
	void LinkEstimatorChangeHandler(WF_LinkEstimationParam_n64_t  _param);
	
private:

		//for validation
	TimerDelegate *validationTimerDelegate;
	Timer *validationTimer;
	
	void ProcessLinkEstimationMsg(WF_MessageBase &msg);	///< Process link estimation messages
	void ProcessDiscoveryMsg(WF_MessageBase &msg);	///< Process discovery messages
	
	void PotentialNeighborUpdate(PotentialNeighborDelegateParam param);
	
	//Signalling to patterns
	void SendToPattern_ControlSignal (PatternId_t pid, ControlResponseParam &param);
	void SendToPattern_NeighborUpdateSignal (PatternId_t pid, NeighborUpdateParam &param);
	void SendToPattern_ReceivedMessageSignal (PatternId_t pid, FMessage_t &msg);
	void SendToPattern_DataNotificationSignal (PatternId_t pid, NeighborUpdateParam &param);
	PatternClient* FindPattern (PatternId_t Id);
	//void ScheduleEvents (EventInfoU64& eventInfo);
	void ScheduleEvents (EventDelegateParam param);
	void PrintState(uint32_t event);///< Does not impact the functioning of the framework, it is just used to give a log of the performance of the different components of the framework.
	//void RegisterBuffer(PacketCollectorI *collector);
	//LinkEstimatorI* CreatePatternLinkEsitmator(uint16_t patternID, char* linkEstimatorName);
	//LinkEstimatorI* CreatePatternLinkEsitmator(uint16_t patternID);
	void inqueryTimer_Handler(uint32_t  flag);
	void validationTimerHandler(uint32_t flag); //for validation
	void debugTimerHandler(uint32_t flag);
	void MinimalWaveformSpray(PatternId_t pid, FMessage_t& msg, WaveformId_t wid, uint16_t nonce);
	void UnAddressedSend (PatternId_t pid, FMessage_t& msg, WaveformId_t wid, uint16_t nonce);
	void RefreshFrameworkAttribute(FrameworkAttributes &attr);



public:
	FrameworkBase();

	/// Data plane functions
	void SendData (PatternId_t pid, NodeId_t *destArray, uint16_t noOfDest, FMessage_t& msg, uint16_t nonce, bool noAck=false);
	void SendData (PatternId_t pid, NodeId_t *destArray, uint16_t noOfDest, LinkComparatorTypeE lcType, FMessage_t& msg, uint16_t nonce, bool noAck);

	void SoftwareBroadCast(PatternId_t pid, FMessage_t& msg, WaveformId_t wid, uint16_t nonce, MessageId_t newMsgToken);
	
#if ENABLE_FW_BROADCAST==1 	
	void BroadcastData (PatternId_t pid, FMessage_t& msg, WaveformId_t wid, uint16_t nonce);
#endif
	void ReplacePayloadRequest (PatternId_t patternId, MessageId_t msgId, void *payload, uint16_t sizeOfPayload);

	///Control plance
	PatternId_t NewPatternInstanceRequest(PatternTypeE ptype, const char uniqueName[128]);
	void RegisterPatternRequest (PatternId_t patternId, PatternTypeE pType);
	void RegisterPatternRequest (PatternId_t patternId, const char uniqueName[128], PatternTypeE type);
	void DeRegisterPatternRequest (PatternId_t patternId);
	void SelectDataNotificationRequest (PatternId_t patternId, uint8_t notifierMask);
	void AddDestinationRequest (PatternId_t patternId, MessageId_t msgId, NodeId_t *destArray, uint16_t noOfNbrs);
	void AddDestinationRequest(PatternId_t patternId, MessageId_t msgId, NodeId_t* destArray, uint16_t noOfNbrs, LinkComparatorTypeE lcType);

	void CancelDataRequest (PatternId_t patternId, MessageId_t msgId, NodeId_t *destArray, uint16_t noOfDest);
	void DataStatusRequest (PatternId_t patternId, MessageId_t msgId);
	void FrameworkAttributesRequest(PatternId_t pid);
	void SetLinkThresholdRequest (PatternId_t patternId, LinkMetrics threshold);
	void SelectLinkComparatorRequest (PatternId_t patternId, LinkComparatorTypeE  lcType);

	virtual ~FrameworkBase() {}


	///Piggybacking/Timestamping interfaces
	bool RegisterPiggyBackee (PB_TypeE type,  Waveform::WF_AddressTypeE addressType, uint8_t dataSize, const std::string &authString, PiggyBackeeI<uint64_t> &receivingModule, WaveformId_t wId){
		return PB->RegisterPiggyBackee(type, addressType, dataSize, authString, receivingModule, wId);
	}
	
	uint8_t GetNumberOfRegisteredPiggyBackees(WaveformId_t wid){
		return PB->GetNumberOfRegisteredPiggyBackees(wid);
	}
	
	bool SendDataNow(WaveformId_t wid,  const std::string &authString, MessageTypeE msgType, uint8_t dataSize, uint8_t *data, uint64_t dest, bool unaddressed){
		return PB->SendDataNow(wid,authString,msgType,dataSize,data,dest,unaddressed);
	}
	
	bool GenericSendMessage (WaveformId_t wfId, uint64_t dest, MessageTypeE msgType, FMessage_t &msg, bool unaddressed);
};


}

#endif /* FRAMEWORKBASE_H_ */
