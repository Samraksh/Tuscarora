////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef STANDARD_FWPI_H_
#define STANDARD_FWPI_H_


#include "FWP.h"

#ifdef PLATFORM_DCE
#include <Platform/dce/Pattern/FWP2AppShim.h>
#elif PLATFORM_LINUX
#include <Platform/linux/Pattern/FWP2AppShim.h>
#endif

//#include <Interfaces/Core/ConfigureFrameworkI.h>

namespace AWI {
  class StandardFWPI{
    uint32_t piID;

  public:
    FWP* patternptr;
    FWP2AppShim* pattern2appshimptr;
    StandardFWPI();
  };

  void SetFwpInterface(StandardFWPI &fi);
} //End namespace


#endif //STANDARD_FWPI_H_
