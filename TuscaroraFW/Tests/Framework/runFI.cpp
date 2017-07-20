////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include "runFI.h"
#include "Sys/Run.h"
#include "Tests/FW_Init.h"

#include <string.h>

// extern bool DBG_PATTERN; // set in Lib/Misc/Debug.cpp
// extern bool DBG_WAVEFORM; // set in Lib/Misc/Debug.cpp
// extern bool DBG_CORE; // set in Lib/Misc/Debug.cpp
// extern bool DBG_CORE_ESTIMATION; // set in Lib/Misc/Debug.cpp
// extern bool DBG_WF_EVENT; // set in Lib/Misc/Debug.cpp
// extern bool DBG_TEST; // set in Lib/Misc/Debug.cpp

bool cancel;
bool add;
bool replace;
bool normal;
bool broadcast;
bool unicast;
uint16_t total_valid_replaced;
uint16_t destCount =0;
FI_Test::FI_Test():SendStat("senddata.bin"), ApiStat("apistat.bin"), CancelStat("cancel.bin"), ReplaceStat("replace.bin"), AddStat("add.bin"), AckStat("ackstat.bin"){
	Debug_Printf(DBG_TEST,"FI_Test:: Constructor:  ....\n");
	pid = 1;
	nonce = 123;
	send_count = 0;
	recv_count = 0;
	total_added = 0;
	total_canceled = 0;
	total_replaced = 0;
	this->total_replace_send = 0;
	this->total_replaced_recv = 0;
	cancel_send_count = 0;
	cancel_send_count_decrement = 0;
	dstAck_count = 0;
	dstNack_count = 0;
	sendAck_count =0;
	total_valid_replaced =0;
	total_replaced_wfnack =0;
	total_replaced_dstnack =0;
	//   //DBG_WAVEFORM=true; // set in Lib/Misc/Debug.cpp


	fi = static_cast<StandardFI *> (&GetFrameworkInterface());
	//fi->RegisterDelegates(1,RecvDelegate, nbrDelegate,dataNotifierDelegate,controlDelegate);

	//PolicyManager::SetLinkSelectionPolicy(Core::Policy::PREFFERED_WAVEFORM, 2);
	rnd = new UniformRandomInt(9000,11000);

	Debug_Printf(DBG_TEST,"FI_Test:: construction Done. HOGE \n");fflush(stdout);
}
FI_Test::~FI_Test(){
	Debug_Printf(DBG_TEST,"I am in the FI_Test::~FI_test.  Destructor\n");
}


