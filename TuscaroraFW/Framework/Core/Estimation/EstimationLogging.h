////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 


#ifndef ESTIMATIONLOGGING_H_
#define ESTIMATIONLOGGING_H_
#include "Lib/PAL/PAL_Lib.h"
#include "Lib/Misc/MemMap.h"
#include "Framework/Core/Neighborhood/NeighborTable.h"
#include <Interfaces/PWI/Framework_I.h>
#include <fstream>

namespace Core {
  namespace Estimation {
    class EstimationLogging{
    private:
      MemMap<EstimationLogElement> map;
    public:
      ~EstimationLogging();
      EstimationLogging();
      void LogEvent(EstimationLogE event, NodeId_t nbr);
    };
  }
}
#endif
