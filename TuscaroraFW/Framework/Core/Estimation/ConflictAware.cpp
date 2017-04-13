////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include "ConflictAware.h"
#include "Framework/PWI/FrameworkBase.h"

//extern NodeId_t MY_NODE_ID;
extern uint16_t NETWORK_SIZE;
// extern bool DBG_CORE; // set in Lib/Misc/Debug.cpp

namespace Core {
	namespace Estimation {
		
		//#define SC(x) static_cast <PatternInterface::FrameworkBase *>(x)
		//#define SCWF(x) static_cast <Waveform::WF_Message_n64_t *>(x)

		/*ConflictAware::ConflictAware(EstimatorCallback_I< uint64_t >& callback): Scheduled(callback)
		{
			
		}
		
		ConflictAware::ConflictAware(EstimatorCallback_I< uint64_t >& callback, WaveformId_t _wid)
		: Scheduled(callback, _wid)
		{
			
		}
		
		ConflictAware::ConflictAware(EstimatorCallback_I< uint64_t >& callback, WaveformId_t _wid, NeighborTable* _coreNbrTable)
		: Scheduled(callback, _wid, _coreNbrTable)
		{
			
		}
		
		ConflictAware::~ConflictAware() {
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
		*/
		void ConflictAware::CleanUp(uint32_t event) {
			struct timeval curtime;
			gettimeofday(&curtime,NULL);
			uint64_t curt = (uint64_t)curtime.tv_sec * 1000000 + curtime.tv_usec;
			logger.LogEvent(PAL::LINK_SKIPPED, 0);
			
			//Go through each node's schedule, and test it for potential conflicts. If there is a potential conflict,
			//change the node's expiry time to be the next scheduled event
			for(uint64_t i = 0; i < numNodes; i++) {
				EstimationInfo<uint64_t> *info = estimationTable->GetEstimationInfo(i, wid);
				if(info != 0 && info->expiryTime <= curt) {
					RandomSchedule<UniformRandomInt, UniformRNGState, uint64_t, uint64_t>* ischd = info->nodeSchd->linkEstSchd;
					for(uint64_t j = 0; j < numNodes; j++) {
						if(i == j || j == MY_NODE_ID) continue;
						if(((nodeSchedule[i]->linkEstSchd->TimeOfLastEvent() > nodeSchedule[j]->linkEstSchd->TimeOfLastEvent()) && 
							(nodeSchedule[i]->linkEstSchd->TimeOfLastEvent() - nodeSchedule[j]->linkEstSchd->TimeOfLastEvent() < 1000)) || 
							((nodeSchedule[j]->linkEstSchd->TimeOfLastEvent() > nodeSchedule[i]->linkEstSchd->TimeOfLastEvent()) && 
							(nodeSchedule[j]->linkEstSchd->TimeOfLastEvent() - nodeSchedule[i]->linkEstSchd->TimeOfLastEvent() < 1000))) {
							info->expiryTime = ischd->TimeOfNextEvent();
							Debug_Printf( DBG_CORE,"ConvlidAvoidEstimation:: Conflict avoided between %lu and %lu\n", i, j);
							logger.LogEvent(PAL::LINK_SKIPPED, i);
							break;
						}
					}
				}
			}
			
			//Then go through and remove any still expired nodes
			estimationTable->CheckExpiration(*leDel);
		}
		
