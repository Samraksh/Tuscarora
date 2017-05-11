////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 


#include <PAL/PseudoRandom/UniformRandomInt.h>
#include "UniformRandomIntImpl.h"
#include <Lib/PAL/PseudoRandom/rng-seed-manager.h>
#include <Lib/PAL/PseudoRandom/RNStreamID.h>

namespace PAL {

UniformRandomInt::UniformRandomInt(uint64_t minVal, uint64_t maxVal){
	implPtr = new UniformRandomIntImpl();

	RNStreamID rnstream;
	rnstream.stream = RngSeedManager::GetNextStreamIndex ();
	rnstream.run = RngSeedManager::GetRun();
	UniformIntDistParameter dist; dist.min = minVal; dist.max = maxVal ;
	implPtr->m_internal_state_ptr = new UniformRandomInt::State(rnstream);
	implPtr->m_internal_state_ptr->SetDistributionParameters(dist);

	Tuscarora_InitializerList mylist = {(uint64_t)(RngSeedManager::GetSeed()), rnstream.stream, rnstream.run};
	Tuscarora_Seed_Seq seed1(mylist);
	implPtr->m_engine_ptr = new Tuscarora_RNG2(seed1);

	if(implPtr->m_distribution_ptr != NULL) delete implPtr->m_distribution_ptr;
	implPtr->m_distribution_ptr = new Tuscarora_UniformIntDist(dist.min, dist.max);
}

UniformRandomInt::UniformRandomInt(const State& _state) : UniformRandomInt()  {
	SetState(_state); // This does not work because in the case of manually created UniformRNGState's the engine_state_string may not be properly defined
}

void UniformRandomInt::SetMinMax(uint64_t minVal, uint64_t maxVal){
	UniformIntDistParameter dist; dist.min = minVal; dist.max = maxVal ;
	implPtr->m_internal_state_ptr->SetDistributionParameters(dist);

	if(implPtr->m_distribution_ptr != NULL) delete implPtr->m_distribution_ptr;
	implPtr->m_distribution_ptr = new Tuscarora_UniformIntDist(dist.min, dist.max);
}
void UniformRandomInt::SetRange(uint64_t mean, uint64_t range){
	uint64_t c_min = mean-range;
	uint64_t c_max = mean+range;
	SetMinMax(c_min,c_max);
}

uint64_t UniformRandomInt::GetNext(){
	return ((*implPtr->m_distribution_ptr)(*implPtr->m_engine_ptr));
}

const UniformRandomInt::State UniformRandomInt::GetState(){

	//implPtr->m_internal_state_ptr->SetEngineStateBuffer(*implPtr->m_engine_ptr);

	State state;
	state = *(implPtr->m_internal_state_ptr);


	return state;
}

void UniformRandomInt::SetState(const State& state){
	*(implPtr->m_internal_state_ptr) = state;

	state.GetEngineState(*implPtr->m_engine_ptr);

	//	UniformDoubleRVDistributionParametersType dist = state.GetDistributionParameters();
	//	SetRange(state.GetDistributionParameters().min, state.GetDistributionParameters().max);
	if(implPtr->m_distribution_ptr != NULL) delete implPtr->m_distribution_ptr;
	implPtr->m_distribution_ptr = new Tuscarora_UniformIntDist(state.GetDistributionParameters().min, state.GetDistributionParameters().max);
}

UniformRandomInt::~UniformRandomInt(){
	if(implPtr->m_engine_ptr != NULL) delete implPtr->m_engine_ptr;
	if(implPtr->m_distribution_ptr != NULL) delete implPtr->m_distribution_ptr;
}



}

