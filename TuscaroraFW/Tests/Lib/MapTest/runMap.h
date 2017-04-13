////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef MAP_TEST_H_
#define MAP_TEST_H_

#include "Lib/PAL/PAL_Lib.h"
#include <Sys/Run.h>


#include "Lib/Math/Rand.h"
#include <string.h>

#define BSTMAP 1

#ifdef BSTMAP
#include "Lib/DS/BSTMapT.h"
#else
#include "Lib/DS/ListMapT.h"
#endif


class MapTest {
#ifdef BSTMAP
  BSTMapT<NodeId_t, string> map;
#else 
  ListMapT<NodeId_t, string> map;
#endif
  UniformRandomInt *rnd;
public:
  MapTest();
  void Execute(int argc, char *argv[]);
  void Level_0A();
  void Level_0B();
  void Level_0C();

};


#endif /* MAP_TEST_H_ */
