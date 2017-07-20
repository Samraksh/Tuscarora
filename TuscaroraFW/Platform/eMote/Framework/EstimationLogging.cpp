////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include "EstimationLogging.h"
#include "Framework/External/Location.h"

extern NodeId_t MY_NODE_ID;

using namespace Core::Estimation;

namespace Core {
  namespace Estimation {
    EstimationLogging::EstimationLogging() {
    }
    
    void EstimationLogging::LogEvent(EstimationLogE event, NodeId_t nbr) {
    }

    EstimationLogging::~EstimationLogging() {
    }
  }
}
