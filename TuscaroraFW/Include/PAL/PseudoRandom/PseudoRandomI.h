////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef PseudoRandomI_h
#define PseudoRandomI_h

namespace PAL{

	///Interface class for a generic pseudo random number generator. Takes two template classes,
	///one for its state and another for the type of random number
	template<class StateT, class ResultT>
	class PseudoRandomI {
	 public:
		//virtual void SetRange(const ResultT min, const ResultT max) = 0;

		///Sets the parameters of the random number generator using parameter type StateT.
		virtual void SetState(const StateT& state) = 0;

		///Returns a random number instance of type ResultT
		virtual ResultT GetNext() = 0;

		///Returns the current state of the random number generator
		virtual const StateT GetState() = 0;

		///Destructor
		virtual ~PseudoRandomI() {} 	//Destructor implementation is important for an interface in C++
	};

}

#endif // PseudoRandomI_h

