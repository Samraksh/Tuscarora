#ifndef BITMAP_TEST_H_
#define BITMAP_TEST_H_

#include "Lib/PAL/PAL_Lib.h"
#include "Lib/DS/FixedBitMapT.h"

#define NUMITEMS 130 //Set it to some non power of 2 is recommended

class BitMapTest {
	FixedBitMapT<NUMITEMS> bmap;
	UniformRandomInt *rnd;
public:
	BitMapTest();
	void Execute(int argc, char *argv[]);
	void Level_0A();
	//void Level_0B();
};


#endif 
