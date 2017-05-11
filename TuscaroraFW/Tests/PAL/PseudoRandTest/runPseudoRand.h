////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef PSEUDORANDTEST_H_
#define PSEUDORANDTEST_H_

#include <iostream>
#include <fstream>
#include <sstream>


#include <stdio.h>
#include <unistd.h>

//#include "Lib/PAL/PAL_Lib.h"
#include "Lib/Math/Rand.h"

using namespace PAL;

#define iter 1000

///The test initializes a uniform random integer generator, with a maximum value of 10000
///Generate 100 values, finds its mean and prints it
class PseudoRandTest {
  UniformRandomInt *rnd0, *rnd1;
  double mean1, mean2; // =0;
  uint32_t randArray0[iter], randArray1[iter];
  std::ofstream outFile;

  bool RandomDrawingSanityCheck(const uint32_t& draw, UniformRandomInt* rnd) const{
	UniformIntDistParameter distpar = rnd->GetState().GetDistributionParameters();
  	if(draw < distpar.min || draw > distpar.max) return false;
  	return true;
  }

 public:
 PseudoRandTest() {
	/*RNStreamID cmrgState;
	cmrgState.stream = 444;
	cmrgState.run = 1;
	UniformRNGState rngState;
	UniformIntDistParameter dist_param;
	dist_param.min = mean - 250000;
	dist_param.max = mean + 250000;
	rngState.SetRNStreamID(cmrgState);
	rngState.SetDistributionParameters(dist_param);
	*/
	
    rnd0 = new UniformRandomInt(0,2000000);
    rnd1 = new UniformRandomInt(0,2000000);

    rnd0->GetNext();
    rnd1->SetState(rnd0->GetState());
    rnd0->GetNext();

    outFile.open("UniformInt_200000.txt");
  }

  void Execute(){
	  printf("PseudoRandTest:: Starting...\n");
//
//	  std::string internalstring1;
//	  std::string internalstring2;
//	  std::stringbuf buffer1(internalstring1);
//	  std::stringbuf buffer2;
//	  std::iostream os1 (&buffer1);
//	  std::iostream os2 (&buffer2);
//	  std::istream os22(&buffer2);
//	  std::uniform_int_distribution<int> dist1(0,9);
//	  std::uniform_int_distribution<int> dist2(0,9);
//
////	  os1 << std::hex << 255 << " End of os1 \n";
////	  outFile << internalstring1 << " line 0 \n";
////	  outFile << buffer1.str() << " line 1 \n";
////	  outFile << buffer1.str() << " line 1 \n";
////	  outFile << buffer2.str() << " line 2 \n";
//
//
//	  std::minstd_rand engine1(4444);
//	  std::minstd_rand engine2(4444);
//
////	  outFile << dist1(engine1) << " ";
////	  outFile << dist1(engine1) << " ";
////	  outFile << dist1(engine1) << " line 3 \n";
//	  outFile << dist1(engine2) <<  " line 4 \n";
//
//	  os2.setf ( std::iostream::dec, std::iostream::basefield );
//	  os2.setf ( std::iostream::left, std::iostream::adjustfield );
//	  os2.fill(' ');
//
//	  outFile<< os2.fill() << "line 45 " << os2.flags() << "  "
//			  << std::iostream::basefield<< "  "
//			  << std::iostream::dec << "  " << std::iostream::hex << "  "
//			  << std::iostream::left<< "  " << std::iostream::right  << " line 45"
//			  //<< os2.fill() << " line 45"
//			  << "\n";
//
//	  os1 << engine1;
//	  if(os1.fail()) {
//		  outFile << "1 Engine opoeration failed \n";
//	  }
//
//	  os2 << engine2;
//	  if(os2.fail()) {
//		  outFile << "2 Engine opoeration failed \n";
//	  }
//
//	  os2 >> engine1;
//	 // (os2 << engine2) >> engine1 ;
//	  if(os2.fail()) {
//		  std::string::size_type sz;
////		  std::string engine_string = buffer2.str();
////		  std::istringstream reader(engine_string);
////			unsigned internalseed2;
////			reader >> internalseed2;
////			outFile << internalseed2 << " line 48 \n";
////			engine1.seed(internalseed2);
////
////			os1<< " ";
////			os1 << engine1;
//////
//////			std::cout<< "internalseed2 = " << internalseed2 << " \n";
//////			std::cout<< "engine1 = " << engine1 << " \n";
//
//
//
//			  outFile << "3 Engine opoeration failed \n";
//			  unsigned long long internalseed = std::stoull (buffer2.str(), &sz);
//			  outFile << internalseed << " " << sz << " line 46 \n";
//			  engine1.seed(internalseed);
//
//	  }
//
//
//
////	  os22.setf ( std::ios::dec, std::ios::basefield );
////	  os22.setf ( std::ios::left, std::ios::adjustfield );
////	  os22.fill(' ');
////	  os22 << engine1 ;
////	  os22 >> engine1 ;
////	  if(os22.fail()) {
////		  outFile << "4 Engine opoeration failed \n";
////	  }
//	  os1<< " ";
//	  os1 << engine1;
//
//
//	  outFile << buffer1.str() << " line 5 \n";
//	  outFile << buffer2.str() << " line 6 \n";
//
//	  outFile << dist1(engine1) << " line 7 \n";
//	  outFile << dist1(engine2) << " line 8 \n";


	//printf("\nPseudoRandTest:: Generating random numbers with mean: %7.0f \n", mean1);

    //rnd->Initialize(987654);
    for (int i=0;i<iter; i++){
      randArray0[i] = rnd0->GetNext();
      if(!RandomDrawingSanityCheck(randArray0[i], rnd0)) printf ("FAIL RandomDrawingSanityChec: The random number instantiation fails sanity test \n" );

      randArray1[i] = rnd1->GetNext();
      if(!RandomDrawingSanityCheck(randArray1[i], rnd1)) printf ("FAIL RandomDrawingSanityChec: The random number instantiation fails sanity test \n" );

      mean1+=randArray0[i];
	  mean2+=randArray1[i];
      printf ("%d %d \n", randArray0[i], randArray1[i]);
      outFile << randArray0[i] << " ";
    }
    mean1 /= iter;
	mean2 /= iter;
    printf("\nMean1: %f, Mean2: %f\n\n",mean1, mean2);
    outFile.close();
  }
};

// int testRandomInt(int argc, char* argv[]);


#endif /* PSEUDORANDTEST_H_ */
