////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef PBTEST_H_
#define PBTEST_H_

#include "Lib/PAL/PAL_Lib.h"
#include <Interfaces/Core/PiggyBackingI.h>

using namespace PAL;
using namespace Core;

bool overSizedPacket;

class TestPiggyBackee :public PiggyBackeeI<uint64_t> {
public:
	uint16_t CreateData (uint64_t dest, bool unaddressed, uint8_t*&, uint8_t dataSize);
	void ReceiveData (uint64_t src, uint8_t *data, uint8_t dataSize, U64NanoTime receiveTimestamp);
};


class PBTest{
  //TimerDelegate *timerDel;
  //Timer *startTestTimer;

public:
	TestPiggyBackee pBackee;
	uint16_t *data;

	PBTest();
	void Execute(int argc, char *argv[]);
	void BasicTest(){overSizedPacket=false;};
	void OverSizedPacketTest(){overSizedPacket=true;}
};


#endif // PBTEST_H_ 
