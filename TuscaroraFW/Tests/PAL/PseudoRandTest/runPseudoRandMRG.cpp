////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 




#include "runPseudoRandMRG.h"


///The test initializes a uniform random integer generator, with a maximum value of 10000
///Generate 100 values, finds its mean and prints it


PseudoRandMRGTest::PseudoRandMRGTest(): mean1(0), mean2(0), base((1ULL)<<63){
	rnd1 = NULL;
	rnd2 = NULL;
}

void PseudoRandMRGTest::Execute(){
	std::ofstream outFile;
	outFile.open("UniformRandom_100000.txt");
	printf("PseudoRandTest:: Starting...\n");

	RngSeedManager::SetSeed(1);
	RngSeedManager::SetRun(1);
	RngSeedManager::SetBaseStreamIndex(MY_NODE_ID);

	ExecuteTwoIdenticalRNG(outFile);
	ExecuteTwoDifferentRNG(outFile);
	ExecuteMeanTest(outFile);

	outFile.close();
}

bool PseudoRandMRGTest::RandomDrawingSanityCheck(const double& draw, UniformRandomValue* rnd) const{
	UniformDoubleRVDistributionParametersType distpar = rnd->GetState().GetDistributionParameters();
	if(draw < distpar.min || draw > distpar.max) return false;
	return true;
}

void PseudoRandMRGTest::ExecuteTwoDifferentRNG(std::ofstream &outFile){
	printf("PseudoRandTest:: Starting ExecuteTwoDifferentRNG...\n");
	outFile << "PseudoRandTest:: Starting ExecuteTwoDifferentRNG...\n";

	UniformValueRNGState rngState1;
	UniformDoubleRVDistributionParametersType dist; dist.min = DIST_MIN; dist.max = DIST_MAX;
	rngState1.SetDistributionParameters(dist);
	UniformValueRNGState rngState2;
	rngState1.SetDistributionParameters(dist);

	if(rnd1 != NULL) delete rnd1;
	rnd1 = new UniformRandomValue(rngState1);
	if(rnd2 != NULL) delete rnd2;
	rnd2 = new UniformRandomValue(rngState2);

	for (int i=0;i<iterations_MRG; i++){
			randArray1[i] = rnd1->GetNext();
			if(!RandomDrawingSanityCheck(randArray1[i], rnd1)) printf ("FAIL RandomDrawingSanityChec: The random number instantiation fails sanity test \n" );
			mean1+=randArray1[i];
			//printf ("%f ", randArray1[i]);
			outFile << randArray1[i] << " ";
	}
	mean1 /= iterations_MRG;
//		printf("Printing 10000 random numbers from rnd2, Stream %lu \n",(rnd1->GetState()).GetRNStreamID().stream);
	for (int i=0;i<iterations_MRG; i++){
			randArray2[i] = rnd2->GetNext();
			if(!RandomDrawingSanityCheck(randArray2[i], rnd2)) printf ("FAIL RandomDrawingSanityChec: The random number instantiation fails sanity test \n" );
			mean2+=randArray2[i];
			//printf ("%f ", randArray1[i]);
			outFile << randArray2[i] << " ";
	}
	mean2 /= iterations_MRG;

	int numdiffeent = 0;
	int numequal = 0;
	for (int i=0;i<iterations_MRG; i++){
		if(randArray1[i] != randArray2[i]){
			++numdiffeent;
		}
		else{
			++numequal;
		}
	}
	if(numdiffeent <= numequal){
		printf ("FAIL ExecuteTwoDifferentRNG 3: Two UniformRands starting with different seeds gets the very similiar streams numdiffeent = %d numequal = %d \n", numdiffeent, numequal);
	}
	else{
		printf ("PASS ExecuteTwoDifferentRNG 3: Two UniformRands starting with different gets the non-similiar streams numdiffeent = %d numequal = %d \n", numdiffeent, numequal);
	}

}

