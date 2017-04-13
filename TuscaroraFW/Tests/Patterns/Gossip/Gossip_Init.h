////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef GOSSIP_INIT_H_
#define GOSSIP_INIT_H_

#include "Patterns/Gossip/StandardGossipI.h"
#include "Lib/Misc/Debug.h"
#include <Sys/Run.h>


template<typename GOSSIPVARIABLE, typename GOSSIPCOMPARATOR = SimpleGenericComparator<GOSSIPVARIABLE> >
class Gossip_Init{

	GOSSIPVARIABLE init_gos_var;

	StandardGossipI<GOSSIPVARIABLE, GOSSIPCOMPARATOR> *fi;

  void InitGossip(const GOSSIPVARIABLE& gs_var){
		Debug_Printf(DBG_TEST, "Gossip_Init::Init Gossip Pattern: \n");fflush(stdout);
		fi = new StandardGossipI<GOSSIPVARIABLE, GOSSIPCOMPARATOR>(gs_var);
		Debug_Printf(DBG_TEST, "Gossip_Init:: Created Gossip Pattern instance, ptr %p\n", fi);
  }
public:

  Gossip_Init(const GOSSIPVARIABLE& gs_var){
	  init_gos_var = gs_var;
  }
  ~Gossip_Init(){
	  Debug_Printf(DBG_TEST, "GOSSIP_Init:: Destructor\n");fflush(stdout);
	  delete fi;
  }

  AWI::StandardGossipI<GOSSIPVARIABLE, GOSSIPCOMPARATOR >* Execute(RuntimeOpts *opts){
	  InitGossip(init_gos_var);
	  fi->Start();
	  return fi;
  }
  //Waveform::WaveformBase* CreateWaveform(char* name, WaveformId_t wid, const char* waveformType, const char* linkEstimation, const char* cost, const char* energy);
};


#endif //FWP_INIT_H_
