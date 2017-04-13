////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 


#include <Sys/Run.h>
#include "Lib/Misc.h"

#include <iostream>
#include <random>
#include <sstream>

#include <string.h>


using namespace std;

int main(int argc, char* argv[]) {
	RuntimeOpts opts ( argc-1, argv+1 );
	InitPlatform(&opts);	//This does a platform specific initialization


	std::cout << "!!!Hello World Within DCE!!!" << endl; // prints !!!Hello World!!!


	std::uniform_int_distribution<uint64_t>* distptr = new std::uniform_int_distribution<uint64_t>(2,15);

	std::initializer_list<uint64_t> mylist = {3,4,5};
	std::seed_seq seed1(mylist);

	std::minstd_rand temp_engine(seed1);


	//std::stringbuf engine_state_buffer;

	//std::iostream str_buf_stream(&engine_state_buffer);

	std::stringstream str_buf_stream;
	str_buf_stream.setf(std::ios::dec, std::ios::basefield);
	str_buf_stream.setf(std::ios::left, std::ios::adjustfield);

//	if(str_buf_stream.fail()){
//		std::cout << "Badbit was set!"<< "Endl" << std::endl;
//	}
//	else{
//		std::cout << "Badbit waS notset!"<< "Endl" << std::endl;
//	}

	str_buf_stream<<(temp_engine)<<flush;

	if(str_buf_stream.fail()){
		std::cout << "Badbit is set!" << "Endl" << std::endl;
	}
	else{
		std::cout << "Badbit is notset!"<< "Endl" << std::endl;
	}
	std::string bufstring;
	str_buf_stream >> bufstring;
	std::cout << "engine_state_buffer = " << bufstring << "Endl" << std::endl;



	uint64_t rand;

	rand = (*distptr)(temp_engine);
	std::cout << "rand1 = " << rand << "Endl" << std::endl;
	rand = (*distptr)(temp_engine);
	std::cout << "rand2 = " << rand << "Endl" << std::endl;
	rand = (*distptr)(temp_engine);
	std::cout << "rand3 = " << rand << "Endl" << std::endl;






	return 0;
}
