////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 


#ifndef NEIGHBOR_TABLE
#define NEIGHBOR_TABLE

#include <Interfaces/PWI/PatternNeighborTableI.h>
#include <Interfaces/Core/LinkEstimatorI.h>
#include "Framework/Core/Discovery/NodeSchedule.h"
#include "Lib/DS/BSTMapT.h"

extern const uint16_t MAX_NBR;

using namespace Core::Discovery;
//using namespace Core::Estimation;
using namespace PWI::Neighborhood;



namespace Core {
namespace Neighborhood {
  typedef BSTMapT<LinkId, Link*> Neighbors_t;
  

  class NeighborTable{
    Neighbors_t  nbrs;
  public:
    uint16_t Size();///< Returns the number of nodes in the neighbor table
    //uint16_t GetNumGoodNeighbors();///< Returns the number of valid links
    uint16_t GetNeighborsIds(NodeId_t *nodeIds);///< Populates nodeIds with a list of valid node ids, returns the number of good neighbors
    Link* GetNeighborLink(NodeId_t nodeId);///< Returns the NeighborInfo object for the specified node id.
    Link* GetLinkwComp(NodeId_t nodeId, LinkComparatorI &comparator); //compares the metrics of existing link for the node and returns the link with best one according to the comparator
    Link* GetNeighborLink(NodeId_t nodeId, WaveformId_t wid);///< Returns the NeighborInfo object for the specified node id.
    uint16_t GetNeighborLinkSet(Link **_links);
	uint16_t GetNeighborLinkSet(Link **_links, WaveformId_t wid);
    NodeId_t GetNeighborId(uint16_t table_index);
    ///Adds a new neighbor to the neighbor table, if the node already exist, the information is updated
    ///Used to change the type of neighbor type or add potential neighbor for who link metrics are not available.
    Link* AddNeighbor(NodeId_t id, WaveformId_t wid, LinkMetrics _metrics);
    
    bool UpdateNeighbor(NodeId_t id,  WaveformId_t wid, LinkMetrics _metrics);
    bool TouchNeighbor(NodeId_t id,  WaveformId_t wid);
    
    bool LinkExists(NodeId_t id);

    ///< Checks the neighbor table for links which are expired. Marks the Neighbor as a PotentialNeighbor if the expiry time is < current time. It only removes a single link each pass, returning the id of the removed link. 
    ///To insure all invalid links are removed in a timely fashion, this should be called in a loop until the return value is -1.
    int32_t AgeNeighbors(uint32_t maxAge);
    
    ///< Removes a neighbor from the neighbor table. Used by the Network Discovery module.
    void RemoveNeighbor(LinkId id);
    
    ///
    uint16_t GetPendingPackets(uint16_t neighbor);
	
	uint16_t SizeOnWF(WaveformId_t wid);
	
	double CalculateAvgDelay (double oldDelayMicroLog, double updateDelayMicro);
	bool UpdateTransmissionDelay(WaveformId_t wid, NodeId_t nid, double delayMicro);
	bool UpdateTransmissionDelay(WaveformId_t wid, double delayMicro);
  };
}
}

#endif //NEIGHBOR_TABLE
