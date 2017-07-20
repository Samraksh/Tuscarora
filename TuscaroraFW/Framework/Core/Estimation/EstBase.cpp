////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include "EstBase.h"

#include "Framework/PWI/FrameworkBase.h"
#include "Framework/External/Location.h"

#ifndef PLATFORM_EMOTE
#include "../../../Platform/linux/Framework/EstimationLogging.h"
#endif

extern NodeId_t MY_NODE_ID;

// extern bool DBG_CORE; // set in Lib/Misc/Debug.cpp
// extern bool DBG_CORE_ESTIMATION; // set in Lib/Misc/Debug.cpp

namespace Core {
namespace Estimation {

	
/*Notes: Packet Reception Rate is an approximate calculation, with a exponential weighted average with the weight being propotional to 1/windowPeriod . Consequently the PRR curve over time looks like a CDF of an exponential function.
An accurate prr is possible, but this would require us to keep state proportional to the size of the windowperiod for each neighbor. But this would be very expensive in memeory.

This would take a long time (technically never) to decrease the prr to zeor of a neighbor which goes away. Hence we have a timeout period to remove the neighbor if no activity is detected from it.

Hence the PRR is biased towards quick estimation of existing neighbor. 
*/

#define SCWF(x) static_cast <Waveform::WF_Message_n64_t *>(x)

//Global Static location

LinkEstimationTable_n64_t estTable;

//#ifndef PLATFORM_EMOTE
//EstimationLogging estBaseLog;
//#endif

EstBase::EstBase(EstimatorCallback_I< uint64_t >& callback)
{
	callbackI = &callback;
	id=(uint32_t) MY_NODE_ID;
	//leDel = new WF_LinkChangeDelegate_n64_t(this, &EstBase::NodeExpired);
	leDel.AddCallback(this, &EstBase::NodeExpired);
	estimationTable = &estTable;
#ifndef PLATFORM_EMOTE
	//logger = &estBaseLog;
	logger = new EstimationLogging();
#endif
}


EstBase::EstBase(EstimatorCallback_I<uint64_t> &callback, WaveformId_t _wid)
:EstBase(callback)
{
	wid = _wid;
}

EstBase::EstBase(EstimatorCallback_I<uint64_t> &callback, WaveformId_t _wid, NeighborTable *_coreNbrTable)
:EstBase(callback,_wid)
{
	coreNbrTable=_coreNbrTable;
}

bool EstBase::RegisterDelegate (WF_LinkChangeDelegate_n64_t &del){

	fworkNbrDel = &del;
	Debug_Printf(DBG_CORE_ESTIMATION, "EstBase:: Framework registerd its delegate, stored ptr %p, my ptr is %p, period is %d \n", fworkNbrDel, this, period);
	return true;
}

void EstBase::SetParam(uint32_t beaconPeriod, uint32_t inactivePeriod){
	Debug_Printf(DBG_CORE_ESTIMATION, "EstBase::SetParam: Initializing link estimation of WF %d, with period %d\n", wid, beaconPeriod );
	leSeqno=0;
	dead = inactivePeriod;
	period = beaconPeriod;

	randInt = new UniformRandomInt(beaconPeriod - (beaconPeriod/2), beaconPeriod + (beaconPeriod/2));
	
	beaconSchedule = new RandomSchedule<UniformRandomInt, UniformRNGState,uint64_t,  uint64_t>(*randInt, 0);
	beaconSchedule->SetNode(MY_NODE_ID);
	/*UniformRNGState state;
	RNStreamID cmrg_state;
	cmrg_state.stream = MY_NODE_ID * MAX_WAVEFORMS + wid;
	cmrg_state.run = 1;
	state.SetRNStreamID(cmrg_state);
	UniformIntDistParameter dist;
	dist.min = beaconPeriod - beaconPeriod/2;
	dist.max = beaconPeriod + beaconPeriod/2;
	state.SetDistributionParameters(dist);
*/
	sendHb = new Delegate<void, EventInfoU64& >(this, &EstBase::SendHB);
	
	beaconSchedule->RegisterDelegate(*sendHb);
	//This will only be valid if the parameters are set within 1 second of beginning the simulation. When doing a real world installation, 
	//there will have to be a time synchronization that also defines the start time to use here.
	beaconSchedule->Start(SysTime::GetEpochTimeInSec() * 1000000);
	
	state_ = new TimerDelegate(this, &EstBase::CleanUp);
	cleaner = new Timer(beaconPeriod / 10, PERIODIC, *state_);
	cleaner->Start();
	
	wfAttributes=callbackI->GetWaveformAttributes(wid);
	Debug_Printf(DBG_CORE_ESTIMATION, "EstBase::SetParam: Initializing link estimation Done.\n");
}

void EstBase::NodeExpired(WF_LinkEstimationParam_n64_t _param) {
	fworkNbrDel->operator ()(_param);
	LogQualityEvent(_param.linkAddress, SysTime::GetEpochTimeInMicroSec());
	//receiveCountOverWindow.Erase(_param.linkAddress);
	packetReceptionRateMap.Erase(_param.linkAddress);
	Debug_Printf( DBG_CORE_ESTIMATION,"EstBase:: Removed dead neighbor %lu from table expires\n", _param.linkAddress);
}

void EstBase::CleanUp(uint32_t event) {
	//Debug_Printf(DBG_CORE_ESTIMATION, "EstBase::CleanUp: \n"); fflush(stdout);
	estimationTable->CheckExpiration(leDel);
	
}

void EstBase::SendHB(EventInfoU64& event) {
	TimeStamp_t cur = SysTime::GetEpochTimeInMicroSec();

	double x = x();
	double y = y();

	//uint32_t *packetId = new uint32_t;
	//*packetId = id;
	FMessage_t* msg = new FMessage_t(sizeof(EstMsg));
	EstMsg *estMsg = (EstMsg*) msg->GetPayload();
	estMsg->packetSeqNo = leSeqno;
	msg->SetSource(MY_NODE_ID);
	msg->SetType(Types::ESTIMATION_MSG);


	Debug_Printf(DBG_CORE_ESTIMATION, "EstBase::SendHB: Sending link estimation packet %d at %lu for %d on %d %f %f\n",leSeqno, cur, MY_NODE_ID, wid, x, y);
	callbackI->SendEstimationMessage(wid, msg);
	//sendDel->operator()(param);
	//SC(fi)->SendToWF(wid, true, Types::LE_Type,0, 0, *msg);
	//fi->BroadcastData(0, *msg, wid);
#ifndef PLATFORM_EMOTE
	logger->LogEvent(Estimation::LINK_SENT, leSeqno);
#endif
	leSeqno++;
	DecrementPRR();
	Debug_Printf(DBG_CORE_ESTIMATION,"EstBase::SendHB: Checking framework delegate value: %p, my ptr is %p, period is %d \n", fworkNbrDel, this, period);fflush(stdout);
}

double EstBase::GetQuality(LinkAddress_t linkAddress)
{
	//return PacketReceptionRate(linkAddress);
	return StabilityQuality(linkAddress);
}

//Simple Packet Reception Rate calculation
double EstBase::PacketReceptionRate(LinkAddress_t linkAddress)
{
	return (double)packetReceptionRateMap[linkAddress];
}

//Uses a complex stability based link quality invented by Vinod Kulathumani
double EstBase::StabilityQuality(LinkAddress_t _linkAddress) {
	double c=1.5;
	double max=5;
	uint32_t w=windowPeriod; //windows size in seconds
	double maxValue=pow(c, max)*w + 1;
	double value=1;
	//uint64_t curtime;
	
	uint32_t totalUnits=1000000;
	uint64_t timeUnitToSec = 1000000;
	
	
	bool nbr= ((events[_linkAddress].Size() % 2) == 1);
	//uint64_t cur = (uint64_t)curtime.tv_sec * 1000000 + curtime.tv_usec;
	//uint64_t windowBegintime = (uint64_t)(curtime.tv_sec - w) * 1000000 + curtime.tv_usec;
	uint64_t cur = SysTime::GetEpochTimeInMicroSec();
	uint64_t windowBegintime = cur - (w * timeUnitToSec);

	for(int x = events[_linkAddress].Size(); x > 0; x--) {
		uint64_t eventTime=events[_linkAddress][x - 1];
		if(nbr) {
			uint64_t dcur = cur - eventTime;
			//printf("EventTime: %lu, WindowBeginTime: %lu, Current Time: %lu:::\n", eventTime, windowBegintime, cur);
			if(eventTime < windowBegintime) {
			//	printf("Eventtime is beyond the window\n");
				//dcur = windowBegintime - eventTime; //why?? this seems to be causing the rollover error in quality
				//This is modified to below line by Mukundan to fix roll over error
				dcur = w * timeUnitToSec;
			}
			if(dcur > totalUnits*max) {
				value+=pow(c, max) * ((1.0f*dcur/totalUnits)-max);
				dcur = totalUnits*max;
			}
			int i = 0;
			for(; dcur > totalUnits; i++) {
				value += pow(c, i);
				dcur-=totalUnits;
			}

			if(dcur != 0) {
				value += pow(c, i) * (1.0f*dcur/totalUnits);
			}
		}
		if(eventTime < windowBegintime) break;
		cur = eventTime;
		nbr=!nbr;
	}

	if(value >= maxValue) return 1.0;
	
	return value/maxValue;
}

//void EstBase::LogQualityEvent(LinkAddress_t linkAddress, timeval time) {
void EstBase::LogQualityEvent(LinkAddress_t linkAddress, TimeStamp_t time) {
	events[linkAddress].InsertBack(time);
	Debug_Printf( DBG_CORE_ESTIMATION,"EstBase::LogQualityEvent: Size of event log for node %lu is %d\n", linkAddress, (int)events[linkAddress].Size());
}



LinkMetrics* EstBase::OnPacketReceive(Waveform::WF_MessageBase *rcvMsg){
	TimeStamp_t cur = SysTime::GetEpochTimeInMicroSec();
	return ProcessMessage(rcvMsg, cur + period * dead * 1.5);
}


void EstBase::IncrementOrSetPRR(LinkAddress_t _linkAddress, float value)
{
	float weight = 1/windowPeriod;
	BSTMapT< LinkAddress_t, float >::Iterator it = packetReceptionRateMap.Find(_linkAddress);
	if (it != packetReceptionRateMap.End()){
		
		if(value >= 0){
			it->Second()=value;
			printf("EstBase::IncrementOrSetPRR: Setting PRR for link %lu: to %2.8f, %2.8f \n", _linkAddress, it->Second(), value);
		}else {
			float prev = it->Second();
			//it->Second() = prev*(1-weight)+weight;
			it->Second() = prev+weight;
			if(it->Second()> 1.0) it->Second() =1.0;
			//printf("Increment PRR for link %lu: from  %f to %f, weigh is %f \n", _linkAddress, prev, it->Second(), weight);
		}
	}
	else {
		if(value >=0) {
			packetReceptionRateMap[_linkAddress] = value;
		}else {
			packetReceptionRateMap[_linkAddress] = 0;
		}
		printf("Setting PRR for link %lu: to %f \n", _linkAddress, packetReceptionRateMap[_linkAddress]);
	}
}


void EstBase::DecrementPRR()
{
	float weight = 1/windowPeriod;
	BSTMapT< LinkAddress_t, float >::Iterator it = packetReceptionRateMap.Begin();
	for (;it != packetReceptionRateMap.End(); ++it){
		it->Second() = it->Second()*(1-weight);
		if(it->Second()<0) it->Second() =0;
	}
}

/*
void EstBase::DecrementReceptionCount(){
BSTMapT< LinkAddress_t, uint16_t >::Iterator it = receiveCountOverWindow.Begin();
	for (;it != receiveCountOverWindow.End(); ++it){
		if(it->Second()<=1) it->Second() =0;
		else it->Second()--;
	}
}
*/

LinkMetrics* EstBase::ProcessMessage(Waveform::WF_MessageBase *rcvMsg, TimeStamp_t _expiryTime){
	LinkAddress_t nbrWfAddress = SCWF(rcvMsg)->GetSource();
	NodeId_t nbrId = SCWF(rcvMsg)->GetSrcNodeID();
	WaveformId_t wfid = SCWF(rcvMsg)->GetWaveform();
	TimeStamp_t curtime = SysTime::GetEpochTimeInMicroSec();
	//printf("EstBase::OnPacketReceive: Received a message with ptr %p, from node %lu, on waveform %d, with expiry time %lu \n", rcvMsg, nbrWfAddress, wfid, rcvMsg->GetExpiryTime().GetMicroseconds());fflush(stdout);
	Debug_Printf(DBG_CORE_ESTIMATION,"EstBase::OnPacketReceive: Received a message with ptr %p, from node %lu, on waveform %d \n", rcvMsg, nbrWfAddress, wfid);fflush(stdout);

	//printf("EstBase::OnPacketReceive: Received a message with ptr %p, from node %lu, on waveform %d of size: %d\n", rcvMsg, nbrWfAddress, wfid, rcvMsg->GetPayloadSize());fflush(stdout);

	//if(coreNbrTable->GetNeighborLink(nbrWfAddress) == 0) {
	if(!estimationTable->GetEstimationInfo(nbrWfAddress, wfid)) {
		//receiveCountOverWindow[nbrWfAddress]=1;
		IncrementOrSetPRR(nbrWfAddress, float(1.0/windowPeriod));
		
		LogQualityEvent(nbrWfAddress, curtime);
		EstimationInfo<LinkAddress_t>* info = estimationTable->AddNeighbor(nbrWfAddress, wfid);
		Link* link = new Link();
		link->linkId.nodeId = info->linkAddress;
		link->linkId.waveformId= wfid;
		link->metrics.quality=GetQuality(nbrWfAddress);
		link->metrics.cost = wfAttributes.cost;
		link->metrics.dataRate = wfAttributes.channelRate;
		link->metrics.energy = wfAttributes.energy;
		//link->metrics.

		//LinkMetrics* rtnMetrics = new LinkMetrics(); //Get the current metrics of the sending node and update it.
		info->link = link;
		info->expiryTime = _expiryTime;
		info->metric = &link->metrics;
		//info->metric->quality = 1;

		
		
		estimationTable->SetLink(nbrWfAddress, wfid, link);

		//Notify pattern interface about new link
		//NeighborUpdateParam _param;
		WF_LinkEstimationParam_n64_t _param;
		_param.nodeId = nbrId;
		_param.changeType = NBR_NEW;
		_param.linkAddress =nbrWfAddress;
		_param.metrics = info->link->metrics;
		_param.type = info->link->type;
		_param.wfid = info->link->linkId.waveformId;
		//_param.link = *link;
		fworkNbrDel->operator ()(_param);
	}
	else {
		EstimationInfo<uint64_t>* info = estimationTable->GetEstimationInfo(nbrWfAddress, wfid);
		//receiveCountOverWindow[nbrWfAddress]=receiveCountOverWindow[nbrWfAddress]+1;
		IncrementOrSetPRR(nbrWfAddress);
			
		if(info->link->metrics.quality != GetQuality(nbrWfAddress)) {
			info->link->metrics.quality = GetQuality(nbrWfAddress);
			estimationTable->UpdateExpiration(nbrWfAddress, wfid,_expiryTime);
			WF_LinkEstimationParam_n64_t _param;
			_param.nodeId = nbrId;
			_param.changeType = NBR_UPDATE;
			_param.linkAddress =nbrWfAddress;
			_param.metrics = info->link->metrics;
			_param.type = info->link->type;
			_param.wfid = info->link->linkId.waveformId;	
			fworkNbrDel->operator ()(_param);
		}
	}
	LinkMetrics *ret= estimationTable->GetEstimationInfo(nbrWfAddress, wfid)->metric;
	delete(rcvMsg);
	return ret;
}


EstBase::~EstBase() {

	//delete(estimationTable);
	delete(sendHb);
	delete(state_);
	//delete(leDel);
	beaconSchedule->Suspend();
	delete(beaconSchedule);
	cleaner->Suspend();
	delete(cleaner);
}

//TODO:: Implement this. Framework will call this method with a list of potential neighbors, use it to figure out who
//is currently your neighbor
void EstBase::PotentialNeighborUpdate(PotentialNeighbor& pnbr, PNbrUpdateTypeE type)
{

}


}//End of namespace
}
