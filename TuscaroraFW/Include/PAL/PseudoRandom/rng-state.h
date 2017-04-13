////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef LIB_MATH_RNG_STATE_H_
#define LIB_MATH_RNG_STATE_H_



namespace PAL {

class RNGStateImpl;
class RNStreamID;

/**
 * @brief  RNGState<DistributionParametersType>: This class holds a complex state definition for Tuscarora Random Number generators and facilitate copying the state of one random number generator into another.
 * 			The complex state definition includes:  (i)  The limits of the distribution namely, min and the max
 * 													(ii) The initialization parameters, RNStreamID
 * 													(iii)Instantenous state of the underlying random number generator.
 *
 *			The limits of the distribution is defined as a min and a max. The default values for [min, max] are [0, 1].
 *
 * 			Initialization parameters are stored as a RNStreamID structure. These parameters can be retrieved using
 * 			GetRNStreamID() method and can be set using SetRNStreamID(RNStreamID _i) method, however setting the
 * 			initialization parameters would reset the state of the random number generator to the initial state
 * 			defined by the RNStreamID.
 *
 * 			The instantenous state of a random variable engine can be copied to another random variable engine
 * 			using SetEngineStateBuffer and GetEngineState methods.
 *
*/
template<typename DistributionParametersType>
class RNGState{
	RNGStateImpl* implPtr;
	DistributionParametersType m_distribution;
public:


	/**
	 * @brief Default constructor. Initializes the state with a next available RNStreamID and sets the [min, max] to [0,1].
	 *
	 * */
	RNGState();

	/**
	 * @brief Copy constructor. Creates an identical state to the input st.
	 *
	 */
	RNGState(const RNGState& st );

	/**
	 * @brief Explicit constructor. Creates the object with the initialization parameters given in i_s, the limits specified by [i_min, i_max].
	 *
	 */
	RNGState(const RNStreamID& i_s);

	/**
	 * @brief  Sets the distribution parameters of the RNGState object by creating a copy of the input.
	 */
	void SetDistributionParameters(const DistributionParametersType& i_s);

	/**
	 * @brief  Retrieves the distribution parameters from RNGState
	 */
	DistributionParametersType GetDistributionParameters() const;

	/**
	 * @brief  Copies the entire state of the input to the object.
	 */
	RNGState& operator=(const RNGState& st);

	/**
	 * @brief  Re-initializes the state as specified by the initialization parameters _i.
	 */
	void SetRNStreamID(RNStreamID _i);

	/**
	 * @brief  Retrieves the initialization parameters used when the random variable was initialized.
	 */
	RNStreamID GetRNStreamID() const;


//	/**
//	 * @brief Extracts the instantenous state from the input and copies it to the internal object.
//	 *
//	 * The input is an random number generator object. This method should be specialized and implemented for random number generator engines used.
//	 */
//	template<typename TemplatedRNGClass>
//	void SetEngineStateBuffer(const TemplatedRNGClass& temp_engine);
//
	/**
	 * @brief Copies the internal state to the input random number generator engine .
	 *
	 * The input is an random number generator object. This method should be specialized and implemented for random number generator engines used.
	 *
	 */
	template<typename TemplatedRNGClass>
	void GetEngineState(TemplatedRNGClass& temp_engine) const;

};

}

#endif /* LIB_MATH_RNG_STATE_H_ */
