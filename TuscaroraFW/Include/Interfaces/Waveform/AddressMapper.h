////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef ADDRESS_MAPPER_H
#define ADDRESS_MAPPER_H

#include <Base/FrameworkTypes.h>
//#include <Interfaces/Waveform/WF_Types.h>

using namespace Types;


namespace Waveform{

//empty class to use as base class
class AddressMapper {};
  
template <class WfAddressType>
class WaveformAddressMapper : public AddressMapper {
  WaveformId_t wid;
  
public:
  static WfAddressType GetWaveformAdress (NodeId_t id);
  static NodeId_t GetFrameworkId (WfAddressType addr);
  
};

template <>
class WaveformAddressMapper<uint64_t> {
  WaveformId_t wid;
  
public:
  static uint64_t GetWaveformAdress (NodeId_t id){
    uint64_t wid = (uint64_t) (id);
    return wid;
  }
  static NodeId_t GetFrameworkId (uint64_t addr){
    NodeId_t id = (uint16_t) (addr & 0x000000000000FFFF);
    return id;
  }
};

} //End of namespace

#endif //ADDRESS_MAP_H