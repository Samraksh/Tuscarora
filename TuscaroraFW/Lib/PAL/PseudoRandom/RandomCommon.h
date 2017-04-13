////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef LIB_PAL_PSEUDORANDOM_RANDOMCOMMON_H_
#define LIB_PAL_PSEUDORANDOM_RANDOMCOMMON_H_

#include <random>
typedef std::minstd_rand Tuscarora_RNG2;

typedef std::uniform_int_distribution<uint64_t> Tuscarora_UniformIntDist;
typedef std::uniform_real_distribution<double> Tuscarora_UniformDoubleDist;

typedef std::initializer_list<uint64_t> Tuscarora_InitializerList;
typedef std::seed_seq Tuscarora_Seed_Seq;




#endif /* LIB_PAL_PSEUDORANDOM_RANDOMCOMMON_H_ */
