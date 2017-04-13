////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef BSTTEST_H_
#define BSTTEST_H_

#include "Lib/PAL/PAL_Lib.h"
#include "Lib/DS/BinarySearchTreeT.h"
#include "Lib/Math/Rand.h"
#include <Sys/Run.h>


class BSTTest {
  BST_T<uint16_t> *shortBST;
  UniformRandomInt *rnd;
public:
  BSTTest();
  void Execute(int argc, char *argv[]);
  void Level_0A();
  void Level_0B();
};


#endif /* BSTTEST_H_ */