void FI_Test::Control_Handler(ControlResponseParam param){
   Debug_Printf(DBG_TEST, "FI_Test::Control message\n");
   switch(param.type){
      
       case PTN_RegisterResponse:
       {
           break;
       }
       case PTN_AddDestinationResponse:
       {
    	   total_added = 0;
    	   Debug_Printf(DBG_TEST, "FI_Test:: Received AddDestinationResponse\n");
    	   AddDestinationResponse_Data * data = (AddDestinationResponse_Data*) param.data;
    	   for(uint16_t i =0; i < data->noOfDest ; i ++){
    		   if(data->status == true){
    			   this->total_added++; //= data->noOfDest;
    			   Debug_Printf(DBG_TEST, "FI_Test:: Successfully added node. total is %d\n",total_added);
    		   }else{
    			   Debug_Printf(DBG_TEST, "FI_Test:: Failed to add node\n");
    		   }
    	   }
    	   AddDataStat obj;
    	   obj.total_node_added = total_added;
    	   obj.total_recv = 0;//recv_count;
    	   obj.total_destSize = destCount;
    	   obj.total_nack = 0;
    	   Debug_Printf(DBG_TEST, "FI_TEST:: destCount is %d, nack %d\n",destCount, obj.total_nack);
    	   AddStat.addRecord(obj);
    	   break;
       }
       case PTN_ReplacePayloadResponse:
       {
    	    Debug_Printf(DBG_TEST, "FI_Test:: Received ReplacePayloadResponse\n");
    	    ReplacePayloadResponse_Data* data = (ReplacePayloadResponse_Data*)param.data;
    	    Debug_Printf(DBG_TEST, "FI_Test:: Show status %d  \n",data->status);
    	    Debug_Printf(DBG_TEST, "FI_Test:: Show result: no of nodes in response is %d\n",data->noOfDest);
    		ListT<NodeId_t,false, EqualTo<NodeId_t> >  list;

            for(uint16_t index =0; index < data->noOfDest ; index++){
            	Debug_Printf(DBG_TEST, "FI_Test:: Node %d :: Status %d\n",data->destArray[index],data->replace_status[index]);
            	if(data->replace_status[index] == true){
            		this->total_replace_send += 1;
            		total_valid_replaced += 1;
            		Debug_Printf(DBG_TEST, "FI_Test::Successfully replaced payload of node %d. total_valid_replace %d\n",data->destArray[index],total_valid_replaced);
            		list.InsertBack(data->destArray[index]);
            		replaced_dst[data->msgId] = list;
            	}
            }
            Debug_Printf(DBG_TEST, "FI_Test:: List size is %d\n",list.Size());

            Debug_Printf(DBG_TEST, "FI_Test:: list at %p, replaced_dst list %p\n",&list, &(replaced_dst[data->msgId]));
            ReplaceDataStat obj;
            obj.total_replaced_recv = this->total_replaced_recv;
            obj.total_replaced_send = total_valid_replaced; //this->total_replace_send;
            ReplaceStat.addRecord(obj);
            //this->total_replaced += data->noOfDest;
            break;
       }
       case PTN_CancelDataResponse:
       {
    	   Debug_Printf(DBG_TEST, "FI_Test:: Received CancelDataResponse\n");
    	   CancelDataResponse_Data * data = (CancelDataResponse_Data*) param.data;
    	   Debug_Printf(DBG_TEST, "FI_Test:: Show result: num of nodes in response is %d\n",data->noOfDest);
    	   for(uint16_t index =0; index < data->noOfDest ; index++){
             Debug_Printf(DBG_TEST, "FI_Test:: Node %d :: Status %d\n",data->destArray[index],data->cancel_status[index]);
             if(data->cancel_status[index] == true){
            	 this->total_canceled++;
            	 CancelDataStat obj;
            	 obj.total_recv = this->recv_count;
            	 obj.total_sent = this->cancel_send_count;
            	 obj.total_cancled = this->total_canceled;
            	 CancelStat.addRecord(obj);
            	 Debug_Printf(DBG_TEST, "FI_Test:: Total canceled: %d\n",obj.total_cancled);
             }else{
            	 Debug_Printf(DBG_TEST, "FI_Test:: Total canceled: %d\n",total_canceled);
             }
           }
    	   break;
       }
       case   PTN_SelectDataNotificationResponse:
       {
    	   Debug_Printf(DBG_TEST, "FI_Test:: Received SetDataNotificationResponse\n");
           DataStatusResponse_Data* data = (DataStatusResponse_Data*)param.data;
           delete(data);
           break;
       }
       case   PTN_SetLinkThresholdResponse:
       {
           break;
       } 
       case   PTN_AttributeResponse:
       {
    	   Debug_Printf(DBG_TEST, "FI_Test:: Received Framework Attribute Response\n");
           fwAttributes = *(FrameworkAttributes*) param.data;
           break;
       }
   }


}
void FI_Test::NeighborUpdateEvent(NeighborUpdateParam param){
   //Debug_Printf(DBG_TEST,"FI_Test::NeighborUpdate message\n");
}
void FI_Test::Execute(RuntimeOpts *opts){
  
  string pargs = opts->patternArgs;
  if (!pargs.compare("Replace")){
    Debug_Printf(DBG_TEST, "\n\nFI_Test:: Running the replace interface test\n");
  }
  else if (!pargs.compare("Cancel")){
    Debug_Printf(DBG_TEST, "\n\nFI_Test:: Running the Cancel interface test\n");
  }
  
  dataNotifierDelegate = new DataflowDelegate_t(this, &FI_Test::DataStatus_Handler);
  recvDelegate = new RecvMessageDelegate_t(this, &FI_Test::Recv_Handler);
  controolResponceDelegate = new ControlResponseDelegate_t(this,&FI_Test::Control_Handler);
  nbrDelegate = new NeighborDelegate (this, &FI_Test::NeighborUpdateEvent);
 
  fi->RegisterDelegates(pid,recvDelegate, nbrDelegate,dataNotifierDelegate,controolResponceDelegate); 

  StartMessaging(opts->runTime);
  Debug_Printf(DBG_TEST,"FI_TEST:: Execute....fuga \n");
}


