/*
 * runFAAVLBSTTest.h
 *
 *  Created on: Feb 10, 2015
 *      Author: Bora Karaoglu
 */

#ifndef FAAVLBSTTEST_H_
#define FAAVLBSTTEST_H_

#include "Lib/PAL/PAL_Lib.h"
#include "Lib/DS/FAAVLBinarySearchTreeT.h"
#include "Lib/Math/Rand.h"

#define NUMITEMS 50

extern uint16_t MY_NODE_ID;

/*struct MyData {
 uint16_t useless; 
 MyData(uint16_t x){
   useless=x;
 }
};
*/

class FAAVLBSTTest {
  FAAVLBST_T<NUMITEMS, uint16_t> *shortFAAVLBST;
  UniformRandomInt *rnd;
public:
  FAAVLBSTTest();
  void Execute(int argc, char *argv[]);
  void Level_0A();
//  void Level_0B();
  void Level_0C();
  void Level_0D();

  void PrintElemnentsofTreeStructured(FAAVLBSTElement<uint16_t> *curElement, FAAVLBSTHeight_t offsetheight );
  void PrintElemnentsofTreeDepthFirst(FAAVLBSTElement<uint16_t> *curElement );
};

#endif /* FAAVLBSTTEST_H_ */
