////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef NETCONVERT_H_
#define NETCONVERT_H_
#include <Base/BasicTypes.h>
namespace PAL {

// make functions to pack data
uint64_t hton64(uint64_t x);
uint32_t hton32(uint32_t x);
uint16_t hton16(uint16_t x);

uint64_t ntoh64(uint64_t x);
uint32_t ntoh32(uint32_t x);
uint16_t ntoh16(uint16_t x);

}
#endif /* NETCONVERT_H_ */
