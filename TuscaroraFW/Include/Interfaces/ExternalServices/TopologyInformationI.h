////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef TOPOLOGY_INFORMATION_I_H_
#define TOPOLOGY_INFORMATION_I_H_

#include <Base/BasicTypes.h>
#include <Interfaces/Core/NetworkDiscoveryI.h>

using namespace PAL;
using namespace Core::Discovery;

namespace ExternalServices {
  
  ///Defines an interface for the topology information service. In our current thinking, this is just another Network Discovery service, hence has the same APIs.
  class TopologyInformationI : public NetworkDiscoveryI {
  public:
    
  };

  
  
} //End of namespace

#endif //TOPOLOGY_INFORMATION_I_H_