////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef SIMPLEGENERICCOMPARATOR_H_
#define SIMPLEGENERICCOMPARATOR_H_

template<class T>
class SimpleGenericComparator {
public:
	static bool LessThan (const T& A, const T& B) {
		return (A < B);
	}
	static bool EqualTo (const T& A, const T& B) {
		return (A == B);
	}
};


#endif //SIMPLEGENERICCOMPARATOR_H_
