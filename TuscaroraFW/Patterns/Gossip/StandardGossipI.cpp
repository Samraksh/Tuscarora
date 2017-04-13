////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

////

#include "StandardGossipI.h"
//#include <Lib/DS/SimpleGenericComparator.h>
//#include <Interfaces/AWI/Gossip_I.h>
//
//#ifndef STANDARD_GOSSIP_CPP_
//#define STANDARD_GOSSIP_CPP_
//
namespace AWI {
//
//
//template<typename GOSSIPVARIABLE, typename GOSSIPCOMPARATOR = SimpleGenericComparator<GOSSIPVARIABLE> >
//Gossip_I<GOSSIPVARIABLE, GOSSIPCOMPARATOR>& GetGossipInterface(){
//    return *( ( static_cast<StandardGossipI<GOSSIPVARIABLE,GOSSIPCOMPARATOR> *>(GossipInterface))->patternptr );
//}
//
//

//

GenericGossipIPtrType *GossipInterface;

GenericGossipIPtrType* GetGenericGossipInterfacePtr() {
	return GossipInterface;
}



//template<typename GOSSIPVARIABLE, typename GOSSIPCOMPARATOR = SimpleGenericComparator<GOSSIPVARIABLE> >
//Gossip_I<GOSSIPVARIABLE, GOSSIPCOMPARATOR>& GetGossipInterface(){
//    return *((static_cast<StandardGossipI<GOSSIPVARIABLE,GOSSIPCOMPARATOR> *>(GossipInterface))->patternptr);
//}

void SetGossipInterface(GenericGossipIPtrType *fi){
	GossipInterface = fi;
}


} //End namespace
