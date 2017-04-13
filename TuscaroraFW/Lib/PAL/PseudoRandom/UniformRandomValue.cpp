////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#include <PAL/PseudoRandom/UniformRandomValue.h>
#include "UniformRandomValueImpl.h"
#include <Lib/PAL/PseudoRandom/rng-seed-manager.h>
#include <Lib/PAL/PseudoRandom/RNStreamID.h>

namespace PAL {


UniformRandomValue::UniformRandomValue(double minVal, double maxVal){

	implPtr = new UniformRandomValueImpl();


	RNStreamID rnstream;
	rnstream.stream = RngSeedManager::GetNextStreamIndex ();
	rnstream.run = RngSeedManager::GetRun();
	UniformDoubleRVDistributionParametersType dist; dist.min = minVal; dist.max = maxVal ;
	implPtr->m_internal_state_ptr = new UniformRandomValue::State(rnstream);
	implPtr->m_internal_state_ptr->SetDistributionParameters(dist);

	Tuscarora_InitializerList mylist = {(uint64_t)(RngSeedManager::GetSeed()), rnstream.stream, rnstream.run};
	Tuscarora_Seed_Seq seed1(mylist);
	implPtr->m_engine_ptr = new Tuscarora_RNG2(seed1);

	if(implPtr->m_distribution_ptr != NULL) delete implPtr->m_distribution_ptr;
	implPtr->m_distribution_ptr = new Tuscarora_UniformDoubleDist(dist.min, dist.max);

}

UniformRandomValue::UniformRandomValue(const State& _state) : UniformRandomValue(){
	SetState(_state);
}

void UniformRandomValue::SetRange(double minVal, double maxVal){

	UniformDoubleRVDistributionParametersType dist; dist.min = minVal; dist.max = maxVal ;
	implPtr->m_internal_state_ptr->SetDistributionParameters(dist);

	if(implPtr->m_distribution_ptr != NULL) delete implPtr->m_distribution_ptr;
	implPtr->m_distribution_ptr = new Tuscarora_UniformDoubleDist(dist.min, dist.max);}

//Inherited through PseudoRandomI

double UniformRandomValue::GetNext(){
	return ((*implPtr->m_distribution_ptr)(*implPtr->m_engine_ptr));
}

const UniformRandomValue::State UniformRandomValue::GetState(){

	//implPtr->m_internal_state_ptr->implPtr->engine_state_string  = implPtr->GetEngineStateBufferString(*implPtr->m_engine_ptr);

	//implPtr->m_internal_state_ptr->SetEngineStateBuffer(*implPtr->m_engine_ptr);

	State state;
	state = *(implPtr->m_internal_state_ptr);

	return state;
}

void UniformRandomValue::SetState(const State& state){
	*(implPtr->m_internal_state_ptr) = state;
	state.GetEngineState(*implPtr->m_engine_ptr);
}

UniformRandomValue::~UniformRandomValue(){
	if(implPtr->m_engine_ptr != NULL) delete implPtr->m_engine_ptr;
	if(implPtr->m_distribution_ptr != NULL) delete implPtr->m_distribution_ptr;
}

}
