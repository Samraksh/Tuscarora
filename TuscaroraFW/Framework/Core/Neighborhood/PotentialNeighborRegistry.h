////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef POTENTIAL_NEIGHBOR_REGISTRY_H
#define POTENTIAL_NEIGHBOR_REGISTRY_H

#include <Interfaces/Core/PotentialNeighborRegistryI.h>
#include <Lib/DS/BSTMapT.h>
#include <Interfaces/Core/EstimationLogI.h>
//#include "../../../Platform/linux-multiprocess/Framework/EstimationLogging.h"

namespace Core {
  namespace Discovery {
    typedef BSTMapT<LinkId, PotentialNeighbor> PotentialNeighbor_t;
    
    class PotentialNeighborRegistry : public PotentialNeighborRegistryI {
    private:
      PotentialNeighbor_t nbrs;
      Estimation::EstimationLogI *logger;

    public: 
      PotentialNeighborRegistry();
      uint16_t GetNumberOfNodes() ;
      uint16_t GetPotentialNeighbors (UFixed_7_8_t threshPotential, PotentialNeighbor *array, uint16_t sizeOfArray);
      PotentialNeighbor* AddNode(LinkId id, PotentialNeighborTypeE type, uint8_t rssi, UFixed_7_8_t potential) ;
      PotentialNeighbor* GetNode(LinkId id) ;
      bool UpdateNode(LinkId id, PotentialNeighbor _info);
      bool RemoveNode(LinkId id) ;
    };
  } //End of namespace
}

#endif //POTENTIAL_NEIGHBOR_REGISTRY
