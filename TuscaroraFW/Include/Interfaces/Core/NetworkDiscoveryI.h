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
#include <Interfaces/Core/MessageT.h>
#include "Node.h"
#include "Link.h"
//#include <Interfaces/Core/PatternNamingI.h>

using namespace Types;
using namespace Core;

namespace Core {
  
///Types and classes related to the network discovery service
namespace Discovery{

enum NetworkDiscoveryTypeE {
	GLOBAL_DISCOVERY,
	ORACLE_DISCOVERY,
	LONG_LINK_DISCOVERY,
	ZSC_DISCOVERY,
	FLOOD_DISCOVERY,
	NULL_DISCOVERY
};

class DiscoveryLogHeader {
 public:
  uint32_t nodeid;
  uint32_t beaconPeriod;
  uint8_t inactivityPeriod;
  uint16_t range;
  uint8_t linkEstimationType;
  uint8_t discoveryType;
  uint16_t numNodes;
  uint16_t runTime;
  uint8_t density;
  uint8_t mobilityModel;
  uint16_t speed;
};



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
	virtual ~NetworkDiscoveryI() {};

    ///Registers the change delegates with the discovery service. The delegate callback occurs when a network discovery event happens.
    virtual bool RegisterDelegate(PotentialNeighborDelegate &del)=0;

    ///Returns the number of neighbors associated with a particular waveform.
    virtual uint16_t NetworkSize (uint16_t waveformId)=0;

    ///Returns the number of potential (local) neighbors for a given node.
    virtual uint16_t NumberOfPotentialNeighbors (NodeId_t node)=0;

    ///Copies potential neighbors information for a given node into the array pointed to by parameter 'nodeInfoArray' and returns the number of nodes which where copied.
    ///The 'nodeInfoArray' parameter should be atleast of size 'arraySize' (parameter 3)
    virtual uint16_t PotentialNeighborsList (NodeId_t node, NodeInfo *nodeInfoArray, uint16_t arraySize)=0;

    virtual void Start()=0;
    virtual void ProcessMessage(FMessage_t& msg)=0;
  };


} //End namespace
}


#endif /* NETWORKDISCOVERYI_H_ */
