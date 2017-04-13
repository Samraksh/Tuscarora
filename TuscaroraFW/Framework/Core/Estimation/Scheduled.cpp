////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include "Scheduled.h"
#include "Framework/PWI/FrameworkBase.h"

extern NodeId_t MY_NODE_ID;

// extern bool DBG_CORE; // set in Lib/Misc/Debug.cpp
// extern bool DBG_CORE_ESTIMATION; // set in Lib/Misc/Debug.cpp
extern uint16_t NETWORK_SIZE;

namespace Core {
namespace Estimation {

//#define SC(x) static_cast <PatternInterface::FrameworkBase *>(x)
#define SCWF(x) static_cast <Waveform::WF_Message_n64_t *>(x)

	Scheduled::Scheduled(EstimatorCallback_I< uint64_t >& callback): EstBase(callback)
	{
		scheduleNextClean = new Delegate<void, EventInfoU64& >(this, &Scheduled::ScheduleNextClean);
	}

	
	Scheduled::Scheduled(EstimatorCallback_I<uint64_t> &callback, WaveformId_t _wid)
	: EstBase(callback,_wid)
	{
		scheduleNextClean = new Delegate<void, EventInfoU64& >(this, &Scheduled::ScheduleNextClean);
		/*fi = static_cast <PWI::FrameworkBase *> (parent);
		id=(uint32_t) MY_NODE_ID;
		wid=_wid;
		sendHb = new Delegate<void, EventInfoU64& >(this, &Scheduled::SendHB);
		state_ = new TimerDelegate(this, &Scheduled::CleanUp);
		leDel = new NeighborDelegate(this, &Scheduled::NodeExpired);
		*/
	}

	Scheduled::Scheduled(EstimatorCallback_I<uint64_t> &callback, WaveformId_t _wid, NeighborTable *_coreNbrTable)
	: EstBase(callback,_wid, _coreNbrTable)
	{
		scheduleNextClean = new Delegate<void, EventInfoU64& >(this, &Scheduled::ScheduleNextClean);
		
		/*fi = static_cast <PWI::FrameworkBase *> (parent);
		coreNbrTable=_coreNbrTable;
		id=(uint32_t) MY_NODE_ID;
		wid=_wid;
		sendHb = new Delegate<void, EventInfoU64& >(this, &Scheduled::SendHB);
		state_ = new TimerDelegate(this, &Scheduled::CleanUp);
		leDel = new NeighborDelegate(this, &Scheduled::NodeExpired);*/
		
		Debug_Printf(DBG_CORE_ESTIMATION, "Scheduled:: wid set to %d\n", _wid);
	}
	
	void Scheduled::CreateUpdateNbrSchedule(uint64_t wfAddress, NodeId_t nodeid, WaveformId_t wid)
	{
		EstimationInfo<uint64_t>* info= estimationTable->GetEstimationInfo(wfAddress, wid);
		if(info){}
		else 
		{
			UniformRandomInt *rand = new UniformRandomInt();
			NodeSchedule *sch = new NodeSchedule();
			sch->nodeid = nodeid;
			sch->linkEstSchd = new RandomSchedule<UniformRandomInt,UniformRNGState, uint64_t,uint64_t>(*rand,0);
			sch->linkEstSchd->SetNode(nodeid);
			sch->linkEstSchd->RegisterDelegate(*scheduleNextClean);
			nodeSchedule[nodeid] = sch;
			//printf("Scheduled::SetParam: Initializing link schedule for nbr %lu\n", i);
		}
	}
	
