////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef COPTEST_H_
#define COPTEST_H_

#include <string>
using std::string;

#include "Lib/PAL/PAL_Lib.h"
#include "Patterns/RCop/BCop.h"
#include "Patterns/RCop/RCop.h"
#include "CopOpts.h"
#include <PAL/PAL.h>

extern NodeId_t MY_NODE_ID;

using namespace Patterns;
using namespace PAL;

class CopTest{
public:

	Cop::Cop* cop;

	CopTest(CopOpts &opts );
	~CopTest();

	TimerDelegate *timerDel;
	Timer *testTimer;
	uint32_t simulationTimeInSeconds;
	void HandleTimer(uint32_t event);
	bool firstTimeInTimerHandler;
	double publishPeriod;

	void Execute(CopOpts &opts );

	void ProcessMessage ( SizedBuffer );
	Delegate <void, SizedBuffer > *processMessageDelegate;
	void Publish();

	uint32_t x() { return GetEnv("CURX"); }
	uint32_t y() { return GetEnv("CURY"); };
	double TestAliveTimeInSecs() { return PTN_Utils::PatternAliveTimeInSecs();}

	double Distance( double x1, double y1, double x2, double y2 );

};

//int testCop (int argc, char* argv[]);

#endif /* COPTEST_H_ */
