//////////////////////////////////////////////////////////////////////////////////
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved.
// Please see License.txt file in the software root directory for usage rights.
// Please see developer's manual for implementation notes.
//////////////////////////////////////////////////////////////////////////////////



#ifndef NETWORKDISCOVERY_H_
#define NETWORKDISCOVERY_H_

#include "Framework/Core/Neighborhood/NeighborTable.h"
#include <Interfaces/Core/NetworkDiscoveryI.h>
#include "../Neighborhood/PotentialNeighborRegistry.h"

extern NodeId_t MY_NODE_ID;

using namespace PAL;
using namespace PWI;

namespace Core {
namespace Discovery {

class NetworkDiscovery : public NetworkDiscoveryI{
protected:
	PotentialNeighborRegistry pNbrs; ///< Potential Neighbor Table
	PotentialNeighborDelegate *del;  ///< Delegate to be informed when nodes become a new potential neighbor and are no longer potential neighbors

public:
	virtual ~NetworkDiscovery() {};

	///Delegate registration for Network Change information.
	bool RegisterDelegate (PotentialNeighborDelegate& delegate) override { del = &delegate; return true; }

	uint16_t NetworkSize(uint16_t waveformId) override { return 0; }

	/// Returns the number of potential neighbors
	uint16_t NumberOfPotentialNeighbors(NodeId_t node) override { return pNbrs.GetNumberOfNodes(); }

	virtual uint16_t PotentialNeighborsList (NodeId_t node, NodeInfo *nodeInfoArray, uint16_t arraySize) override { return 0;}

	///Processing incoming network discovery messages
	virtual void ProcessMessage(FMessage_t& msg)=0;

	/// Begins the network discovery module
	virtual void Start()=0;
}; //end of class

}//End of namespace
}

#endif	//NETWORKDISCOVERY_H_
