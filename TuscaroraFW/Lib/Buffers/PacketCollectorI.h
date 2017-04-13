////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef PACKETCOLLECTORI_H_
#define PACKETCOLLECTORI_H_

#include <Base/FrameworkTypes.h>

namespace Patterns {
namespace Buffers {

class PacketCollectorI{
public:
  virtual FMessage_t& GetFirstPacket(uint8_t radioInterface) =0;
  //virtual FMessage_t& GetPacketForNode(uint8_t radioInterface, uint16_t dst) = 0;
  //virtual uint16_t GetNumberPendingPackets(uint8_t radioInterface, uint16_t dest) =0;
  //virtual uint32_t GetNumberPendingBytes(uint8_t radioInterface, uint16_t dest) =0;
  virtual ~PacketCollectorI(){}
};

}
}

#endif /* PACKETCOLLECTORI_H_ */
