////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef PSEUDORANDTESTMRG_H_
#define PSEUDORANDTESTMRG_H_

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>

#include <string.h>
//#include "Lib/PAL/PAL_Lib.h"
#include <Interfaces/Core/MessageT.h>
#include <Sys/Run.h>
//#include <iostream>
#include <fstream>
#include <stdlib.h>

#include "Lib/Math/Rand.h"

//#include "Lib/PAL/PAL_Lib.h"
using namespace PAL;

#define iterations_MRG 10000
#define iterations_MeanTest 10000

class PseudoRandMRGTest {

	UniformRandomValue *rnd1; //to test user assigned stream numbers
	UniformRandomValue *rnd2; //to test self generated stream numbers
	double mean1; //=0;
	double mean2; //=0;
	double randArray1[iterations_MRG];
	double randArray2[iterations_MRG];
	// uint64_t base = ((1ULL)<<63);
	uint64_t base;

public:
	PseudoRandMRGTest();
	void Execute();

	void ExecuteTwoIdenticalRNG(std::ofstream& outFile);
	void ExecuteTwoDifferentRNG(std::ofstream& outFile);

	void ExecuteMeanTest(std::ofstream& outFile);
};

//int testRandomFloat(int argc, char* argv[]);

#endif /* PSEUDORANDTESTMRG_H_ */
