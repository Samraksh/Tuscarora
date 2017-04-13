////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include "PotentialNeighborRegistry.h"



namespace Core {
namespace Discovery {
    
  uint16_t PotentialNeighborRegistry::GetNumberOfNodes() {
    return nbrs.size();
  }

  //TODO: check the registry and return the nodes above the threshold potential
  uint16_t PotentialNeighborRegistry::GetPotentialNeighbors (UFixed_7_8_t threshPotential, PotentialNeighbor *array, uint16_t sizeOfArray) {
    return 0;
  }

  PotentialNeighbor* PotentialNeighborRegistry::AddNode(LinkId id, PotentialNeighborTypeE type, uint8_t rssi, UFixed_7_8_t potential) {
    PotentialNeighbor_t::iterator it = nbrs.find(id);
    if(it!=nbrs.end()) 
      return 0;

    nbrs[id].linkId = id;
    nbrs[id].nodeType = type;
    nbrs[id].normalizedRssi = rssi;
    nbrs[id].potential = potential;
    
    return GetNode(id);
  }

  PotentialNeighbor* PotentialNeighborRegistry::GetNode(LinkId id) {
    PotentialNeighbor_t::iterator it = nbrs.find(id);
    if(it!=nbrs.end()) 
      return &(it->second);
    return 0;
  }

  bool PotentialNeighborRegistry::UpdateNode(LinkId id, PotentialNeighbor _info) {
    PotentialNeighbor_t::iterator it = nbrs.find(id);
    if(it==nbrs.end()) 
      return 0;
    nbrs[id].nodeType = _info.nodeType;
    nbrs[id].normalizedRssi = _info.normalizedRssi;
    nbrs[id].potential = _info.potential;
    nbrs[id].otherInfo = _info.otherInfo;
    return 1;
  }

  bool PotentialNeighborRegistry::RemoveNode(LinkId id) {
    PotentialNeighbor_t::iterator it = nbrs.find(id);
    if(it==nbrs.end()) 
      return 0;
    nbrs.erase(id);
    
    return 1;
  }
    
} //End of namespace
}
