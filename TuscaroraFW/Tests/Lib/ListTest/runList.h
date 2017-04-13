////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef LISTTEST_H_
#define LISTTEST_H_

#include "Lib/PAL/PAL_Lib.h"
#include "Lib/Math/Rand.h"
#include "Lib/DS/SortedListT.h"

extern uint16_t MY_NODE_ID;

//uncomment to allow duplicates in the list
#define DUPS 1

class ListTest {

#ifdef DUPS  
  SortedListT<uint16_t,true, StdLess<uint16_t> > *shortList;
#else
  SortedListT<uint16_t,false, StdLess<uint16_t> > *shortList;
#endif
  UniformRandomInt *rnd;
public:
  ListTest();
  void Execute(int argc, char *argv[]);
  void Level_0A();
  void Level_0B();
};


#endif /* LISTTEST_H_ */
