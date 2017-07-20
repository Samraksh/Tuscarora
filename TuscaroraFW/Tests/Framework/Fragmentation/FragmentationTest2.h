/*
 * FragmentationTest2.h
 *
 *  Created on: Jun 15, 2017
 *      Author: bora
 */

#ifndef TESTS_FRAMEWORK_FRAGMENTATIONTEST2_H_
#define TESTS_FRAMEWORK_FRAGMENTATIONTEST2_H_

//#include "Test/ModuleTesterBase.h"
#include <Interfaces/AWI/Gossip_I.h>

//
//#include <PAL/Timer.h>
//

namespace FragTest2{
#define MSGSIZE 2000

struct LongMessage{
	uint8_t array[MSGSIZE];
};

class LongMessageComparator {
public:
	static bool LessThan(const LongMessage& A, const LongMessage& B){
		for(auto i = 0; i < MSGSIZE; ++i){
			if(A.array[i] >= B.array[i]){
				return false;
			}
			else if(A.array[i] < B.array[i]){
				return true;
			}
		}
		return false;
	}
	static bool EqualTo(const LongMessage& A, const LongMessage& B){
		for(auto i = 0; i < MSGSIZE; ++i){
			if(A.array[i] != B.array[i]){
				return false;
			}
		}
		return true;
	}

};


class FragmentationTest2 {

	AWI::Gossip_I<LongMessage, LongMessageComparator>* gossip;
	typedef typename AWI::Gossip_I<LongMessage, LongMessageComparator>::GossipVariableUpdateDelegate_t GossipVariableUpdateDelegate_t_l;


	GossipVariableUpdateDelegate_t_l* recvUpdateVarDelegate;
//
//	PAL::TimerDelegate *timerDel;
//	PAL::Timer *startTestTimer;

	NodeId_t otherNodesID ;


	void ReceiveGossipVariableUpdate(LongMessage& updated_gossip_variable);

	bool IsRxMsgValid(void *data, uint16_t size );

public:
	FragmentationTest2(NodeId_t _otherNodesID);
	virtual ~FragmentationTest2();



	void Execute();


};

}
#endif /* FRAMEWORK_CORE_DATAFLOW_TEST_FRAGMENTATIONTEST_H_ */