//Pick a neighbor, send generate a random message, send to neighbor, wait for knowledgement
void FI_Test::StartMessaging(double runTime){
	runTime= runTime*1000000; //Convert to microseconds
	logTimerDelegate = new TimerDelegate(this, &FI_Test::LogTimer_Handler);
	logTimer = new Timer(runTime-100, ONE_SHOT, *logTimerDelegate);;
	logTimer->Start();
	
	msgTimerDelegate = new TimerDelegate(this, &FI_Test::LocalMsgTimer_Handler);
	uint16_t delay = rnd->GetNext();
	Debug_Printf(DBG_TEST, "FI_Test:: delay is %d\n",delay);
	msgTimer = new Timer(1000000+delay, PERIODIC, *msgTimerDelegate);
	apiTimerDelegate = new TimerDelegate(this, &FI_Test::FrameworkApiTimer_Handler);
	apiTimer = new Timer(100, ONE_SHOT, *apiTimerDelegate);;
	//uses small value for broadcast test. 20
	//unicast test  1000

	//msgTimer = new Timer(10000, ONE_SHOT, *timerDelegate);
	msgTimer->Start();
	fi->FrameworkAttributesRequest(pid);
	Debug_Printf(DBG_TEST,"FI_TEST:: started timer \n");
	
}

void FI_Test::LogTimer_Handler(uint32_t event)
{
	Debug_Printf(DBG_TEST,"FI_TEST::LogTimer_Handler:: Print Stats to StdOut \n");
	Debug_Printf(DBG_TEST,"Total packets sent: %u\n",send_count);
	Debug_Printf(DBG_TEST,"Total packets received: %u\n",recv_count );
	Debug_Printf(DBG_TEST,"Total calls to add api: %u\n",total_added );
	Debug_Printf(DBG_TEST,"Total calls to cancel api: %u\n",total_canceled );
	Debug_Printf(DBG_TEST,"Total calls to replace api: %u\n",total_replaced );
	Debug_Printf(DBG_TEST,"Total replace_sent: %u\n",total_replace_send );
	Debug_Printf(DBG_TEST,"Total replaced_recv:%u\n",total_replaced_recv );
	Debug_Printf(DBG_TEST,"Total cancel packet count %u\n",cancel_send_count );
	//Debug_Printf(DBG_TEST,"",cancel_send_count_decrement );
	Debug_Printf(DBG_TEST,"Total dest ack count: %u\n",dstAck_count );
	Debug_Printf(DBG_TEST,"Total dest Nack count: %u\n",dstNack_count );
	Debug_Printf(DBG_TEST,"Total send ack count: %u\n",sendAck_count );
	Debug_Printf(DBG_TEST,"Total valid replaced: %u\n",total_valid_replaced );
	Debug_Printf(DBG_TEST,"Total replaced_wfnack: %u\n",total_replaced_wfnack );
	Debug_Printf(DBG_TEST,"Total replaced packets acked: %u\n",total_replaced_dstnack);

	//ackunicastStat.addRecord(u_summary);
    //broadcastStat.addRecord(b_summary);
	
}


