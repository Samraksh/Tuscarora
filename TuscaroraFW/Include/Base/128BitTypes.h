////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef T128BIT_TYPES_H_
#define T128BIT_TYPES_H_

#include <sys/types.h>
#include <stdint.h>

namespace Types {

//Implementation notes:
//Boost has an implementation of 128 bit numbers, but is quite involved. We need to decide on an implementation.
// For boost implementation look at http://www.boost.org/doc/libs/1_55_0/boost/multiprecision/number.hpp and  
//http://www.boost.org/doc/libs/1_54_0/libs/multiprecision/doc/html/boost_multiprecision/tut/ints/cpp_int.html
// There is another implementation at http://cpp-bigint.sourceforge.net/
// One more at http://www.codef00.com/code/uint128.h
  
///This needs to be implemented.  
template <bool sign>
class Interger128Bit {
  
  
};


  
typedef Interger128Bit<true> int128_t;
typedef Interger128Bit<false> uint128_t;

}

#endif //T128BIT_TYPES_H_H