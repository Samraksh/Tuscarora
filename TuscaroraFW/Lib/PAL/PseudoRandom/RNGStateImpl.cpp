////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#include "RNGStateImpl.h"

namespace PAL {

RNGStateImpl::RNGStateImpl() {
	// TODO Auto-generated constructor stub
}

RNGStateImpl::~RNGStateImpl() {
	// TODO Auto-generated destructor stub
}


//void RNGStateImpl::SetEngineStateBuffer(const Tuscarora_RNG2& temp_engine) {
//	std::stringbuf engine_state_buffer;
//	std::iostream str_buf_stream(&engine_state_buffer);
//	str_buf_stream<<(temp_engine);
//
//	//std::string engine_state_string2;
//	//std::stringbuf engine_state_buffer2;
//	//std::iostream str_buf_stream2(&engine_state_buffer2);
//	//str_buf_stream2<<(temp_engine);
//
//	engine_state_string = engine_state_buffer.str();
//}


void RNGStateImpl::GetEngineState(Tuscarora_RNG2& temp_engine) const {
//	std::string::size_type sz;
//	unsigned long long internalseed = std::stoull (engine_state_string, &sz);
//	temp_engine.seed(internalseed);


	std::initializer_list<uint64_t> mylist = {(uint64_t)(RngSeedManager::GetSeed()), cmrgState.stream, cmrgState.run};
	std::seed_seq seed1(mylist);
	temp_engine.seed(seed1);

	if(num_drawals > 0) temp_engine.discard(num_drawals);

}

//template<typename DataOutType>
//void RNGStateImpl::InitializeEngineState(RNGState<DataOutType> rngstateptr){
//	//initialize engine_state_string
//	std::initializer_list<uint64_t> mylist = {(uint64_t)(RngSeedManager::GetSeed()), cmrgState.stream, cmrgState.run};
//	std::seed_seq seed1(mylist);
//	Tuscarora_RNG temp_engine(seed1);
//	rngstateptr->SetEngineStateBuffer(temp_engine);
//}

} /* namespace PAL */