	void Scheduled::SetParam(uint32_t beaconPeriod, uint32_t inactivePeriod)
	{
		EstBase::SetParam(beaconPeriod,inactivePeriod);
		numNodes = NETWORK_SIZE;
		/*leSeqno=0;
		period = beaconPeriod;
		
		
		MemMap<Header> map("configuration.bin");
		map.openForRead();
		Header* header = map.getNext();
		if(header->discoveryType == LONG_LINK_DISCOVERY) numNodes--;
		
		gettimeofday(&cur,NULL);
		cur.tv_sec--;
		
		//Create scheduler for link estimation beaconing
		beaconSchedule = new RandomSchedule<UniformRandomInt, UniformRNGState,uint64_t,  uint64_t>(0);
		beaconSchedule->SetNode(MY_NODE_ID);
		UniformRNGState state;
		state.cmrgState.stream = MY_NODE_ID * MAX_WAVEFORMS + wid;
		state.cmrgState.run = 1;
		state.mean = beaconPeriod;
		state.range = beaconPeriod/2;
		
		beaconSchedule->Set(state, (uint64_t)(cur.tv_sec) * 1000000);
		beaconSchedule->RegisterDelegate(*sendHb);
		*/
		//Create uninitilized schedulers for each node in the network
		for(uint64_t i = 0; i < numNodes; i++) {
			if(i != MY_NODE_ID) {
				UniformRandomInt *rand = new UniformRandomInt();
				NodeSchedule *sch = new NodeSchedule();
				sch->nodeid = i;
				sch->linkEstSchd = new RandomSchedule<UniformRandomInt,UniformRNGState, uint64_t,uint64_t>(*rand,0);
				sch->linkEstSchd->SetNode(i);
				sch->linkEstSchd->RegisterDelegate(*scheduleNextClean);
				nodeSchedule[i] = sch;
				Debug_Printf(DBG_CORE_ESTIMATION,"Scheduled::SetParam: Initializing link schedule for nbr %lu\n", i);
			}
		}
		
		//Create the timers for the cleanup calls. The delay between schedule expiration and cleanup is to allow time for actual transmission and processing
		idx = 0;
		for(int x = 0; x < 10; x++) {
			clean[x] = new Timer(2000, ONE_SHOT, *state_);
		}
		Debug_Printf(DBG_CORE_ESTIMATION, "Scheduled::SetParam: Initializing link estimation of WF %d\n", wid);
	}
	
	void Scheduled::ScheduleNextClean(EventInfoU64& event) {
		clean[idx]->Start();
		idx++;
		idx = idx % 10;
	}

	Scheduled::~Scheduled() {
		
		//Delete schedulers for each node in the network
		for(uint64_t i = 0; i < numNodes; i++) {
			if(i != MY_NODE_ID) {
				NodeSchedule *sch = nodeSchedule[i];
				delete(sch->linkEstSchd);
				delete(sch);
			}
		}
		
		delete(sendHb);
		delete(state_);
		delete(leDel);
		beaconSchedule->Suspend();
		delete(beaconSchedule);
	}
	
	LinkMetrics* Scheduled::OnPacketReceive(Waveform::WF_MessageBase *rcvMsg) {
		NodeId_t nbr = SCWF(rcvMsg)->GetSrcNodeID();
		uint64_t nbrWfAddress = SCWF(rcvMsg)->GetSource();
		WaveformId_t wfid = SCWF(rcvMsg)->GetWaveform();
		//EstimationInfo<uint64_t>* info= estimationTable->GetEstimationInfo(nbrWfAddress, wfid);
		if(nodeSchedule[nbr]){
			if(nodeSchedule[nbr]->linkEstSchd->IsSuspended()) {
				RNStreamID cmrgState;
				cmrgState.stream = (uint64_t)(nbr * MAX_WAVEFORMS) + wid;
				cmrgState.run = 1;
				UniformRNGState rngState(cmrgState);
				UniformIntDistParameter dist; dist.min = period - period/2; dist.max = period + period/2 ;
				rngState.SetDistributionParameters(dist);
				nodeSchedule[nbr]->linkEstSchd->SetState(rngState, (uint64_t)(cur.tv_sec) * 1000000);
			}
		}else {
			CreateUpdateNbrSchedule(nbrWfAddress, nbr, wfid);
		}
		return EstBase::ProcessMessage(rcvMsg,nodeSchedule[nbr]->linkEstSchd->TimeOfNextEvent() + 1990);
		
		//Debug_Error("Scheduled LinkEstimation:: OnPacketReceive:: Couldnt find schedule entry for link nbr %lu on waveform %d\n", nbrWfAddress, wfid);
		//return NULL;
	}
	
