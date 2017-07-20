////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef FI_TEST_H_
#define FI_TEST_H_

// extern bool DBG_TEST; // set in Lib/Misc/Debug.cpp
//#include <stdlib.h>
#include <iostream>

#include <Interfaces/Core/MessageT.h>
#include "Lib/PAL/PAL_Lib.h"
#include <Interfaces/PWI/Framework_I.h>
#include "Framework/PWI/StandardFI.h"
#include <Sys/Run.h>
#include "FITestStats.h"


#define DEBUG_CORE 1

using namespace PWI;

class RandMsg{
public:
  uint64_t src;
  uint64_t dst;
  char msg[256];
  uint16_t seqNo;
  bool acked;

public:
  RandMsg(){
   const char *hex_digits = "0123456789ABCDEF";
   for(int i = 0 ; i < 256; i++ ) {
      msg[i] = hex_digits[ ( rand() % 16 ) ];
   }
  }
  bool CompareMsg(const char *r1){
    bool rtn=false;
    for(int i = 0 ; i < 256; i++ ) {
      if(msg[i] != r1[i]) return rtn;
    }
    return true;
  }
};



class FI_Test{
	FrameworkAttributes fwAttributes;
	uint16_t seqNumber;
	FMessage_t sendMsg;


	UniformRandomInt *rnd;


	PatternId_t pid;
	FMessageId_t msgId;
	uint16_t nonce;

	Timer *msgTimer;
	Timer *apiTimer;
	Timer *logTimer;
	TimerDelegate *logTimerDelegate;
	TimerDelegate *msgTimerDelegate;
	TimerDelegate *apiTimerDelegate;


	RecvMessageDelegate_t *recvDelegate; 
	DataflowDelegate_t *dataNotifierDelegate;
	ControlResponseDelegate_t *controolResponceDelegate;
	NeighborDelegate* nbrDelegate;

	PWI::StandardFI *fi;


	//for validation

	MemMap<SendDataStat> SendStat;
	MemMap<ApiDataStat> ApiStat;
	MemMap<CancelDataStat> CancelStat;
	MemMap<ReplaceDataStat> ReplaceStat;
	MemMap<AddDataStat> AddStat;
	MemMap<AckDataStat> AckStat;
	
	uint16_t send_count;
	uint16_t recv_count;
	uint16_t total_canceled;
	uint16_t total_added;
	uint16_t total_replaced;
	uint16_t total_replace_send;
	uint16_t total_replaced_recv; 
	uint16_t cancel_send_count;
	uint16_t cancel_send_count_decrement;
	uint16_t dstNack_count;
	uint16_t dstAck_count;
	uint16_t sendAck_count;
	uint16_t total_replaced_wfnack;
	uint16_t total_replaced_dstnack;


	BSTMapT  <FMessageId_t , uint16_t> dst_nack_count;
	BSTMapT  <FMessageId_t , uint16_t> dst_ack_count;
	BSTMapT  <FMessageId_t , uint16_t> dest_added_ack_count;
	BSTMapT  <FMessageId_t , uint16_t> sendout_count;
	BSTMapT  <FMessageId_t , uint16_t> ack_fw_sent;
	BSTMapT  <FMessageId_t , uint16_t> ack_wf_recv;
	BSTMapT  <FMessageId_t , ListT<NodeId_t,false, EqualTo<NodeId_t> > > replaced_dst;

public:

	FI_Test();
	~FI_Test();
	void Execute(RuntimeOpts *opts);
	void StartMessaging(double runTime);

	void LocalMsgTimer_Handler(uint32_t event);
	void FrameworkApiTimer_Handler(uint32_t event);
	void LogTimer_Handler(uint32_t event);
	void DataStatus_Handler(DataStatusParam& ack);
	void Recv_Handler(FMessage_t& msg);
	void Control_Handler(ControlResponseParam param);
	void NeighborUpdateEvent(NeighborUpdateParam param);

};

#endif /* FI_TEST_H_ */
