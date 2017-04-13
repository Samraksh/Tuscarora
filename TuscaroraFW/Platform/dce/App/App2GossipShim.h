////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef APP2Gossip_SHIM_H_
#define APP2Gossip_SHIM_H_

#include <Interfaces/AWI/Gossip_I.h>
#include "Lib/DS/SimpleGenericComparator.h"

//#include "Platform/dce/Pattern/Gossip2AppShim.h"


//#include "Patterns/Gossip/StandardGossipI.h"


using namespace AWI;


namespace Apps {

template<typename GOSSIPVARIABLE, typename GOSSIPCOMPARATOR = SimpleGenericComparator<GOSSIPVARIABLE> >
class App2GossipShim : public Gossip_I<GOSSIPVARIABLE, GOSSIPCOMPARATOR> {

	Gossip_I<GOSSIPVARIABLE, GOSSIPCOMPARATOR> *fi;

	AppId_t m_appid;
	typedef typename Gossip_I<GOSSIPVARIABLE,GOSSIPCOMPARATOR>::GossipVariableUpdateDelegate_t GossipVariableUpdateDelegate_t_l;

public:

	App2GossipShim();

	//Inherited from Gossip_I
	void UpdateGossipVariable(GOSSIPVARIABLE& initialGossipVariable);
	void RegisterGossipVariableUpdateDelegate(GossipVariableUpdateDelegate_t_l* _gvu_del);

	~App2GossipShim(){};
};


template<typename GOSSIPVARIABLE, typename GOSSIPCOMPARATOR >
App2GossipShim<GOSSIPVARIABLE,GOSSIPCOMPARATOR>::App2GossipShim(){
//	fi = &(GetGossipShim<GOSSIPVARIABLE, GOSSIPCOMPARATOR>());


	GenericGossipIPtrType* p = AWI::GetGenericGossipInterfacePtr();
	fi = (static_cast<Gossip_I<GOSSIPVARIABLE, GOSSIPCOMPARATOR>*>(p));
//	fi = static_cast<>(GetGenericGossipInterfacePtr());
	//fi = ((static_cast<StandardGossipI<GOSSIPVARIABLE,GOSSIPCOMPARATOR> *>(GetGenericGossipInterfacePtr()))->patternptr);
}

template<typename GOSSIPVARIABLE, typename GOSSIPCOMPARATOR>
void App2GossipShim<GOSSIPVARIABLE,GOSSIPCOMPARATOR>::RegisterGossipVariableUpdateDelegate(GossipVariableUpdateDelegate_t_l* _gvu_del){
	fi -> RegisterGossipVariableUpdateDelegate(_gvu_del);

}
template<typename GOSSIPVARIABLE, typename GOSSIPCOMPARATOR>
void App2GossipShim<GOSSIPVARIABLE,GOSSIPCOMPARATOR>::UpdateGossipVariable(GOSSIPVARIABLE& initialGossipVariable){
	fi->UpdateGossipVariable(initialGossipVariable);
}




//template<typename GOSSIPVARIABLE>
//typedef Gossip_I<GOSSIPVARIABLE, GOSSIPCOMPARATOR> App2GossipShim;

template<typename GOSSIPVARIABLE, typename GOSSIPCOMPARATOR = SimpleGenericComparator<GOSSIPVARIABLE> >
Gossip_I<GOSSIPVARIABLE, GOSSIPCOMPARATOR>* GetApp2GossipShimPtr(){
	Gossip_I<GOSSIPVARIABLE, GOSSIPCOMPARATOR> *pS = static_cast<Gossip_I<GOSSIPVARIABLE, GOSSIPCOMPARATOR> *> (new App2GossipShim<GOSSIPVARIABLE, GOSSIPCOMPARATOR>());
	return pS;
}

}//End of namespace Apps



#endif //APP2Gossip_SHIM_H_
