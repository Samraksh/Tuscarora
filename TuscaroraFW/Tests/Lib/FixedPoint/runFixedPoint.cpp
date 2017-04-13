////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include <stdio.h>

#include "Lib/Misc/Debug.h"
#include "Lib/PAL/PAL_Lib.h"
#include "runFixedPoint.h"
#include <Sys/Run.h>

using namespace PAL;


void Test1_7(UFixed_7_8_t a, UFixed_7_8_t b)
{
  //TODO: commenting code since, fixed point in type cast to float
 /* UFixed_7_8_t temp;

  printf("Results of operations on 1_7 variables\n");
  temp = a.state.full + b.state.full;
  printf("Addition result is %d.%2.2d\n", temp.state.part.integer, (temp.state.part.fraction*100+64)/128);
  if (a.state.full < b.state.full)
  {
    printf("a is less than b. Subtraction overflows.\n");
  }
  if (a.state.full == b.state.full)
  {
    printf("a is the same as b. Result = 0.\n");
  }
  if (a.state.full > b.state.full)
  {
    temp.state.full = a.state.full - b.state.full;
    printf("Subtraction result is %d.%2.2d\n", temp.state.part.integer, 
    (temp.state.part.fraction*100+64)/128);
  }
  */
}



bool FixedPointTest::Execute_U78()
{
  //TODO: commenting code since, fixed point in type cast to float
  /*
  UFixed_7_8_t a(1,5); //a is 1.5
  UFixed_7_8_t b(0,2); //b is 0.2
  printf("A is %s \n", a.print());
  printf("B is %s \n", b.print());
 
  
  Test1_7(a,b);
  
  */
  return true;
}


int main(int argc, char* argv[]){
  //Make sure RuntimeOpts construction is the very first line of main
  RuntimeOpts opts ( argc-1, argv+1 );
  InitPlatform(&opts);    //This does a platform specific initialization

  Debug::SetTextOutput(atoi(argv[13]));

  FixedPointTest fpTest;

  fpTest.Execute_U78();
  
  RunTimedMasterLoop();
  return 0;
}
