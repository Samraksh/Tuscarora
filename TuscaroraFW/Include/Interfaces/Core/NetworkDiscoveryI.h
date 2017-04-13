////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef NETWORKDISCOVERYI_H_
#define NETWORKDISCOVERYI_H_


#include <Base/BasicTypes.h>
#include <Base/FrameworkTypes.h>
#include <Base/FixedPoint.h>
#include "Node.h"
#include "Link.h"
//#include <Interfaces/Core/PatternNamingI.h>

using namespace Types;
using namespace Core;

namespace Core {
  
///Types and classes related to the network discovery service
namespace Discovery{

  ///Enum for the type of neighbor change event.
  enum PNbrUpdateTypeE {
    PNBR_INVALID,	///Previous info about tihs pnbr is not valid anymore
    PNBR_UPDATE 	///An update a potential nbr
  };
  
      ///Enum for the type of potential neighbor
  enum PotentialNeighborTypeE {
    BEYOND_COMM_PNODE,		///Node is beyond comm range currently
    ALTERNATEMODE_PNODE,	///Comms currently not possible, but may be possible with a mode change
    SCHEDULED_PNODE,		///This node has a predictable location/mobility. Comms currently not possible, but might be available for comms in future.
    HOSTILE_PNODE
  };
  
  
  struct PotentialNeighbor{
    LinkId linkId;
    PotentialNeighborTypeE nodeType;
    uint8_t normalizedRssi;
    UFixed_7_8_t potential; ///Measure of potential to become a neighbor
    void* otherInfo;
  };
  
  struct PotentialNeighborDelegateParam {
    PNbrUpdateTypeE changeType;
    PotentialNeighbor* pnbr;
  };

  ///A delegate definition which will be called to notify events about a node or link 
  typedef Delegate<void, PotentialNeighborDelegateParam> PotentialNeighborDelegate;
  
  ///Inteface definition for a network discovery service.
  class NetworkDiscoveryI {

  public:
    ///Registers the change delegates with the discovery service. The delegate callback occurs when a network discovery event happens.
    bool RegisterDelegate(PotentialNeighborDelegate &del);

    ///Returns the number of neighbors associated with a particular waveform.
    uint16_t NetworkSize (uint16_t waveformId);

    ///Returns the number of potential (local) neighbors for a given node.
    uint16_t NumberOfPotentialNeighbors (NodeId_t node);

    ///Copies potential neighbors information for a given node into the array pointed to by parameter 'nodeInfoArray' and returns the number of nodes which where copied.
    ///The 'nodeInfoArray' parameter should be atleast of size 'arraySize' (parameter 3)
    uint16_t PotentialNeighborsList (NodeId_t node, NodeInfo *nodeInfoArray, uint16_t arraySize);
  };


} //End namespace
}


#endif /* NETWORKDISCOVERYI_H_ */
