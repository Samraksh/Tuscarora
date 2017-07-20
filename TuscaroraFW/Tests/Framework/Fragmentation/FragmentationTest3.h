/*
 * GossipTest.h
 *
 *  Created on: Jun 10, 2016
 *      Author: bora
 */

#ifndef TESTS_FRAMEWORK_FRAGMENTATIONTEST3_H_
#define TESTS_FRAMEWORK_FRAGMENTATIONTEST3_H_

#include "Lib/PAL/PAL_Lib.h"

#include "Patterns/Gossip/Gossip.h"
#include <Sys/RuntimeOpts.h>

#include <fstream>
#include <string>

extern NodeId_t MY_NODE_ID;

using namespace Patterns;
using namespace PAL;

namespace FragmentationTest_Number3{


#define BYTELENGTHOFNESSAGE 2000
#define MSGSIZE BYTELENGTHOFNESSAGE/8

typedef uint64_t RepeatedBasicMsgType3 ;
typedef uint16_t MsgIteratorT3;

struct LongMessage3{
	RepeatedBasicMsgType3 array[MSGSIZE];
	LongMessage3(){
		SetMsg(1);
	};
	void SetMsg(uint8_t x){
		for(auto i = 0; i < MSGSIZE ; ++i){
			this->array[i] = x;
		}
	};

	bool operator ==(const LongMessage3& rhs) const{
		for(uint16_t i = 0; i < MSGSIZE; ++i){
			if(array[i] != rhs.array[i]){
				return false;
			}
		}
		return true;
	};
	bool operator=(const LongMessage3& rhs){
		for(uint16_t i = 0; i < MSGSIZE ; ++i){
			this->array[i] = rhs.array[i];
		}
		return true;
	};
	bool operator<(const LongMessage3& rhs) const{
		for(uint16_t i = 0; i < MSGSIZE; ++i){
			if(array[i] >= rhs.array[i]){
				return false;
			}
			else if(array[i] < rhs.array[i]){
				return true;
			}
		}
		return false;
	};

};

class LongMessage3Comparator {
public:
	static bool LessThan(const LongMessage3& A, const LongMessage3& B){
//		for(auto i = 0; i < MSGSIZE; ++i){
//			if(A.array[i] >= B.array[i]){
//				return false;
//			}
//			else if(A.array[i] < B.array[i]){
//				return true;
//			}
//		}
//		return false;
		return A<B;
	}
	static bool EqualTo(const LongMessage3& A, const LongMessage3& B){
//		for(auto i = 0; i < MSGSIZE; ++i){
//			if(A.array[i] != B.array[i]){
//				return false;
//			}
//		}
//		return true;
		return A==B;
	}

};



class FragmentationTest3 {
	std::ofstream outFile;
	bool isTested;

	LongMessage3 gossip_variable;
	typedef Gossip<LongMessage3, LongMessage3Comparator> TestGossipT;
	TestGossipT *gossip;
	TestGossipT::GossipVariableUpdateDelegate_t* recvUpdateVarDelegate;

	TimerDelegate *timerDel;
//	TimerDelegate *updatevarDel;
	Timer *startTestTimer;
//	Timer *updateVariableTimer;
	uint16_t *data;

	bool IsRxMsgValid(void *data, uint16_t size );

public:
	FragmentationTest3();
	void TimerHandler(uint32_t event);
//	void IncrementGossipVariable(uint32_t event);
	void ReceiveGossipVariableUpdate(LongMessage3& updated_gossip_variable);
	void Execute(RuntimeOpts *opts);
	virtual ~FragmentationTest3();
};

}

#endif /* TESTS_PATTERNS_GOSSIP_GOSSIPTEST_H_ */
