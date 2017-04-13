////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef PBTEST_H_
#define PBTEST_H_

#include "Lib/PAL/PAL_Lib.h"
#include <Lib/ExternalServices/TimeStamping.h>

using namespace PAL;
using namespace ExternalServices;
using namespace PWI;


class TimeStampTest{
  TimeStamping<uint64_t> *tss;
  FrameworkBase* fb;

public:
	uint16_t *data;

	TimeStampTest(FrameworkBase* fb);
	void Execute(int argc, char *argv[]);
	void ReceiveTimestampTest();
	void SenderTimestampTest();
	
};


#endif // PBTEST_H_ 
