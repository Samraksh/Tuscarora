////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef LIB_MATH_UNIFORMRANDOMVALUE_H_
#define LIB_MATH_UNIFORMRANDOMVALUE_H_

#include <iostream>
#include <sstream>
#include <typeinfo>
//#include <chrono>
#include <random>
#include <PAL/PseudoRandom/PseudoRandomI.h>
#include "rng-state.h"


namespace PAL {

struct UniformDoubleRVDistributionParametersType{
	double min;
	double max;
};

typedef RNGState<UniformDoubleRVDistributionParametersType> UniformValueRNGState;

class UniformRandomValueImpl;

/**
 * @brief  The random number generator that returns continuous random values of type double.
 *
 * 	This is the continuous random number generator used in Tuscarora.
 * 	The internal state is derived from RNGState class. The internal state defines
 * 	the pseudo random stream used to generate the random numbers and the position of the random variable generator within that stream
 * 	as well as the limits of the produced random values.
 *
 * 	UniformRandomInt implements methods to construct the generator, customize it with desired parameters,
 * 	retrieving random numbers from the underlying pseuodo random stream, setting/getting states from other UniformRandomValue objects.
 *
*/
class UniformRandomValue : public PseudoRandomI<UniformValueRNGState, double>{
	UniformRandomValueImpl* implPtr;

public:
	/**
	 * @brief  The definition of the state of the random number generator. It is derived from RNGState.
	*/
    typedef UniformValueRNGState State;

	/**
	 * @brief 	Default constructor. Creates a UniformRandomInt object that draws uniform integer
	 * 			random numbers from range [minVal,maxVal].  The defaults for minVal is 0 and the maxVal is 1.
	 *
	 */
	UniformRandomValue(double minVal = 0, double maxVal = 1);

	/**
	 * @brief Copy constructor. Creates an UniformRandomInt object with an internal state equal to _state.
	 * 			UniformRandomInt objects with the State produce random varibles from identical streams.
	 *
	 */
	UniformRandomValue(const State& _state);


	/**
	 * @brief Sets the minimum and the maximum of the uniform random variable generator to c_min and c_max.
	 *
	 *	c_min and c_max can be negative as well as positive. Also, the range can be reversed (c_min>c_max).
	 */
	void SetRange(double c_min, double c_max);

	//Inherited through PseudoRandomI
	/**
	 * @brief Advances to the next random value from the pseudorandom stream and returns it.
	 *
	 */
	double GetNext();

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
	virtual ~UniformRandomValue();
};

} /* namespace PAL */

#endif /* LIB_MATH_UNIFORMRANDOMVALUE_H_ */
