////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#include "StandardFWPI.h"





// extern bool DBG_CORE; // set in Lib/Misc/Debug.cpp

namespace AWI {
  
  StandardFWPI::StandardFWPI(){
	  pattern2appshimptr = new FWP2AppShim ();
	  patternptr = new FWP(pattern2appshimptr);
	  AWI::SetFwpInterface(*this);
  }
  
  ///Instantiate the FWP.
  StandardFWPI *fwpInterface;

  void SetFwpInterface(StandardFWPI &fi){
    fwpInterface = (&fi);
  }
  
  Fwp_I& GetFwpInterface(){
    return *(fwpInterface->patternptr);
  }  
  
  Fwp_I* GetFwpInterfacePtr(){
    return (fwpInterface->patternptr);
  }
//  void GetWaveformIDs(WaveformId_t *idArray, uint16_t noOfWaveforms){
//   return fwpInterface->GetWaveformIDs(idArray, noOfWaveforms);
//  }
//  uint16_t GetNumberOfWaveforms(){
//    return fwpInterface->GetNumberOfWaveforms();
//  }

}//End of namespace
