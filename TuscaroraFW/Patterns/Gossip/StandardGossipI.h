////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef STANDARD_GOSSIP_H_
#define STANDARD_GOSSIP_H_

#include "Gossip.h"

#ifdef PLATFORM_DCE
#include <Platform/dce/Pattern/Gossip2AppShim.h>
#elif PLATFORM_LINUX
#include <Platform/linux/Pattern/Gossip2AppShim.h>
#endif


//#include <Interfaces/Core/ConfigureFrameworkI.h>

namespace AWI {



template<typename GOSSIPVARIABLE, typename GOSSIPCOMPARATOR = SimpleGenericComparator<GOSSIPVARIABLE> >
  class StandardGossipI: public Gossip<GOSSIPVARIABLE,GOSSIPCOMPARATOR>{
    uint32_t piID;

  public:
    //Gossip<GOSSIPVARIABLE, GOSSIPCOMPARATOR>* patternptr;
    Gossip2AppShim<GOSSIPVARIABLE, GOSSIPCOMPARATOR>* pattern2appshimptr;
    StandardGossipI(const GOSSIPVARIABLE&  gs_var);
  };

template<typename GOSSIPVARIABLE, typename GOSSIPCOMPARATOR>
StandardGossipI<GOSSIPVARIABLE, GOSSIPCOMPARATOR>::StandardGossipI(const GOSSIPVARIABLE&  gs_var)
	: Gossip<GOSSIPVARIABLE,GOSSIPCOMPARATOR>(gs_var){
//  pattern2appshimptr = new Gossip2AppShim<GOSSIPVARIABLE, GOSSIPCOMPARATOR> ();
//  patternptr = new Gossip<GOSSIPVARIABLE,GOSSIPCOMPARATOR>(gs_var, pattern2appshimptr);
  SetGossipInterface(this);
}


//template<typename GOSSIPVARIABLE>
//StandardGossipI<GOSSIPVARIABLE> *GossipInterface;




//template<typename GOSSIPVARIABLE, typename GOSSIPCOMPARATOR = SimpleGenericComparator<GOSSIPVARIABLE> >
//Gossip_I<GOSSIPVARIABLE, GOSSIPCOMPARATOR>& GetGossipInterface();


} //End namespace




#endif /* STANDARD_GOSSIP_H_ */
