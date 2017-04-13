////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include <PAL/PseudoRandom/rng-state.h>
#include <PAL/PseudoRandom/UniformRandomInt.h>
#include <PAL/PseudoRandom/UniformRandomValue.h>


#include <cstdint>
#include <sstream>
#include <string>

#include "RNStreamID.h"
#include "RNGStateImpl.h"

#include "RandomCommon.h"


namespace PAL {


template<typename DistributionParametersType>
RNGState<DistributionParametersType>::RNGState(){
	implPtr = new RNGStateImpl();
	//Initialize RNStreamID
	implPtr->cmrgState.stream =  RngSeedManager::GetNextStreamIndex ();
	implPtr->cmrgState.run = RngSeedManager::GetRun();

	//initialize engine_state_string
	implPtr->InitializeEngineState();

	//Initialize the rest
//	min = 0;
//	max = 1;
	//implPtr->dummyinteger = 5;
}

template<typename DistributionParametersType>
RNGState<DistributionParametersType>::RNGState(const RNGState& st ){
	implPtr = new RNGStateImpl();
	//Initialize RNStreamID
	implPtr->cmrgState = st.implPtr->cmrgState;


//		std::stringbuf engine_state_buffer;
//		std::ostream os1 (&engine_state_buffer);
//		os1 << st.engine_state_buffer.str();
//		engine_state_string = engine_state_buffer.str();
	//engine_state_buffer.str(st.engine_state_buffer.str());
	//implPtr->engine_state_string = st.implPtr->engine_state_string;
	implPtr->num_drawals = st.implPtr->num_drawals;

	//Initialize the rest
//	min = st.min;
//	max = st.max;
}

template<typename DistributionParametersType>
RNGState<DistributionParametersType>::RNGState(const RNStreamID& i_s){

	implPtr = new RNGStateImpl();
	implPtr->cmrgState = i_s;
	//initialize engine_state_string
	implPtr->InitializeEngineState();

	//Initialize the rest
//	min = i_min;
//	max = i_max;
}

template<typename DistributionParametersType>
void RNGState<DistributionParametersType>::SetDistributionParameters(const DistributionParametersType& i_distribution){
	m_distribution = i_distribution;
}

template<typename DistributionParametersType>
DistributionParametersType RNGState<DistributionParametersType>::GetDistributionParameters() const{
	return m_distribution;
}

template<typename DistributionParametersType>
RNGState<DistributionParametersType>& RNGState<DistributionParametersType>::operator=(const RNGState<DistributionParametersType>& st){
		implPtr->cmrgState = (st.implPtr->cmrgState);
		//implPtr->engine_state_string = st.implPtr->engine_state_string;
		implPtr->num_drawals = st.implPtr->num_drawals;

//		min = st.min;
//		max = st.max;
		return (*this);
}


template<typename DistributionParametersType>
void RNGState<DistributionParametersType>::SetRNStreamID(RNStreamID _i){
	implPtr->cmrgState = _i;
	implPtr->InitializeEngineState();
}

template<typename DistributionParametersType>
RNStreamID RNGState<DistributionParametersType>::GetRNStreamID() const{
	return implPtr->cmrgState;
}


//template<typename DistributionParametersType>
//template<typename TemplatedRNGClass>
//void RNGState<DistributionParametersType>::SetEngineStateBuffer(const TemplatedRNGClass& temp_engine) {
//	printf("!!!!!!!!!!!!!!SetEngineStateBuffer Wrong Template Instantiation!!!!!!!!!!1!! ");
//	//SetEngineStateBufferOutside<TemplatedRNGClass>(temp_engine, implPtr);
////	std::stringbuf engine_state_buffer;
////	std::iostream str_buf_stream(&engine_state_buffer);
////	str_buf_stream<<(temp_engine);
////
////	std::string engine_state_string2;
////	std::stringbuf engine_state_buffer2;
////	std::iostream str_buf_stream2(&engine_state_buffer2);
////	str_buf_stream2<<(temp_engine);
////
////	implPtr->engine_state_string = engine_state_buffer.str();
////	engine_state_string2 = engine_state_buffer2.str();
//}
//
//template<> template<> void RNGState<UniformIntDistParameter>::SetEngineStateBuffer(const Tuscarora_RNG2& temp_engine) {
//	std::stringbuf engine_state_buffer;
//	std::iostream str_buf_stream(&engine_state_buffer);
//	str_buf_stream<<(temp_engine);
//
//	//std::string engine_state_string2;
//	//std::stringbuf engine_state_buffer2;
//	//std::iostream str_buf_stream2(&engine_state_buffer2);
//	//str_buf_stream2<<(temp_engine);
//
//	implPtr->engine_state_string = engine_state_buffer.str();
//}
//template<> template<> void RNGState<UniformDoubleRVDistributionParametersType>::SetEngineStateBuffer(const Tuscarora_RNG2& temp_engine) {
//	std::stringbuf engine_state_buffer;
//	std::iostream str_buf_stream(&engine_state_buffer);
//	str_buf_stream<<(temp_engine);
//
//	//std::string engine_state_string2;
//	//std::stringbuf engine_state_buffer2;
//	//std::iostream str_buf_stream2(&engine_state_buffer2);
//	//str_buf_stream2<<(temp_engine);
//
//	implPtr->engine_state_string = engine_state_buffer.str();
//}
//
template<typename DistributionParametersType>
template<typename TemplatedRNGClass>
void RNGState<DistributionParametersType>::GetEngineState(TemplatedRNGClass& temp_engine) const{
	printf("!!!!!!!!!!!!!!SetEngineStateBuffer Wrong Template Instantiation!!!!!!!!!!1!! ");
}

template<> template<> void RNGState<UniformIntDistParameter>::GetEngineState(Tuscarora_RNG2& temp_engine) const{
//	std::string::size_type sz;
//	if(!implPtr->engine_state_string.empty()){
//		unsigned long long internalseed = std::stoull (implPtr->engine_state_string, &sz);
//		temp_engine.seed(internalseed);
//	}
//	else {
//		printf("Something wrong with Random Generator Engine; RNG's cant be duplicated\n");
//	}
	implPtr->GetEngineState(temp_engine);
}
template<> template<> void RNGState<UniformDoubleRVDistributionParametersType>::GetEngineState(Tuscarora_RNG2& temp_engine) const{
//	std::string::size_type sz;
//	unsigned long long internalseed = std::stoull (implPtr->engine_state_string, &sz);
//	temp_engine.seed(internalseed);
	implPtr->GetEngineState(temp_engine);
}





//Template "Explicit Instantiation"
template class RNGState<UniformIntDistParameter>;
template class RNGState<UniformDoubleRVDistributionParametersType>;


}
