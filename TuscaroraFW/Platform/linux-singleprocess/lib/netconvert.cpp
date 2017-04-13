////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

// #include <Common/BasicTypes.h>
#include <PAL/netconvert.h>

namespace PAL {
// When porting this to a big-endian machine, 
// define the appropriate conversions.
// Because this library was first developed on little-endian machines, 
// we define network order as little-endian.

// make functions to pack data
int64_t hton(int64_t x) { return x; }
int32_t hton(int32_t x) { return x; }
int16_t hton(int16_t x) { return x; }

int64_t ntoh(int64_t x) { return x; }
int32_t ntoh(int32_t x) { return x; }
int16_t ntoh(int16_t x) { return x; }

uint64_t hton(uint64_t x) { return x; }
uint32_t hton(uint32_t x) { return x; }
uint16_t hton(uint16_t x) { return x; }

uint64_t ntoh(uint64_t x) { return x; }
uint32_t ntoh(uint32_t x) { return x; }
uint16_t ntoh(uint16_t x) { return x; }

// linker complains if these functions are made inline.
// inline int32_t hton(int32_t x) { return x; }
// inline int16_t hton(int16_t x) { return x; }
// inline int32_t ntoh(int32_t x) { return x; }
// inline int16_t ntoh(int16_t x) { return x; }

// inline uint32_t hton(uint32_t x) { return x; }
// inline uint16_t hton(uint16_t x) { return x; }
// inline uint32_t ntoh(uint32_t x) { return x; }
// inline uint16_t ntoh(uint16_t x) { return x; }

// #include "endian.h"
// inline int32_t hton(int32_t x) { return htole32(x); }
// inline int16_t hton(int16_t x) { return htole16(x); }
// inline int32_t ntoh(int32_t x) { return le32toh(x); }
// inline int16_t ntoh(int16_t x) { return le16toh(x); }

// inline uint32_t hton(uint32_t x) { return htole32(x); }
// inline uint16_t hton(uint16_t x) { return htole16(x); }
// inline uint32_t ntoh(uint32_t x) { return le32toh(x); }
// inline uint16_t ntoh(uint16_t x) { return le16toh(x); }

}


