////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef POLICYMANAGERI_H_
#define POLICYMANAGERI_H_

#include <Include/Base/BasicTypes.h>
#include <Include/Interfaces/Core/Node.h>
#include <Include/Interfaces/Core/PiggyBackingI.h>

using namespace Types;
using namespace PAL;
using namespace Core;

namespace Core {
  
///Types and classes related to Policy Manager service
namespace Policy {

///types of packet scheduling currently 
enum PacketSchedulingPolicyE {
  PRIORITY_FCFS,
  FCFS,
};
  
enum LinkSelectionPolicyE{
  PATTERN_DEFINED, //The default policy. Each pattern selects its own policy based on its own comparator
  BEST_QUALITY,
  LEAST_DELAY,
  LEAST_ENERGY,
  PREFFERED_WAVEFORM,
};



///Defines the interface for the policy manager  
class PolicyManagerI {
	
public:
    /// Set the policy for the framework packet scheduler. The default is FCFS.
    bool SetPacketPolicy (PacketSchedulingPolicyE _policy);
    
    /// Can set indivitual pattern priorities, if the policy is PriorityFCFS. Has no effect if PacketPolicy is FCFS.
    bool SetAsnpPriority (uint16_t patternInstanceId, uint8_t priority);
    
    /// Get the priority of a pattern instance.
    uint16_t GetAsnpPriority (uint16_t asnpInstanceId);
    
    /// Add a node not discovered by framework, like special nodes
    bool AddNode (NodeId_t id, NodeInfo info);
    
    /// Remove a special nodes from network
    bool RemoveNode (NodeId_t id);
    
    /// Add radio silence noes to network, the 3 and 4 parameter specify the lattitude and longitude of a GPS location and 5 paramerter specify the radius around the GPS location, where the 
    /// node should be added to the network. Any node which is inside this circle will see this radio silence node in its neighbor table
    bool AddRadioSilenceNode (NodeId_t id, NodeInfo info, double lat, double long, uint64_t radius);
    
    ///Blacks lists the nodes passed as parameters. The black listed nodes will be removed from neighborhood even if they are detected. 
    bool SetBlackList(NodeId_t *nodeArray, uint16_t noOfNodes);
    
    ///Clears the current black-listed nodes
    bool ClearBlackList();  
	
	/// Returns the status of piggy backing in the framework
	bool IsPiggyBackingEnabled();
	
	/// Returns the maximum payload size that the framework accepts
	uint16_t GetMaxFWPacketPayloadSize();

	bool IsValidPiggyBackee (PB_TypeE type, Waveform::WF_AddressTypeE addressType, uint8_t dataSize, const std::string &authString);
};

  
}//End of namespace
}

#endif /* POLICYMANAGERI_H_ */