	void Scheduled::PotentialNeighborUpdate(PotentialNeighbor& pnbrArray, PNbrUpdateTypeE type)
	{
		if(type == PNBR_UPDATE) {
			Debug_Printf(DBG_CORE_ESTIMATION, "Updating potential neighbor %d\n", pnbrArray.linkId.nodeId);
			//Start schedulers for new potential neighbors
			RNStreamID cmrgState;
			cmrgState.stream = (uint64_t)(pnbrArray.linkId.nodeId * MAX_WAVEFORMS) + wid;
			cmrgState.run = 1;
			UniformRNGState rngState(cmrgState);
			UniformIntDistParameter dist; dist.min = period - period/2; dist.max = period + period/2 ;
			rngState.SetDistributionParameters(dist);
			nodeSchedule[pnbrArray.linkId.nodeId]->linkEstSchd->SetState(rngState, (uint64_t)(cur.tv_sec) * 1000000);
		} else if(type == PNBR_INVALID) {
			//Suspend scheduler for non-potential neighbors
			Debug_Printf(DBG_CORE_ESTIMATION, "Suspending potential neighbor %d\n", pnbrArray.linkId.nodeId);
			nodeSchedule[pnbrArray.linkId.nodeId]->linkEstSchd->Suspend();
		}
	}
	
	
	/*	
	LinkMetrics* Scheduled::OnPacketReceive(Waveform::WF_MessageBase *rcvMsg) {
		uint64_t nbrWfAddress = SCWF(rcvMsg)->GetSource();
		NodeId_t nbr = SCWF(rcvMsg)->GetSrcNodeID();
		WaveformId_t wfid = SCWF(rcvMsg)->GetWaveform();
		struct timeval curtime;
		gettimeofday(&curtime,NULL);
		
		
		//Add new link or update expiry time of existing link
		//int32_t newNbr= coreNbrTable->AddNeighbor(nbr, Neighbor, nodeSchedule[nbr]->linkEstSchd->TimeOfNextEvent() + 1990);
		if(nodeSchedule[nbr]->linkEstSchd->IsSuspended()) {
			UniformRNGState rngState;
			rngState.cmrgState.stream = (uint64_t)(nbr * MAX_WAVEFORMS) + wid;
			rngState.cmrgState.run = 1;
			rngState.mean = period;
			rngState.range = period/2;
			nodeSchedule[nbr]->linkEstSchd->Set(rngState, (uint64_t)(cur.tv_sec) * 1000000);
		}
		
		if(estimationTable.GetEstimationInfo(nbr, wfid) == 0) {
			LogQualityEvent(nbr, curtime);
			EstimationInfo* info = estimationTable.AddNeighbor(nbr, wfid);
			Link *link= new Link();
			link->linkId.nodeId = info->nodeId;
			link->linkId.waveformId= wfid;
			link->metrics.quality=GetQuality(nbr);
			
			info->link = link;
			info->nodeSchd = nodeSchedule[nbr];
			info->expiryTime = info->nodeSchd->linkEstSchd->TimeOfNextEvent() + 1990;
			//assert(info->expiryTime != 1990);
			Debug_Printf(DBG_CORE_ESTIMATION, "%d expires at %lu on %d\n", nbr, info->expiryTime, MY_NODE_ID);
			info->metric = &link->metrics;
			
			estimationTable.SetLink(nbr, wfid, link);
			
			//Notify pattern interface about new link
			NeighborUpdateParam _param;
			_param.changeType = NBR_NEW;
			_param.link = *link;
			fworkNbrDel->operator ()(_param);
		}else {
			EstimationInfo* info = estimationTable.GetEstimationInfo(nbr, wfid);
			if(info->link->metrics.quality != GetQuality(nbr)) {
				info->link->metrics.quality = GetQuality(nbr);
				estimationTable.UpdateExpiration(nbr, wfid, estimationTable.GetEstimationInfo(nbr, wfid)->nodeSchd->linkEstSchd->TimeOfNextEvent() + 1990);
				NeighborUpdateParam _param;
				_param.changeType = NBR_UPDATE;
				_param.nodeId =nbr;
				_param.link = *(estimationTable.GetEstimationInfo(nbr, wfid)->link);
				fworkNbrDel->operator ()(_param);
			}
		}
		
	}
	

	

	bool Scheduled::RegisterDelegate (NeighborDelegate& del){
		fworkNbrDel = &del;
		Debug_Printf(DBG_CORE_ESTIMATION, "Scheduled:: Framework registerd its delegate, stored ptr %p, my ptr is %p, period is %d \n", fworkNbrDel, this, period);
		return true;
	}*/
	