//void FI_Test::SendAck_Handler(MessageAckParam_n64_t& ack){
void FI_Test::DataStatus_Handler(DataStatusParam& ack)
{
	Debug_Printf(DBG_PATTERN,"FI_Test:: Recv DataNotification, for nonce %d, for %d destinations\n",ack.nonce, ack.noOfDest);
	bool fw_recv = false;
	bool wf_recv = false;
	bool fw_sent = false;
	cancel_send_count =0;
	cancel_send_count_decrement = 0;
	//for (uint16_t index=0; index< ack.noOfDest; index++){
	uint16_t index =0;
	if (ack.messageId == 0){
		Debug_Printf(DBG_PATTERN, "FI_Test:: Error: Framework refused my packet with nonce: %d, returned message ID 0\n",ack.nonce);
		///Handle error;
		return;
	}
	
	do{
		switch(ack.statusType[index])
		{
			case PDN_ERROR_PKT_TOOBIG: {
				Debug_Printf(DBG_PATTERN, "FI_Test:: Error: Framework refused my packet with nonce: %d, packet size was too big\n",ack.nonce);
				break;
			}
			case PDN_ERROR:{
				break;
			}
			case PDN_FW_RECV:
			{
				
				if(ack.statusValue[index] == true){
					cancel_send_count++;
					Debug_Printf(DBG_PATTERN, "FI_Test:: Show positive PDN_FW_RECV count %d\n",cancel_send_count);
					fw_recv = true;
					
				}else {
					Debug_Printf(DBG_PATTERN, "FI_Test:: Error: Framework refused my packet with nonce: %d, for node: %d\n",ack.nonce, ack.destArray[index]);
				}
				break;
			}
			case PDN_WF_RECV:{
				Debug_Printf(DBG_PATTERN, "FI_Test::Received PDN_WF_RECV for Node: %d\n",ack.destArray[index]);
				if(ack.statusValue[index] == true){
					// send_count++;
				}else{
					BSTMapT <FMessageId_t , ListT<NodeId_t,false, EqualTo<NodeId_t> > >::Iterator it_replaced = replaced_dst.Find(ack.messageId);
					if(it_replaced == replaced_dst.End()){
						Debug_Printf(DBG_PATTERN, "FI_Test:: msgId not found\n");
					}else{
						//ListT<NodeId_t,false, EqualTo<NodeId_t> > list = it_replaced->Second();
						//bool found = false;
						Debug_Printf(DBG_PATTERN, "Msgid %d size %d\n", ack.messageId, it_replaced->Second().Size());
						for(uint16_t i =0; i < it_replaced->Second().Size(); i++){
							Debug_Printf(DBG_PATTERN, "FI_Test:: list[%d] is %d\n",i,it_replaced->Second().GetItem(i));
							if(it_replaced->Second().GetItem(i) == (ack.destArray[index])){
								//found = true;
								if(it_replaced->Second().DeleteItem(ack.destArray[index])){
									total_valid_replaced--;
									Debug_Printf(DBG_PATTERN, "FI_Test:: msgId %d dest %d payload was replaced but received wf_nack. total_valid_replaced adjusted to %d\n",ack.messageId, ack.destArray[index],total_valid_replaced);

								}

								break;
							}
						}

							Debug_Printf(DBG_PATTERN, "FI_Test:: msgId %d dest %d payload was replaced but received wf_nack. total_valid_replaced adjusted to %d\n", ack.messageId, ack.destArray[index],total_valid_replaced);
					}
					cancel_send_count_decrement++; //framework reject this destination
					Debug_Printf(DBG_PATTERN, "FI_Test:: Show cancel_send_count %d\n",cancel_send_count);
					wf_recv = true;
				}
				break;
			}
			case PDN_WF_SENT:
			{
				Debug_Printf(DBG_PATTERN, "FI_Test::Received PDN_WF_SENT for Node: %d\n",ack.destArray[index]);
				if(ack.statusValue[index] == true){
					sendAck_count++;
					fw_sent = true;
				}
				break;
			}
			case PDN_DST_RECV:
			{
				Debug_Printf(DBG_PATTERN, "FI_Test::Received PDN_DST_RECV for Node: %d\n",ack.destArray[index]);
				if(ack.statusValue[index] == true){
					Debug_Printf(DBG_PATTERN, "FI_Test:: This is Positive DST acknowledgement\n");
					send_count++;
					if(ack.destArray[index] == 4 ||ack.destArray[index] == 5 || ack.destArray[index] == 6){
						Debug_Printf(DBG_PATTERN, "FI_Test:: Positive DST ack for newly added nodes\n");
						dstAck_count++;
						}
				}else{
					Debug_Printf(DBG_PATTERN, "FI_Test:: This is Negative DST acknowledgemenet\n");
					this->dstNack_count++;
					BSTMapT <FMessageId_t , ListT<NodeId_t,false, EqualTo<NodeId_t> > >::Iterator it_replaced = replaced_dst.Find(ack.messageId);
					if(it_replaced == replaced_dst.End()){
							Debug_Printf(DBG_PATTERN, "FI_Test:: msgId not found\n");
					}else{
						Debug_Printf(DBG_PATTERN, "Msgid %d size %d , %p\n", ack.messageId, it_replaced->Second().Size(), &(it_replaced->Second()));
							//ListT<NodeId_t,true, EqualTo<NodeId_t> >* list = &it_replaced->Second();
						for(uint16_t i =0; i < it_replaced->Second().Size(); i++){
							Debug_Printf(DBG_PATTERN, "FI_Test:: list[%d] is %d\n",i,it_replaced->Second().GetItem(i));
								if(it_replaced->Second().GetItem(i) == ack.destArray[index])
								{
									total_valid_replaced--;
									it_replaced->Second().DeleteItem(ack.destArray[index]);
									Debug_Printf(DBG_PATTERN, "FI_Test:: msgId %d dest %d payload was replaced but received wf_nack. total_valid_replaced adjusted to %d\n",ack.messageId, ack.destArray[index],total_valid_replaced);
								}
						}

					}
				}
				break;
			}
			case PDN_BROADCAST_NOT_SUPPORTED:
			{
				break;
			}
		}//close swtich
		index++;
	}while(index< ack.noOfDest);

	uint16_t total_dst_nack = 0;
	if(!broadcast){
		this->dst_ack_count[ack.messageId] = send_count;
		//Debug_Printf(DBG_PATTERN, "FI_Test:: dst_ack_count[%d] is  %d\n",ack.messageId, send_count);
		//Debug_Printf(DBG_PATTERN, "FI_Test::Show destination contained in this acknowledgment message\n");
		msgId = ack.messageId;
		Debug_Printf(DBG_PATTERN,"FI_Test::Received message id is %d\n",ack.messageId);
		SendDataStat obj;
		obj.total_sent = 0;
		
		BSTMapT <FMessageId_t , uint16_t>::Iterator it_ack = dst_ack_count.Begin();
		while(it_ack != dst_ack_count.End()){
			obj.total_sent += it_ack->Second();
			//Debug_Printf(DBG_PATTERN, "FI_Test:: messageId %d got %d PDN_DST_RECV\n",it_ack->First(), it_ack->Second());
			++it_ack;
		}
		Debug_Printf(DBG_PATTERN, "FI_Test:: Total PDN_DST_RECV is %d\n",obj.total_sent);
		
		obj.total_recv = recv_count;
		SendStat.addRecord(obj);
		send_count = 0;

		//fidn out dst nack  count
		this->dst_nack_count[ack.messageId] = dstNack_count;
		
		msgId = ack.messageId;
		Debug_Printf(DBG_PATTERN,"FI_Test::Received message id is %d\n",ack.messageId);
		BSTMapT <FMessageId_t , uint16_t>::Iterator it_dst_nack = dst_nack_count.Begin();
		while(it_dst_nack != dst_nack_count.End()){
			total_dst_nack += it_dst_nack->Second();
			//Debug_Printf(DBG_PATTERN, "FI_Test:: messageId %d got %d PDN_DST_RECV\n",it_dst_nack->First(), it_dst_nack->Second());
			++it_dst_nack;
		}
		//Debug_Printf(DBG_PATTERN, "FI_Test:: Total Negative PDN_DST_RECV is %d\n",total_dst_nack);
		//dstNack_count = 0;



		//find out dst ack for added node without duplicates
		this->dest_added_ack_count[ack.messageId] = dstAck_count;
		AddDataStat add_obj;
		add_obj.total_nack = 0;
		BSTMapT <FMessageId_t , uint16_t>::Iterator it_addedack = dest_added_ack_count.Begin();
		while(it_addedack != dest_added_ack_count.End()){
		add_obj.total_nack += it_addedack->Second();
		//Debug_Printf(DBG_PATTERN, "FI_Test:: ADD messageId %d got %d PDN_DST_RECV\n",it_addedack->First(), it_addedack->Second());
		++it_addedack;
		}
		//Debug_Printf(DBG_PATTERN, "FI_Test:: Total PDN_DST_RECV for additional nodes are %d\n",add_obj.total_nack);
		add_obj.total_recv = obj.total_sent;
		add_obj.total_node_added = total_added;
		add_obj.total_destSize = destCount;
		add_obj.total_nack = total_dst_nack;
		//Debug_Printf(DBG_PATTERN, "FI_TEST:: destCount is %d, nack is %d\n",destCount,add_obj.total_nack);
		AddStat.addRecord(add_obj);
		dstAck_count =0;
		dstNack_count = 0;
	}
	
	//find out number of wf_nack and fw_ack
	if(wf_recv || fw_recv ||fw_sent){
	CancelDataStat cancel;
	cancel.total_sent = 0;
	cancel.total_wf_nack = 0;
	cancel.total_sent_out = 0;
	if(fw_recv){
		this->ack_fw_sent[ack.messageId] = cancel_send_count;
	}
	if(wf_recv){
		this->ack_wf_recv[ack.messageId] = cancel_send_count_decrement;
	}

	BSTMapT <FMessageId_t , uint16_t>::Iterator it_fwack = ack_fw_sent.Begin();
	while(it_fwack != dst_ack_count.End()){
		cancel.total_sent += it_fwack->Second();
		//Debug_Printf(DBG_PATTERN, "FI_Test:: FW ACK :: messageId %d got %d PDN_FW_RECV\n",it_fwack->First(), it_fwack->Second());
		++it_fwack;
	}
	//Debug_Printf(DBG_PATTERN, "FI_Test:: Total PDN_FW_RECV is %d\n",cancel.total_sent);
	BSTMapT <FMessageId_t , uint16_t>::Iterator it_wfack = ack_wf_recv.Begin();
	while(it_wfack != dst_ack_count.End()){
		cancel.total_wf_nack += it_wfack->Second();
		//Debug_Printf(DBG_PATTERN, "FI_Test:: messageId %d got %d PDN_WF_RECV\n",it_wfack->First(), it_wfack->Second());
		++it_wfack;
	}
	//Debug_Printf(DBG_PATTERN, "FI_Test:: Total Negative PDN_WF_RECV is %d\n", cancel.total_wf_nack);
	//find out number of wf sent out ack
	this->sendout_count[ack.messageId] = sendAck_count;
	BSTMapT <FMessageId_t , uint16_t>::Iterator it_sendack = sendout_count.Begin();
	while(it_sendack != sendout_count.End()){
		cancel.total_sent_out += it_sendack->Second();
		//Debug_Printf(DBG_PATTERN, "FI_Test:: messageId %d got %d PDN_WF_SENT\n",it_sendack->First(), it_sendack->Second());
		++it_sendack;
	}
	//Debug_Printf(DBG_PATTERN, "FI_Test:: Total PDN_WF_SENT is %d\n", cancel.total_sent_out);
	sendAck_count =0;
	cancel.total_recv = this->recv_count;
	cancel.total_cancled = this->total_canceled;
	cancel.total_dst_ack = total_dst_nack;
	CancelStat.addRecord(cancel);
	}


	///replaced
	/* uint16_t total_replaced = 0;
	BSTMapT  <FMessageId_t , ListT<NodeId_t,true, EqualTo<NodeId_t> > >::Iterator it_replaced_count = replaced_dst.Begin();
	while(it_replaced_count != replaced_dst.End()){
	total_replaced += it_replaced_count->Second().Size();
	Debug_Printf(DBG_PATTERN, "Total replaced adjusted to %d \n",total_replaced);
	++it_replaced_count;
	}*/
	ReplaceDataStat replacedobj;
	replacedobj.total_replaced_recv = total_replaced_recv;
	replacedobj.total_replaced_send = total_valid_replaced;//total_replaced;
	ReplaceStat.addRecord(replacedobj);


}

