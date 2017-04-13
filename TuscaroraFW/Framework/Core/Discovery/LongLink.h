////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef LONG_LINK_H_
#define LONG_LINK_H_

#include <math.h>
#include "NetworkDiscovery.h"
#include "Framework/Core/Neighborhood/NeighborTable.h"
#include <Interfaces/PWI/Framework_I.h>
#include "Framework/PWI/FrameworkBase.h"
#include "Lib/PAL/PAL_Lib.h"
//#include "Lib/Math/UniformRandomValue.h"
#include <PAL/PseudoRandom/UniformRandomValue.h>


#if defined(PLATFORM_LINUX)
#include "Platform/dce/ExternalServices/LocationService/LocationService.h"
#elif defined(PLATFORM_DCE_DC)
#include "Platform/dce/ExternalServices/LocationService/LocationService.h"
#elif defined(PLATFORM_DCE)
#include "Platform/dce/ExternalServices/LocationService/LocationService.h"
#elif defined(PLATFORM_ANDROID)
#endif

namespace Core {
namespace Discovery {

  struct LongLinkData {
    uint64_t beacon;
    int hops;
  };
  
class LongLink : public NetworkDiscovery{
  private:
    UniformRandomValue *rndDouble;
    Timer *longTimer;
    void SendLongBeacon(uint32_t);
    Framework_I *fi;
    ExternalServices::Location2D* locations;
    int numNodes;
    bool isBase;
    uint64_t beacon;
    int hops;
    uint16_t range;
  public:
    ~LongLink() {}
 LongLink(Framework_I *parent, void* optional=0) : fi(parent) { if(optional != 0) {LongLinkData* data = (LongLinkData*)optional; beacon = data->beacon; hops = data->hops;} else { beacon = 10000000; hops=2;} }///< Creates the neighbor discovery module
    void Start();///< Begins the neighbor discovery module, starts the timer to send the long link beacons
    void ProcessMessage(FMessage_t& msg);
};

}
}
#endif
