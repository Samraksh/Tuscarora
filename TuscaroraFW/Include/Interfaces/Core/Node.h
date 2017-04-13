////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef NODE_H_
#define NODE_H_

#include <Base/BasicTypes.h>
#include <Base/FrameworkTypes.h>
#include "Link.h"

using namespace Types;
using namespace Core;

namespace Core {

  ///A neighbor can be one of these types.
  enum NeighborTypeE {
    REGULAR_NODE,	///Comms are possible with this node.
    RADIOSILENT_NODE,	///Special node, can only receive messages.
    C3_NODE,		///Special node, can run additional services.
  };
  
  const uint8_t RELAY_FLAG = 0x01;
  const uint8_t JAMMER_FLAG = 0x02;
  const uint8_t BLACK_LISTED_FLAG = 0x04;
  
  struct NodeInfo {
    NodeId_t nodeId;
    uint8_t flags;
    NeighborTypeE type;
    Link *linkArray;
    uint8_t numberOfLinks;
  };
  
}




#endif //NODE_H_