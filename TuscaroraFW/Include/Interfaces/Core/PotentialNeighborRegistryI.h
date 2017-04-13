////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef POTENTIAL_NEIGHBOR_REGISTRY_I_H
#define POTENTIAL_NEIGHBOR_REGISTRY_I_H

#include <Base/BasicTypes.h>
#include <Base/FrameworkTypes.h>
#include <Interfaces/Core/Node.h>
#include <Interfaces/Core/NetworkDiscoveryI.h>
#include <Base/FixedPoint.h>

using namespace Types;
using namespace Core::Discovery;

namespace Core { 
namespace Discovery { 
 
 class PotentialNeighborRegistryI{
  
  public:
    ///Returns the number of nodes currently reachable that satisfies the Patterns neighbor criteria.
    virtual uint16_t GetNumberOfNodes () = 0;
    
    /// , 

    /**
     * @brief Returns an array of pottential neighbors, which are above the potential specified by the _potential specified
     * 
     * @param threshPotential Threshold to be satisfied
     * @param array Array into which the potential neighbor info is copied
     * @param sizeOfArray Size of the array provided. If the number of potential neighbors satisfying the threshold is higher the size of the array, 
     * then the top sizOfArray number of nodes info is copied to the array
     * @return uint16_t
     */
    virtual uint16_t GetPotentialNeighbors (UFixed_7_8_t threshPotential, PotentialNeighbor *array, uint16_t sizeOfArray) = 0;
    
    ///Adds a new node to the registry . Discovery modules should use this to update the registry
    virtual PotentialNeighbor* AddNode( LinkId id, PotentialNeighborTypeE type, uint8_t normalizedRssi, UFixed_7_8_t potential) = 0;

    //Returns PotentialNeighbor object for provided NodeId
    virtual PotentialNeighbor* GetNode (LinkId id) = 0;
    
    /// Updates info a existing node
    virtual bool UpdateNode (LinkId id,  PotentialNeighbor _info) = 0;  
  
    /// Updates info a existing node
    virtual bool RemoveNode (LinkId id) = 0;  
    
    virtual ~PotentialNeighborRegistryI () {}
    
  };

  
} //End namespace
}

#endif //POTENTIAL_NEIGHBOR_REGISTRY_I_H