		/*
		ConflictAware::ConflictAware(Framework_I *parent, WaveformId_t _wid){
		    fi = static_cast <PWI::FrameworkBase *> (parent);
		    wid=_wid;
		    id=(uint32_t) MY_NODE_ID;
		    sendHb = new Delegate<void, EventInfoU64& >(this, &ConflictAware::SendHB);
		    scheduleNextClean = new Delegate<void, EventInfoU64& >(this, &ConflictAware::ScheduleNextClean);
		    state_ = new TimerDelegate(this, &ConflictAware::CleanUp);
		    leDel = new NeighborDelegate(this, &ConflictAware::NodeExpired);
		}
	
		ConflictAware::ConflictAware(Framework_I *parent, WaveformId_t _wid, NeighborTable *_coreNbrTable){
			fi = static_cast <PWI::FrameworkBase *> (parent);
			coreNbrTable=_coreNbrTable;
			wid = _wid;
			id=(uint32_t) MY_NODE_ID;
			sendHb = new Delegate<void, EventInfoU64& >(this, &ConflictAware::SendHB);
			scheduleNextClean = new Delegate<void, EventInfoU64& >(this, &ConflictAware::ScheduleNextClean);
			state_ = new TimerDelegate(this, &ConflictAware::CleanUp);
			leDel = new NeighborDelegate(this, &ConflictAware::NodeExpired);
		}
		
		bool ConflictAware::RegisterDelegate (NeighborDelegate& del){
			fworkNbrDel = &del;
			return true;
		}
		
		void ConflictAware::SetParam(uint32_t beaconPeriod, uint32_t inactivePeriod){
			leSeqno=0;
			period = beaconPeriod;
			
			numNodes = NETWORK_SIZE;
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
			tate.cmrgState.stream = MY_NODE_ID * MAX_WAVEFORMS + wid;
			state.cmrgState.run = 1;
			state.mean = beaconPeriod;
			state.range = beaconPeriod/2;
			
			beaconSchedule->Set(state, (uint64_t)(cur.tv_sec) * 1000000);
			beaconSchedule->RegisterDelegate(*sendHb);
			
			//Create uninitilized schedulers for each node in the network
			for(uint64_t i = 0; i < numNodes; i++) {
				if(i != MY_NODE_ID) {
				    NodeSchedule *sch = new NodeSchedule();
				    sch->nodeid = i;
				    sch->linkEstSchd = new RandomSchedule<UniformRandomInt,UniformRNGState, uint64_t,uint64_t>(0);
				    sch->linkEstSchd->SetNode(i);
				    sch->linkEstSchd->RegisterDelegate(*scheduleNextClean);
				    nodeSchedule[i] = sch;
				}
			}
		
			//Create the timers for the cleanup calls. The delay between schedule expiration and cleanup is to allow time for actual transmission and processing
			idx = 0;
			for(int x = 0; x < 10; x++) {
				clean[x] = new Timer(2000, ONE_SHOT, *state_);
			}
		}
		
		void ConflictAware::ScheduleNextClean(EventInfoU64& event) {
			clean[idx]->Start();
			idx++;
			idx = idx % 10;
		}
		
		void ConflictAware::NodeExpired(NeighborUpdateParam _param) {
			fworkNbrDel->operator ()(_param);
			Debug_Printf( DBG_CORE,"ConflictAwareEstimation:: Removed dead neighbor %d from table expires\n", _param.link.linkId.nodeId);
			coreNbrTable->RemoveNeighbor(_param.link.linkId);
		}
		
		void ConflictAware::SendHB(EventInfoU64& event) {
			uint32_t *packetId = new uint32_t;
			packetId = MY_NODE_ID;
			FMessage_t* msg = new FMessage_t();
			msg->SetPayload((uint8_t*)packetId);
			msg->SetSource(MY_NODE_ID);
			msg->SetType(LE_Type);
			msg->SetPayloadSize(sizeof &packetId);
			
			fi->BroadcastData(0, *msg, wid, 0); //Masahiro adding nonce value
			
			logger.LogEvent(PAL::LINK_SENT, leSeqno);
			leSeqno++;
			Debug_Printf( DBG_CORE, "Sending hb\n");
		}
		
		
		double ConflictAware::GetQuality(NodeId_t link) {
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
		
		void ConflictAware::LogQualityEvent(NodeId_t link, timeval time) {
			events[link].push_back(time);
		}
		
		
		LinkMetrics* ConflictAware::OnPacketReceive(Waveform::WF_MessageBase *rcvMsg) {
		 *  struct timeval curtime;
		 *  gettimeofday(&curtime,NULL);
		 *  uint64_t nbrWfAddress = SCWF(rcvMsg)->GetSource();
		 *  NodeId_t nbr = SCWF(rcvMsg)->GetSrcNodeID();
		 *  WaveformId_t wfid = SCWF(rcvMsg)->GetWaveform();
		 *  //Add new link or update expiry time of existing link
		 *  //int32_t newNbr= coreNbrTable->AddNeighbor(nbr, Neighbor, nodeSchedule[nbr]->linkEstSchd->TimeOfNextEvent() + 1990);
		 *  if(nodeSchedule[nbr]->linkEstSchd->IsSuspended()) {//TODO 0.7 Should this be handeled by the Discovery?
		 *    UniformRNGState rngState;
		 *    rngState.cmrgState.stream = (uint64_t)(nbr * MAX_WAVEFORMS) + wid;
		 *    rngState.cmrgState.run = 1;
		 *    rngState.mean = period;
		 *    rngState.range = period/2;
		 *    nodeSchedule[nbr]->linkEstSchd->Set(rngState, (uint64_t)(cur.tv_sec) * 1000000);
		 *    printf("Setting the linkEstSchd\n");
		}
		
			if(estimationTable.GetEstimationInfo(nbr, wfid) == 0) {
				LogQualityEvent(nbr, curtime);
				EstimationInfo* info = estimationTable.AddNeighbor(nbr, wfid);
				Link* link = new Link();
				link->linkId.nodeId = info->nodeId;
				link->linkId.waveformId= wid;
				link->metrics.quality=GetQuality(nbr);
				
				info->link = link;
				info->nodeSchd = nodeSchedule[nbr];
				info->expiryTime = info->nodeSchd->linkEstSchd->TimeOfNextEvent() + 1990;
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
			return estimationTable.GetEstimationInfo(nbr, wfid)->metric;
		}

		void ConflictAware::PotentialNeighborUpdate(PotentialNeighbor& pnbrArray, PNbrUpdateTypeE type)
		{
			if(type == PNBR_UPDATE) {
				//Start schedulers for new potential neighbors
				UniformRNGState rngState;
				rngState.cmrgState.stream = (uint64_t)(pnbrArray.linkId.nodeId * MAX_WAVEFORMS) + wid;
				rngState.cmrgState.run = 1;
				rngState.mean = period;
				rngState.range = period/2;
				nodeSchedule[pnbrArray.linkId.nodeId]->linkEstSchd->Set(rngState, (uint64_t)(cur.tv_sec) * 1000000);
				printf("Setting the linkEstSchd\n");
			} else if(type == PNBR_INVALID) {
				//Suspend scheduler for non-potential neighbors
				nodeSchedule[pnbrArray.linkId.nodeId]->linkEstSchd->Suspend();
			}
		}*/
		
		
		
		} //End of namespace
	}
	