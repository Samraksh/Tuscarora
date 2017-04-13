////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef HEAPTEST_H_
#define HEAPTEST_H_

#include "Lib/PAL/PAL_Lib.h"
#include "Lib/Math/Rand.h"
#include "Lib/DS/HeapT.h"
#include <Sys/Run.h>

#define MINHEAP 1

class HeapTest {
#ifdef MINHEAP
  HeapT<uint16_t,MyLess<uint16_t> > *shortHeap;
#else
  HeapT<uint16_t,MyGreater<uint16_t> > *shortHeap;
#endif
  
  
	UniformRandomInt *rnd;
public:
	HeapTest();
	void Execute(int argc, char *argv[]);
	void Level_0A();
	void Level_0B();
};


#endif /* HEAPTEST_H_ */
