////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <Interfaces/Core/NetworkDiscoveryI.h>
#include "Framework/Core/Discovery/NetworkDiscovery.h"
#include "Framework/Core/Neighborhood/NeighborTable.h"

namespace Core {
namespace Discovery {

class Global : public NetworkDiscovery{
 public:
    ~Global() {}
    void Start();
    void ProcessMessage(FMessage_t& msg) {}
    void AddLink(NodeId_t node, WaveformId_t waveform);
};

}
}
#endif
