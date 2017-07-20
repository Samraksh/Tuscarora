////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 


#ifndef ESTIMATIONLOGGING_H_
#define ESTIMATIONLOGGING_H_
#include "Lib/PAL/PAL_Lib.h"
#include "Framework/Core/Neighborhood/NeighborTable.h"
#include <Interfaces/PWI/Framework_I.h>
#include <fstream>
#include <Interfaces/Core/EstimationLogI.h>
#include "../PAL/Logs/MemMap.h"


namespace Core {
  namespace Estimation {

  struct EstimationLogElement {
     double x;
     double y;
     NodeId_t id;
     NodeId_t nbr;
     uint64_t time;
     EstimationLogE type;
   };


  class EstimationLogging: public EstimationLogI {
    private:
      MemMap<EstimationLogElement> map;
    public:
      //~EstimationLogging();
      EstimationLogging();
      void LogEvent(EstimationLogE event, NodeId_t nbr);
    };
  }
}
#endif
