#ifndef ESTIMATIONLOGI_H_
#define ESTIMATIONLOGI_H_

namespace Core {
  namespace Estimation {

  enum EstimationLogE {
     LINK_ADDED = 1,
     LINK_REMOVED = 2,//
     LINK_UPDATED = 3,
     LINK_SKIPPED = 4,//
     LINK_SENT = 5,
     LINK_POTENTIAL_ADD = 6,
     LINK_POTENTIAL_DEL = 7
   };


  class EstimationLogI{

    public:
      virtual ~EstimationLogI(){}
      virtual void LogEvent(EstimationLogE event, NodeId_t nbr) = 0;
    };
  }
}

#endif //ESTIMATIONLOGI_H_
