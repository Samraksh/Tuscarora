////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef BASICTYPES_H_
#define BASICTYPES_H_

#include <sys/types.h>
#include <stdint.h>
#include "128BitTypes.h"
using namespace Types;
/*
namespace Types {
	typedef void* null;
	typedef unsigned char uint8_t;
	typedef signed char int8_t;
	typedef unsigned short uint16_t;
	typedef signed short int16_t;
	typedef unsigned int uint32_t;
	typedef signed int int32_t;
	
	
// Check GCC
#if __GNUC__
	#if __x86_64__ || __ppc64__
		#define ENVIRONMENT64
		#ifndef uint64_t
			typedef unsigned long uint64_t;
		//typedef unsigned long uint64_t;
		#endif
		#ifndef int64_t
			typedef signed long int64_t;
		#endif
	#else
		#define ENVIRONMENT32
		#ifndef uint64_t
			typedef unsigned long long uint64_t;
		//typedef unsigned long uint64_t;
		#endif
		#ifndef int64_t
			typedef signed long long int64_t;
		#endif
	#endif
#endif
	
}
*/

#endif /* BASICTYPES_H_ */
