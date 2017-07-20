////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include "Platform/linux/Framework/LongLink.h"
#include <Sys/RuntimeOpts.h>

#include "Platform/linux/ExternalServices/LocationService/LocationService.h"

#ifdef PLATFORM_DCE
#include "Platform/dce/ExternalServices/NS3WaveformSpec.h"
#endif


#ifndef PLATFORM_EMOTE
#include "../PAL/Logs/MemMap.h"
#endif

extern NodeId_t MY_NODE_ID;
extern RuntimeOpts* RUNTIME_OPTS;

using namespace PAL;
using namespace PWI;
using namespace ExternalServices;

namespace Core {
namespace Discovery {

#define SC(x) static_cast <FrameworkBase*>(x)
    
void LongLink::Start() {
	TimerDelegate *state_ = new TimerDelegate(this, &LongLink::SendLongBeacon);
	
	numNodes = RUNTIME_OPTS->nodes -1;
	range = RUNTIME_OPTS->range;

#ifndef PLATFORM_EMOTE
	MemMap<DiscoveryLogHeader> map("configuration.bin");
	map.openForRead();
	DiscoveryLogHeader* header = map.getNext();
	range = header->range;
#endif

	isBase = numNodes == MY_NODE_ID;
	if(!isBase) {
		longTimer = new Timer(1000*numNodes - (1000 * (MY_NODE_ID + 1)), ONE_SHOT, *state_);
		longTimer->Start();//Have this send the long link message to base station
	} else if(isBase) {
		longTimer = new Timer(1000*(numNodes+1), ONE_SHOT, *state_);
		longTimer->Start();//Have this send the long link message to base station
	}
	locations = new Location2D[numNodes + 1];
	for(int x = 0; x < numNodes; x++) {
		locations[x].x = 0;
		locations[x].y = 0;
	}
}
    
void LongLink::SendLongBeacon(uint32_t event) {
	static int iter = 0;
	int group = ceil(1.0f*numNodes / 25);
	if(isBase) {
		//Broadcast the stored data
		Debug::PrintTimeMicro();printf(" message sent for iter %d\n", iter);

		uint8_t* payload = new uint8_t[sizeof(Location2D)*((MY_NODE_ID - (iter*25))>25?25:MY_NODE_ID-(iter*25))];
		memcpy(payload, (uint8_t*)&locations[25*iter], sizeof(Location2D)*((MY_NODE_ID - (iter*25))>25?25:MY_NODE_ID-(iter*25)));

		FMessage_t* msg = new FMessage_t();
		msg->SetPayload(payload);
		msg->SetSource(MY_NODE_ID);
		msg->SetType(Types::DISCOVERY_MSG);
		msg->SetPayloadSize(sizeof(Location2D)*((MY_NODE_ID - (iter*25))>25?25:MY_NODE_ID-(iter*25)));
		Location2D* locs = (Location2D*) msg->GetPayload();
		printf("Message sent for node %d with loc of %f,%f\n", iter*25 + 1, locs[1].x, locs[1].y);
		//fi->BroadcastData(0, *msg, Waveform::LONG_LINK_WF, 0);//Masahiro adding nonce value
		SC(fi)->SendToWF(Waveform::LONG_LINK_WF, true, Types::DISCOVERY_MSG, 0, 0, *msg);
		if(iter + 1 == group){
			longTimer->Change(beacon - 1000 * (group - 1), ONE_SHOT);
		} else {
			longTimer->Change(1000, ONE_SHOT);
		}
	} else 
	{
		Debug::PrintTimeMicro();
		printf("Sending long link\n");
		longTimer->Change(10000000, ONE_SHOT);
		Location2D location = LocationService::GetLocation();

		uint8_t* payload = new uint8_t[sizeof(location)];
		memcpy(payload, &location, sizeof location);

		FMessage_t* msg = new FMessage_t();
		msg->SetPayload(payload);
		msg->SetSource(MY_NODE_ID);
		msg->SetType(Types::DISCOVERY_MSG);
		msg->SetPayloadSize(sizeof location);
		//fi->BroadcastData(0, *msg, Waveform::LONG_LINK_WF, 0); //Masahiro adding nonce value
		SC(fi)->SendToWF(Waveform::LONG_LINK_WF, true, Types::DISCOVERY_MSG, 0, 0, *msg);
	}
	iter++;
	iter = iter % group;
}

void LongLink::ProcessMessage(FMessage_t& msg) {
	static int iter2 = 0;
	int group = ceil(1.0f*numNodes / 25);
	if(isBase) {
		uint16_t nbr = 0;
		nbr = msg.GetSource();
		Location2D* location = (Location2D*) msg.GetPayload();
		locations[nbr].x = location->x;
		locations[nbr].y = location->y;
		Debug::PrintTimeMicro();printf(" message received from %d at %f,%f\n", nbr, location->x, location->y);
		
	} else 
	{
		if(msg.GetSource() == numNodes) {
			Location2D location = LocationService::GetLocation();
			Location2D* locs = (Location2D*) msg.GetPayload();
			Debug::PrintTimeMicro();printf("GOT MESSAGE FOR ITER %d of %d\n", iter2, group);fflush(stdout);
			for(NodeId_t x = iter2 * 25; x < numNodes && x < (iter2 + 1) * 25; x++) {
				if(x == MY_NODE_ID) continue;
#ifdef PLATFORM_DCE
				if((location.x - locs[x-(iter2*25)].x) * (location.x - locs[x-(iter2*25)].x) + (location.y - locs[x-(iter2*25)].y) * (location.y - locs[x-(iter2*25)].y) < (range * hops) * (range * hops)) {
					PotentialNeighbor *i = pNbrs.GetNode(x);
					if(i == 0) {
						for(int w = 2; w < MAX_WAVEFORMS; w++) {
							if(NS3WaveformSpec::HasWaveform(w, x)) {
								LinkId link;
								link.nodeId = x;
								link.waveformId = w;
								i = pNbrs.AddNode(link, BEYOND_COMM_PNODE, 0, 1 - (((range * 2)*(range*2)) / ((location.x - locs[x].x)*(location.x-locs[x].x) + (location.y-locs[x].y)*(location.y-locs[x].y))));
								if(del != 0) {
									//Sending the event to the delegate, this should be the link estimation method
									PotentialNeighborDelegateParam _param;
									_param.changeType = PNBR_UPDATE;
									_param.pnbr = i;
									del->operator ()(_param);
								}
							}
						}
					}
				} else 
				{

					PotentialNeighbor *i = pNbrs.GetNode(x);
					if(i != 0) {
						for(int w = 2; w < MAX_WAVEFORMS; w++) {
							if(NS3WaveformSpec::HasWaveform(w, x)) {
								LinkId link;
								link.nodeId = x;
								link.waveformId = w;
								if(del != 0) {
									//Sending the event to the delegate, this should be the link estimation method
									PotentialNeighborDelegateParam _param;
									_param.changeType = PNBR_INVALID;
									_param.pnbr = i;
									del->operator ()(_param);	
								}
								pNbrs.RemoveNode(link);
							}
						}
					}

				}
#endif
			}
			iter2++;
			iter2 = iter2 % group;
		}
	}
	delete(&msg);
}

	
}	//End namespace
}