void FI_Test::Recv_Handler(FMessage_t& msg){
	//Debug::PrintTimeMilli();
	Debug_Printf(DBG_TEST, "FI_Test::Received Message\n\n\n");
	Debug_Printf(DBG_TEST, "FI_Test::Got message from node %d, payload size of %d\n",msg.GetSource(), msg.GetPayloadSize());
	Debug_Printf(DBG_TEST, "Show payload:\n")
	uint8_t* payload = msg.GetPayload();
	for(uint16_t index =0; index < 256; index++){
		printf("%x", payload[index]);
	}
    printf("\n");
    bool all_zero = true;
    for(uint16_t index =0; index < 256; index++){
    		if(index > 1 && payload[index] != 0){
    			all_zero = false; //replaced payload should be all zero.
    		}
    }
    if(all_zero == true){
    	Debug_Printf(DBG_TEST, "FI_Test:: Recieved packt all zeros\n");
    	this->total_replaced_recv++;
    	ReplaceDataStat obj;
    	obj.total_replaced_recv = total_replaced_recv;
    	obj.total_replaced_send = total_valid_replaced;//total_replace_send;
    	ReplaceStat.addRecord(obj);
    }


    recv_count++;
    SendDataStat obj;
    obj.total_sent = send_count;
    obj.total_recv = recv_count;
    SendStat.addRecord(obj);

    CancelDataStat cancel;
    cancel.total_recv = this->recv_count;
    cancel.total_sent = this->cancel_send_count;
    cancel.total_cancled = this->total_canceled;
    cancel.total_wf_nack = this->cancel_send_count_decrement;
    CancelStat.addRecord(cancel);

/*
	AddDataStat addobj;
	addobj.total_node_added = total_added;
	addobj.total_recv = recv_count;
	addobj.total_nack = 0;
	addobj.total_destSize = destCount;
	Debug_Printf(DBG_TEST, "FI_TEST:: destCount is %d\n",destCount);
	AddStat.addRecord(addobj);
*/
    //delete(&msg);  Do not delete now. This is deleted in PacketHandler

	//PrintMsg(msg);
}

