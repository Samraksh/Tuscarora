////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 


#include <Base/AppTypes.h>
#include <Interfaces/AWI/Gossip_I.h>
#include <Lib/DS/BSTMapT.h>
#include <Lib/DS/ListT.h>

//#include <Patterns/Gossip/StandardGossipI.cpp>

#ifndef PLATFORM_DCE_PATTERN_Gossip2APPSHIM_H_
#define PLATFORM_DCE_PATTERN_Gossip2APPSHIM_H_

namespace AWI {



template<typename GOSSIPVARIABLE, typename GOSSIPCOMPARATOR= SimpleGenericComparator<GOSSIPVARIABLE>>
class Gossip2AppShim : public Gossip2AppShim_I<GOSSIPVARIABLE > {
	typedef typename Gossip_I<GOSSIPVARIABLE, GOSSIPCOMPARATOR>::GossipVariableUpdateDelegate_t GossipVariableUpdateDelegate_t_l;
	//typedef std::vector<GossipVariableUpdateDelegate_t*> GossipVariableUpdateDelegateVector_t;
	//GossipVariableUpdateDelegateVector_t vector_gvu_delegates;
	Gossip_I<GOSSIPVARIABLE, GOSSIPCOMPARATOR> *fi;
	ListT<GossipVariableUpdateDelegate_t_l*, true, EqualTo<GossipVariableUpdateDelegate_t_l*>> list_gvu_delegates;


//	typename Gossip_I<GOSSIPVARIABLE, GOSSIPCOMPARATOR>::GossipVariableUpdateDelegate_t* m_recvDelegate;
public:
	Gossip2AppShim(Gossip_I<GOSSIPVARIABLE, GOSSIPCOMPARATOR> *_fi);
	~Gossip2AppShim();
	void ReceiveUpdatedGossipVariable(GOSSIPVARIABLE& msg);
	void RegisterGossipVariableUpdateDelegate(GossipVariableUpdateDelegate_t_l* _gvu_del);

};

template<typename GOSSIPVARIABLE, typename GOSSIPCOMPARATOR>
Gossip2AppShim<GOSSIPVARIABLE, GOSSIPCOMPARATOR>::Gossip2AppShim(Gossip_I<GOSSIPVARIABLE, GOSSIPCOMPARATOR> *_fi) {
	// TODO Auto-generated constructor stub
	fi = _fi;
}
template<typename GOSSIPVARIABLE, typename GOSSIPCOMPARATOR>
Gossip2AppShim<GOSSIPVARIABLE, GOSSIPCOMPARATOR>::~Gossip2AppShim() {
	// TODO Auto-generated destructor stub
	GossipVariableUpdateDelegate_t_l* ptr;
	for(uint32_t i = 0; i < list_gvu_delegates.Size(); ++i){
		ptr = list_gvu_delegates[i];
		delete ptr;
	}
}
template<typename GOSSIPVARIABLE, typename GOSSIPCOMPARATOR>
void Gossip2AppShim<GOSSIPVARIABLE, GOSSIPCOMPARATOR>::ReceiveUpdatedGossipVariable(GOSSIPVARIABLE& msg){

//	for(typename AppDelegatesList_t_l::iterator it = list_gvu_delegates.begin(); it != list_gvu_delegates.end(); ++it){
//		(*it)->operator()(currentStatusId);
//	}
	GossipVariableUpdateDelegate_t_l* ptr;
	for(uint32_t i = 0; i < list_gvu_delegates.Size(); ++i){
		ptr = list_gvu_delegates[i];
		ptr->operator()(msg);
	}

//	if(m_recvDelegate !=NULL ){
//		Debug_Printf(DBG_PATTERN, "Gossip2AppShim::ReceiveMessage Invo\n");
//		m_recvDelegate->operator()(msg);
//	}
//	else{
//		Debug_Printf(DBG_PATTERN, "Gossip2AppShim::ReceiveMessage AppDelegate not available \n");
//	}

}
template<typename GOSSIPVARIABLE, typename GOSSIPCOMPARATOR>
void Gossip2AppShim<GOSSIPVARIABLE, GOSSIPCOMPARATOR>::RegisterGossipVariableUpdateDelegate(GossipVariableUpdateDelegate_t_l* _gvu_del) {
	Debug_Printf(DBG_PATTERN, "Gossip2AppShim:: Storing delegates \n");
	//m_recvDelegatesMap.Insert(appId, _gvu_del);
//	m_recvDelegate = _gvu_del;
//	 list_gvu_delegates.push_back(_gvu_del);
	list_gvu_delegates.InsertBack(_gvu_del);
}

//
//template<typename GOSSIPVARIABLE, typename GOSSIPCOMPARATOR = SimpleGenericComparator<GOSSIPVARIABLE> >
//Gossip_I<GOSSIPVARIABLE, GOSSIPCOMPARATOR>& GetGossipShim(){
//	Gossip_I<GOSSIPVARIABLE, GOSSIPCOMPARATOR> *fi = &GetGossipInterface<GOSSIPVARIABLE,GOSSIPCOMPARATOR>();
//  return *fi;
//}

} /* namespace AWI */


#endif /* PLATFORM_DCE_PATTERN_Gossip2APPSHIM_H_ */
