////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef PrimesDB_h
#define PrimesDB_h

#include <Base/BasicTypes.h>
namespace PAL {
	///Maintains a list of prime numbers less than 65536, this will be useful for asynchronous protocols if duty cycle needs to be configured dynamically
	 class PrimesDB {
	 private:
		uint16_t* list;

	 public:
		uint16_t GetPrimeLessThan(uint16_t  num);
		uint16_t GetPrimeGreaterThan(uint16_t  num);
		uint16_t GetMaxPrime();
	};
}

#endif // PrimesDB_h
