////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef FWP_INIT_H_
#define FWP_INIT_H_

#include "Patterns/FloodingWithPruning/StandardFWPI.h"
#include "Lib/Misc/Debug.h"
#include <Sys/RuntimeOpts.h>



class FWP_Init{

  AWI::StandardFWPI *fi;

  void InitFI();
public:

  FWP_Init();
  ~FWP_Init();

  AWI::Fwp_I *Execute(RuntimeOpts *opts);
  //Waveform::WaveformBase* CreateWaveform(char* name, WaveformId_t wid, const char* waveformType, const char* linkEstimation, const char* cost, const char* energy);
};


#endif //FWP_INIT_H_
