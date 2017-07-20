////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 


//if debug flag is set, statistics will be written into memmap.

#include <string>
#include <math.h>
#include <Interfaces/Waveform/WF_Types.h>
#include "FrameworkBase.h"
#include "Framework/Core/DataFlow/CoreTypes.h"
#include "Framework/Core/ControlPlane/WF_Controller.h"
#include "Framework/External/Location.h"
#include "Framework/Core/Neighborhood/Nbr_Utils.h"
#include "Lib/Framework/FrameworkStat.h"

extern NodeId_t MY_NODE_ID;
extern NodeId_t CONTROL_BASE_ID;
extern uint32_t NETWORK_SIZE;
extern uint32_t runTime;

#define SC_AO_WF(x) dynamic_cast<WF_AlwaysOn_DCE *>(x)

namespace Core {
namespace WFControl {
	extern WF_Controller *wfControl;
	extern BSTMapT<RequestId_t, WF_ControlResponseParam> rIdToResult;  //map between requestId and its status
	extern WF_ControlP_StatusE WF_Control_Status[MAX_WAVEFORMS];
}
}

#if (DEPLOY==1)
#define Debug_Flag 1
FrameworkStat fstat;
NeighborStat nstat;
locationStat lstat;
FrameworkStatBroadcast fstatb;
#else
#define Debug_Flag 0
#endif

using namespace PWI;
using namespace Core;

