////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef CONFIGURE_FRAMEWORK_I
#define CONFIGURE_FRAMEWORK_I

#include "LinkEstimatorI.h"

using namespace Core::Estimation;

namespace Core {

class ConfigureFramework_I {

public:
  
  void RegisterLinkEstimator(LinkEstimatorI<uint64_t> &est, EstimatorTypeE type, WaveformId_t wid);
  virtual ~ConfigureFramework_I() {}
  
};

} //End of namespace


#endif //CONFIGURE_FRAMEWORK_I