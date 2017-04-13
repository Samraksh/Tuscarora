////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef AVLBSTTEST_H_
#define AVLBSTTEST_H_

#include "Lib/PAL/PAL_Lib.h"
#include "Lib/DS/AVLBinarySearchTreeT.h"
#include "Lib/Math/Rand.h"

extern uint16_t MY_NODE_ID;

/*struct MyData {
 uint16_t useless; 
 MyData(uint16_t x){
   useless=x;
 }
};
*/

class AVLBSTTest {
  AVLBST_T<uint16_t> *shortAVLBST;
  UniformRandomInt *rnd;
public:
  AVLBSTTest();
  void Execute(int argc, char *argv[]);
  void Level_0A();
  void Level_0B();
  void PrintElemnentsofTreeStructured(AVLBSTElement<uint16_t> *curElement, AVLBSTHeight_t offsetheight );
  void PrintElemnentsofTreeDepthFirst(AVLBSTElement<uint16_t> *curElement );
};


#endif /* AVLBSTTEST_H_ */
