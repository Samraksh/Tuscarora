////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef LINK_COMPARATORS_H_
#define LINK_COMPARATORS_H_

#include <Interfaces/PWI/LinkCompartorI.h>

namespace PWI {
namespace Neighborhood {

struct LinkMap{
  LinkId id;
  Link link;
  
  LinkMap(LinkId _id, Link _link){id=_id; link=_link;}
  LinkMap(LinkId _id){id=_id; }
};
  
class LinkMapComparatorI : public LinkComparatorI {
  ///Returns true if A is better than B, false otherwise
public:
  virtual bool BetterThan (Core::LinkMetrics& A, Core::LinkMetrics& B) =0;
  virtual bool BetterThan (LinkMap* A, LinkMap* B) = 0;
  bool EqualTo (LinkMap *A, LinkMap *B) {
    return A->id == B->id;
  }
  bool LessThan (LinkMap* A, LinkMap* B) { return BetterThan(A,B);}  
  virtual ~LinkMapComparatorI() {}
};


class QualityComparator:public LinkMapComparatorI {
public:
  
  bool BetterThan (Core::LinkMetrics& A, Core::LinkMetrics& B) {
    return A.quality > B.quality;
  }
  bool BetterThan (LinkMap *A, LinkMap *B) {
    return A->link.metrics.quality > B->link.metrics.quality;
  }

};  

class MinEnergyComparator:public LinkMapComparatorI {
public:

  bool BetterThan (Core::LinkMetrics& A, Core::LinkMetrics& B) {
    return A.energy < B.energy;
  }
  bool BetterThan (LinkMap *A, LinkMap *B) {
    return A->link.metrics.energy < B->link.metrics.energy;
  }

};

class CostComparator:public LinkMapComparatorI{
public:
  bool BetterThan (Core::LinkMetrics& A, Core::LinkMetrics& B) {
    return A.cost < B.cost;
  }
  bool BetterThan (LinkMap *A, LinkMap *B) {
    return A->link.metrics.cost < B->link.metrics.cost;
  }
};  

class QualAndXmitDelayComparator:public LinkMapComparatorI{
public:
  bool BetterThan (Core::LinkMetrics& A, Core::LinkMetrics& B) {
    if(A.quality > B.quality) {
      return true;
    }else if(A.quality > B.quality){
      return false;
    }else {
      return A.avgLatency < B.avgLatency;
    }
  }

  bool BetterThan (LinkMap *A, LinkMap *B) {
    if(A->link.metrics.quality > B->link.metrics.quality) {
      return true;
    }
    else {
      return A->link.metrics.avgLatency < B->link.metrics.avgLatency;
    }
  }
}; 

class MinAvgLatencyComparator: public LinkMapComparatorI {
public:
  bool BetterThan (Core::LinkMetrics& A, Core::LinkMetrics& B){
   return (A.avgLatency < B.avgLatency);
  }
  bool BetterThan (LinkMap *A, LinkMap *B) {
    return A->link.metrics.avgLatency < B->link.metrics.avgLatency;
  }

};
  
} //End of namespace
}

#endif //LINK_COMPARATORS_H_
