/*
 * FragmentationTest.h
 * 	Creates a large message by node 0 that is destined to all nodes
 *	Things that are tested:
 *		1) Whether all notifications for all destinations have been received and they are in order they were supposed to be received
 *		2) Whether the received message contents identical to the sent message contents
 *		3) Exercises add destination after FW acceptance of the packet and makes sure the remaining destinations are added
 *		--) Exercises another add destination after WF accept, that receives a failure to add
 *		4) Exercices ReplacePayloadRequest
 *		--) Exercises ReplacePayloadRequest after WF accept notification, that receives a failure to replace
 *		--) CancelDataRequest
 *		--) DataStatusRequest
 *  Created on: Jun 15, 2017
 *      Author: bora
 */


#ifndef TESTS_FRAMEWORK_FRAGMENTATIONTEST_H_
#define TESTS_FRAMEWORK_FRAGMENTATIONTEST_H_

#include <stdio.h>

#include "Test/ModuleTesterBase.h"
#include "Lib/Pattern/BasicPattern/BasicPattern.h"
#include "Lib/DS/BSTMapT.h"

#include <PAL/Timer.h>

#include <Sys/RuntimeOpts.h>

namespace FragTest{



#define FPRINT_TEST_RESULT(...) {Lib::Debug::PrintTimeMicro(); fprintf(outFile, __VA_ARGS__); fflush(outFile);}

#define MSG_EMBED 5
#define BYTELENGTHOFNESSAGE 2000
#define MSGSIZE BYTELENGTHOFNESSAGE/8

typedef uint64_t RepeatedBasicMsgType ;
typedef uint16_t MsgIteratorT;

struct LongMessage{
	RepeatedBasicMsgType array[MSGSIZE];
	LongMessage(){
		SetMsg(1);
	};
	void SetMsg(RepeatedBasicMsgType x){
		for(auto i = 0; i < MSGSIZE ; ++i){
			this->array[i] = x;
		}
	};

	bool operator ==(const LongMessage& rhs) const{
		for(uint16_t i = 0; i < MSGSIZE; ++i){
			if(array[i] != rhs.array[i]){
				return false;
			}
		}
		return true;
	};
	void operator=(const LongMessage& rhs){
		for(uint16_t i = 0; i < MSGSIZE ; ++i){
			this->array[i] = rhs.array[i];
		}
	};
	bool operator<(const LongMessage& rhs) const{
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

class NotificationList{
public:
	bool list[4];
	NotificationList() : list{false,false,false,false}{}
	void operator=(const NotificationList& rhs){
		for(uint8_t i = 0; i < 4 ; ++i){
			this->list[i] = rhs.list[i];
		}
	}
};

class FragmentationTest : public ModuleTesterBase, public BasicPattern {
//	std::ofstream outFile; //Keep writing test results here
	FILE * outFile;

	uint8_t numsend;
	BSTMapT<NodeId_t, NotificationList> sel_dest_and_notifications;

	bool isMsgReceived;
	FMessageId_t assignedMsgId;
	bool isMsgIdAssigned;

	PAL::TimerDelegate *startTesttimerDel;
	PAL::Timer *startTestTimer;

	PAL::TimerDelegate *addDesttimerDel;
	PAL::Timer *addDestTimer;

	PAL::TimerDelegate *removeDesttimerDel;
	PAL::Timer *removeDestTimer;


	PAL::TimerDelegate *replacePayloadtimerDel;
	PAL::Timer *replacePayloadTimer;

	PAL::TimerDelegate *endTesttimerDel;
	PAL::Timer *endTestTimer;


	bool IsRxMsgValid(void *data, uint16_t size );

	void Send(uint32_t event);
	void ReceiveMessageEvent(FMessage_t& msg);

	void InitializeTestVariables();
	void SelectAllDestinationNodes();

	void CheckTestResults(uint32_t event);

	void AddDest(uint32_t event);
	void RemoveDest(uint32_t event);
	void ReplacePayload(uint32_t event);



protected:

	virtual void DataStatusEvent(DataStatusParam ntfy);

	void Handle_AddDestinationResponse(ControlResponseParam response);
	void Handle_ReplacePayloadResponse(ControlResponseParam response);
	void Handle_CancelDataResponse(ControlResponseParam response);

public:
	FragmentationTest();
	virtual ~FragmentationTest();



	void Execute(RuntimeOpts *opts);



};

}
#endif /* TESTS_FRAMEWORK_FRAGMENTATIONTEST_H_ */
