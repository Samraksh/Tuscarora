////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef LIB_MATH_RNGSTATEIMPL_H_
#define LIB_MATH_RNGSTATEIMPL_H_


#include <iostream>
#include <sstream>
#include <typeinfo>
#include <cstdint>
#include <sstream>
#include <string>

#include <random>
#include <initializer_list>
#include <PAL/PseudoRandom/PseudoRandomI.h>
#include <PAL/PseudoRandom/rng-state.h>
#include <Lib/PAL/PseudoRandom/rng-seed-manager.h>
#include <Lib/PAL/PseudoRandom/RNStreamID.h>

#include "RandomCommon.h"

namespace PAL {


//class DummyForwardDeclaredClass{
//public:
//	DummyForwardDeclaredClass(){};
//	~DummyForwardDeclaredClass(){};
//};

class RNGStateImpl {
protected:

public:
	//std::string engine_state_string;//TODO: BK: The stream method of retrieving state does not work in 16.04 within DCE. (It works in DCE in 14.04 and outside of the DCE in both systems). We are reverting to a less efficient method.
	uint64_t num_drawals;
	RNStreamID cmrgState;

	template<class> friend class RNGState;

	RNGStateImpl();
	virtual ~RNGStateImpl();

	//template<typename DataOutType>
	//void InitializeEngineState(RNGState<DataOutType> *rngstateptr){
	void InitializeEngineState(){
		//initialize engine_state_string
		//std::initializer_list<uint64_t> mylist = {(uint64_t)(RngSeedManager::GetSeed()), cmrgState.stream, cmrgState.run};
		//std::seed_seq seed1(mylist);
//		Tuscarora_RNG2 temp_engine(seed1);
//		//temp_engine();
//		SetEngineStateBuffer(temp_engine);


		num_drawals = 0;

	}

	//void SetEngineStateBuffer(const Tuscarora_RNG2& temp_engine);//Setting the internal state definition of the underlying random number generator equal to that

	void GetEngineState(Tuscarora_RNG2& temp_engine) const;
};

} /* namespace PAL */

#endif /* LIB_MATH_RNGSTATEIMPL_H_ */
