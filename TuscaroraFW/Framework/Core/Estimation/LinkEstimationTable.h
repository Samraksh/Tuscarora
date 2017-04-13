////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 


#ifndef LINK_ESTIMATION_TABLE
#define LINK_ESTIMATION_TABLE

#include <Interfaces/Core/LinkEstimatorI.h>
#include "Framework/Core/Discovery/NodeSchedule.h"
#include <map>
#include "EstimationLogging.h"

extern const uint16_t MAX_NBR;

using namespace Core::Discovery;
using namespace Core::Estimation;

namespace Core {
namespace Estimation {

template <class WFAddressType>
struct EstimationInfo {
public:
	//NodeId_t nodeId;
	WFAddressType linkAddress;
	NodeSchedule *nodeSchd;
	Link *link;
	LinkMetrics *metric;
	uint64_t expiryTime;
};


template <class WFAddressType>
using  Estimation_t = std::map<WF_LinkId<WFAddressType>, EstimationInfo<WFAddressType> >;


template <class WFAddressType>
class LinkEstimationTable{
	
	Estimation_t<WFAddressType>*  nbrs;
	EstimationLogging logger;
public:
	LinkEstimationTable();
	EstimationInfo<WFAddressType>* GetEstimationInfo(WFAddressType linkAddress, WaveformId_t wfId);///< Returns the NeighborInfo object for the specified node id.

	///Adds a new neighbor to the neighbor table, if the node already exist, the information is updated
	///Used to change the type of neighbor type or add potential neighbor for who link metrics are not available.
	EstimationInfo<WFAddressType>* AddNeighbor(WFAddressType id, WaveformId_t wfId);
	void SetLink(WFAddressType nodeId, WaveformId_t wfId, Link *link);
	void UpdateExpiration(WFAddressType id, WaveformId_t wfId, uint64_t expiry);
	void RemoveNeighbor(WFAddressType id, WaveformId_t wfId);

	int32_t CheckExpiration(Delegate<void, WF_LinkEstimationParam<WFAddressType> >  &del);
};


typedef LinkEstimationTable<uint64_t> LinkEstimationTable_n64_t;
typedef LinkEstimationTable<uint32_t> LinkEstimationTable_n32_t;
typedef LinkEstimationTable<NodeId_t> LinkEstimationTable_NodeId_t;
	
}	//End namespace
}

#endif //LINK_ESTIMATION_TABLE
