////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef BiMAP_TEST_H_
#define BiMAP_TEST_H_

#include "Lib/PAL/PAL_Lib.h"


#include "Lib/Math/Rand.h"
#include <string>

#define BSTMAP2

//#include "Lib/DS/BSTMapT.h"
#include "../../../Lib/DS/BiBSTMapT.h"



class BiMapTest {
  //BSTMapT<int, int > map;
  BiBSTMapT<NodeId_t, string > map;
  UniformRandomInt *rnd;
public:
  BiMapTest();
  void Execute(int argc, char *argv[]);
  void Level_0A();
  void Level_0B();
  void Level_0C();

};


#endif /* MAP_TEST_H_ */