	/*void Scheduled::NodeExpired(NeighborUpdateParam _param) {
		struct timeval curtime;
		gettimeofday(&curtime,NULL);
		fworkNbrDel->operator ()(_param);
		Debug_Printf( DBG_CORE,"ScheduledEstimation:: Removed dead neighbor %d from table expires\n", _param.link.linkId.nodeId);
		LogQualityEvent(_param.link.linkId.nodeId, curtime);
	}*/
	
	/*void Scheduled::CleanUp(uint32_t event) {
		estimationTable.CheckExpiration(*leDel);
	}*/
	
	/*void Scheduled::SendHB(EventInfoU64& event) {
		uint32_t *packetId = new uint32_t;
		*packetId = MY_NODE_ID;
		FMessage_t* msg = new FMessage_t();
		msg->SetPayload((uint8_t*)packetId);
		msg->SetSource(MY_NODE_ID);
		msg->SetType(LE_Type);
		msg->SetPayloadSize(sizeof &packetId);
		
		fi->BroadcastData(0, *msg, wid, 0);//Masahiro adding nonce value
		
		logger.LogEvent(PAL::LINK_SENT, leSeqno);
		leSeqno++;
		Debug_Printf( DBG_CORE, "Sending hb\n");
	}
	
	double Scheduled::GetQuality(NodeId_t link) {
		struct timeval curtime;
		gettimeofday(&curtime,NULL);
		double c=1.5;
		double max=5;
		int w=30;
		int maxValue=pow(c, max)*w + 1;
		double value=1;
		bool nbr=events[link].size() % 2 == 1;
		uint64_t cur = (uint64_t)curtime.tv_sec * 1000000 + curtime.tv_usec;
		uint64_t last = (uint64_t)(curtime.tv_sec - w) * 1000000 + curtime.tv_usec;
		for(int x = events[link].size(); x > 0; x--) {
			curtime=events[link][x - 1];
			uint64_t pcur = (uint64_t)curtime.tv_sec * 1000000 + curtime.tv_usec;
			if(nbr) {
				uint64_t dcur = cur - pcur;
				if(pcur < last) {
					dcur = last - pcur;
				}
				if(dcur > 1000000*max) {
					value+=pow(c, max) * ((1.0f*dcur/1000000)-max);
					dcur = 1000000*max;
				}
				int i = 0;
				for(; dcur > 1000000; i++) {
					value += pow(c, i);
					dcur-=1000000;
				}
				
				if(dcur != 0) {
					value += pow(c, i) * (1.0f*dcur/1000000);
				}
			}
			if(pcur < last) break;
			cur = pcur;
			nbr=!nbr;
		}
		
		if(value>maxValue) return 1;
		return value/maxValue;
	}
	
	void Scheduled::LogQualityEvent(NodeId_t link, timeval time) {
		events[link].push_back(time);
	}
	
	*/
	
  
}//End of namespace
}