namespace PWI {
#if ENABLE_FW_BROADCAST==1 	
void FrameworkBase::BroadcastData (PatternId_t pid, FMessage_t& msg, WaveformId_t wid, uint16_t nonce){
	UnAddressedSend(pid,msg,wid,nonce);
}
#endif	


FrameworkBase::FrameworkBase()
{
#ifndef PLATFORM_EMOTE
	data.SetFileName("output.bin");
#endif

    Debug_Printf(DBG_CORE, "FrameworkBase::Constructor Starting\n");

#if Debug_Flag==1
    //for validation
    fstat.positive_frameworkAck = 0;
    fstat.negative_frameworkAck = 0;
    fstat.total_valid_sent = 0;
    fstat.total_sent = 0;
    fstat.total_invalid_destination = 0;

    fstatb.data_broadcast_sent = 0;
    fstatb.le_broadcast_sent =0;
    fstatb.positive_fw_ack = 0;
    fstatb.negative_fw_ack = 0;
#endif

    Debug_Printf(DBG_CORE,"FrameworkBase:: Contructor:: Creating framework base\n");
    //Debug_Printf(DBG_CORE, "(%d,%d) \n",x(), y());
    outstandingPacketsPetPattern = PolicyManager::GetPacketsBufferedPerPattern();
    //frameworkMsgId=0; moved to packet handler
    //newReqnumber = 0; moved to packet handler
    patternMsgId=0;

    /*Not used anymore
    //Mask is used to in the performance logging in PrintState()
    mask[0]=0x00000001;  mask[1]=0x00000002;  mask[2]=0x00000004;  mask[3]=0x00000008;  mask[4]=0x00000010;  mask[5]=0x00000020;
    mask[6]=0x00000040;  mask[7]=0x00000080;  mask[8]=0x00000100;  mask[9]=0x00000200; mask[10]=0x00000400; mask[11]=0x00000800;
    mask[12]=0x00001000; mask[13]=0x00002000; mask[14]=0x00004000; mask[15]=0x00008000; mask[16]=0x00010000; mask[17]=0x00020000;
    mask[18]=0x00040000; mask[19]=0x00080000; mask[20]=0x00100000; mask[21]=0x00200000; mask[22]=0x00400000; mask[23]=0x00800000;
    mask[24]=0x01000000; mask[25]=0x02000000; mask[26]=0x04000000; mask[27]=0x08000000; mask[28]=0x10000000; mask[29]=0x20000000;
    mask[30]=0x40000000; mask[31]=0x80000000;
	*/

    numNodes = NETWORK_SIZE;//Used only in post processing

    Debug_Printf(DBG_CORE_DATAFLOW,"FrameworkBase:: Init:: my NODE_ID is %d \n", MY_NODE_ID);
    numNeighbors = 0;
    seq = 0;
    noOfWaveforms=0;
    noOfPatterns=0;

    //Create the objects needed by framework
    FrameworkBase::InitializeFrameworkObjects(this);
	
    //Set the framework neighbortable as the base table for the manager
    nbrManager.SetCoreNbrTable(nbrTable);

    ///Initialize naming service
    nameService = &NameService;

    //defaultPatternTable = new PatternNeighborTable (&nbrTable);F
    nbrManager.SetPatternEventDispatcher(eventDispatcher);

    LES.longLinkActive=false;

    //Find out how big of a packet can be supported from waveforms
    //maxFrameworkPacketSize=960;

    inqueryTimer->Start();


    uint64_t base = runTime *1000 *1000;
    base = base - 1000;
    TimerDelegate *debugTimerDelegate = new TimerDelegate(this, &FrameworkBase::debugTimerHandler);
    Timer *debugTimer = new Timer(1000000, PERIODIC, *debugTimerDelegate);
    debugTimer->Start();

    #if Debug_Flag
    Debug_Printf(DBG_CORE, "FrameworkBase::validation timer base is %ld\n",base);
    TimerDelegate *validationTimerDelegate = new TimerDelegate(this, &FrameworkBase::validationTimerHandler);
    //Timer *validationTimer = new Timer(99999000, ONE_SHOT, *validationTimerDelegate);
    //Timer *validationTimer = new Timer(29999000, ONE_SHOT, *validationTimerDelegate);
    Timer *validationTimer = new Timer(base, ONE_SHOT, *validationTimerDelegate);
    validationTimer->Start();
#endif
    //Debug_Printf(DBG_CORE, "FrameworkBase:: Show size %ld\n", sizeof(WF_Message_n64_t));
}


void FrameworkBase::RegisterDelegates(PatternId_t _pid, RecvMessageDelegate_t* _recvDelegate, NeighborDelegate* _nbrDelegate, DataflowDelegate_t* _dataNotifierDelegate, ControlResponseDelegate_t* _controlDelegate)
{
	Debug_Printf(DBG_CORE, "FramewrokBase:: Registering delegates to eventDispatcher for pid %d\n",_pid);
	eventDispatcher->RegisterDelegates(_pid, _recvDelegate,_nbrDelegate,_dataNotifierDelegate,_controlDelegate);
	RegisterPatternRequest (_pid, UNKNOWN_PTN);
}

void FrameworkBase::inqueryTimer_Handler(uint32_t flag){
    //for each waveform, make sure control inquery reply is received.
    for(int wf =0; wf < MAX_WAVEFORMS; wf++){
        if(attribute_request_pending[wf] == true){
            Debug_Printf(DBG_CORE, "FramewrokBase::Control request reply did not arrive for wf %d. Try it again\n",wf);
            Core::WFControl::wfControl->Send_AttributesRequest(wf,pktHandler->GetNewReqId());
        }
    }       
}            

void FrameworkBase::SetLocalLink(Waveform::WaveformBase &ref, WaveformId_t wid, const char* name) {
    Debug_Printf(DBG_CORE,"FramewrokBase::Entered SetLocalLink. Setting Local link\n");
    SetLink(ref, wid, name);
    //CreateLinkEstimator(wid);
}

void FrameworkBase::CreateLinkEstimator(WaveformId_t wid){

    if(LES.linkEstimationType == FW_SCHEDULED_EST) {
        Debug_Printf(DBG_CORE, "FramewrokBase::Creating Scheduled for wfid %d\n", wid);
        LES.estimatorHash[wid] = static_cast<Scheduled*>(new Scheduled(*this, wid, &nbrTable));
        Debug_Printf(DBG_CORE, "FramewrokBBase::Created Scheduled link estimation %d, with estimation ptr %p\n", wid, LES.estimatorHash[wid]);  fflush(stdout);
		//Debug_Error("FramewrokBase:: Error: Scheduled link estimation not supported at the moment\n");  fflush(stdout);
    } else if(LES.linkEstimationType == FW_CONFLICTAWARE_EST) {
        Debug_Printf(DBG_CORE, "FramewrokBase::Creating CoonflictAvoid, wid %d\n", wid);
        //LES.estimatorHash[wid] = static_cast<ConflictAware*>(new ConflictAware(this, wid, &nbrTable));
		LES.estimatorHash[wid] = static_cast<ConflictAware*>(new ConflictAware(*this, wid, &nbrTable));
        Debug_Printf(DBG_CORE, "FramewrokBase::Created Schedule Aware link estimation %d, with estimation ptr %p\n", wid, LES.estimatorHash[wid]);  fflush(stdout);
		//Debug_Error("FramewrokBBase:: Error: Conflict Avoidance link estimation not supported at the moment\n");  fflush(stdout);
    } else {
        Debug_Printf(DBG_CORE, "FramewrokBase::Creating Periodic, wid %d\n", wid);
        //estimatorHash[wid] = static_cast<Periodic*>(new Periodic(this,wid, &nbrTable));
        LES.estimatorHash[wid] = new EstBase(*this,wid, &nbrTable);
        Debug_Printf(DBG_CORE, "FramewrokBase::Created Periodic link estimation %d, with estimation ptr %p\n", wid, LES.estimatorHash[wid]);  fflush(stdout);
    }
    LES.estimatorHash[wid]->SetParam(beaconPeriod, deadNbrPeriod);
    LES.estimatorHash[wid]->RegisterDelegate(*leDel);
}


void FrameworkBase::DestroyLinkEstimator(WaveformId_t wid){

	LinkEstimatorI<uint64_t> *est =  LES.estimatorHash[wid];
	
    if(LES.linkEstimationType == FW_SCHEDULED_EST) {
        Debug_Printf(DBG_CORE, "FramewrokBase::Destroying Scheduled for wfid %d\n", wid);
        delete (static_cast<Scheduled*>(est));
        Debug_Printf(DBG_CORE, "FramewrokBBase::Destroyed Scheduled link estimation %d, with estimation ptr %p\n", wid, est);  fflush(stdout);
    } else if(LES.linkEstimationType == FW_CONFLICTAWARE_EST) {
        Debug_Printf(DBG_CORE, "FramewrokBase::Destroying CoonflictAvoid, wid %d\n", wid);
        delete(static_cast<ConflictAware*>(est));
        Debug_Printf(DBG_CORE, "FramewrokBase::Destroyed Schedule Aware link estimation %d, with estimation ptr %p\n", wid, est);  fflush(stdout);
    } else {
        Debug_Printf(DBG_CORE, "FramewrokBase::Destroying Randomized, wid %d\n", wid);
        delete (static_cast<EstBase*>(est));
        Debug_Printf(DBG_CORE, "FramewrokBase::Destroyed Randomized link estimation %d, with estimation ptr %p\n", wid, est);  fflush(stdout);
    }
    //LES.estimatorHash[wid]->SetParam(beaconPeriod, deadNbrPeriod);
    //LES.estimatorHash[wid]->RegisterDelegate(*leDel);
}



void FrameworkBase::SetLongLink(Waveform::WaveformBase &ref, WaveformId_t wid, const char* name) {
    Debug_Printf(DBG_CORE,"FramewrokBase::Entered SetLongLink. Setting Long link\n");
    LES.longLinkActive=true;
    SetLink(ref, wid, name);
}

WaveformId_t FrameworkBase::SetLink(Waveform::WaveformBase &ref, WaveformId_t wid, const char* name) {
    wfMap[wid]=static_cast<Waveform::Waveform_I<uint64_t>*>(&ref);
    noOfWaveforms++;
    wfExists[wid] = true;
    return wid;
}

bool FrameworkBase::HasWaveform(WaveformId_t wfid) {
    return wfExists[wfid];
}

void FrameworkBase::SetLinkEstimationPeriod(uint32_t period_in_microsec) {
    beaconPeriod = period_in_microsec;
    Debug_Printf( DBG_CORE,"FrameworkBase:: Beaconing at rate %d Hz\n", 1000000/period_in_microsec);
}

void FrameworkBase::SetNeighborInactivityPeriod(uint32_t periods_to_miss_to_count_as_dead) {
    deadNbrPeriod = periods_to_miss_to_count_as_dead;
}

void FrameworkBase::SetDiscoveryType(NetworkDiscoveryTypeE _type) {
    LES.discoveryType = _type;
}

void FrameworkBase::SetLinkEstimationType(EstimatorTypeE _type) {
    LES.linkEstimationType = _type;
    if(LES.linkEstimationType == FW_SCHEDULED_EST) {
        Debug_Printf(DBG_CORE, "FramewrokBase::Setting link estimation type to scheduled %d\n", _type);
    } else if(LES.linkEstimationType == FW_CONFLICTAWARE_EST) {
        Debug_Printf(DBG_CORE, "FramewrokBase::Setting link estimation type to conflict %d\n", _type);
    } else {
        Debug_Printf(DBG_CORE, "FramewrokBase::Setting link estimation type to periodic %d\n", _type);
    }
}

void FrameworkBase::Start() {
    //Storing basic data about the framework's current configuration
    MemMap<DiscoveryLogHeader> headerMap("configuration.bin");
    header.nodeid = MY_NODE_ID;
    header.beaconPeriod = beaconPeriod;
    header.inactivityPeriod = deadNbrPeriod;
    header.range = communicationRange;
    header.discoveryType = LES.discoveryType;
    header.linkEstimationType = LES.linkEstimationType;
    if(LES.discoveryType == Discovery::NetworkDiscoveryTypeE::LONG_LINK_DISCOVERY) numNodes--;
    header.numNodes = numNodes;
    header.runTime = runTime;
    header.density = density;
    header.mobilityModel = mobilityModel;
    header.speed = speed;
    headerMap.addRecord(header);
    //Creating the discovery module

    if(LES.discoveryType == ORACLE_DISCOVERY) {
        Debug_Printf(DBG_CORE,"ORACLE_DISCOVERY\n");
        //discovery = static_cast<Core::Discovery::OracleLongLink*>(new Core::Discovery::OracleLongLink());
    } else if(LES.discoveryType == NULL_DISCOVERY) {
        Debug_Printf(DBG_CORE,"Null discovery\n");
        discovery = static_cast<Core::Discovery::None*>(new Core::Discovery::None());
    } else if(LES.discoveryType == LONG_LINK_DISCOVERY) {
        Debug_Printf(DBG_CORE,"Long Link discovery\n");
        discovery = static_cast<Core::Discovery::LongLink*>(new Core::Discovery::LongLink(this));
    } else {
        Debug_Printf(DBG_CORE,"Global discovery\n");
        discovery = static_cast<Core::Discovery::Global*>(new Core::Discovery::Global());
    }
    discoveryDel = new PotentialNeighborDelegate(this, &FrameworkBase::PotentialNeighborUpdate);
    discovery->RegisterDelegate(*discoveryDel);

    //Creating the link estimation module, and setting the discovery delegates
    if(MY_NODE_ID != CONTROL_BASE_ID) {
        for (int32_t i=0; i < MAX_WAVEFORMS; i++){
            if(wfExists[i]){
                attribute_request_pending[i] = true;
                Core::WFControl::wfControl->Send_AttributesRequest(i,pktHandler->GetNewReqId());
                Debug_Printf(DBG_CORE, "FrameworkBase::Start:: Sent wf attribute request for wf id %d\n",i);
            }
        }

        //Create and schedule a event for checking outgoing packets in the framework
        corePacketDel = new EventDelegate(this, &FrameworkBase::ScheduleEvents);
        //uint64_t curTime = Debug::GetTimeMicro();
        int64_t eventDelay = 987;
        corePacketEvent = new Event(eventDelay, *corePacketDel, (void *) (0));
    }
    discovery->Start();

    TimerDelegate *state_ = new TimerDelegate(this, &FrameworkBase::PrintState);
    printstate = new Timer(beaconPeriod / 10, PERIODIC, *state_);
    if(CONTROL_BASE_ID != MY_NODE_ID) printstate->Start();

    fflush(stdout);
    /*WaveformMap_t ::Iterator it_wf = wfMap.Begin();
    for(; it_wf != wfMap.End();){
        WaveformId_t current_wfid = it_wf->First();
        Debug_Printf(DBG_CORE, "FramewrokBase::Get Attribute infromation from wfid %d\n",current_wfid);
        //Debug_Printf(DBG_CORE,"I am in FrameworkBase showing Address of Core::WFControl::wfControl is %p\n",Core::WFControl::wfControl);//
        Core::WFControl::wfControl->Send_AttributesRequest(current_wfid,pktHandler->GetNewReqId());
        Debug_Printf(DBG_CORE, "FramewrokBase::Setting attribute request pending on wf %d\n",current_wfid);
        attribute_request_pending[it_wf->First()] = true;
        Core::WFControl::wfControl->Send_ControlStatusRequest(current_wfid,pktHandler->GetNewReqId());
        ++it_wf;
    }*/
    Debug_Printf(DBG_CORE_DATAFLOW, "FrameworkBase::Start:: Exiting\n");
}


void FrameworkBase::InitializeWaveform(WaveformId_t wid) {
  
	//Creating the link estimation module, and setting the discovery delegates
	if(MY_NODE_ID != CONTROL_BASE_ID) {
		wfExists[wid]=true;
		attribute_request_pending[wid] = true;
		Core::WFControl::wfControl->Send_AttributesRequest(wid,pktHandler->GetNewReqId());
        Debug_Printf(DBG_CORE, "FrameworkBase::InitializeWaveform:: Sent wf attribute request for wf id %d\n",wid);
    }
    Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: InitializeWaveform :: Exiting\n");
}

void FrameworkBase::RemoveWaveform(WaveformId_t wid) {
  
	//Creating the link estimation module, and setting the discovery delegates
	if(MY_NODE_ID != CONTROL_BASE_ID) {
		wfExists[wid]=false;
		delete(wfMap[wid]);
		wfMap[wid]=0;
		attribute_request_pending[wid] = false;
		DestroyLinkEstimator(wid);
		
        Debug_Printf(DBG_CORE, "FrameworkBase::InitializeWaveform:: Sent wf attribute request for wf id %d\n",wid);
    }
    Debug_Printf(DBG_CORE_DATAFLOW, "PacketHandler:: InitializeWaveform :: Exiting\n");
}


void FrameworkBase::PotentialNeighborUpdate(PotentialNeighborDelegateParam param) {
    PotentialNeighbor* nbr = param.pnbr;
    if(param.changeType == PNBR_UPDATE) {
        //PotentialNeighbor* i = potential.AddNode(nbr->nodeId, BEYOND_COMM_PNODE, 0, 1);
        if(potential.AddNode(nbr->linkId, BEYOND_COMM_PNODE, 0, 1) != 0) {
            LES.logger->LogEvent(Estimation::LINK_POTENTIAL_ADD, nbr->linkId.nodeId);
            Debug_Printf(DBG_CORE_ESTIMATION, "Adding Potential Neighbor %d\n", nbr->linkId.nodeId);
        } else {
            Debug_Printf(DBG_CORE_ESTIMATION, "Updating Potential Neighbor %d\n", nbr->linkId.nodeId);
        }
        potential.UpdateNode(nbr->linkId, *nbr);//this should remain outside of the else block
    } else if(param.changeType == PNBR_INVALID) {
        potential.RemoveNode(nbr->linkId);
        LES.logger->LogEvent(LINK_POTENTIAL_DEL, nbr->linkId.nodeId);
    }

    if(wfExists[nbr->linkId.waveformId])
    {
        if(LES.estimatorHash.Find(nbr->linkId.waveformId) != LES.estimatorHash.End()){
            LES.estimatorHash[nbr->linkId.waveformId]->PotentialNeighborUpdate(*(param.pnbr), param.changeType);
        }
    }
}

//This is the main loop of the framework
void FrameworkBase::ScheduleEvents (EventDelegateParam param){
    Debug_Printf(DBG_CORE,"FramewrokBase::Entered Schedule Events\n");
    //Check for packets Local link
    //uint16_t remPackets1 =
    //pktHandler->ProcessOutgoingMessages(2);
    //uint16_t remPackets2 =
    //pktHandler->ProcessOutgoingMessages(3);
    //Debug_Printf(DBG_CORE_DATAFLOW, "OutGoingMessages:: %d packets waiting on %d waveform\n\n",remPackets, Waveform::ALWAYS_ON_LOCAL_WF);
    /*if(Send_Busy){ //WF is still handling message do not schedule event
      return;
  }*/
    /*if(remPackets1+remPackets2){
    uint64_t _delay = rnd->GetNext();
    param.eventPtr->ReSchedule(_delay,(void *) (0));
  }*/
    //pktHandler->ProcessCollectors(Core::Naming::ALWAYS_ON_LOCAL_WF,noOfPatterns, &asnpPriority,2);
}

void FrameworkBase::PrintState(uint32_t event) {
    //REMOVING THIS FROM THE CURRENT VERSION, IF IT IS USED BY ANYONE, LET ME KNOW, AND IT WILL BE REINSTATED
    /*ExternalServices::Location2D location = ExternalServices::LocationService::GetLocation();
  double x = location.x;
  double y = location.y;

  //memory map printstate
   if(seq == 0)Debug_Printf( DBG_CORE,"Starting pos is %f, %f\n", x, y);
  Store state;
  state.x = (uint32_t)(10000 * x);
  state.y = (uint32_t)(10000 * y);
  for(int i = 0; i < (MAXNBR / BITS) + 1; i++) {
    state.neighbors[i] = 0;
  }

  for(uint64_t xx = 0; xx < numNodes; xx++) {
    if(xx == MY_NODE_ID) continue;

    //TODO: fix this for v0.7, no status
    NeighborInfo* nbr = nbrTable.GetNeighborInfo(xx);
    if(nbr != 0 && nbr->status == Neighbor) {
      state.neighbors[xx / BITS] |= mask[xx % BITS];
    }
  }

  data.addRecord(state);
  seq++;

  if(seq > (1000000 * header.runTime) / beaconPeriod * 10) {
    printstate->Suspend();
  }

  //Visiual printstate
  uint16_t goodNbrs= nbrTable.GetNumGoodNeighbors();
  Debug_Printf( DBG_CORE_ESTIMATION,"FrameworkBase:: Print State:: node-%d, X:%f, Y:%f, Good Neighbors: %d: NeighborList: ", MY_NODE_ID, x, y, goodNbrs);

  if (goodNbrs > 0) {
    NodeId_t *goodNbrIds = new NodeId_t[goodNbrs];
    nbrTable.GetNumGoodNeighborsIds(goodNbrIds);

    for(uint16_t i=0; i< goodNbrs; i++) {
       DebugNoTime_Printf( DBG_CORE_ESTIMATION, "%d, ", goodNbrIds[i]);
    }

    delete[] goodNbrIds;
     DebugNoTime_Printf( DBG_CORE_ESTIMATION, "\n");
  }
  else {
    Debug_Printf( DBG_CORE_ESTIMATION,"No good neighbors found. \n");
    }*/
}


void FrameworkBase::SetRange(int range) {
    Debug_Printf(DBG_CORE_DATAFLOW,"Setting range to %d\n", range);
    communicationRange = range;
}


void FrameworkBase::RegisterPatternRequest (PatternId_t patternId, PatternTypeE pType){

    //patternClientsMap[patternId].
    //patternClientsMap[patternId].SetPatternReferences(recvDelegate, nbrDelegates, dataNotifierDelegate);

    PatternClient *obj = new PatternClient();
    patternClientsMap.Insert(patternId,obj);

    patternClientsMap[patternId]->PID=patternId;
    nbrManager.InitializeNeighborhood(patternId);
    noOfPatterns++;
    ControlResponseParam param;
    param.type = PTN_RegisterResponse;
    param.sequenceNo = patternClientsMap[patternId]->responseSeqNo++;
    RegistrationResponse_Data dt;
    dt.noOfOutStandingPkts = PolicyManager::GetPacketsBufferedPerPattern();
    dt.status = true;
    dt.priority= PolicyManager::GetAsnpPriority(patternId);
    dt.patternId = patternId;
    param.data = &dt;
    param.dataSize = sizeof(dt);
    eventDispatcher->InvokeControlEvent(patternId,param);
    Debug_Printf( DBG_CORE_API,"FrameworkBase:: Registered ASNP/Pattern %d, with prioriry: %d \n", patternId, noOfPatterns);fflush(stdout);
}

void FrameworkBase::RegisterPatternRequest (PatternId_t patternId, const char uniqueName[128], PatternTypeE type){
//    if(patternId == 0){
//        patternId = Core::Naming::GetNewPatternInstanceId(type, uniqueName );
//        Debug_Printf(DBG_SHIM, "PTN_ControllerShim::Deserialize: PTN2FW_Call_RegisterPatternRequest got new PID patternId = %d type = %d \n", patternId, type);
//
//    }
//    InitPattern(patternId, sockfd);
    Debug_Printf( DBG_CORE,"FrameworkBase::RegisterPatternRequest from patternId=%d \n", patternId);fflush(stdout);


}

void FrameworkBase::DeRegisterPatternRequest (PatternId_t patternId){

    //patternClientsMap[patternId].
    //patternClientsMap[patternId].SetPatternReferences(recvDelegate, nbrDelegates, dataNotifierDelegate);

//    PatternClientMap_t::Iterator it = patternClientsMap.Find(patternId);
//    if(it != patternClientsMap.End()) {
//        patternClientsMap.Erase(it);
//    }
    patternClientsMap.Erase(patternId);

}

void FrameworkBase::SelectDataNotificationRequest (PatternId_t patternId, uint8_t notifierMask) {
    Debug_Printf(DBG_CORE_API, "FrameworkBase::SelectDataNotificationRequest from %d pattern n \n",patternId);
    patternClientsMap[patternId]->SelectDataNotifications(notifierMask);

    ControlResponseParam res;
    SelectDataNotificationResponse_Data data;
    data.status=true;
    res.type = PTN_SelectDataNotificationResponse;
    res.data = &data;
    res.dataSize = sizeof(data);

    eventDispatcher->InvokeControlEvent(patternId, res);
}



/*
 * Framework Behaviro: It will check if destination exists in neighbor table or not. It will only send packet to destination which exits in neighbor table.
                       It stores nonce value and it will drop packet if nonce value is same(not implemented yet).
 * Pattern Behavior:   Pattern starts timer to wait for reply from Framework. If it does not receive reply before timer expires.
                       it may send same packet with same nonce value
                       Should ignore message id returned with PDN_FW_RECV if status is false;
 */


void FrameworkBase::SendData (PatternId_t pid, NodeId_t *destArray, uint16_t noOfDest, FMessage_t& msg,uint16_t nonce, bool noAck){
    LinkComparatorTypeE lcType = nbrManager.GetLinkComparator(pid);
    SendData (pid, destArray, noOfDest, lcType, msg,nonce, noAck);

}

void FrameworkBase::SendData (PatternId_t pid, NodeId_t *destArray, uint16_t noOfDest, LinkComparatorTypeE lcType, FMessage_t& msg,uint16_t nonce, bool noAck)
{
    //check number of outstanding packet associated with this pid.
#if Debug_Flag
    fstat.total_sent += noOfDest;
#endif
    PatternClientMap_t :: Iterator it = patternClientsMap.Find(pid);
    if(it != patternClientsMap.End()){
        Debug_Printf(DBG_CORE_API, "FramewrokBase::patternClientsMap exists for this pid %d\n",pid);
        Debug_Printf(DBG_CORE_API, "FramewrokBase::Now check if framework can accept new packet or not\n");
        if(patternClientsMap[pid]->noOfOutstandingPkts >= outstandingPacketsPetPattern){
            Debug_Printf(DBG_CORE_API, "FramewrokBase::Framework cannot accept any packet from Pid %d, noOfOustandingPkets is %d\n",pid,patternClientsMap[pid]->noOfOutstandingPkts);
            Debug_Printf(DBG_CORE_API, "FramewrokBase::Rejecting incoming packet...Prepare and send PDN_FW_RECV nack\n");
            struct DataStatusParam param(0);
            for(int index = 0; index < noOfDest; index++){
                param.destArray[index] = destArray[index];
                param.statusType[index]= Core::Dataflow::PDN_FW_RECV;
                param.statusValue[index]=false;
            }
            param.messageId=0;
            param.noOfDest = noOfDest;
            param.readyToReceive = false;
            param.nonce = nonce;
            if(noOfDest != 0){
#if Debug_Flag
                fstat.negative_frameworkAck += noOfDest;
#endif
                eventDispatcher->InvokeDataStatusEvent(pid,param);
                return;
            }
        }
    }
    else {
        Debug_Printf(DBG_CORE_API, "FramewrokBBase::Didn't find pattern id %d in my db, something wrong, did pattern register? ", pid);
    }

    //check nonce value
    if(patternClientsMap[pid]->nonce == nonce){
        Debug_Printf(DBG_CORE_API, "FrameworkBase::SendData:: Incoming packet has same nonce value %d\n",nonce);
        //Send Acknowledgement with assigned fwMsgId
        Debug_Printf(DBG_CORE_API,"FrameworkBase::SendData:: send acknowledgement again\n");
        struct DataStatusParam param(patternClientsMap[pid]->msgId);
        for(int index = 0; index < noOfDest; index++){
            param.destArray[index] = destArray[index];
            param.statusType[index]= PDN_FW_RECV;
            param.statusValue[index]=true;
        }
        param.noOfDest = noOfDest;
        param.readyToReceive = false;
        param.nonce = nonce;
        eventDispatcher->InvokeDataStatusEvent(pid,param);
        return; //exit SendData
    }
    
    //Check incoming packet size, if too big send a error
    if(msg.GetPayloadSize() > fwAttribute.maxFrameworkPacketSize){
		Debug_Printf(DBG_CORE_API, "FrameworkBase::SendData:: Incoming packet size is very big:  %d\n",msg.GetPayloadSize());
        
        struct DataStatusParam param(patternClientsMap[pid]->msgId);
        for(int index = 0; index < noOfDest; index++){
            param.destArray[index] = destArray[index];
            param.statusType[index]= Core::Dataflow::PDN_ERROR_PKT_TOOBIG;
            param.statusValue[index]=true;
        }
        param.noOfDest = noOfDest;
        param.readyToReceive = true;
        param.nonce = nonce;
        eventDispatcher->InvokeDataStatusEvent(pid,param);
        return; //exit SendData
	}
    
    FMessageId_t  newMsgToken = pktHandler->GetNewFrameworkMsgId();
    Debug_Printf(DBG_CORE_API, "FrameworkBase::SendData:: newMsgToken %d\n",newMsgToken);
    Debug_Printf(DBG_CORE_API,"FrameworkBase::SendData, framework nonce %d, nonce send with msg %d\n",patternClientsMap[pid]->nonce,nonce);
    patternClientsMap[pid]->noOfOutstandingPkts++; //increment number of outstanding Packets;
    Debug_Printf(DBG_CORE_API, "Incremented noOfOutstandingPkts by 1 :%d\n",patternClientsMap[pid]->noOfOutstandingPkts);
    Debug_Printf(DBG_CORE_API, "Showing noOfOutstandingPkt %d for pid %d\n",patternClientsMap[pid]->noOfOutstandingPkts,pid);
    patternClientsMap[pid]->nonce = nonce;
    patternClientsMap[pid]->msgId = newMsgToken;
    patternClientsMap[pid]->readyToReceive = false;
    Debug_Printf(DBG_CORE_API, "Updating noOfOutstandingPkts to %d and nonce value to %d\n",patternClientsMap[pid]->noOfOutstandingPkts, nonce);
    //Debug_Printf(DBG_CORE_API, "Showing address of perMessageInfo %p, %p\n", obj, patternClientsMap[pid]->perMessageInfoMap[newMsgToken]);
    Debug_Printf(DBG_CORE_API, "FramewrokBase::SendData Prepare acknowledgment PDN_FW_RECV\n");
    
	
	struct DataStatusParam param(newMsgToken);
    for(int index = 0; index < noOfDest; index++){
        param.destArray[index] = destArray[index];
        param.statusType[index]=Core::Dataflow::PDN_FW_RECV;
				//Debug_Printf(DBG_CORE_API, "FramewrokBase::SendData Prepare acknowledgment PDN_FW_RECV for dest %d, %d\n", destArray[index], param.destArray[index]);
        param.statusValue[index]=true;
    }
    param.nonce = nonce;
    param.noOfDest = noOfDest;
    param.readyToReceive = false;
    eventDispatcher->InvokeDataStatusEvent(pid,param);
#if Debug_Flag
    fstat.positive_frameworkAck += noOfDest;
    Debug_Printf(DBG_CORE_API, "FrameworkBase:: HOGE!!! %d\n",fstat.positive_frameworkAck);
#endif
    //Find out the links for the nodes listed
    Link* destLinkArray[noOfDest];
    NodeId_t nbrsNotFound[noOfDest];
    Debug_Printf(DBG_CORE_API,"FrameworkBase::Send::Looking up the link details for %d destinations(s) in send...\n", noOfDest); fflush(stdout);
    uint16_t found=0;
    uint16_t notFound=0;
    Debug_Printf(DBG_CORE_API, "FrameworkBBase::Assign new message Id: %d\n",newMsgToken);

    //add entry in msgIdToPidMap
    Debug_Printf(DBG_CORE_API, "FramewrokBBase::Add msgId(%d) ,pid (%d) pair to msgIdToPidMap\n",newMsgToken,pid);
    //just in case, check I am not trying to put same information.
    BSTMapT< FMessageId_t  , PatternId_t > ::Iterator it_msg = pktHandler->msgIdToPidMap.Find(newMsgToken);
    if(it_msg != pktHandler->msgIdToPidMap.End()){
        Debug_Warning("FrameworkBase: SendData: Something is wrong. msgId %d already exists in msgIdToPidMap!\n",newMsgToken);
        //abort();
    }
    pktHandler->msgIdToPidMap.Insert(newMsgToken,pid);

    //Needs to return message id via notification. Prepare and Send Framework Acknowledgement and populate perMessageInfo stored in patternClients
    //Debug_Printf(DBG_CORE_API, "FramewrokBase::populate perMessageInfo for msgId %d",newMsgToken);

    //TODO:Check if this comment block is needed
    /*perMessageInfo* obj = new perMessageInfo;
  for(int index = 0 ; index < noOfDest; index++){
    Debug_Printf(DBG_CORE_API, "Set destination: %d ackFWRECV[%d] to true\n",destArray[index],index);
     obj->destinatioins[index] = destArray[index];
     obj->validDest[index] = -1; //mark all destinations as invalid first
     obj->ackStatus[index] = PDN_FW_RECV; //update all nodes

  }
  obj->type = Pattern_Type; //Assuming there is no unicast packet other than Pattern_Type.
  obj->noOfDest = noOfDest;  //Set number of destination in this message
  obj->remaining_node = noOfDest;
  //obj->pid = pid;
 Debug_Printf(DBG_CORE_API, "Insert to perMessageInformation map\n");
  patternClientsMap[pid]->perMessageInfoMap.Insert(newMsgToken,obj);
     */

    /***********************************************************************************
  Moved inside nonce check logic.
  patternClientsMap[pid]->noOfOutstandingPkts++; //increment number of outstanding Packets;
  //Debug_Printf(DBG_CORE_API, "Showing address of perMessageInfo %p, %p\n", obj, patternClientsMap[pid]->perMessageInfoMap[newMsgToken]);
 Debug_Printf(DBG_CORE_API, "FramewrokBase::SendData Prepare acknowledgement PDN_FW_RECV\n");
  struct DataStatusParam param(PDN_FW_RECV, newMsgToken, true);
  for(int index = 0; index < noOfDest; index++){
      param.dest[index] = destArray[index];
  }
  param.noOfDest = noOfDest;
  param.status = true;
  param.readyToReceive = false;
  eventDispatcher->InvokeDataStatusEvent(pid,param);
     ************************************************************************************/


    //store nodeid for each waveform id.
    //validDest = new ListT<NodeId_t,false, EqualTo<NodeId_t> >[16];
    //wfList = new ListT<WaveformId_t,false, EqualTo<WaveformId_t> >;
    //invalidDest = new ListT<NodeId_t, false, EqualTo<NodeId_t> >;
    /*Debug_Printf(DBG_CORE_API, "Copy destArray into invalidDest\n");
  for(uint16_t index = 0; index < noOfDest; index++){
     invalidDest->InsertBack(destArray[index]);
    Debug_Printf(DBG_CORE_API, "invalidDest[%d] = %d\n", index, invalidDest->GetItem(index));
  }*/
    for(uint16_t i=0; i< noOfDest; i++){
        //for(int wfid =0; wfid < 16; wfid++){
        //Debug_Printf(DBG_CORE_API, "search wf %d, nodeid %d\n",wfid,destArray[i]);
        //for each destination go thourgh all wfid
        //Debug_Printf(DBG_CORE_API,"Looking up info for detination %d ...\n", destArray[i]); fflush(stdout);
        //destLinkArray[found]= nbrTable.GetNeighborLink(destArray[i],wfid);
        //destLinkArray[found]= nbrTable.GetNeighborLink(destArray[i]);
        destLinkArray[found]= GetPolicyBasedLinkForNeighbor(destArray[i], lcType);
        if(destLinkArray[found]){
            Debug_Printf(DBG_CORE_API,"FrameworkBase::Send:: Node %d found in Neighbor Table. Node id from table %d, wf is %d\n", destArray[i], destLinkArray[found]->linkId.nodeId, destLinkArray[found]->linkId.waveformId); fflush(stdout);
            //validDest[destLinkArray[found]->linkId.waveformId].InsertBack(destLinkArray[found]->linkId.nodeId);
            // patternClientsMap[pid]->perMessageInfoMap[newMsgToken]->waveform[i] = wfid;
            //Debug_Printf(DBG_CORE_API, "Setting waveform[%d] to %d\n", i, wfid);
            //Debug_Printf(DBG_CORE_API, "Adding node %d to wf %d\n",destLinkArray[found]->linkId.nodeId,destLinkArray[found]->linkId.waveformId);
            Debug_Printf(DBG_CORE_API, "FramewrokBase::node %d is valid destination\n",destLinkArray[found]->linkId.nodeId);
            //Debug_Printf(DBG_CORE_API, "Size of validDest[%d] is %d\n",destLinkArray[found]->linkId.waveformId,validDest[destLinkArray[found]->linkId.waveformId].Size());
            found++;
            //ListT<NodeIt_t, false, EqualTo<NodeId_t> > :: Iterator it = invalidDest.Find(
            //invalidDest->DeleteItem(destArray[i]);
            //mark patternClientsMap[pid]->perMessageInfoMap[msgId]->validDest[i] as valid.
            //patternClientsMap[pid]->perMessageInfoMap[newMsgToken]->validDest[i] = wfid;
            //break; //no need to keep looking for node id in remaining waveform
        }
        //Debug_Printf(DBG_CORE_API, "Node %d is not on wf %d\n",destArray[i], wfid);
        //if(wfid == 16){//Exhausted all waveform. Generate NAck for the ones not found
        else{
            Debug_Printf(DBG_CORE_API,"FrameworkBase::Send:: Node %d not present in Neighbor Table pattern %d should check its logic\n", destArray[i], pid); fflush(stdout);
            nbrsNotFound[notFound] = destArray[i];
            notFound++;
            //Debug_Printf(DBG_CORE_API,"Destination %d has link %p, with quality %f!\n", destArray[i], destLinkArray[i], destLinkArray[i]->metrics.quality); fflush(stdout);
        }
        //}
    }
    /*Debug_Printf(DBG_CORE_API, "Show waveform which each destination is on \n");
  for(int index = 0 ; index < noOfDest; index++){
     Debug_Printf(DBG_CORE_API, "destination %d , node id %d is on wf %d\n",index, patternClientsMap[pid]->perMessageInfoMap[newMsgToken]->destinatioins[index],
                                                    patternClientsMap[pid]->perMessageInfoMap[newMsgToken]->waveform[index]);
  } */

    /*Debug_Printf(DBG_CORE_API, "Size of invalidDest is %d\n", invalidDest->Size());
  for(unsigned int index =0 ; index < invalidDest->Size(); index++){
     Debug_Printf(DBG_CORE_API, "invalidDest[%d] is %d\n", index, invalidDest->GetItem(index));
  }*/

    struct DataStatusParam nack(newMsgToken);
    for(uint16_t index = 0; index < notFound; index++){
        nack.destArray[index] = nbrsNotFound[index];
        nack.statusType[index]=Core::Dataflow::PDN_WF_RECV;
        nack.statusValue[index]=false;
        Debug_Printf(DBG_CORE_API, "FrameworkBase::Node %d not found \n",nack.destArray[index]);
    }
    nack.noOfDest = notFound;
    if(nack.noOfDest != 0){
        Debug_Printf(DBG_CORE_API, "FramewrokBase::Send Nack(PDN_WF_RECV) to framework\n");
        param.nonce = nonce;
        eventDispatcher->InvokeDataStatusEvent(pid,nack);
#if Debug_Flag
        fstat.total_invalid_destination += nack.noOfDest;
#endif
    }
    if(notFound == noOfDest){
        Debug_Printf(DBG_CORE_API, "Decrementing noOfoutstandingPkts by 1 as no destination was found on current neighbor table\n");
        patternClientsMap[pid]->noOfOutstandingPkts--;
    }
    Debug_Printf(DBG_CORE_API, "FramewrokBase::now remaining_node is %d\n", noOfDest-notFound);
    //patternClientsMap[pid]->perMessageInfoMap[newMsgToken]->remaining_node -= invalidDest->Size();


    //update perMessageInfo
    //TODO:CINT
    /*Debug_Printf(DBG_CORE_API, "size of invalidDest is %d\n",invalidDest->Size());
  for(unsigned int nacK_index =0; nacK_index < invalidDest->Size(); nacK_index++){
      for(int index =0; index < noOfDest; index++){
          //Debug_Printf(DBG_CORE_API, "destArray[%d] is %d, nacked_node is %d\n",index,destArray[index], invalidDest->GetItem(nacK_index));
          if(destArray[index] == invalidDest->GetItem(nacK_index)){
             Debug_Printf(DBG_CORE_API, "This node,(id %d) get PDN_WF_RECV nack\n",invalidDest->GetItem(nacK_index));
              patternClientsMap[pid]->perMessageInfoMap[newMsgToken]->ackWFRECV[index] = false;
              patternClientsMap[pid]->perMessageInfoMap[newMsgToken]->ackStatus[index] = PDN_WF_RECV;
          }
      }
  }*/

    //Debug.. check validDest look ok.
    /*for(int index =0; index < noOfDest; index++){
     Debug_Printf(DBG_CORE_API, "validDest[%d] is %d\n", index, patternClientsMap[pid]->perMessageInfoMap[newMsgToken]->validDest[index]);
  }
  delete(invalidDest);
  invalidDest = NULL;
     */


    //debuggin show current ackStatus array
    /*Debug_Printf(DBG_CORE_API, "Show current ackStatus\n");
  for(int index=0; index < noOfDest; index++){
      switch(patternClientsMap[pid]->perMessageInfoMap[newMsgToken]->ackStatus[index]){
          case PDN_FW_RECV:Debug_Printf(DBG_CORE_API, "PDN_FW_RECV\n"); break;
          case PDN_WF_RECV:Debug_Printf(DBG_CORE_API, "PDN_WF_RECV\n"); break;
          case PDN_WF_SENT:Debug_Printf(DBG_CORE_API, "PDN_WF_SENT\n"); break;
          case PDN_DST_RECV:Debug_Printf(DBG_CORE_API, "PDN_DST_RECV\n"); break;
      }
  }*/

    //check size of found
    Debug_Printf(DBG_CORE_API, "FramewrokBase::size of destLinkArray is %d\n", found);
    Debug_Printf(DBG_CORE_API, "FramewrokBase::Setting message type of msgId %d to Pattern_Type\n",newMsgToken);
    msg.SetType(Types::PATTERN_MSG);
    //msg.SetType(Types::Ack_Request_Type );
    //Send message to the packetHandler, (masahiro, I think the original design assumed wfid to be specified.)
    uint16_t wf = 0;
    Debug_Printf(DBG_CORE_API, "FrameworkBase:: show first destination %d\n",destArray[0]);

    //Do not bother to send information to MC if message has 0 destination
    if(found != 0){
        pktHandler->EnqueueToMC(pid, &msg, destArray, destLinkArray, found ,false,wf,newMsgToken,nonce,false);
        Debug_Printf(DBG_CORE_API, "FrameworkBase:: Enqueued message.\n");
#if Debug_Flag
        fstat.total_valid_sent += found;
#endif
    }

    for(uint16_t wf = 0; wf < MAX_WAVEFORMS; wf++){
        Debug_Printf(DBG_CORE_API,"FramewrokBase::number of remaining packet on wf %d is %d\n",wf,pktHandler->GetRemainingPacket(wf));
        if(pktHandler->GetRemainingPacket(wf) >= 1){
            //uint64_t _delay = rnd->GetNext();
            Debug_Printf(DBG_CORE_API, "FramewrokBase::Trigger Fetching message from MC on %d\n",wf);
            //pktHandler->wfTimer[wf]->Change(_delay, ONE_SHOT);
            pktHandler->ProcessOutgoingMessages(wf);
        }
    }
    return;
}

Link* FrameworkBase::GetPolicyBasedLinkForNeighbor(NodeId_t node_id,LinkComparatorTypeE lcType)
{
    LinkSelectionPolicyE linkPolicy = PolicyManager::GetLinkSelectionPolicy();
    Link* ret;
    LinkMapComparatorI *_comp;
    switch (linkPolicy){
    case Core::Policy::BEST_QUALITY:{
        _comp= new QualityComparator();
        ret = nbrTable.GetLinkwComp(node_id,*_comp);
        delete _comp;
        break;
    }
    case Core::Policy::PREFFERED_WAVEFORM:{
        WaveformId_t wid= PolicyManager::GetPrefferedWaveform();
        ret = nbrTable.GetNeighborLink(node_id, wid);
        break;
    }
    case Core::Policy::LEAST_DELAY:
        _comp= new MinAvgLatencyComparator();
        ret = nbrTable.GetLinkwComp(node_id,*_comp);
        delete _comp;
        break;
    case Core::Policy::LEAST_ENERGY:
        _comp= new MinEnergyComparator();
        ret = nbrTable.GetLinkwComp(node_id,*_comp);
        delete _comp;
        break;
    case Core::Policy::PATTERN_DEFINED:
    default:
        ret = GetCompBasedLinkForNeighbor(node_id,lcType);
        break;
    }
    return ret;
}
Link* FrameworkBase::GetCompBasedLinkForNeighbor(NodeId_t node_id,LinkComparatorTypeE lcType){
    Link* ret;
    //create a new one.
    LinkMapComparatorI *_comp;
    switch(lcType){
    case QUALITY_LC:
        _comp= new QualityComparator();
        break;
    case COST_LC:
        _comp= new CostComparator();
        break;
    case QUAL_XMIT_DELAY_LC:
        _comp= new QualAndXmitDelayComparator();
        break;
    case ENERGY_LC:
    	_comp= new MinEnergyComparator();
    	break;
    default:
        _comp= new QualityComparator();
        break;
    }
    ret = nbrTable.GetLinkwComp(node_id,*_comp);
    delete _comp;
    return ret;
}



/*
 * Framework Behaviro: If waveform does not support broadcast, it will convert it to multiple destination unicast. 
                       It stores nonce value and it will drop packet if nonce value is same.
                       The acknowledgement says noOfDest is 0.
 * Pattern Behavior:   Pattern starts timer to wait for reply from Framework. If it does not receive reply before timer expires.
                       it may send same packet with same nonce value
 */

void FrameworkBase::UnAddressedSend(PatternId_t pid, FMessage_t& msg, WaveformId_t wid, uint16_t nonce){
    FMessageId_t  newMsgToken = pktHandler->GetNewFrameworkMsgId();
    //check wavefirn id is valid
    WF_AttributeMap_t::Iterator it_waveform = wfAttributeMap->Find(wid);
    if(it_waveform == wfAttributeMap->End()){
        Debug_Printf(DBG_CORE_API, "FrameworkBase::UnAddressedSend  wfid %d does not exist!\n",wid);
        Debug_Warning("FrameworkBase::UnAddressedSend: Waveform %d, doesnot exist. Send NACK (PDN_WF_RECV) to pattern\n", wid);
        struct DataStatusParam param(newMsgToken);
        //param.dest = 0;
        param.statusType[0]=Core::Dataflow::PDN_FW_RECV;
        param.statusValue[0]=false;
        param.noOfDest = 0;
        param.readyToReceive = false;
        eventDispatcher->InvokeDataStatusEvent(pid,param);
        return;
    }
    //waveform id is valid. Check message type
    if(msg.GetType() == Types::ESTIMATION_MSG || msg.GetType() == Types::DISCOVERY_MSG){
        if(!(*wfAttributeMap)[wid].broadcastSupport){
            //waveform does not support broadcast
            Debug_Printf(DBG_CORE_API,"FramewrokBase::But broadcast is not supported on waveform %d, dropping packet!!\n", wid);
            struct DataStatusParam param(pid);
            param.noOfDest = 0;
            param.statusType[0]=Core::Dataflow::PDN_BROADCAST_NOT_SUPPORTED;
            param.statusValue[0] = true;
            param.readyToReceive = true; //As this request is rejected, Framework can still accept data
            param.nonce = nonce;
            eventDispatcher->InvokeDataStatusEvent(pid,param);
            return;
        }else {
            //FMessageId_t newMsgToken = pktHandler->GetNewFrameworkMsgId();
            Debug_Printf(DBG_CORE_API, "FramewrokBase::Assign new message Id:%d\n",newMsgToken);
            //
#if Debug_Flag
            fstatb.le_broadcast_sent++;
#endif
            //pktHandler->msgIdToPidMap.Insert(newMsgToken,pid);

            Debug_Printf(DBG_CORE_API,"FramewrokBase::Sending either LE_Type of Discovery_Type\n");
            Debug_Printf(DBG_CORE_API,"FramewrokBase::msg points to address %p\n",&msg);

            pktHandler->EnqueueToMC(pid, &msg, NULL, NULL, 0,true, wid, newMsgToken,nonce,0);
            Debug_Printf(DBG_CORE_API, "FramewrokBase::Number of remaining packet on wf %d is %d\n",wid,pktHandler->GetRemainingPacket(wid));
            if(pktHandler->GetRemainingPacket(wid) == 1){
                pktHandler->ProcessOutgoingMessages(wid);
            }
            return;
        }
    }else { //Pattern Type
        if(pid==0){
            Debug_Warning("FrameworkBase::UnAddressedSend: Pattern id is 0, something is wrong\n");
        }
        //check if this waveform supports broradcast or not.
        //if(!(*wfAttributeMap)[wid].broadcastSupport){
        if(!(*wfAttributeMap)[wid].broadcastSupport){
            Debug_Printf(DBG_CORE_API,"FramewrokBase::Broadcast is not supported on waveform %d\n", wid);
            this->MinimalWaveformSpray(pid, msg, wid, nonce);
            return;
        }
        PatternClientMap_t :: Iterator it = patternClientsMap.Find(pid);
        if(it != patternClientsMap.End()){
            Debug_Printf(DBG_CORE_API, "FramewrokBase::UnAddressedSend:patternClientsMap exists for this pid %d\n",pid);
            Debug_Printf(DBG_CORE_API, "FramewrokBase::UnAddressedSend:Now check if framewrok can accept new packet or not\n");
            if(patternClientsMap[pid]->noOfOutstandingPkts >= outstandingPacketsPetPattern){
                Debug_Printf(DBG_CORE_API, "FramewrokBase::UnAddressedSend:Framework cannot accept any packet from Pid %d, noOfOustandingPkets is %d\n",pid,patternClientsMap[pid]->noOfOutstandingPkts);
                Debug_Printf(DBG_CORE_API, "FramewrokBase::UnAddressedSend:Prepare and send PDN_FW_RECV nack\n");
                struct DataStatusParam param( 0);
                //param.dest= NULL;
                param.messageId=0;
                param.noOfDest =0;
                param.statusType[0] = Core::Dataflow::PDN_FW_RECV;
                param.statusValue[0]=false;
                param.nonce = nonce;
                param.readyToReceive = false;
#if Debug_Flag
                fstatb.negative_fw_ack++;
#endif
                eventDispatcher->InvokeDataStatusEvent(pid,param);
                return;
            }
        }else {
            Debug_Printf(DBG_CORE_API, "FramewrokBase::Didn't find pattern id %d in my db, something wrong, did pattern register? ", pid);
        }
        //check nonce value
        if(patternClientsMap[pid]->nonce == nonce){
            Debug_Warning("FrameworkBase::Broadcast:: Incoming packet has same nonce value %d\n",nonce);
            //Send Acknowledgment with assigned fwMsgId
            Debug_Printf(DBG_CORE_API,"FrameworkBase::Broadcast:: send acknowledgment again\n");
            struct DataStatusParam param(patternClientsMap[pid]->msgId);
            //param.dest=NULL;
            param.noOfDest = 0;
            param.statusType[0]=Core::Dataflow::PDN_FW_RECV;
            param.statusValue[0] = true;
            param.readyToReceive = false;
            param.nonce = nonce;
            eventDispatcher->InvokeDataStatusEvent(pid,param);
#if Debug_Flag
            fstatb.negative_fw_ack++;
#endif
            return; //exit SendData
        }
        //Nonce value is different.
        Debug_Printf(DBG_CORE_API,"FrameworkBase::Broadcast, framework nonce %d, nonce send with msg %d\n",patternClientsMap[pid]->nonce,nonce);
        // FMessageId_t  newMsgToken = pktHandler->GetNewFrameworkMsgId();
        Debug_Printf(DBG_CORE_API, "FrameworkBase::Broadccast:: newMsgToken %d\n",newMsgToken);
        Debug_Printf(DBG_CORE_API, "FrameworkBase::Broadcast:: Incoming packet has different nonce value %d\n",nonce);
        patternClientsMap[pid]->noOfOutstandingPkts++; //increment number of outstanding Packets;
        Debug_Printf(DBG_CORE_API, "Incremented noOfOutstandingPkts by 1 :%d\n",patternClientsMap[pid]->noOfOutstandingPkts);
        patternClientsMap[pid]->nonce = nonce;
        patternClientsMap[pid]->msgId = newMsgToken;
        patternClientsMap[pid]->readyToReceive = false;
        //just in case, check I am not trying to put same information.
        BSTMapT< FMessageId_t  , PatternId_t > ::Iterator it_msg = pktHandler->msgIdToPidMap.Find(newMsgToken);
        if(it_msg != pktHandler->msgIdToPidMap.End()){
            Debug_Warning("FrameworkBase::Broadcast: Something is wrong. msgId %d already exists in msgIdToPidMap!\n",newMsgToken);
        }
        Debug_Printf(DBG_CORE_API, "FramewrokBBase::Add msgId(%d) ,pid (%d) pair to msgIdToPidMap\n",newMsgToken,pid);
        pktHandler->msgIdToPidMap.Insert(newMsgToken,pid);
        Debug_Printf(DBG_CORE_API, "FrameworkBase::Broadast::Updating noOfOutstandingPkts to %d and nonce value to %d\n",patternClientsMap[pid]->noOfOutstandingPkts, nonce);
        //Debug_Printf(DBG_CORE_API, "Showing address of perMessageInfo %p, %p\n", obj, patternClientsMap[pid]->perMessageInfoMap[newMsgToken]);
        Debug_Printf(DBG_CORE_API, "FramewrokBase::Broadcast Prepare acknowledgment PDN_FW_RECV\n");
        struct DataStatusParam param(newMsgToken);
        param.noOfDest = 0;
        param.statusType[0]=Core::Dataflow::PDN_FW_RECV;
        param.statusValue[0] = true;
        param.readyToReceive = false;
        param.nonce = nonce;
#if Debug_Flag
        fstatb.positive_fw_ack++;
#endif
        eventDispatcher->InvokeDataStatusEvent(pid,param);

        //check

        WF_AttributeMap_t::Iterator it_waveform = wfAttributeMap->Find(wid);
        if(it_waveform == wfAttributeMap->End()){
            Debug_Printf(DBG_CORE_API, "FrameworkBase::UnAddressedSend  wfid %d does not exist!\n",wid);
            Debug_Printf(DBG_CORE_API, "FrameworkBase::UnAddressedSend Send NACK (PDN_WF_RECV) to patter\n");
            struct DataStatusParam param(newMsgToken);
            //param.dest = 0;
            param.statusType[0]=Core::Dataflow::PDN_FW_RECV;
            param.statusValue[0]=false;
            param.noOfDest = 0;
            param.readyToReceive = false;
            eventDispatcher->InvokeDataStatusEvent(pid,param);
        }else{
            //wid exists in wfAttributeMap. check if this wid supports broadcast or not
            if(it_waveform->Second().broadcastSupport == true){
#if Debug_Flag
                fstatb.data_broadcast_sent++;
#endif
                //bool flag = false;
                //if(flag){
                Debug_Printf(DBG_CORE_API, "FramewrokBase::Broadcast wid %d supports broadcast\n",wid);
                Debug_Printf(DBG_CORE_API, "FrameworkBase::Setting msg type to Pattern_Type\n");
                msg.SetType(Types::PATTERN_MSG);
                pktHandler->EnqueueToMC(pid,&msg,0,0,0,true,wid,newMsgToken,nonce,false);
                //Debug_Printf(DBG_CORE_API,"FramewrokBase::msg points to address %p\n",&msg);
            }
            else{
                Debug_Printf(DBG_CORE_API, "FrameworkBase::Broadcast wid %d does not support broadcast\n",wid);
                //this->SoftwareBroadCast(pid, msg, wid, nonce, newMsgToken);
            }
        }

        //Now message is enqued to corresponding MessageCabinet. Fetch message to be sent
        if(pktHandler->GetRemainingPacket(wid) > 0){
            pktHandler->ProcessOutgoingMessages(wid);
        }
        //Debug_Printf(DBG_CORE_API,"FrameworkBase::Broadcast:: Sent the packet to the message cabinet with %d \n", newMsgToken); fflush(stdout);



        /*******************************************************************************************************
   Debug_Printf(DBG_CORE_API,"FramewrokBase::Convert broadcast to multiple destination unicast\n");
    //First get size of neighbor table. This table contains neighbors for all waveform.
    uint16_t size = nbrTable.Size();
    //create list contains nodeId for wid
    ListT<NodeId_t,false, EqualTo<NodeId_t> >* nodeList = new ListT<NodeId_t,false, EqualTo<NodeId_t> >;
    //now count number of neighbor for wfid wid
    uint16_t count =0;
    for(uint16_t index =0; index < size; index++){
	//Get neighbor id
	NodeId_t neighbor_id = nbrTable.GetNeighborId(index);
	//Then get neighbor link
	Link* neighbor_link = nbrTable.GetNeighborLink(neighbor_id);
	Debug_Printf(DBG_CORE_API,"FramewrokBase::link %p\n",neighbor_link);
	Debug_Printf(DBG_CORE_API, "FramewrokBase:: nbrTable[%d] is %d, wfid: %d\n",index, neighbor_link->linkId.nodeId,neighbor_link->linkId.waveformId);
	Debug_Printf(DBG_CORE_API, "FramewrokBase::neighbor_id is %d\n",neighbor_id);
	if(nbrTable.GetNeighborLink(neighbor_id)->linkId.waveformId == wid){
	   Debug_Printf(DBG_CORE_API,"FramewrokBase::Match found\n");
	    nodeList->InsertBack(neighbor_id);
	    count++;
	}
    }

    //now create destArray
    NodeId_t* destArray = new NodeId_t[count];
    Link* destLinkArray[count];
   Debug_Printf(DBG_CORE_API,"FramewrokBase::Show count value %d\n",count);
    //populate array
   Debug_Printf(DBG_CORE_API, "FramewrokBase::Show dest list: ");
    for(uint16_t index =0; index < count; index++){
	    destArray[index] = (*nodeList)[index];
	    destLinkArray[index] = nbrTable.GetNeighborLink(destArray[index]);
	   Debug_Printf(DBG_CORE_API,"link %p\n",destLinkArray[index]);
	   Debug_Printf(DBG_CORE_API,"%d\n",destArray[index]);
    }
   Debug_Printf(DBG_CORE_API, "\n");

    for(uint16_t index =0 ; index < count; index++){
     Debug_Printf(DBG_CORE_API, "FramewrokBase::destLinkArray[%d], NodeId %d, wfid%d\n", index, destLinkArray[index]->linkId.nodeId, destLinkArray[index]->linkId.waveformId);
    }
    //Put the packet in the cabinet and send it according to its turn 
    msg.SetType(Types::Pattern_Type);
    pktHandler->EnqueueToMC(pid, &msg, destArray, destLinkArray, count,false,wid,newMsgToken);
         ************************************************************************************************/
    }
}

//Neighborhood::PatternNeighborTableI& FrameworkBase::GetNeighborTable (PatternId_t id){
//  return nbrManager.GetNeighborTable(id);
//}

void  FrameworkBase::ProcessLinkEstimationMsg (WF_MessageBase& msg){
    //WF_Message_n64_t *rcvmsg = (WF_Message_n64_t *)&msg;

    Debug_Printf(DBG_CORE,"FrameworkBase::ProcessLinkEstimationMsg:: msg.GetWaveform %d\n", msg.GetWaveform()); fflush(stdout);
    LinkEstimatorI<uint64_t> *li = LES.estimatorHash[msg.GetWaveform()];

    if(li != 0){
        //Debug_Printf(DBG_CORE_ESTIMATION,"FrameworkBase:: ProcessLinkEstimationMsg:: Address of link estimator is %p\n", li);fflush(stdout);
        li->OnPacketReceive(&msg);
    }else {
        Debug_Printf(DBG_CORE,"FrameworkBase::ProcessLinkEstimationMsg:: Error, link estimation reference is not set for waveform %d\n", msg.GetWaveform()); fflush(stdout);
    }
    Debug_Printf(DBG_CORE_DATAFLOW,"FrrameworkBase::ProcessLinkEstimationMsg:: Deleting WF_MessageBase obj at %p\n",&msg);
}

void  FrameworkBase::ProcessDiscoveryMsg (WF_MessageBase& msg){
	FMessage_t *disMsg = new FMessage_t();
	disMsg->SetPayloadSize(msg.GetPayloadSize());
	disMsg->SetPayload(msg.GetPayload());
	disMsg->SetSource(msg.GetSrcNodeID());
    //discovery->ProcessMessage(pktAdaptor64->Convert_WM_to_FM(*rcvmsg,false));
	discovery->ProcessMessage(*disMsg);
    Debug_Printf(DBG_CORE_DATAFLOW,"FrameworkBase::ProcessDiscoveryMsg:: Deleting WF_MessageBase obj at %p\n",&msg);
	delete(&msg);
}



PatternClient* FrameworkBase::FindPattern (uint16_t id) {
    return patternClientsMap[id];
}


/*
const WaveformInfo& FrameworkBase::GetWaveformInfo(uint16_t wfId){
  localWfInfo = shortLink->GetWaveformInfo(wfId);
  return localWfInfo;
}
 */

void FrameworkBase::SendToWF(WaveformId_t wfId, bool bcast, MessageTypeE type, NodeId_t *destArray, uint16_t noOfDest, FMessage_t& msg) {
    if(bcast){
        Debug_Printf(DBG_CORE,"Core:: SendtoWF: broadcast packet..\n");
        msg.SetType(type);
        UnAddressedSend(0,msg,wfId,0);//UnAddressedSend does not use nonce value for non pattern type
    }
    else {
        Debug_Printf(DBG_CORE,"Core:: SendtoWF: we don't support nonbroadcast packets for non patterns, at this point\n");
    }
}


void FrameworkBase::GetWaveformIDs(WaveformId_t* idArray, uint8_t sizeOfArray)
{

    WaveformMap_t::Iterator it = wfMap.Begin();
    uint8_t i=0;
    for ( ;it!= wfMap.End();++it ){
        idArray[i] = it->First();
        i++;
    }

    //return wfMap.size();
}


uint8_t FrameworkBase::GetNumberOfWaveforms()
{
    return wfMap.Size();
}

PatternId_t FrameworkBase::NewPatternInstanceRequest(PatternTypeE ptype, const char uniqueName[128])
{
    PatternId_t patternId = Core::Naming::GetNewPatternInstanceId(ptype, uniqueName );
    Debug_Printf(DBG_SHIM, "PTN_ControllerShim::Deserialize: PTN2FW_Call_RegisterPatternRequest got new PID patternId = %d uniqueName = %s \n", patternId,uniqueName);
    return(patternId);
}


/*Framewrok: For given destination, it will check if anyof them already exists or not
 *           For destination does not exist or already exists, it will return not successful
 *
 * Pattern: This method returns multiples responses. Pattern is responsible for keep tracking of all the responces
 *          Framewrok returns.
 */
//TODO: The following should be implemented
void FrameworkBase::AddDestinationRequest(PatternId_t patternId, FMessageId_t  msgId, NodeId_t* destArray, uint16_t noOfNbrs)
{
    LinkComparatorTypeE lcType = nbrManager.GetLinkComparator(patternId);
    AddDestinationRequest(patternId, msgId, destArray, noOfNbrs, lcType);
}
//void FrameworkBase::AddDestinationRequest(PatternId_t patternId, FMessageId_t  msgId, NodeId_t* destArray, uint16_t noOfNbrs,LinkComparatorTypeE lcType ){
//
//}
void FrameworkBase::AddDestinationRequest(PatternId_t patternId, FMessageId_t  msgId, NodeId_t* destArray, uint16_t noOfNbrs, LinkComparatorTypeE lcType)
{
    Debug_Printf(DBG_CORE, "FrameworkBase:: Entered AddDestinationRequest\n");

    AddDestinationResponse_Data data;
    data.msgId = msgId;


    Debug_Printf(DBG_CORE, "FrameworkBase:: If this is broadcast reject\n");
    FMsgMap::Iterator it_fmsg1 = pktHandler->mc->framworkMsgMap->Find(msgId);
    if(it_fmsg1 != pktHandler->mc->framworkMsgMap->End()){
        if(it_fmsg1->Second()->broadcast){
            Debug_Printf(DBG_CORE_DATAFLOW, "FramewrokBase:: Rejecting request as this msgId is for broadcast\n");
            data.status = false;
            data.noOfDest = 0;
            data.msgId = msgId;
            Debug_Printf(DBG_CORE, "PacketHandler::AddDestinationRequest: Return what Framework knows to pattern\n");
            ControlResponseParam param;
            param.data = &data;
            param.type =  PTN_AddDestinationResponse;
            eventDispatcher->InvokeControlEvent(patternId, param);
            return;
        }
        Debug_Printf(DBG_CORE, "FrameworkBase:: FMsgMap %p msg_ptr %p  payload_ptr %p. msg type is %d \n",it_fmsg1->Second(), it_fmsg1->Second()->msg, it_fmsg1->Second()->msg->GetPayload(),it_fmsg1->Second()->msg->GetType());
    }

    data.noOfDest = 0;
    pktHandler->Add_Node(patternId, msgId, destArray, noOfNbrs, lcType, data);
    if(data.noOfDest > 0){
        bool all_success = true;
        for(uint16_t index =0; index < data.noOfDest ; index++){
            if(data.add_status[index] == false){
                all_success = false;
                break;
            }
        }
        data.status = all_success;
        Debug_Printf(DBG_CORE, "PacketHandler::AddDestinationRequest: Return what Framework knows to pattern\n");
        ControlResponseParam param;
        param.data = &data;
        param.type =  PTN_AddDestinationResponse;
        eventDispatcher->InvokeControlEvent(patternId, param);
    }


    //Debugging
    FMsgMap::Iterator it_fmsg = pktHandler->mc->framworkMsgMap->Find(msgId);
    if(it_fmsg != pktHandler->mc->framworkMsgMap->End()){
        Debug_Printf(DBG_CORE, "FrameworkBase:: No of dest is %d\n", it_fmsg->Second()->noOfDest);
        for(uint16_t wf_index = 1; wf_index < MAX_WAVEFORMS; wf_index++){
            if(it_fmsg->Second()->wfQElement[wf_index].Size() != 0){
                Debug_Printf(DBG_CORE, "FrameworkBase:: Heap:: No of dest is %d\n", it_fmsg->Second()->wfQElement[wf_index].Begin()->GetData()->noOfDest);

                for(uint16_t index =0; index < it_fmsg->Second()->wfQElement[wf_index].Begin()->GetData()->noOfDest; index++){
                    Debug_Printf(DBG_CORE, "FrameworkBase:: dest[%d] is %d\n", index, it_fmsg->Second()->linkArray[index]->linkId.nodeId);
                    Debug_Printf(DBG_CORE, "FrameworkBase:: Heap:: dest[%d] is %ld\n",index, it_fmsg->Second()->wfQElement[wf_index].Begin()->GetData()->noOfDest);
                }
            }
        }
    }
}
/*
 *Framework: Returns separate response for successful nodes and unsuccessful nodes.
             If request propagates to wavefrom, framework sends further responses
             if msgId does not exist, it will set status to false and noOfDest to 0
 *Pattern. It needs to check if it receives all the responses from framework or waveform.
             pattern should not access destArray if noOfDest is 0.
 */

void FrameworkBase::ReplacePayloadRequest(PatternId_t patternId, FMessageId_t  msgId, void* payload, uint16_t sizeOfPayload)
{/*
   Debug_Printf(DBG_CORE, "FrameworkBase:: Check if msgId %d exists or not. \n",msgId);
	BSTMapT< FMessageId_t , FMessageQElement* > ::Iterator it_fmsg2 = pktHandler->mc->framworkMsgMap->Find(msgId);
	if(it_fmsg2 != pktHandler->mc->framworkMsgMap->End()){
		uint8_t* addr = it_fmsg2->Second()->msg->GetPayload();
    	for(uint16_t index =0; index < it_fmsg2->Second()->msg->GetPayloadSize(); index++){
    		printf("%x",addr[index]);
    	}
	}*/
    printf("\n");
    Debug_Printf(DBG_CORE, "FrameworkBase:: Entered ReplacePayloadRequest\n");
    ReplacePayloadResponse_Data data;// = new ReplacePayloadResponse_Param;

    pktHandler->ReplacePayloadRequest(sizeOfPayload, patternId, msgId, payload, data);
    bool all_success = true;
    for(uint16_t index =0; index < data.noOfDest ; index++){
        if(data.replace_status[index] == false){
            all_success = false;
            break;
        }
    }
    if(data.noOfDest == 0){
        all_success = data.status;
    }
    data.status = all_success;
    Debug_Printf(DBG_CORE, "PacketHandler::ReplacePayload:: Exhausted all waveform. Return what Framework knows to pattern\n");
    ControlResponseParam param;
    param.data = &data;
    param.type = PTN_ReplacePayloadResponse;
    eventDispatcher->InvokeControlEvent(patternId, param);
    /*
	//debugging
	BSTMapT< FMessageId_t , FMessageQElement* > ::Iterator it_fmsg = pktHandler->mc->framworkMsgMap->Find(msgId);
	if(it_fmsg != pktHandler->mc->framworkMsgMap->End()){
	   	for(uint16_t index =0; index < it_fmsg->Second()->noOfDest; index++){
	   		Debug_Printf(DBG_CORE, "FrameworkBase:: dest[%d] payload is %p \n", index, it_fmsg->Second()->msg->GetPayload());
	   		Debug_Printf(DBG_CORE, "FrameworkBase:: Heap:: dest[%d] payload is %p  \n",index, it_fmsg->Second()->msg->GetPayload());
	   	}
		uint8_t* addr1 = it_fmsg->Second()->msg->GetPayload();
	    for(uint16_t index =0; index < it_fmsg->Second()->msg->GetPayloadSize(); index++){
	    	printf("%x",addr1[index]);
	    }
	}
     */
}
/*framework: framework returns either true or false for each destination given. */
void FrameworkBase::CancelDataRequest(PatternId_t patternId, FMessageId_t  msgId, NodeId_t* destArray, uint16_t noOfDest)
{

    //declare return variable
    CancelDataResponse_Data data;

    //set flag to detect all success
    bool all_success = true;
    pktHandler->CancelDataSendRequest(patternId, msgId, destArray, noOfDest, data);
    for(uint16_t index =0; index < data.noOfDest ; index++){
        Debug_Printf(DBG_CORE, "FrameworkBase:: Node %d :: status %d\n",data.destArray[index],data.cancel_status[index]);
        if(data.cancel_status[index] == false){
            all_success = false;
            break;
        }
    }
    if(noOfDest >0 && data.noOfDest == 0){ //If there are destinations for  this message bit we have no knowledge of any of them
    	return;
    }
    else{
        Debug_Printf(DBG_CORE, "FrameworkBase:: Send response to Pattern\n");
        data.status = all_success;
        ControlResponseParam param;
        param.data = &data;
        param.type =   PTN_CancelDataResponse;
        eventDispatcher->InvokeControlEvent(patternId, param);
    }

    /*
    BSTMapT< FMessageId_t , FMessageQElement* > ::Iterator it_fmsg = pktHandler->mc->framworkMsgMap->Find(msgId);
    if(it_fmsg != pktHandler->mc->framworkMsgMap->End()){
    	Debug_Printf(DBG_CORE, "some destination is removed.\n");
    	Debug_Printf(DBG_CORE, "No of dest is %d\n", it_fmsg->Second()->noOfDest);
    	Debug_Printf(DBG_CORE, "Heap:: No of dest is %d\n", it_fmsg->Second()->wfQElement[2]->noOfDest);
    	for(uint16_t index =0; index < it_fmsg->Second()->noOfDest; index++){
    		Debug_Printf(DBG_CORE, "dest[%d] is %d\n", index, it_fmsg->Second()->linkArray[index]->linkId.nodeId);
    		Debug_Printf(DBG_CORE, "Heap:: dest[%d] is %ld\n",index, it_fmsg->Second()->wfQElement[2]->destArray[index]);
    	}
    }*/
}


void FrameworkBase::DataStatusRequest(PatternId_t patternId, FMessageId_t  msgId)
{
    Debug_Printf(DBG_CORE, "Received DataStatusRequest from pattern %d, msgId %d\n",patternId, msgId);
    //DataStatusResponse_Data* data = new DataStatusResponse_Data;
    pktHandler->PrepareAndSendDataStatusResponse(msgId);
} 


void FrameworkBase::RefreshFrameworkAttribute()
{
	//struct FrameworkAttributes attr;
    int wf_count = 0;
    for(int index = 0; index < MAX_WAVEFORMS; index++){
        if(wfExists[index] == true){
            Debug_Printf(DBG_CORE, "FramewrokBase::waveform %d exits\n", index);
            fwAttribute.waveformIds[wf_count] = index;
            wf_count++;
        }
    }


    Debug_Printf(DBG_CORE, "FramewrokBase::Show wfAttributeMap size: %d\n",(*wfAttributeMap).Size());
    fwAttribute.numberOfWaveforms = wf_count;

    if(PolicyManager::GetMaxFWPacketPayloadSize() != 0){
    	fwAttribute.maxFrameworkPacketSize = PolicyManager::GetMaxFWPacketPayloadSize();
    }
    else{
    	fwAttribute.maxFrameworkPacketSize = 0;
        for(WF_AttributeMap_t::Iterator it = (*wfAttributeMap).Begin();it != (*wfAttributeMap).End(); ++it){
            Debug_Printf(DBG_CORE, "FramewrokBase::wf %d has maxFrameworkPacketSize of %d\n",it->Second().wfId,it->Second().maxPayloadSize);
        	if( fwAttribute.maxFrameworkPacketSize > it->Second().maxPayloadSize){
                Debug_Printf(DBG_CORE, "FramewrokBase::Current smallest maxFrameworkPacketSize is %d, updating it to %d",fwAttribute.maxFrameworkPacketSize,it->Second().maxPayloadSize);
                fwAttribute.maxFrameworkPacketSize = it->Second().maxPayloadSize;
        	}
        }
    }

//    Debug_Printf(DBG_CORE, "FramewrokBase::Show wfAttributeMap size: %d\n",(*wfAttributeMap).Size());
//    attr.numberOfWaveforms = wf_count;
//    WF_AttributeMap_t::Iterator it;
//    it = (*wfAttributeMap).Begin();
//    if(it== wfAttributeMap->End()){
//        printf("FramewrokBase:: Something seriously wrong. I have no waveforms.\n");
//    }
//    else {
//        if(PolicyManager::GetFrameworkMaxPacketPayloadSize() != 0){
//        	attr.maxFrameworkPacketSize = PolicyManager::GetFrameworkMaxPacketPayloadSize();
//        }
//        else{
//            attr.maxFrameworkPacketSize = it->Second().maxPayloadSize;
//            Debug_Printf(DBG_CORE, "FramewrokBase::wf %d has maxFrameworkPacketSize of %d\n", it->First() , attr.maxFrameworkPacketSize);
//            ++it;
//            for(;it != (*wfAttributeMap).End(); ++it){
//                Debug_Printf(DBG_CORE, "FramewrokBase::wf %d has maxFrameworkPacketSize of %d\n",it->Second().wfId,it->Second().maxPayloadSize);
//                if((maxFrameworkPacketSize != 0) && (maxFrameworkPacketSize < it->Second().maxPayloadSize)){
//                    Debug_Printf(DBG_CORE, "FramewrokBase::Current smallest maxFrameworkPacketSize is %d, updating it to %d",attr.maxFrameworkPacketSize,it->Second().maxPayloadSize);
//                    attr.maxFrameworkPacketSize = it->Second().maxPayloadSize;
//                }
//            }
//
//        }
//    }
   
}


/* Framework: For given pid, return number of wf given pid is useing and minmum maxFrameworkPacketSize */
void FrameworkBase::FrameworkAttributesRequest(PatternId_t pid)
{
    Debug_Printf(DBG_CORE, "FramewrokBase::Got FrameworkAttribugtesRequest\n");
    
	RefreshFrameworkAttribute();
	
    struct ControlResponseParam param;
    //param.sequenceNo;
    param.type = PTN_AttributeResponse;//1b
    param.dataSize = sizeof(FrameworkAttributes);//2b
    param.data = &fwAttribute;
    //Pass packet back to Pattern
    Debug_Printf(DBG_CORE, "FramewrokBase:: has %d waveforms, Invoke Control Event\n", fwAttribute.numberOfWaveforms);
    eventDispatcher->InvokeControlEvent(pid, param);
}


void FrameworkBase::SelectLinkComparatorRequest(PatternId_t patternId, LinkComparatorTypeE lcType)
{
    nbrManager.SetLinkComparator(patternId, lcType);

    struct ControlResponseParam param;
    SelectDataNotificationResponse_Data data;
    param.type = PTN_SelectDataNotificationResponse;
    param.dataSize = sizeof(data) ;//2b
    param.data = &data;
    data.status = true;

    Debug_Printf(DBG_CORE, "FramewrokBase:: Set the Link threshold for Patern %d waveforms, Invoke Control Event\n", patternId);
    eventDispatcher->InvokeControlEvent(patternId, param);

}

void FrameworkBase::SetLinkThresholdRequest(PatternId_t patternId, LinkMetrics threshold)
{
    nbrManager.SetNeighborThreshold(patternId, &threshold);

    struct ControlResponseParam param;
    SetLinkThresholdResponse_Data data;
    param.type = PTN_SetLinkThresholdResponse;
    param.dataSize = sizeof(data) ;//2b
    param.data = &data;
    data.status = true;

    Debug_Printf(DBG_CORE, "FramewrokBase:: Set the Link threshold for Patern %d waveforms, Invoke Control Event\n", patternId);
    eventDispatcher->InvokeControlEvent(patternId, param);

}

void FrameworkBase::SoftwareBroadCast(PatternId_t pid, FMessage_t& msg, WaveformId_t wid, uint16_t nonce, FMessageId_t  newMsgToken){

    Debug_Printf(DBG_CORE_DATAFLOW,"FramewrokBase::Convert broadcast to multiple destination unicast\n");
    //First get size of neighbor table. This table contains neighbors for all waveform.
    uint16_t size = nbrTable.Size();
    //create list contains nodeId for wid
    ListT<NodeId_t,false, EqualTo<NodeId_t> >* nodeList = new ListT<NodeId_t,false, EqualTo<NodeId_t> >;
    //now count number of neighbor for wfid wid
    uint16_t count =0;
    for(uint16_t index =0; index < size; index++){
        //Get neighbor id
        NodeId_t neighbor_id = nbrTable.GetNeighborId(index);
        //Then get neighbor link
        Link* neighbor_link = nbrTable.GetNeighborLink(neighbor_id);
        Debug_Printf(DBG_CORE_DATAFLOW,"FramewrokBase::link %p\n",neighbor_link);
        Debug_Printf(DBG_CORE_DATAFLOW, "FramewrokBase:: nbrTable[%d] is %d, wfid: %d\n",index, neighbor_link->linkId.nodeId,neighbor_link->linkId.waveformId);
        Debug_Printf(DBG_CORE_DATAFLOW, "FramewrokBase::neighbor_id is %d\n",neighbor_id);
        if(nbrTable.GetNeighborLink(neighbor_id)->linkId.waveformId == wid){
            Debug_Printf(DBG_CORE_DATAFLOW,"FramewrokBase::Match found\n");
            nodeList->InsertBack(neighbor_id);
            count++;
        }
    }

    //now create destArray
    NodeId_t* destArray = new NodeId_t[count];
    Link* destLinkArray[count];
    Debug_Printf(DBG_CORE_DATAFLOW,"FramewrokBase::Show count value %d\n",count);
    //populate array
    Debug_Printf(DBG_CORE_DATAFLOW, "FramewrokBase::Show dest list: ");
    for(uint16_t index =0; index < count; index++){
        destArray[index] = (*nodeList)[index];
        destLinkArray[index] = nbrTable.GetNeighborLink(destArray[index]);
        Debug_Printf(DBG_CORE_DATAFLOW,"FramewrokBase::link %p\n",destLinkArray[index]);
        Debug_Printf(DBG_CORE_DATAFLOW,"FramewrokBase::%d\n",destArray[index]);
    }
    Debug_Printf(DBG_CORE_DATAFLOW, "\n");

    for(uint16_t index =0 ; index < count; index++){
        Debug_Printf(DBG_CORE, "FramewrokBase::destLinkArray[%d], NodeId %d, wfid%d\n", index, destLinkArray[index]->linkId.nodeId, destLinkArray[index]->linkId.waveformId);
    }
    //Put the packet in the cabinet and send it according to its turn
    msg.SetType(Types::PATTERN_MSG);
    pktHandler->EnqueueToMC(pid, &msg, destArray, destLinkArray, count,false,wid,newMsgToken,nonce,true);
}

void FrameworkBase::MinimalWaveformSpray(PatternId_t pid, FMessage_t& msg, WaveformId_t wid, uint16_t nonce){
    Debug_Printf(DBG_CORE_DATAFLOW,"FramewrokBase::Waveform does not support Broadcast. Check if any known neighbor on wf exists\n");
    //First get size of neighbor table. This table contains neighbors for all waveform.
    uint16_t size = nbrTable.Size();
    if(size ==0){
        Debug_Printf(DBG_CORE_DATAFLOW,"FrameworkBase::Size of neighbor table is zero\n");
        //return error
        struct DataStatusParam param(pid);
        param.noOfDest = 0;
        param.statusType[0]=Core::Dataflow::PDN_BROADCAST_NOT_SUPPORTED;
        param.statusValue[0] = true;
        param.readyToReceive = true; //As this request is rejected, Framework can still accept data
        param.nonce = nonce;
        eventDispatcher->InvokeDataStatusEvent(pid,param);
        return;
    }
    //there is some known neighbor. But it may not be on this waveform.
    //create list contains nodeId for wid
    ListT<NodeId_t,false, EqualTo<NodeId_t> >* nodeList = new ListT<NodeId_t,false, EqualTo<NodeId_t> >;
    //now count number of neighbor for wfid wid
    uint16_t count =0;
    for(uint16_t index =0; index < size; index++){
        //Get neighbor id
        NodeId_t neighbor_id = nbrTable.GetNeighborId(index);
        //Then get neighbor link
        Link* neighbor_link = nbrTable.GetNeighborLink(neighbor_id);
        Debug_Printf(DBG_CORE_DATAFLOW,"FramewrokBase::link %p\n",neighbor_link);
        Debug_Printf(DBG_CORE_DATAFLOW, "FramewrokBase:: nbrTable[%d] is %d, wfid: %d\n",index, neighbor_link->linkId.nodeId,neighbor_link->linkId.waveformId);
        Debug_Printf(DBG_CORE_DATAFLOW, "FramewrokBase::neighbor_id is %d\n",neighbor_id);
        if(nbrTable.GetNeighborLink(neighbor_id)->linkId.waveformId == wid){
            Debug_Printf(DBG_CORE_DATAFLOW,"FramewrokBase::Match found\n");
            nodeList->InsertBack(neighbor_id);
            count++;
        }
    }
    if(count ==0){
        Debug_Printf(DBG_CORE_DATAFLOW,"FrameworkBase::No known neighbor on wf %d",wid);
        struct DataStatusParam param(pid);
        param.noOfDest = 0;
        param.statusType[0]=Core::Dataflow::PDN_BROADCAST_NOT_SUPPORTED;
        param.statusValue[0] = true;
        param.readyToReceive = true; //As this request is rejected, Framework can still accept data
        param.nonce = nonce;
        eventDispatcher->InvokeDataStatusEvent(pid,param);
        return;
    }
    //check outstanding packet
    PatternClientMap_t :: Iterator it = patternClientsMap.Find(pid);
    if(it != patternClientsMap.End()){
        Debug_Printf(DBG_CORE, "FramewrokBase::UnAddressedSend:patternClientsMap exists for this pid %d\n",pid);
        Debug_Printf(DBG_CORE, "FramewrokBase::UnAddressedSend:Now check if framewrok can accept new packet or not\n");
        if(patternClientsMap[pid]->noOfOutstandingPkts >= outstandingPacketsPetPattern){
            Debug_Printf(DBG_CORE, "FramewrokBase::UnAddressedSend:Framework cannot accept any packet from Pid %d, noOfOustandingPkets is %d\n",pid,patternClientsMap[pid]->noOfOutstandingPkts);
            Debug_Printf(DBG_CORE, "FramewrokBase::UnAddressedSend:Prepare and send PDN_FW_RECV nack\n");
            struct DataStatusParam param( 0);
            //param.dest= NULL;
            param.messageId=0;
            param.noOfDest =0;
            param.statusType[0] = Core::Dataflow::PDN_FW_RECV;
            param.statusValue[0]=false;
            param.nonce = nonce;
            param.readyToReceive = false;
#if Debug_Flag
            fstatb.negative_fw_ack++;
#endif
            eventDispatcher->InvokeDataStatusEvent(pid,param);
            return;
        }
    }else {
        Debug_Printf(DBG_CORE, "FramewrokBBase::Didn't find pattern id %d in my db, something wrong, did pattern register? ", pid);
    }
    //check nonce value
    if(patternClientsMap[pid]->nonce == nonce){
        Debug_Printf(DBG_CORE, "FrameworkBase::Broadcast:: Incoming packet has same nonce value %d\n",nonce);
        //Send Acknowledgment with assigned fwMsgId
        Debug_Printf(DBG_CORE,"FrameworkBase::Broadcast:: send acknowledgment again\n");
        struct DataStatusParam param(patternClientsMap[pid]->msgId);
        //param.dest=NULL;
        param.noOfDest = 0;
        param.statusType[0]=Core::Dataflow::PDN_FW_RECV;
        param.statusValue[0] = true;
        param.readyToReceive = false;
        param.nonce = nonce;
        eventDispatcher->InvokeDataStatusEvent(pid,param);
#if Debug_Flag
        fstatb.negative_fw_ack++;
#endif
        return; //exit SendData
    }
    //Nonce value is different.
    Debug_Printf(DBG_CORE_DATAFLOW,"FrameworkBase::Broadcast, framework nonce %d, nonce send with msg %d\n",patternClientsMap[pid]->nonce,nonce);
    FMessageId_t  newMsgToken = pktHandler->GetNewFrameworkMsgId();
    Debug_Printf(DBG_CORE_DATAFLOW, "FrameworkBase::Broadcast: newMsgToken %d",newMsgToken);
    Debug_Printf(DBG_CORE, "FrameworkBase::Broadcast:: Incoming packet has different nonce value %d\n",nonce);
    patternClientsMap[pid]->noOfOutstandingPkts++; //increment number of outstanding Packets;
    Debug_Printf(DBG_CORE_DATAFLOW, "Incremented noOfOutstandingPkts by 1 :%d\n",patternClientsMap[pid]->noOfOutstandingPkts);
    patternClientsMap[pid]->nonce = nonce;
    patternClientsMap[pid]->msgId = newMsgToken;
    patternClientsMap[pid]->readyToReceive = false;
    //just in case, check I am not trying to put same information.
    BSTMapT< FMessageId_t  , PatternId_t > ::Iterator it_msg = pktHandler->msgIdToPidMap.Find(newMsgToken);
    if(it_msg != pktHandler->msgIdToPidMap.End()){
        Debug_Printf(DBG_CORE_DATAFLOW, "Something is wrong. msgId %d already exists in msgIdToPidMap!\n",newMsgToken);
        abort();
    }
    Debug_Printf(DBG_CORE, "FramewrokBBase::Add msgId(%d) ,pid (%d) pair to msgIdToPidMap\n",newMsgToken,pid);
    pktHandler->msgIdToPidMap.Insert(newMsgToken,pid);
    Debug_Printf(DBG_CORE, "FrameworkBase::Broadast::Updating noOfOutstandingPkts to %d and nonce value to %d\n",patternClientsMap[pid]->noOfOutstandingPkts, nonce);
    //Debug_Printf(DBG_CORE, "Showing address of perMessageInfo %p, %p\n", obj, patternClientsMap[pid]->perMessageInfoMap[newMsgToken]);
    Debug_Printf(DBG_CORE, "FramewrokBase::Broadcast Prepare acknowledgment PDN_FW_RECV\n");
    struct DataStatusParam param(newMsgToken);
    param.noOfDest = 0;
    param.statusType[0]=Core::Dataflow::PDN_FW_RECV;
    param.statusValue[0] = true;
    param.readyToReceive = false;
    param.nonce = nonce;
#if Debug_Flag
    fstatb.positive_fw_ack++;
#endif
    eventDispatcher->InvokeDataStatusEvent(pid,param);


    //Known neighbor exists on wf. Randomly pick one and send unicast to it.
    uint64_t random_number = rnd->GetNext();
    uint16_t index = random_number % count;
    //now create destArray
    NodeId_t* destArray = new NodeId_t[1];
    Link* destLinkArray[1];
    Debug_Printf(DBG_CORE_DATAFLOW,"FramewrokBase::Show count value %d, random index %d\n",count, index);
    destArray[0] = (*nodeList)[index];
    destLinkArray[0] = nbrTable.GetNeighborLink(destArray[0]);
    Debug_Printf(DBG_CORE_DATAFLOW, "FramewrokBase::Show randomly selected dest %d\n",destArray[0]);
    Debug_Printf(DBG_CORE_DATAFLOW, "\n");

    //Put the packet in the cabinet and send it according to its turn
    msg.SetType(Types::PATTERN_MSG);
    //this is unicast with one destination so fix dest size to "1"
    pktHandler->EnqueueToMC(pid, &msg, destArray, destLinkArray, 1,false,wid,newMsgToken,nonce,true);
    Debug_Printf(DBG_CORE_DATAFLOW,"FrameworkBase::Show remaining packet on %d: %d\n",wid,pktHandler->GetRemainingPacket(wid));
    if(pktHandler->GetRemainingPacket(wid) == 1){
        pktHandler->ProcessOutgoingMessages(wid);
    }
}
void FrameworkBase::validationTimerHandler(uint32_t flag){
    //for validation
#if Debug_Flag
    MemMap<FrameworkStat> FrameworkAcknowledgement("framework.bin");
    FrameworkAcknowledgement.addRecord(fstat);
    Debug_Printf(DBG_CORE, "FrameworkBase::  writing to memmap %d %d \n",fstat.positive_frameworkAck , fstat.negative_frameworkAck);
    MemMap<FrameworkStatBroadcast> broadcast_summary("broadcast.bin");
    broadcast_summary.addRecord(fstatb);
    Debug_Printf(DBG_CORE, "FrameworkBase::  writing to memmap %d %d %d %d\n",fstatb.data_broadcast_sent, fstatb.le_broadcast_sent,fstatb.negative_fw_ack, fstatb.positive_fw_ack);
#endif
}
void FrameworkBase::debugTimerHandler(uint32_t flag){
	//for validation
#if Debug_Flag
	MemMap<NeighborStat> neighbor("neighbor.bin");
	nstat.neighbor_size = this->nbrTable.Size();

	neighbor.addRecord(nstat);
	MemMap<locationStat> location("locationi.bin");
	Location2D loc= LocationService::GetLocation();
	lstat.x = loc.x;
	lstat.y = loc.y;
	location.addRecord(lstat);
#endif
	if(DBG_FW_REPORT()){
		uint32_t nbrSize= nbrTable.Size();
		printf("\n\n");
		Debug::PrintTimeMicro();
		printf("::Framework:: Nbr Table Report: Size is %d \n", nbrSize);
		//NodeId_t ids[nbrTable.Size()];
		Link* linkSet[nbrSize];
		//nbrTable.GetNeighborsIds(ids);
		nbrTable.GetNeighborLinkSet(linkSet);
		for (int i=0;i< nbrSize; i++){
			//Link *lnk= nbrTable.GetNeighborLink(ids[i]);
			Link *lnk = linkSet[i];
			printf(" \t Nbr ID: %d,\t Wf Id: %d, \t WF Address: %s,\t Quality: %0.2f Delay: %0.2f\n", lnk->linkId.nodeId, lnk->linkId.waveformId, addressMap.LookUpWfAddress(lnk->linkId).c_str(), lnk->metrics.quality, lnk->metrics.avgLatency);
		}
		printf("\n");
	}

}

bool FrameworkBase::GenericSendMessage (WaveformId_t wfId, uint64_t wfdest, MessageTypeE msgType, FMessage_t &msg, bool unaddressed){
	if(unaddressed){
		
		Debug_Printf(DBG_CORE,"FrameworkBase:: SendEstimationMessage: broadcast packet..\n");
		UnAddressedSend(0,msg,wfId,0);//UnAddressedSend does not use nonce value for non pattern type
	}else {
		string wfAddrStr = NbrUtils::ConvertU64ToEthAddress(wfdest);
		LinkId id = addressMap.LookUpLinkId(wfAddrStr);
		SendData(0,&(id.nodeId),1,msg,0,true);	
	}
	return true;
}


////////////////EstimationCallback_I methods//////////////
bool FrameworkBase::SendEstimationMessage(WaveformId_t wfId, FMessage_t* msg)
{
    Debug_Printf(DBG_CORE,"FrameworkBase:: SendEstimationMessage: broadcast packet..\n");
    UnAddressedSend(0,*msg,wfId,0);//UnAddressedSend does not use nonce value for non pattern type
	return true;
}

void FrameworkBase::LinkEstimatorChangeHandler(WF_LinkEstimationParam_n64_t _param)
{
	//Update the neighbor table first.
	switch (_param.changeType){
		case NBR_NEW:
		{
			//Debug_Printf(DBG_CORE_ESTIMATION, "Core::FrameworkBase::Neighbor %X, on Waveform %d, has been added with quality %0.2f\n",_param.linkAddress,_param.wfid , _param.metrics.quality);
			pktHandler->AddOrUpdateWFAddress(_param.wfid, _param.linkAddress, _param.nodeId);
			string wfAddrStr = NbrUtils::ConvertU64ToEthAddress(_param.linkAddress);
			//uint64_t tempid =NbrUtils::ConvertEthAddressToU64(wfAddrStr);
			Debug_Printf(DBG_CORE_ESTIMATION, "Core::FrameworkBase::Neighbor %s, on Waveform %d, has been added with quality %0.2f\n",wfAddrStr.c_str(),_param.wfid , _param.metrics.quality);
			nbrTable.AddNeighbor(addressMap.LookUpLinkId(wfAddrStr).nodeId , _param.wfid, LinkMetrics(_param.metrics));
			break;
		}
		case NBR_DEAD:
		{
			//Debug_Printf(DBG_CORE_ESTIMATION, "Core::FrameworkBase::Neighbor %lu,  on Waveform %d, has died\n",_param.linkAddress, _param.wfid );
			
			string wfAddrStr = NbrUtils::ConvertU64ToEthAddress(_param.linkAddress);
			//printf("Core::FrameworkBase::Neighbor %X,  on Waveform %d, has died\n",_param.linkAddress, _param.wfid );
			printf("Core::FrameworkBase::Neighbor %s,  on Waveform %d, has died\n",wfAddrStr.c_str(), _param.wfid );
			LinkId lid = addressMap.LookUpLinkId(wfAddrStr); 
			
			//LinkId link(lid.nodeId, _param.wfid);
			nbrTable.RemoveNeighbor(lid);
			break;
		}
		case NBR_UPDATE:
		{
			Debug_Printf(DBG_CORE_ESTIMATION, "Core::FrameworkBase::Neighbor %lu,  on Waveform %d, was updated, with quality %f\n",_param.linkAddress, _param.wfid ,_param.metrics.quality);
			string wfAddrStr = NbrUtils::ConvertU64ToEthAddress(_param.linkAddress);
			nbrTable.UpdateNeighbor(addressMap.LookUpLinkId(wfAddrStr).nodeId, _param.wfid, _param.metrics);
			break;
		}
	}

    //Send the signal to the pattern tables to update themselves.
    //The manager will figure out which patters to signal
    
	string wfAddrStr = NbrUtils::ConvertU64ToEthAddress(_param.linkAddress);
	NodeId_t id = addressMap.LookUpLinkId(wfAddrStr).nodeId;
    nbrManager.UpdatePatternTables(NeighborUpdateParam(_param, id));

    //now figureout if any pattern tables need to be notified.
    /*for(PatternHash_t::iterator it = patternClientsMap.begin(); it!=patternClientsMap.end(); ++it) {
    if(it->second.nbrDelegates){
      //it->second.nbrDelegates->newNeighbor->operator()(_param.node);
    }
  }*/
}

WF_Attributes FrameworkBase::GetWaveformAttributes(WaveformId_t wfId)
{
	return wfAttributeMap->operator[](wfId);
}


}//End of namespace