void FI_Test::LocalMsgTimer_Handler(uint32_t event)
{
	if(1){//MY_NODE_ID == 0 || MY_NODE_ID == 2){
		//fi->FrameworkAttributesRequest(pid);
		Debug_Printf(DBG_TEST, "FI_Test:: Entered message timer handler\n");fflush(stdout);
		FMessage_t sendMsg(256);
		if(sendMsg.GetPayload() == NULL){
			Debug_Printf(DBG_TEST, "FI_Test:: Returned pointer is null\n");fflush(stdout);
		}else{
			Debug_Printf(DBG_TEST, "FI_Test Address of payload is %p\n",sendMsg.GetPayload());fflush(stdout);
		}
		memset(sendMsg.GetPayload(), 0, 256);
		RandMsg *storeMsg = new RandMsg();
		storeMsg->seqNo = seqNumber;
		RandMsg *sndPtr = (RandMsg*) sendMsg.GetPayload();
		sndPtr->src = MY_NODE_ID;
		sndPtr->seqNo=seqNumber;
		Debug_Printf(DBG_TEST, "FI_Test::LocalMsgTimer_Handler: %d event: Sending Message %d to node %lu \n",event, seqNumber, sndPtr->dst);
		
		memcpy(sndPtr->msg,storeMsg->msg,256);
		sendMsg.SetPayloadSize(sizeof(class RandMsg));
		Debug_Printf(DBG_TEST,"FI TEST::Before transmit\n");

		int destsize=MAX_DEST;
		if(add){
			destsize=1;
			this->msgTimer->Suspend();
		}
		destCount = destCount + destsize;
		Debug_Printf(DBG_TEST, "FI_TEST:: destCount is updated to %d\n",destCount);
		NodeId_t* destArray = new uint16_t[destsize];
		for (int j=0; j<destsize; j++) {
				destArray[j] = j+1;
		}
		if(unicast){
			Debug_Printf(DBG_TEST, "TestCore::Nonce is %d, payload address is %p\n",nonce,sendMsg.GetPayload());
			fi->SendData (pid, destArray, (uint16_t) destsize, sendMsg,nonce++);
			//fi->SendData (pid, destArray, (uint16_t) destsize, sendMsg,nonce++);
			//fi->SendData (pid, destArray, (uint16_t) destsize, sendMsg,nonce++);
			//fi->SendData (pid, destArray, (uint16_t) destsize, sendMsg,nonce++);
		}
#if ENABLE_FW_BROADCAST==1		
		else
		{
			for (int i=0; i < fwAttributes.numberOfWaveforms; i++){
				WaveformId_t wid= fwAttributes.waveformIds[i];
				Debug_Printf(DBG_TEST,"FI_Test:: Sending Broadcast on waveform %d\n", wid);
				fi->BroadcastData(pid,sendMsg,wid,nonce++);
				//usleep(50);
			}
		}
#endif		
		if(!normal){
			apiTimer->Start();
			Debug_Printf(DBG_TEST,"Starting apiTimer\n");
		}
	}
	seqNumber++;
}