void PseudoRandMRGTest::ExecuteTwoIdenticalRNG(std::ofstream &outFile){
	printf("PseudoRandTest:: Starting ExecuteTwoIdenticalRNGTest...\n");
	outFile << "PseudoRandTest:: Starting ExecuteTwoIdenticalRNGTest...\n";



	UniformValueRNGState rngState;
	UniformDoubleRVDistributionParametersType dist; dist.min = DIST_MIN; dist.max = DIST_MAX;
	rngState.SetDistributionParameters(dist);


	if(rnd1 != NULL) delete rnd1;
	rnd1 = new UniformRandomValue(rngState);
	if(rnd2 != NULL) delete rnd2;
	rnd2 = new UniformRandomValue(rngState);

	outFile << "PseudoRandTest:: Starting ExecuteTwoIdenticalRNG   rnd1 iterations_MRG = " << iterations_MRG << "\n";
//		printf("Printing 10000 random numbers from rnd1, Stream %lu \n",(rnd1->GetState()).GetRNStreamID().stream);
	for (int i=0;i<iterations_MRG; i++){
			randArray1[i] = rnd1->GetNext();
			if(!RandomDrawingSanityCheck(randArray1[i], rnd1)) printf ("FAIL RandomDrawingSanityChec: The random number instantiation fails sanity test \n" );
			mean1+=randArray1[i];
			//printf ("%f ", randArray1[i]);
			outFile << randArray1[i] << " ";
	}
	mean1 /= iterations_MRG;
//		printf("Printing 10000 random numbers from rnd2, Stream %lu \n",(rnd1->GetState()).GetRNStreamID().stream);
	outFile << "PseudoRandTest:: Starting ExecuteTwoIdenticalRNG   rnd2 iterations_MRG = " << iterations_MRG << "\n";
	for (int i=0;i<iterations_MRG; i++){
			randArray2[i] = rnd2->GetNext();
			if(!RandomDrawingSanityCheck(randArray2[i], rnd2)) printf ("FAIL RandomDrawingSanityChec: The random number instantiation fails sanity test \n" );
			mean2+=randArray2[i];
			//printf ("%f ", randArray1[i]);
			outFile << randArray2[i] << " ";
	}
	outFile << "\n";
	mean2 /= iterations_MRG;

	bool fail = false;
	for (int i=0;i<iterations_MRG; i++){
		if(randArray1[i] != randArray2[i]){
			printf ("FAIL ExecuteTwoIdenticalRNGTest 1: Non-mathching random numbers randArray1[%d] = %f randArray2[%d] = %f  \n", i, randArray1[i], i, randArray2[i]);
			fail = true;
//			break;
		}
	}
	if(!fail){
		printf ("PASS ExecuteTwoIdenticalRNGTest 1: All numbers match. \n");
	}

	int numdiffeent = 0;
	int numequal = 0;
	for (int i=1;i<iterations_MRG; i++){
		if(randArray1[i] != randArray1[i-1]){
			++numdiffeent;
		}
		else{
			++numequal;
		}
	}
	if(numdiffeent <= numequal){
		printf ("FAIL ExecuteTwoIdenticalRNGTest 1.2: Consecutive numbers in the stream of random numbers numdiffeent = %d numequal = %d \n", numdiffeent, numequal);
	}
	else{
		printf ("PASS ExecuteTwoIdenticalRNGTest 1.2: Consecutive numbers in the stream of random numbers numdiffeent = %d numequal = %d \n", numdiffeent, numequal);
	}

//		printf("\nMean1: %f\n",mean1);

}

void PseudoRandMRGTest::ExecuteMeanTest(std::ofstream &outFile){
	printf("PseudoRandTest:: Starting ExecuteMeanTest...\n");
	outFile << "PseudoRandTest:: Starting ExecuteMeanTest...\n";

	UniformValueRNGState rngState;
	UniformDoubleRVDistributionParametersType dist; dist.min = DIST_MIN; dist.max = DIST_MAX;
	rngState.SetDistributionParameters(dist);

	if(rnd1 != NULL) delete rnd1;
	rnd1 = new UniformRandomValue(rngState);

	double r;
	for (int i=0;i<iterations_MeanTest; i++){
			r = rnd1->GetNext();
			if(!RandomDrawingSanityCheck(r, rnd1)) printf ("FAIL RandomDrawingSanityChec: The random number instantiation fails sanity test \n" );
			mean1+=r;
	}
	mean1 /= iterations_MRG;

	double expected_variance = (rngState.GetDistributionParameters().max - rngState.GetDistributionParameters().min)*(rngState.GetDistributionParameters().max - rngState.GetDistributionParameters().min) /12;
	double expected_std = sqrt(expected_variance);
	double expected_mean = (rngState.GetDistributionParameters().max - rngState.GetDistributionParameters().min) / 2 ;

	if(mean1 > expected_mean + 6*expected_std || mean1 < expected_mean - 6*expected_std){
		printf ("FAIL ExecuteMeanTest 2: Mean is not in the 6 sigma confidence interval mean1 = %f  CI:=[%f,%f] \n", mean1, expected_mean - 6*expected_std , expected_mean + 6*expected_std );
	}
	else {
		printf ("PASS ExecuteMeanTest 2: Mean is in the 6 sigma confidence interval mean1 = %f  CI:=[%f,%f] \n", mean1, expected_mean - 6*expected_std , expected_mean + 6*expected_std );
	}

}

int main(int argc, char* argv[]) {
	//Make sure RuntimeOpts construction is the very first line of main
	RuntimeOpts opts ( argc-1, argv+1 );
	InitPlatform(&opts);	//This does a platform specific initialization

	PseudoRandMRGTest rtest;
	rtest.Execute();

	//RunMasterLoopNeverExit(); //never exists on dce
	RunTimedMasterLoop();

	return 0;
}
