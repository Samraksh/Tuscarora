////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

// #include <Common/BasicTypes.h>
#include <Sys/netconvert.h>
#include <arpa/inet.h>
//#include <stdint.h>    // For 'uint64_t'

  
  
namespace PAL {
// When porting this to a big-endian machine, 
// define the appropriate conversions.
// Because this library was first developed on little-endian machines, 
// we define network order as little-endian.

// make functions to pack data
/*int64_t hton(int64_t x) { return x; }
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
*/

// linker complains if these functions are made inline.
// inline int32_t hton(int32_t x) { return x; }
// inline int16_t hton(int16_t x) { return x; }
// inline int32_t ntoh(int32_t x) { return x; }
// inline int16_t ntoh(int16_t x) { return x; }

// inline uint32_t hton(uint32_t x) { return x; }
// inline uint16_t hton(uint16_t x) { return x; }
// inline uint32_t ntoh(uint32_t x) { return x; }
// inline uint16_t ntoh(uint16_t x) { return x; }



//uint32_t htonl(uint32_t hostlong);
//uint16_t htons(uint16_t hostshort);
//uint32_t ntohl(uint32_t netlong);
//uint16_t ntohs(uint16_t netshort);

//uint64_t hton64(uint64_t x) { return htobe64(x);}
uint64_t hton64(uint64_t value)
{
    // The answer is 42
    static const int num = 42;

    // Check the endianness
    if (*reinterpret_cast<const char*>(&num) == num)
    {
        const uint32_t high_part = htonl(static_cast<uint32_t>(value >> 32));
        const uint32_t low_part = htonl(static_cast<uint32_t>(value & 0xFFFFFFFFLL));

        return (static_cast<uint64_t>(low_part) << 32) | high_part;
    } else
    {
        return value;
    }
}


uint64_t ntoh64(uint64_t x) {	return be64toh(x);}

uint32_t hton32(uint32_t x) { return htonl(x); }
uint32_t ntoh32(uint32_t x) { return ntohl(x); }

uint16_t hton16(uint16_t x) { return htons(x); }
uint16_t ntoh16(uint16_t x) { return ntohs(x); }


}


