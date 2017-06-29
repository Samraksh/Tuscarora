/*
 * runFAMap.h
 *
 *  Created on: Feb 13, 2015
 *      Author: Mukundan Sridharan
 */

#ifndef MAP_TEST_H_
#define MAP_TEST_H_

#include "Lib/PAL/PAL_Lib.h"
#include <Sys/Run.h>


#include "Lib/Math/Rand.h"
#include <string.h>

#include "Lib/DS/FABSTMapT.h"

#define NUMITEMS 20

class FAMapTest {

  UniformRandomInt *rnd;
public:
  FAMapTest();
  void Execute(int argc, char *argv[]);
  void Level_0A();
  void Level_ADDBEYONDSIZE();
  void Level_0B();
  void Level_0C();
  void PrintElemnentsofTreeStructured(FAAVLBSTElement<FAMapElement<NodeId_t, string>> *curElement, AVLBSTHeight_t offsetheight=0 );

};


#endif /* MAP_TEST_H_ */
