////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef LIB_MATH_UNIFORMRANDOMINT_H_
#define LIB_MATH_UNIFORMRANDOMINT_H_

#include <iostream>
#include <sstream>
#include <typeinfo>
#include <random>
#include <initializer_list>
#include <PAL/PseudoRandom/PseudoRandomI.h>
#include "rng-state.h"

namespace PAL {



struct UniformIntDistParameter{
	uint64_t min;
	uint64_t max;
};

typedef RNGState<UniformIntDistParameter> UniformRNGState;

class UniformRandomIntImpl;

/**
 * @brief  The random number generator that returns integers of type uint64_t.
 *
 * 	This is the integer random number generator used in Tuscarora.
 * 	The internal state is derived from RNGState class. The internal state defines
 * 	the pseudo random stream used to generate the random numbers and the position of the random variable generator within that stream
 * 	as well as the limits of the produced random values.
 *
 * 	UniformRandomInt implements methods to construct the generator, customize it with desired parameters,
 * 	retrieving random numbers from the underlying pseuodo random stream, setting/getting states from other UniformRandomInt objects.
 *
*/
class UniformRandomInt : public PseudoRandomI<UniformRNGState, uint64_t>{
	UniformRandomIntImpl* implPtr;
public:
	/**
	 * @brief  The definition of the state of the random number generator. It is derived from RNGState.
	*/
    typedef UniformRNGState State;

	/**
	 * @brief 	Default constructor. Creates a UniformRandomInt object that draws uniform integer
	 * 			random numbers from range [minVal,maxVal].  The defaults for minVal is 0 and the maxVal is 1.
	 *
	 */
	UniformRandomInt(uint64_t minVal = 0, uint64_t maxVal = 1);

	/**
	 * @brief Copy constructor. Creates an UniformRandomInt object with an internal state equal to _state.
	 * 			UniformRandomInt objects with the State produce random varibles from identical streams.
	 *
	 */
	UniformRandomInt(const State& _state);

	/**
	 * @brief Sets the minimum and the maximum of the uniform random variable generator.
	 *
	 */
	void SetMinMax(uint64_t c_min, uint64_t c_max);

	/**
	 * @brief Sets the minimum and the maximum of the uniform random variable generator to mean-range and mean+range.
	 *
	 */
	void SetRange(uint64_t mean, uint64_t range);

	//Inherited through PseudoRandomI
	/**
	 * @brief Advances to the next random value from the pseudorandom stream and returns it.
	 *
	 */
	uint64_t GetNext();

	/**
	 * @brief Returns the state of the generator.
	 *
	 */
	const State GetState();

	/**
	 * @brief Sets the state of the generator.
	 *
	 */
	void SetState(const State& state);

	/**
	 * @brief Destructor.
	 *
	 */
	virtual ~UniformRandomInt();
};

} /* namespace PAL */

#endif /* LIB_MATH_UNIFORMRANDOMInt_H_ */
