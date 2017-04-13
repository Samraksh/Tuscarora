////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef FIFOTEST_H_
#define FIFOTEST_H_

#include <Interfaces/PWI/Framework_I.h>
#include "Lib/Math/Rand.h"
//#include "Lib/PAL/PAL_Lib.h"
#include "Framework/Core/Naming/StaticNaming.h"
#include "Lib/Buffers/FifoList.h"


using namespace PAL;
using namespace PWI;


class FifoTest {
	FIFOList *sendBuffer, *rcvBuffer;
	UniformRandomInt *rnd;

public:
	FifoTest();
	void Execute(int argc, char *argv[]);
	void Dequeue(int32_t x);
	void Enqueue(int32_t x);
};

// int testFIFO(int argc, char* argv[]);


#endif /* FIFOTEST_H_ */
