////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef STANDARD_FI_H_
#define STANDARD_FI_H_


#include "FrameworkBase.h"
#include <Interfaces/Core/ConfigureFrameworkI.h>

namespace PWI {
  class StandardFI : public FrameworkBase, ConfigureFramework_I{
    uint32_t piID;
    //static uint16_t noOfWaveforms;
    //uint16_t supportedWaveformsIds[10];
    //static uint16_t waveformIdGenerator;

  public:
    StandardFI();
    /*static uint16_t GetNewWaveformId(){
      if (waveformIdGenerator+1 <= noOfWaveforms){
	return waveformIdGenerator++;
      }
      else return 0;
    }*/

    //uint16_t GetWaveformIds(uint16_t* buf, uint16_t bufsize);
    void RegisterLinkEstimator(LinkEstimatorI<uint64_t> &est, EstimatorTypeE type, WaveformId_t wid);
    
  };
  
} //End namespace


#endif //STANDARD_FI_H_
