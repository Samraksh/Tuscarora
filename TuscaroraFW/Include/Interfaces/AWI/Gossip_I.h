////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef Gossip_I_H_
#define Gossip_I_H_

#include <Base/AppTypes.h>
#include <Interfaces/Core/MessageT.h>
#include <vector>
#include "Lib/DS/SimpleGenericComparator.h"

namespace AWI {

class GenericGossipIPtrType{
public:
	GenericGossipIPtrType(){};
};


template<typename GOSSIPVARIABLE, typename GOSSIPCOMPARATOR = SimpleGenericComparator<GOSSIPVARIABLE> >
class Gossip_I : public GenericGossipIPtrType {
public:
	typedef Delegate<void, GOSSIPVARIABLE&> GossipVariableUpdateDelegate_t;

	  /**
	    * @brief Used by the application to register its GossipVariableUpdateDelegate_t,
	    * which in turn is used by the pattern to notify the application about the changes in the GOSSIPVARIABLE.
	    *
	    * The Gossip pattern stores delegates registered to itself and invokes these
	    * delegates whenever there is an update on the GOSSIPVARIABLE.
	    * The update can be as a result of a reception of a GossipMessage from another node as well as
	    * the UpdateGossipVariable request from another application connected to the Pattern locally.
	    *
	    * @param GossipVariableUpdateDelegate_t The delegate that specifies the function/method used for receiving updates from pattern.
	    * @return void.
	    */
	virtual void RegisterGossipVariableUpdateDelegate(GossipVariableUpdateDelegate_t* _gvu_del) = 0; //Used by the application to register the delegate

	  /**
	    * @brief Used by the applications to update the underlying GOSSIPVARIABLE of the pattern.
	    *
	    * This method lets a application to request changing the variable being gossiped with a newVariable.
	    * The newgossipVariable is compared with the existing one.
	    * 	If the existing variable is larger(based on the comparator), no further action is taken.
	    * 	Else if, the newvariable is larger(based on the comparator),
	    * 		the gossip variable is updated,
	    * 		and all registered GossipVariableUpdateDelegate_t are invoked with the new GOSSIPVARIABLE.
	    *
	    * @param newgossipVariable The new gossip
	    * @return void.
	    */
	virtual void UpdateGossipVariable(GOSSIPVARIABLE& newgossipVariable) = 0; 	//Used by the application to update the GOSSIPVARIABLE


	///Virtual destructor
	virtual ~Gossip_I (){}
};

template<typename GOSSIPVARIABLE>
class Gossip2AppShim_I {
	typedef typename Gossip_I<GOSSIPVARIABLE>::GossipVariableUpdateDelegate_t GossipVariableUpdateDelegate_t_l;
public:
  virtual void ReceiveUpdatedGossipVariable(GOSSIPVARIABLE& msg) = 0;
  virtual void RegisterGossipVariableUpdateDelegate(GossipVariableUpdateDelegate_t_l* _gvu_del){};
//  virtual void RegisterGossipApp(AppId_t _app_id) = 0;
};

///Returns a reference to an implentation of the pattern interface.
template<typename GOSSIPVARIABLE, typename GOSSIPCOMPARATOR = SimpleGenericComparator<GOSSIPVARIABLE> >
Gossip_I<GOSSIPVARIABLE>& GetGossipInterface();


void SetGossipInterface(GenericGossipIPtrType *fi);

GenericGossipIPtrType* GetGenericGossipInterfacePtr();


} //End of namespaceA

#endif /* Gossip_I_H_ */
