////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#include "StandardFI.h"

// extern bool DBG_CORE; // set in Lib/Misc/Debug.cpp

namespace PWI {
  


  //uint16_t StandardFI::noOfWaveforms=2;
  //uint16_t StandardFI::waveformIdGenerator=0;

  ///Returns the list of supported waveforms 
  /*uint16_t StandardFI::GetWaveformIds(uint16_t* buf, uint16_t bufsize)
  {
    for (uint8_t i=0; i < noOfWaveforms; i++){
      buf[i]= supportedWaveformsIds[i];
    }
    return noOfWaveforms;
    
  }*/
  
  
  void StandardFI::RegisterLinkEstimator(LinkEstimatorI<uint64_t> &est, EstimatorTypeE type, WaveformId_t wid)
  {
    if(type == Core::Estimation::FW_TEST_EST) {
     Debug_Printf(DBG_CORE, "StandardFI:: Passing  delegate ptr %p for a test link estimator\n", leDel);
      est.RegisterDelegate(*(leDel));
    }
  }

  StandardFI::StandardFI(){
      SetFrameworkInterface(*this);
  }
  
  ///Instantiate the framework.
  StandardFI *frameworkInterface;

  void SetFrameworkInterface(Framework_I &fi){
    frameworkInterface =static_cast<StandardFI *> (&fi);
  }
  
  Framework_I& GetFrameworkInterface(){
    return *frameworkInterface;
  }  
  
  void GetWaveformIDs(WaveformId_t *idArray, uint16_t noOfWaveforms){
   return frameworkInterface->GetWaveformIDs(idArray, noOfWaveforms); 
  }
  uint16_t GetNumberOfWaveforms(){
    return frameworkInterface->GetNumberOfWaveforms();
  }

}//End of namespace
