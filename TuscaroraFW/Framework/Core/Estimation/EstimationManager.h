////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef ESTIMATION_MANAGER_H_
#define ESTIMATION_MANAGER_H_

#include <Interfaces/Core/LinkEstimatorI.h>

namespace Core {
namespace Estimation{

  class EstimationManager: public EstimationManagerI {
  public:
    bool RegisterCustomizer (LinkEstimatorI* est);
    bool UpdatePatternNegihborhood ();

  };

}
}

#endif /* ESTIMATION_MANAGER_H_ */
