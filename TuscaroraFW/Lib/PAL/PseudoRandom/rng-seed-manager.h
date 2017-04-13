////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef RNG_SEED_MANAGER_H
#define RNG_SEED_MANAGER_H
#define STREAMS_PER_NODE 10000
#define STREAM_OFFSET 5000


#include <stdint.h>

class RngSeedManager
{
	static bool seedInitialized;
	static bool runInitialized;
	static bool initialized;
public:
  /**
   * \brief set the seed
   * it will duplicate the seed value 6 times
   * \code
   * RngSeedManger::SetSeed(15);
   * UniformVariable x(2,3);     //these will give the same output everytime
   * ExponentialVariable y(120); //as long as the seed stays the same
   * \endcode
   * \param seed
   *
   * Note, while the underlying RNG takes six integer values as a seed;
   * it is sufficient to set these all to the same integer, so we provide
   * a simpler interface here that just takes one integer.
   */
  static void SetSeed (uint64_t seed);

  /**
   * \brief Get the seed value
   * \return the seed value
   *
   * Note:  returns the first of the six seed values used in the underlying RNG
   */
  static uint32_t GetSeed (void);

  /**
   * \brief Set the run number of simulation
   *
   * \code
   * RngSeedManager::SetSeed(12);
   * int N = atol(argv[1]); //read in run number from command line
   * RngSeedManager::SetRun(N);
   * UniformVariable x(0,10);
   * ExponentialVariable y(2902);
   * \endcode
   * In this example, N could successivly be equal to 1,2,3, etc. and the user
   * would continue to get independent runs out of the single simulation.  For
   * this simple example, the following might work:
   * \code
   * ./simulation 0
   * ...Results for run 0:...
   *
   * ./simulation 1
   * ...Results for run 1:...
   * \endcode
   */
  static void SetRun (uint64_t run);
  /**
   * \returns the current run number
   * @sa SetRun
   */
  static uint64_t GetRun (void);

  static uint64_t GetNextStreamIndex(void);

  static void SetBaseStreamIndex(uint64_t node_id);

  static bool Initialized(){
	  return initialized;
  }

};


#endif /* RNG_SEED_MANAGER_H */
