////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef UNIFORMRANDOMTEST_H_
#define UNIFORMRANDOMTEST_H_

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <unistd.h>

//#include "Lib/Math/UniformRandomValue.h"
#include <PAL/PseudoRandom/UniformRandomValue.h>
//#include "Lib/PAL/PAL_Lib.h"

using namespace PAL;

#define iter 1000

/// The test initializes a uniform random generator.
/// Generate 100 values, finds its mean and prints it
class UniformRandomTest {
  UniformRandomValue rnd;
  double mean; // =.5;
  double randArray[iter];
  std::ofstream outFile;
  
public:
 UniformRandomTest(): mean(0.5){    
    outFile.open("UniformRandom_100000.txt");
  }

  void ChangeState() {
	  UniformRandomValue::State rngState = rnd.GetState();

//    rngState.cmrgState.run++;
//    rngState.cmrgState.stream++;
    rnd.SetState(rngState);
    rnd.SetRange(-20.0, 60.0);
    return;

    /* UniformValueRNGState rngState; */
    /* rngState.cmrgState.run = 1; */
    /* rngState.cmrgState.stream = base+j*10000; */
    /* rngState.min=0; */
    /* rngState.max=1; */
    /* rnd.SetState(rngState); */
  }

  /// Run (via Execute1) with different rng states.
  void Execute(){




//	ChangeState();
    Execute1();
    //ChangeState();
    //Execute1();
  }

  /// Run with current rng state.
  void Execute1(){
    printf("UniformRandomTest:: Starting...\n");
    
    //rnd->Initialize(987654);
    for (int i=0;i<iter; i++){
      randArray[i] = rnd.GetNext();
      mean+=randArray[i];
      //printf ("%f ", randArray[i]);
      outFile << randArray[i] << " ";
    }
    mean /= iter;
    printf("\nMean: %f\n\n",mean);
    outFile.close();
  }
};

// int testUniformRandom(int argc, char* argv[]);


#endif /* UNIFORMRANDOMTEST_H_ */
