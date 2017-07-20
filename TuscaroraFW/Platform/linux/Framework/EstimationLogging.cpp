#include "EstimationLogging.h"

#include "Framework/External/Location.h"

extern NodeId_t MY_NODE_ID;

using namespace Core::Estimation;

namespace Core {
  namespace Estimation {
    EstimationLogging::EstimationLogging() : map("linkestimation.bin"){
    }
    
    void EstimationLogging::LogEvent(EstimationLogE event, NodeId_t nbr) {
      struct timeval curtime;
      gettimeofday(&curtime,NULL);
      uint64_t cur = (uint64_t)curtime.tv_sec * 1000000 + curtime.tv_usec;
      double x = x();
      double y = y();
      
      EstimationLogElement le;
      le.x = x;
      le.y = y;
      le.time = cur;
      le.type = event;
      le.id = MY_NODE_ID;
      le.nbr = nbr;
      map.addRecord(le);
    }

    /*EstimationLogging::~EstimationLogging() {
    }*/
  }
}
