////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



//#include <stdlib.h>
//#include "FWP_Init.h"
#include "Tests/Patterns/FWP/FWP_Init.h"

#include <string.h>
#include "Lib/PAL/PAL_Lib.h"

#if defined(PLATFORM_LINUX)
//#include <Platform/linux/Framework/PatternEventDispatch.h>
#endif

extern NodeId_t MY_NODE_ID;
extern uint16_t NETWORK_SIZE;
extern RuntimeOpts* RUNTIME_OPTS;
// extern bool DBG_TEST; // set in Lib/Misc/Debug.cpp

FWP_Init::FWP_Init(){
  //Initialize the Pattern Interface first
  InitFI();
}

FWP_Init::~FWP_Init(){
	Debug_Printf(DBG_TEST, "FWP_Init:: Destructor\n");fflush(stdout);
}

void FWP_Init::InitFI(){
	Debug_Printf(DBG_TEST, "FWP_Init::Init FWP Pattern: \n");fflush(stdout);
	fi = new StandardFWPI();

	Debug_Printf(DBG_TEST, "FWP_Init:: Created FWP Pattern instance, ptr %p\n", fi);
}

AWI::Fwp_I *FWP_Init::Execute(RuntimeOpts *opts)
{
//	//Debug_Printf(DBG_TEST, "STARTING FWP_Initbed\n");
//	fflush(stdout);
//	NETWORK_SIZE = opts->nodes;
//	RUNTIME_OPTS = opts;
//	Debug_Printf(DBG_TEST, "FWP_Init:: Creating framework for node %d of %d.....\n", MY_NODE_ID, opts->nodes);
//
//	if(opts->discoveryType == Longlink2HDiscoveryType) {
//		char longlink[] = "long0";
//		float cost_of_longlink = 1.0;
//		float energyInDbm = 2.0;
//		WaveformBase *wb = new WF_AlwaysOn_Ack(LONG_LINK_WID, Waveform::ALWAYS_ON_LOCAL_WF, Waveform::WF_NO_EST, longlink, cost_of_longlink, energyInDbm);
//		fi->SetLongLink(*wb, LONG_LINK_WID, longlink);
//	}
//
//	//To test multiple wavefrom, assign node 0 and node 1 in wf1
//	//                                  node 2 and node 3 in wf2
//	Debug_Printf(DBG_TEST, "FWP_Init:: MY_NODE_ID is %d\n", MY_NODE_ID);
//	Debug_Printf(DBG_TEST, "FWP_Init:: opts->range is %f\n",opts->range);
//	//Debug_Printf(DBG_TEST, "FWP_Init:: opts->speedMin is %f\n",opts->speedMin);
//	//Debug_Printf(DBG_TEST, "FWP_Init:: opts->speedMax is %f\n",opts->speedMax);
//	Debug_Printf(DBG_TEST, "FWP_Init:: opts->mobilityModel is %s\n",opts->interferenceModel.c_str());
//	int beacon_period_in_microseconds = opts->linkEstimationPeriod;
//	int missed_periods_to_be_dead = opts->deadNeighborPeriod;
//	int max_nodes = opts->nodes;
//	Debug_Printf(DBG_TEST, "FWP_Init:: FWP_Init:: Execute:: Beacon Period: %d, Inactivity Period: %d, Max Nodes: %d \n",
//	beacon_period_in_microseconds,  missed_periods_to_be_dead,  max_nodes);
//	fflush(stdout);
//
//	fi->SetLinkEstimationPeriod(opts->linkEstimationPeriod);
//	fi->SetNeighborInactivityPeriod( opts->deadNeighborPeriod);
//	fi->SetRange(opts->range );
//	fi->runTime = opts->runTime;
//	fi->density = opts->density;
//	fi->mobilityModel = opts->mobilityModelInt();
//	fi->speed = opts->averageSpeed();
//
//	if ( opts->discoveryType == GlobalDiscoveryType ) {
//		fi->SetDiscoveryType ( GLOBAL_DISCOVERY );
//	}
//	else if ( opts->discoveryType == Oracle2HDiscoveryType ) {
//		fi->SetDiscoveryType ( ORACLE_DISCOVERY );
//	}
//	else if ( opts->discoveryType == Longlink2HDiscoveryType ) {
//		fi->SetDiscoveryType ( LONG_LINK_DISCOVERY );
//	}
//	else if ( opts->discoveryType == NullDiscoveryType ) {
//		fi->SetDiscoveryType ( NULL_DISCOVERY );
//	}
//	else {
//		printf ( "unknown discovery type '%s'\n",opts->discoveryType.c_str() );
//		exit ( 1 );
//	}
//
//	if ( opts->linkEstimationType == RandomAlwaysOnLinkEstimation ) {
//		Debug_Printf(DBG_TEST, " FWP_Init:: LinkEstimation Type %s to %s \n", opts->linkEstimationType.c_str(), RandomAlwaysOnLinkEstimation.c_str());
//		fi->SetLinkEstimationType( FWP_RANDOM_EST );
//	}
//	else if ( opts->linkEstimationType == ScheduledLinkEstimation ) {
//		Debug_Printf(DBG_TEST, " FWP_Init:: LinkEstimation Type %s to %s \n", opts->linkEstimationType.c_str(), ScheduledLinkEstimation.c_str());
//		fi->SetLinkEstimationType( FWP_SCHEDULED_EST );
//	}
//	else if ( opts->linkEstimationType == ConflictAwareLinkEstimation ) {
//		Debug_Printf(DBG_TEST, " FWP_Init:: LinkEstimation Type %s to %s \n", opts->linkEstimationType.c_str(), ConflictAwareLinkEstimation.c_str());
//		fi->SetLinkEstimationType( FWP_CONFLICTAWARE_EST );
//	}
//	else {
//		printf ( "unknown link estimation type %s\n",opts->linkEstimationType.c_str() );
//		exit ( 1 );
//	}
//
//#if defined(PLATFORM_DCE)
//
//	//fi->SetMaxNodes( max_nodes );
//	char locallink[] = "local";
//	for(int x = 2; x < MAX_WAVEFORMS; x++) {
//		if(NetworkDiscovery::hasWaveform(x)) {
//			Debug_Printf(DBG_TEST, "FWP_Init:: Creating waveform %d\n",x);
//			CreateWaveform(locallink, x, NetworkDiscovery::getWaveformType(x),  NetworkDiscovery::getWaveformEstimationType(x),
//						   NetworkDiscovery::getWaveformCost(x),  NetworkDiscovery::getWaveformEnergy(x));
//		}
//	}
//#endif

	Debug_Printf(DBG_TEST, "FWP_Init:: Starting FWP Pattern\n"); fflush(stdout);
	fi->patternptr->Start();
	Debug_Printf(DBG_TEST, "FWP_Init:: FWP Pattern up and running...\n");  fflush(stdout);
	return fi->patternptr;
}