void FI_Test::FrameworkApiTimer_Handler(uint32_t event){
	Debug_Printf(DBG_TEST, "FI_Test:: Sending FrameworkApiRequest\n");
	// ReplacePayload test
	if(replace){
		Debug_Printf(DBG_TEST, "FI_Test:: Sending ReplacePayload\n");
		FMessage_t sendMsg(256);
		if(sendMsg.GetPayload() == NULL){
			Debug_Printf(DBG_TEST,"Returned pointer is null\n");fflush(stdout);
		}else{
			Debug_Printf(DBG_TEST,"Address of replacepayload is %p\n",sendMsg.GetPayload());fflush(stdout);
		}
		memset(sendMsg.GetPayload(), 0, 256);
		RandMsg *storeMsg = new RandMsg();
		storeMsg->seqNo = seqNumber;
		RandMsg *sndPtr = (RandMsg*) sendMsg.GetPayload();
		sndPtr->src = MY_NODE_ID;
		sndPtr->seqNo=seqNumber;
		//uint8_t* data = (uint8_t*) sendMsg.GetPayload();
		//memcpy(sndPtr->msg,storeMsg->msg,256);
		Debug_Printf(DBG_TEST, "Show payload before calling replacepayloadrequest\n");
		//for(uint16_t index =0; index < 256; index++){
		//	  printf("%x",data[index]);
		//}
		sendMsg.SetPayloadSize(sizeof(class RandMsg));
		fi->ReplacePayloadRequest(pid,msgId,sendMsg.GetPayload(), sendMsg.GetPayloadSize());
	}

	if(cancel){
		Debug_Printf(DBG_TEST, "FI_Test:: Sending cancelDataRequest\n");
		//cancelDatarequest test
		uint16_t size =3;
		NodeId_t cancel_dest[size];
		for(uint16_t index= 0; index < size; index++){
			cancel_dest[index]= index+1;
		}
		fi->CancelDataRequest(pid,msgId,cancel_dest,size);
	}
	if(add){
		//adddestination
		uint16_t size =3;
		NodeId_t add_dest[size];
		for(uint16_t index= 0; index < size; index++){
			add_dest[index]= index+4;
		}
		fi->AddDestinationRequest(pid,msgId,add_dest,size);
	}
}

