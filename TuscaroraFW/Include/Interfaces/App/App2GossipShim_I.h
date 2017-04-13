////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef APP2Gossip_SHIM_I_H_
#define APP2Gossip_SHIM_I_H_


#include <Interfaces/AWI/Gossip_I.h>

using namespace AWI;


namespace Apps {

template<typename GOSSIPVARIABLE, typename GOSSIPCOMPARATOR = SimpleGenericComparator<GOSSIPVARIABLE> >
  class App2GossipShim_I : public Gossip_I<GOSSIPVARIABLE, GOSSIPCOMPARATOR> {
  
    //Framework_I *fi;
    
  public:
    
		virtual void RegisterGossipVariableUpdateDelegate(GossipVariableUpdateDelegate_t* _gvu_del) = 0;
		virtual ~App2GossipShim_I(){}
  };
  
template<typename GOSSIPVARIABLE>
  Gossip_I& GetApp2GossipShimPtr();
  
}//End of namespace



#endif //APP2Gossip_SHIM_I_H_
