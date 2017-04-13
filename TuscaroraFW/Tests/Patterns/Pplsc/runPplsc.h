////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

/*
 * runPplsc.h
 *
 */

#ifndef PPLSCTEST_H_
#define PPLSCTEST_H_

#include "Lib/PAL/PAL_Lib.h"

#include "Patterns/Pplsc/Pplsc.h"
#include <Sys/Run.h>

extern NodeId_t MY_NODE_ID;

using namespace Patterns;
using namespace PAL;

class PplscTest{
	Pplsc *pplsc;

	TimerDelegate *timerDel;
	Timer *startTestTimer;
public:
	uint16_t *data;
	PplscTest();
	void TimerHandler(uint32_t event);
	void Execute(RuntimeOpts *opts);
};

//int testPplsc(int argc, char* argv[]);


#endif /* PPLSCTEST_H_ */
