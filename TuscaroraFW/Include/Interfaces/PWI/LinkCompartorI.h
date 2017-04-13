////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef LINK_COMPARATOR_I_H_
#define LINK_COMPARATOR_I_H_

#include <Base/BasicTypes.h>
#include <Base/FrameworkTypes.h>
#include <Interfaces/Core/Node.h>
#include <Interfaces/Core/Link.h>

using namespace PAL;

///Patern Writers Interface definition and types
namespace PWI {
  
///Types and APIs for the Neighborhood service
namespace Neighborhood {
  
  ///Approved list of link comparators
  enum LinkComparatorTypeE {
    QUALITY_LC,
    COST_LC,
    QUAL_XMIT_DELAY_LC,
	ENERGY_LC,
  };
  
  class LinkComparatorI {
    ///Returns true if A is better than B, false otherwise
  public:
    virtual bool BetterThan (Core::LinkMetrics& A, Core::LinkMetrics& B) = 0;
    virtual ~LinkComparatorI() {}
  };
  
} //End of namespace
}

#endif //LINK_COMPARATOR_I_H_
