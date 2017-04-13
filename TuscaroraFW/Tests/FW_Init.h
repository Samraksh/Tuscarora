////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef FRAMEWORK_INIT_H_
#define FRAMEWORK_INIT_H_

#include "Platform/PlatformSelector.h"
#include "Framework/PWI/StandardFI.h"
#include "Lib/Misc/Debug.h"
#include <Sys/RuntimeOpts.h>




class FW_Init{

  PWI::StandardFI *fi;

  void InitFI();
public:

  FW_Init();
  ~FW_Init();

  PWI::Framework_I* Execute(RuntimeOpts *opts);
  Waveform::WaveformBase* CreateWaveform(char* name, WaveformId_t wid, const char* waveformType, const char* linkEstimation, const char* cost, const char* energy);
};


#endif //FRAMEWORK_INIT_H_
