////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#include "UniformRandomIntImpl.h"

namespace PAL {

UniformRandomIntImpl::UniformRandomIntImpl() {
	// TODO Auto-generated constructor stub
	m_internal_state_ptr = NULL;
	m_distribution_ptr = NULL;
	m_engine_ptr = NULL;
}

UniformRandomIntImpl::~UniformRandomIntImpl() {
	// TODO Auto-generated destructor stub
}

} /* namespace PAL */
