////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef PATTERN_IDS_H
#define PATTERN_IDS_H

#include <Interfaces/Core/PatternNamingI.h>
using namespace Core::Naming;

namespace Core {
namespace Naming {
  
///Enum for the pattern instance Id, this will go away. Currently one instance of each pattern is supported.
PatternTypeE PatternsSupported[] = {
  COP_PTN,
  COP_PTN,
  CENSUS_PTN, 
  CENSUS_PTN,
  FWP_PTN,		//FloodingWithPruning
  FWP_PTN,
  SHARE_PTN,
  SHARE_PTN,
  GOSSIP_PTN,	//Gossip
  GOSSIP_PTN,
  NON_STD_PTN,
  NON_STD_PTN,
  NON_STD_PTN,
  NON_STD_PTN,
  NON_STD_PTN,
  NON_STD_PTN
};

} //End of namespace
} 
#endif //PATTERN_IDS_H