int main(int argc, char* argv[]) {
  //Make sure RuntimeOpts construction is the very first line of main
  RuntimeOpts opts ( argc-1, argv+1 );

	
	InitPlatform(&opts);
  
	add = false;
	cancel = false;
	replace = false;
	normal = false;
	broadcast = false;
	if(opts.patternArgs == "cancel"){
		Debug_Printf(DBG_TEST, "FI_Test:: Run cancelDest test\n");
		cancel= true;
	}
	else if(opts.patternArgs == "add"){
		Debug_Printf(DBG_TEST, "FI_Test:: Run addDest test\n");
	add = true;
	}
	else if(opts.patternArgs == "replace"){
		Debug_Printf(DBG_TEST, "FI_Test:: Run replace payload test\n");
		replace = true;
	}else if(opts.patternArgs == "normal"){
		Debug_Printf(DBG_TEST, "FI_Test:: Run unicast test\n");
		normal = true;
		unicast = true;
	}
	else if(opts.patternArgs == "bcancel"){
		Debug_Printf(DBG_TEST, "FI_Test:: Run cancelDest test with broadcast\n");
		cancel= true;
		broadcast = true;
	}
	else if(opts.patternArgs == "badd"){
		Debug_Printf(DBG_TEST, "FI_Test:: Run addDest test with broadcast\n");
		add = true;
		broadcast = true;
	}
	else if(opts.patternArgs == "breplace"){
		Debug_Printf(DBG_TEST, "FI_Test:: Run replace payload test with broadcast\n");
		replace = true;
		broadcast = true;
	}
	else if(opts.patternArgs == "bnormal"){
		Debug_Printf(DBG_TEST, "FI_Test:: Run basic test with broadcast\n");
		normal = true;
		broadcast = true;
	}
	else {
		normal = true;
		unicast = true;
	}
	FW_Init fwInit;
	fwInit.Execute(&opts);

	Debug_Printf(DBG_TEST, "FI_Test:: Creating FI_Test object I am here. RunTime:: %d\n",runTime);fflush(stdout);
	FI_Test ftest;
	ftest.Execute ( &opts );

	RunTimedMasterLoop();
	return 0;
}
