////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef NEIGHBOR_TABLE_I_H
#define NEIGHBOR_TABLE_I_H

#include <Base/BasicTypes.h>
#include <Base/FrameworkTypes.h>
#include <Interfaces/Core/Node.h>

namespace Core { 
  
 class NeighborTableI{
  
  public:
    ///Returns the number of nodes currently reachable that satisfies the Patterns neighbor criteria.
    virtual uint16_t GetNumberOfNeighbors ()=0;
    
    ///Returns a number (specified by the first parameter) of best neighbor sorted by quality, in the array specified by the second parameter.
    virtual uint16_t GetBestQualityNeighbors (uint16_t numNeighbors, NodeId_t *nodeIds)=0;
    
    ///Returns a number(specified by the first parameter) of neighbors of least expected transmit latency, in the array specified by the second parameter.
    virtual uint16_t GetLeastLatencyNeighbors (uint16_t numNeighbors,  NodeId_t *nodeIds)=0;
    
    ///< Returns the NeighborInfo object for the specified node id.
    virtual NodeInfo* GetNeighborInfo(NodeId_t nodeId);
    
    ///Given an index in the table returns the NodeId of the corresponding node
    virtual NodeId_t GetNeighborID(uint16_t table_index);
    
    ///Adds a new neighbor to the neighbor table, if the node already exist, the information is updated
    ///Used to change the type of neighbor type or add potential neighbor for who link metrics are not available.
    virtual int32_t AddNeighbor(NodeId_t id, NodeTypeE type);
    
    /// Updates info about nbr, if node does not exist, it is created
    virtual int32_t UpdateNeighbor(NodeId_t id, NodeInfo _info);
    
    ///< Removes a neighbor from the neighbor table. Used by the Network Discovery module.
    virtual void RemoveNeighbor(NodeId_t id);
    
    ///Returns the number of packets pending for the neighbor
    virtual uint16_t GetPendingPackets(NodeId_t neighbor);
    
  };
} //End namespace

#endif //NEIGHBOR_TABLE_I_H