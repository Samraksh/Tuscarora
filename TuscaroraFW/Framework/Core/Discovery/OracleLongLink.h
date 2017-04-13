////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef ORACLE_LONG_LINK_H_
#define ORACLE_LONG_LINK_H_

#include "NetworkDiscovery.h"
#include "Framework/Core/Neighborhood/NeighborTable.h"

namespace Core {
namespace Discovery {

class OracleLongLink : public NetworkDiscovery{
  private:
    Timer *longTimer;
    void UpdateNeighborList(uint32_t);///< Value of parameter does not matter, updates the list of potential neighbors in the neighbor table
  public:
    ~OracleLongLink() {}
    void Start();///< Begins the neighbor discovery module, starts the timer to call UpdateNeighborList every 10 seconds
    void ProcessMessage(FMessage_t& msg) {}
    void AddLink(NodeId_t node, WaveformId_t waveform, Header* header, double dx, double dy);
    void RemoveLink(NodeId_t node, WaveformId_t waveform);
};

}
}
#endif
