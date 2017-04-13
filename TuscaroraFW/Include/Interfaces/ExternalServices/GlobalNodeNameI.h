////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef GLOBAL_NODE_NAMING_I_H_
#define GLOBAL_NODE_NAMING_I_H_

#include <Include/Base/BasicTypes.h>

using namespace Types;

///Definitions and APIs for External Services. Tuscarora does not provide these services.
namespace ExternalServices {
  
  class GlobalNameI {
  public:
    uint128_t GetMyNodeName ();
    uint128_t GetWaveformName(WaveformId_t id);  
  };
  
}//End of namespace

#endif	//GLOBAL_NODE_NAMING_I_H_