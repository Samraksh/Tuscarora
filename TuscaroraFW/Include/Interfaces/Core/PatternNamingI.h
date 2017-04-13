////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef NAMING_H_
#define NAMING_H_

#include <Base/BasicTypes.h>
#include <Base/FrameworkTypes.h>

using namespace Types;

namespace Core {
  
///Types and classes related to Naming service
namespace Naming {
  
///Enum for the support Pattern Types
enum PatternTypeE {
  UNKNOWN_PTN,	//Framework does not recognise this 
  COP_PTN,	//Common Operating Picture
  OLSR_PTN,  	//Event based link state routing
  CENSUS_PTN, //Census (counting)
  FWP_PTN,	//FloodingWithPruning
  RBCAST_PTN,		//ReliableBroadcast
  IWAVE_PTN,		//WaveExfiltration
  NTKR_PTN,	//Need-to-know routing
  SENTRY_PTN,	//Sentry
  ELG_PTN,	//Emergent Local group
  GOSSIP_PTN,	//Gossip like pattern 
  SHARE_PTN,	//BBN's pattern
  NON_STD_PTN, //Non standard pattern
};

///Generic interface specification for a name service
class PatternNameServiceI {
public:
  /// Returns a unique Id for use as the pattern Id, takes the pattern type as parameter.
  PatternId_t GetNewPatternInstanceId(PatternTypeE ptype,const char uniqueName[128]);
  PatternTypeE GetPatternType(PatternId_t id);

  virtual ~PatternNameServiceI(){}
};

PatternId_t GetNewPatternInstanceId(PatternTypeE ptype, const char uniqueName[128]);
  
}//End of namespace
}
#endif // NAMING_H_
