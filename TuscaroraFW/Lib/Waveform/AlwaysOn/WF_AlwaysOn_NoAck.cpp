////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 




#ifndef Debug_Flag
#define Debug_Flag 1
#endif

#include "WF_AlwaysOn_NoAck.h"
#include "Lib/Misc/Debug.h"
#include "Framework/Core/Naming/StaticNaming.h"
#include "Framework/Core/Estimation/EstBase.h"
#include <Interfaces/Waveform/EventTimeStamp.h>
#include <Sys/netconvert.h>

//Program  implemented using examples  from
//http://austinmarton.wordpress.com/2011/09/14/sending-raw-ethernet-packets-from-a-specific-interface-in-c-on-linux/
///and
///http://yusufonlinux.blogspot.com/2010/11/data-link-access-and-zero-copy.html

#if defined(PLATFORM_LINUX) || defined(PLATFORM_DCE)

#include <Platform/linux/PAL/Logs/MemMap.h>

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <endian.h>

#include <netpacket/packet.h>
#include <netinet/ether.h>
#include <signal.h>
#include <fcntl.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <sstream>  //generate string key for map

#endif


// extern bool DBG_WAVEFORM; // set in Lib/Misc/Debug.cpp
extern NodeId_t MY_NODE_ID;
extern uint16_t NETWORK_SIZE;
extern uint32_t runTime;

extern IO_Signals ioSignals;

namespace Waveform {

extern int32_t sockfd[MAX_WAVEFORMS];
extern Waveform_64b_I* myWFPtr[MAX_WAVEFORMS];


void WF_AlwaysOn_NoAck::DataNotification(WF_DataStatusParam_n64_t ackParam)
{
    dataNotifierEvent->Invoke(ackParam);
}

//energy in mW
WF_AlwaysOn_NoAck::WF_AlwaysOn_NoAck (WaveformId_t _wfID, WF_TypeE _type, WF_EstimatorTypeE _estType, char* _deviceName, float _cost, float _energy)
{
	//validation
	u_summary.negative_dst_recv = 0;
	u_summary.negative_wf_recv = 0;
	u_summary.negative_wf_sent = 0;
	u_summary.positive_dst_recv = 0;
	u_summary.positive_wf_sent = 0;
	u_summary.positive_wf_recv = 0;
	u_summary.total_unicast_sent = 0;
	u_summary.total_unicast_recv = 0;
	u_summary.total_ack = 0;

	b_summary.negative_wf_recv_b = 0;
	b_summary.negative_wf_sent_b = 0;
	b_summary.positive_wf_sent_b = 0;
	b_summary.positive_wf_recv_b = 0;
	b_summary.total_le_broadcast_sent = 0;
	b_summary.total_data_broadcast_sent = 0;
	b_summary.total_discovery_recv = 0;
	b_summary.total_data_recv = 0;

	MAX_AO_PAYLOAD_SIZE = MAX_AO_WF_PACKET_SIZE - WF_Message_n64_t::GetHeaderSize();
	Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn:: Instantiating NoAck waveform\n");
	
	WID = _wfID;
	printf("BUF_SIZE is %d\n",BUF_SIZE);
	newwfMsgId = 0;
	//Set IsBroadcast flag to false;
	IsBroadcast = false;
	//show initial coordinate of node
	Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::(%d , %d) \n",x(),y());
	Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::Address of Ack_Type is %p\n",&Ack_Type);
	//static bool registerEvent = false;
	IsTimerSet = false;
	//WF_Buffer_FULL = false;
	readyToRecv = true;  // Initially waveform can accept packet from framework
	destination_index = 0;
	wfAttrib.wfId = _wfID;
	//DBG_WAVEFORM=true;
	rcv_msg_ptr = new WF_Message_n64_t((uint16_t)(MAX_AO_PAYLOAD_SIZE*2));
	
	wfAttrib.estType = _estType;
	if(_estType == WF_FULL_EST){
		Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn:: Waveform supports link estimation. Framework should not instantiate link estimation\n");
		InstantiateEstimation(DEFAULT_ESTIMATION_PERIOD);
	}else {
		Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn:: Waveform Does NOT supports link estimation. Framework should instantiate link estimation\n");
	}
	
	protocol= ETH_P_C2E1;
	sockID=InitPacSoc(WID, _deviceName, &if_mac, myIfAddress);
#ifdef PLATFORM_DCE
	//uint64_t nid = hton64(MapNodeIdToWaveformAddress(MY_NODE_ID));
	//uint8_t *nidPtr = (uint8_t *)&nid;
	uint64_t hid = NodeIdToNS3MacAddress(MY_NODE_ID);
	//printf("WF_AlwaysOnAck:: host address %lu, net address %lu \n", hid, nid);
	ConvertU64ToAddress(hid,(unsigned char*)myIfAddress);
	//strncpy(myIfAddress, (char*)&nidPtr[2], 6);
#endif
	
	wfAttrib.type = _type;
	wfAttrib.ifindex = if_mac.ifr_ifindex;
	//Masahiro setting other attribues
	wfAttrib.headerSize = WF_HDR_SIZE;
	wfAttrib.maxPayloadSize = MAX_AO_PAYLOAD_SIZE;
	wfAttrib.maxPacketSize = MAX_WF_PKT_SIZE;
	wfAttrib.minPacketSize = MAX_AO_WF_PACKET_SIZE;
	wfAttrib.minInterPacketDelay = 0;
	wfAttrib.channelRate = 11000000; //Assuming a 11Mpbs average sustained rate
	wfAttrib.maxBurstRate = 0;
	wfAttrib.broadcastSupport = false;//true;
	//WF does not support acknowledgement.
	wfAttrib.broadcastSupport = true;
	wfAttrib.destReceiveAckSupport = false;
	wfAttrib.ackTimerPeriod = 10000;
	wfAttrib.receiverTimeStampSupport=true;
	
	//using 802.11n as the base here, from here
	//http://www.cse.buffalo.edu/faculty/dimitrio/publications/pingen12.pdf
	//Wifi transmission takes about 1200-1500mW for a packet.
	//a packet is about 8000 bits -> 1200/8000
	//at 11Mpbs, bitTime is 90.9 ns
	//wfAttrib.energy = MilliWattToLog2PicoJoules(1200, 1000, 90.9);
	wfAttrib.energy = DBmToLog2PicoJoules(_energy,wfAttrib.channelRate);
	myWFPtr[WID] = this;
	
	wfAttrib.cost = _cost;


	printf("WF_AlawaysOn_Ack:: My MAC is :: ");
	PrintMAC((unsigned char*)myIfAddress);
	Debug_Printf(DBG_WAVEFORM,"WF_AlwaysOn::Setting socket to Async worked, using device index: %u\n", wfAttrib.ifindex);	
	//Make socket non blocking
	//MakeSocketNonBlocking(sockfd[wfID]);
	socketDelegate = new SocketSignalDelegate_t(this, &WF_AlwaysOn_NoAck::OnSocketSignal); 
	ioSignals.Register(sockID, socketDelegate);
    
    ///Initialize the timer structures for sending message Acks.
    ackTimerDelegate = new TimerDelegate(this, &WF_AlwaysOn_NoAck::AckTimer_Handler);
    ackTimer = new Timer(20000, ONE_SHOT, *ackTimerDelegate);
	Debug_Printf(DBG_WAVEFORM,"WF_AlwaysOn:: Created ack time at %p \n", ackTimer);	
    
#if Debug_Flag
    uint64_t base = runTime *1000 *1000;
    base = base - 200;
    Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::validation timer base is %ld\n",base);
    validationTimerDelegate = new TimerDelegate(this, &WF_AlwaysOn_NoAck::validationTimer_Handler);
    uint16_t delay = 5 * this->WID;
    //validationTimer = new Timer(99999000 + delay, ONE_SHOT, *validationTimerDelegate);
    //validationTimer = new Timer(29999000 + delay, ONE_SHOT, *validationTimerDelegate);
    validationTimer = new Timer(base + delay, ONE_SHOT, *validationTimerDelegate);
    validationTimer->Start();
#endif
    //debugTimerDelegate = new TimerDelegate(this,&WF_AlwaysOn_NoAck::DebugTimer_Handler);
    //debugTimer = new Timer(50000,ONE_SHOT,*debugTimerDelegate);
    wfAttrib.ifindex = wfAttrib.wfId;

    ///////////////Starting from V09, initialize signals back to framework////////////////////
    dataNotifierEvent = new WF_DataStatusEvent_t(wfAttrib.wfId);
    linkEstimateEvent = new WF_LinkEstimateEvent_t(wfAttrib.wfId);
    recvMsgEvent = new  WF_RcvMessageEvent_t(wfAttrib.wfId);
    controlResponseEvent= new WF_ControlResponseEvent_t(wfAttrib.wfId);

#ifdef PLATFORM_LINUX
	fasade = new WaveformFasade(WID, this);
#endif
		
    Debug_Printf(DBG_WAVEFORM,"WF_AlwaysOn:: Done initializing Always On WF (dce) ...We are good to go.\n");
    //Set Control Status to be NORMAL
    stat = WF_BUFFER_LOW;
}

  
void WF_AlwaysOn_NoAck::PrintMAC(uint8_t *mac){
	//for (int i=5; i>=0; i--){
	for (int i=0; i<6; i++){
		printf("%02X:", mac[i]);
		//Debug_Printf(DBG_WAVEFORM, "%X:", mac[i]);
	}
	//Debug_Printf(DBG_WAVEFORM,"\n");
	printf("\n");
}

  /*bool WF_AlwaysOn_NoAck::SendAll (WF_Message_n64_t& msg, uint16_t payloadSize, WF_MessageId_t msgId, bool ack){
    if(~ack){
      
    }
    return false;
  }*/
  
/*******************************************************************************************
* Send Data.
* AlwaysOn_Dce gets pkt to be send through this method. 
* Send it and sends two types of acknoweldgement
* Current version assumes it has only 1 destination.
**********************************************************************************************/
void WF_AlwaysOn_NoAck::SendData (WF_Message_n64_t& msg, uint16_t size, uint64_t *destArray, uint16_t noOfDestinations, WF_MessageId_t _msgId, bool ack)
{
	rcvFromFwNo++;
	Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn:: SendData: Local count number %lu, Packet msg id %lu\n",rcvFromFwNo, _msgId);fflush(stdout);
	Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn:: msg ptr %p, msg->payload %p\n",&msg, msg.GetPayload());
	msg.SetWaveformMessageID(_msgId);
	//Just in case, check destArray is not null and noOfDestinations is not 0
	if((destArray == NULL) || (noOfDestinations ==0)){
		Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::destArray or noOfDestination is invalid.\n");
		exit(2);
	}
	Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::Starting Send DataMessage Id for this message is %lu\n", msg.GetWaveformMessageID());
	Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::Show message type: %d\n",msg.GetType());
	//Check current status of AlwaysOn_Dce and prepare first acknowledgement
	WF_MessageStatusE Status = WF_ST_SUCCESS;
	//if(WF_Buffer_FULL == false){ 

	/* Comment out testing code
	Debug_Printf(DBG_WAVEFORM,"show current debug_count %d\n",debug_count1);
	//set readyToRecv
	if((debug_count1 %3) == 2){
		readyToRecv = false;
		debug_count1++;
	}*/

	if(readyToRecv == true){ //buffer not full
		//debug_count1++;
		Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::current_messageId.Size is %d\n", current_messageId.Size());
		WF_MessageId_t fwMsgId = _msgId;
		current_messageId.InsertBack(fwMsgId);
		Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::Adding messageId %lu to current_messageId. Now size is %d\n",_msgId, current_messageId.Size());
		//WF_Buffer_FULL = true;
		readyToRecv = false; // AlwaysOnDCE has no buffering
		Debug_Printf(DBG_WAVEFORM,"WF_AlwaysOn::Setting WF_Buffer_FULL to true\n");
		//AlwaysOn_Dce is not handling message right now.
		Status = WF_ST_SUCCESS;  
		//validation
		Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn:: Show msg type %d\n",msg.GetType());
	#if Debug_Flag
		if(msg.GetType() == Types::ACK_REQUEST_MSG){
			u_summary.positive_wf_recv += noOfDestinations;
			Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn:: incrementing positive wf_recv\n");
		}
	#endif

		//Need to add (messageId, address of message) pair to map
		MessageIdToPointer[_msgId]=&msg;
		Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn:: Show number of destination %d\n",noOfDestinations);
		for(int i=0; i < noOfDestinations; i++){
			Ack_Type[destArray[i]] = WDN_WF_RECV;   //Keep track acknowledgement
			MessageStatus[destArray[i]] = Status;
			Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn:: Ack_Type[%ld] is set to %d\n",destArray[i],Ack_Type[destArray[i]]);
		}
		Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::Ack_Type.Size is %d\n",Ack_Type.Size());
	}else{
		//AlwaysOn_Dce cannot accept message to be send now. return Nack
		//Debug_Printf(DBG_WAVEFORM, "WF_Buffer_FULL is true!!\n");
		Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::readyToRecv is false!!\n");
		Status = WF_ST_BUFFER_FULL; //Cannot accept message right now
		for(int i=0; i < noOfDestinations; i++){
			Ack_Type[destArray[i]] = WDN_WF_RECV;
			MessageStatus[destArray[i]] = Status;
		}
		//Just for testing purpose
		//readyToRecv = true;
		//this->debugTimer->Start();

		//validation
	#if Debug_Flag
		if(msg.GetType() == Types::ACK_REQUEST_MSG){
			u_summary.negative_wf_recv += noOfDestinations;
		}
	#endif
	} 

	//Prepare and signal DataNotification to Framework 
	Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn:: Prepare WDN_WF_RECV\n");
	//signal WDN_WF_RECV to framework. First create object to be passed
	//WF_DataStatusParam<uint64_t>param(msg,Status);
	//WF_DataStatusParam<uint64_t>* param = new WF_DataStatusParam<uint64_t>(msg.GetFrameworkMessageID(), msg.GetWaveform(),Status);
	WF_DataStatusParam<uint64_t> param(msg.GetWaveformMessageID(), msg.GetWaveform());

	//Prepare WDN_WF_RECV,
	this->CopyToDN(param);
	//param->WF_Buffer_FULL = WF_Buffer_FULL;
	param.readyToReceive = readyToRecv;
	dataNotifierEvent->Invoke(param);

	//If Status is Buffer_Full, exit
	if(Status ==WF_ST_BUFFER_FULL){
		Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::Exiting from SendData method. Cannot accept incoming message now\n");
		//I think I need to clear Ack_Type here.
		BSTMapT<NodeId_t, WF_DataStatusTypeE>::Iterator it;
		it = Ack_Type.Begin();
		while(it != Ack_Type.End()){ //for loop does not work as we have not it++?
			//Debug_Printf(DBG_WAVEFORM,"Show what it is pointing %d \n", it->First());
			Ack_Type.Erase(it);
		}
		//clear MessageStatus map;
		BSTMapT<NodeId_t, WF_MessageStatusE>::Iterator it_msg = MessageStatus.Begin();
		while(it_msg != MessageStatus.End()){
			MessageStatus.Erase(it_msg);
		}
		return;
	}    
	//store message ID for later use
	current_Id = _msgId;
	//store message Size for later use
	message_size = size;
	//Check Packet Size
	if(size > MAX_AO_PAYLOAD_SIZE){
		last_sent_msg_ptr = &msg;
		lastMsgStatus = WF_ST_PKT_TOO_BIG;
		Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn:: Payload size %d is too big, only payloads upto %d can be sent\n",size, MAX_AO_PAYLOAD_SIZE);
		//return PktTooBig;
		//MessageStatus[_msgId] = PktTooBig;
		msgStat = WF_ST_PKT_TOO_BIG; 
		for(uint16_t i =0; i < noOfDestinations; i++){
			MessageStatus[destArray[i]] = WF_ST_PKT_TOO_BIG;
		}
		return;
	}
		
	Debug_Printf( DBG_WAVEFORM,"WF_AlwaysOn::Send:: Received a mult-desitnation message id %lu from framework %d dests (ptr %p) \n", _msgId, noOfDestinations, destArray);
	msg.SetNumberOfDest(noOfDestinations);
	Debug_Printf(DBG_WAVEFORM, "Size of destList should be 0.  Size: %d\n",destList.Size());
	if(destList.Size() != 0){
		abort();
	}
	//Store all destination into List.
	for(int i=0; i < noOfDestinations; i++){
		destList.InsertBack(destArray[i]);
		Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::destList[%d] = %ld\n",i,destList[i]);
	}
	memcpy(last_sent_msg_dest_ptr,destArray, noOfDestinations*sizeof(uint64_t)); //Copy of destArray
	Debug_Printf( DBG_WAVEFORM,"WF_AlwaysOn::Sending unicast to dest %lu \n", destArray[0]);
	//Send data out  from chanel.
	Unicast(destArray[0],msg,size);


	this->current_destination_ = destArray[0];

	//validation
	#if Debug_Flag
	u_summary.positive_wf_sent++;
	#endif

	Debug_Printf(DBG_WAVEFORM, "Exiting from SendData method\n");
	
}

  
WF_MessageStatusE WF_AlwaysOn_NoAck::Unicast(uint64_t dest, WF_Message_n64_t& msg, uint16_t size){
	//uint64_t deviceId= MapNodeIdToWaveformAddress(dest);
	unsigned char destMAC[6];
	ConvertU64ToAddress(dest, destMAC);
	Debug_Printf( DBG_WAVEFORM,"WF_AlwaysOn:: WFID: %d:: Unicast:: Sending a unicast message to node: %lu, at address: ", wfAttrib.wfId , dest); 
	if(DBG_WAVEFORM()) { PrintMAC((uint8_t*)destMAC); fflush(stdout);}
	Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::Checking noOfDest %d\n",msg.GetNumberOfDest());
	//printf("WF_AlwaysOn::Unicast:: Sending a unicast message to node: %lu, at address: ", dest); PrintMAC((uint8_t*)&deviceId); fflush(stdout);
	WF_MessageStatusE rtn = SendToDevice((uint8_t *)destMAC, msg, size);


	// Record the new status and trigger the data status
	//	SendDataStatus(dest, rtn, WDN_WF_SENT);
	Ack_Type[dest] = WDN_WF_SENT;
	MessageStatus[dest] = rtn;
	WF_DataStatusParam<uint64_t> param(msg.GetWaveformMessageID(), msg.GetWaveform());
	this->CopyToDN(param);
	param.readyToReceive = readyToRecv;
	dataNotifierEvent->Invoke(param);

	return rtn;
}


void WF_AlwaysOn_NoAck::BroadcastData(WF_Message_n64_t& msg, uint16_t size, WF_MessageId_t _msgId)
{
	rcvFromFwNo++;
	Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn:: Broadcast: Local count number %lu, Packet msg id %lu,  Size of Ack_Type is  %d \n",rcvFromFwNo, _msgId, Ack_Type.Size());fflush(stdout);
	//Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn:: Sending Broadcast.\n");
	WF_MessageStatusE Status = WF_ST_SUCCESS;
	msg.SetWaveformMessageID(_msgId);

	/* Comment out testing code
	//If type is pattern related, increment debug_count1.
	Debug_Printf(DBG_WAVEFORM,"show current debug_count %d, message type is %d\n",debug_count1,msg.GetType());
	if((msg.GetType() != Types::LE_Type) && (msg.GetType() != Types::Discovery_Type)){
		debug_count1++;
		Debug_Printf(DBG_WAVEFORM, "Incrementing debug_count1 to %d\n",debug_count1);
	}
		//set readyToRecv
	if((debug_count1 %3) == 2){
		readyToRecv = false;
		debug_count1++;
	}*/
	//if(WF_Buffer_FULL == false){
	if(readyToRecv == true){ //buffer is not full 
		Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::current_messageId.Size is %d\n", current_messageId.Size());
		WF_MessageId_t fwMsgId = _msgId;
		current_messageId.InsertBack(fwMsgId);
		Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::Adding messageId %lu to current_messageId. Now size is %d\n",_msgId, current_messageId.Size());
		//Debug_Printf(DBG_WAVEFORM, "Setting WF_Buffer_FULL to true\n");
		Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::Setting readyToRecv to false\n");
		//WF_Buffer_FULL = true;
		readyToRecv = false;
		//AlwaysOn_Dce is not handling message right now.
		Status = WF_ST_SUCCESS;  
		msgStat = WF_ST_SUCCESS;
		MessageStatus[0] = WF_ST_SUCCESS;

		Ack_Type[0] = WDN_WF_RECV;   //Keep track acknowledgement. For broadcast use 0 as key value
		Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::Ack_Type.Size is %d\n",Ack_Type.Size());
		Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::Address of Ack_Type is %p\n",&Ack_Type);
		uint64_t value = 0;
		destList.InsertBack(value);
		//validation
	#if Debug_Flag
		b_summary.positive_wf_recv_b += 1;
	#endif
	}else{
		//AlwaysOn_Dce cannot accept message to be send now. return Nack
		//Debug_Printf(DBG_WAVEFORM, "WF_Buffer_FULL is true!!\n");
		Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::readyToRecv is false!!\n");
		MessageStatus[0] = WF_ST_BUFFER_FULL;
		Ack_Type[0] = WDN_WF_RECV;
		Status = WF_ST_BUFFER_FULL; //Cannot accept message right now
		//readyToRecv = true;
		//this->debugTimer->Start();
	#if Debug_Flag
		b_summary.negative_wf_recv_b += 1;
	#endif
	} 

	//Prepare and signal DataNotification to Framework 
	Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn:: Prepare WDN_WF_RECV\n");
	//signal WDN_WF_RECV to framework. First create object to be passed
	//WF_DataStatusParam<uint64_t> param(msg,Status);
	//WF_DataStatusParam<uint64_t>* param = new WF_DataStatusParam<uint64_t>(_msgId, msg.GetWaveform(),Status);
	WF_DataStatusParam<uint64_t> param(_msgId, msg.GetWaveform());

	this->CopyToDN(param);
	/*******************
	//status.Insert(msgId,WDN_WF_RECV);
	param.ackType = WDN_WF_RECV;
	param.dest = NULL; //Return pointer to destination array
	param.noOfDest = 0; //Return noOfDestination
	*****************************************************/
	param.destArray[0] = 0;
	//param->WF_Buffer_FULL = WF_Buffer_FULL;
	param.readyToReceive = readyToRecv;
	dataNotifierEvent->Invoke(param);

	//If Status is Buffer_Full, exit
	if(Status ==WF_ST_BUFFER_FULL){
		Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::Exiting from Broadcast method. Cannot accept incoming message now\n");
		return;
	}    

	//Add entry in messageId <=> destination
	Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::Message Id for this message is %lu\n",_msgId);

	uint8_t bcastAddress[6]={0xff,0xff,0xff,0xff,0xff,0xff};
	//if(msg.GetType() != Types::LE_Type && msg.GetType() != Types::Discovery_Type){
	//  printf("WF_AlwaysOn:: Broadcast:: sending patttern broadcast from node: %ld, of size: %d \n", msg.GetSource(), size);
	//}

	/*
	Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn:: Prepare WDN_WF_RECV\n");
		
	//Need to add (messageId, address of message) pair to map
	MessageIdToPointer[msgId]=&msg;
	Debug_Printf(DBG_WAVEFORM, "Address of message (messageId %d) is %p\n",msgId, &msg);
	//Prepare DataNotification signal to sendWDN_WF_RECV to framework. First create object to be passed
	//WF_DataStatusParam<uint64_t> param(msg,Status);
	param->ackType = WDN_WF_RECV;

	//Need to keep track of (messageId, AckType) pair and (messageId, messageStatus) pair 
	Ack_Type[0] = WDN_WF_RECV;
	msgStat = WF_ST_SUCCESS;
	//MessageStatus[msgId] = WF_ST_SUCCESS; 
	param->dest = NULL; //Need to confrim what to be set for broadcast case
	param->noOfDest = 0; //I guess this is used to get destination so I think it is safe to set 0. 
	dataNotifierEvent->Invoke(*param);
	*/
	//DataNotification is signaled... Now prepare to send data 
	/***************************************************************
		Packet size has not been verified!!!!!
	****************************************************************/
	Debug_Printf( DBG_WAVEFORM,"WF_AlwaysOn:: WFID: %d:: Broadcast:: sending broadcast from node: %ld, of size: %d \n", wfAttrib.wfId, msg.GetSource(), size);
	msg.SetNumberOfDest(0);
	Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::Ack_Type.Size is %d\n",Ack_Type.Size());
	last_packet = true; //I need to pay attention to this. There is a possibility that this value is overwritten by another SendRequest?
	IsBroadcast = true;
	WF_MessageStatusE ret=SendToDevice(bcastAddress, msg, size);
#if Debug_Flag
	Debug::PrintTimeMicro();
	//printf("WF_AlwaysOn:: Broadcast Success 1\n");
	if(ret == WF_ST_SUCCESS){
		//printf("WF_AlwaysOn:: Broadcast Success 2\n");
		if(msg.GetType() == Types::PATTERN_MSG){
			printf("WF_AlwaysOn:: Broadcast Success 3\n");
			b_summary.total_data_broadcast_sent++;
		}else{
			b_summary.total_le_broadcast_sent++;
		}
	}
#endif
	/*  disable ack in broadcast method for now. As it is in the ackTimer handler. //BK: WHY?????
	//Now finished sending data thorugh socket. Prepare DataNotification signal to send WDN_WF_SENT
	Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn:: Prepare WDN_WF_SENT\n");

	//param->status  this is still success if pkt is not too big
	//signal WDN_WF_RECV to framework. First create object to be passed
	param->ackType = WDN_WF_SENT;
	Ack_Type[msgId] = WDN_WF_SENT;
	MessageStatus[msgId] = WF_ST_SUCCESS;
	param->dest = NULL; //Does not make sence to tell which destination for WDN_WF_RECV
	param->noOfDest = 0; //I guess this is used to get destination so I think it is safe to set 0. 
	dataNotifierEvent->Invoke(param);
	*/

	Ack_Type[0] = WDN_WF_SENT;
	MessageStatus[0] = ret;
	WF_DataStatusParam<uint64_t> ackParam(last_sent_msg_ptr->GetWaveformMessageID(),last_sent_msg_ptr->GetWaveform());
	this->CopyToDN(ackParam);
	ackParam.destArray[0] = 0;
	ackParam.statusType[0] = Core::Dataflow::WDN_WF_SENT;
	ackParam.statusValue[0] = ret;
	ackParam.readyToReceive = readyToRecv;
	dataNotifierEvent->Invoke(ackParam); //Event only for SendData.


	/*
	//Now delete entry in map
	BSTMapT<WF_MessageId_t, WF_DataStatusTypeE>::Iterator it  = Ack_Type.Find(msgId);
	//BSTMapT<WF_MessageId_t, int>::Iterator it  = Ack_Type.Find(msgId);
	if(it == Ack_Type.End()){
		Debug_Printf(DBG_WAVEFORM, "Key not found. Already deleted????\n");
		exit(1);
	}else{
		Debug_Printf(DBG_WAVEFORM, "Key %d found. Deleting...",msgId);
		Ack_Type.Erase(it); //Delete msgId as I already send final acknowledgement.
		it  = Ack_Type.Find(msgId);
		if(it == Ack_Type.End()){
			Debug_Printf(DBG_WAVEFORM, "Key %d deleted.\n",msgId);
		}else{
			Debug_Printf(DBG_WAVEFORM, "Key not deleted!!!\n");
			abort();
		}
	}

	BSTMapT<WF_MessageId_t, WF_MessageStatusE>::Iterator i  = MessageStatus.Find(msgId);
	if(i == MessageStatus.End()){
		Debug_Printf(DBG_WAVEFORM, "Key not found. Already deleted????\n");
		exit(1);
	}else{
		Debug_Printf(DBG_WAVEFORM, "Key %d found. Deleting...",msgId);
		MessageStatus.Erase(msgId); //Delete msgId as I already send final acknowledgement.
		i  = MessageStatus.Find(msgId);
		if(i == MessageStatus.End()){
			Debug_Printf(DBG_WAVEFORM, "Key %d deleted.\n",msgId);
		}else{
			Debug_Printf(DBG_WAVEFORM, "Key not deleted!!!\n");
			abort();
		}
	}*/
}

/***************************************************************************************************************
*
*  SendToDevice
*
****************************************************************************************************************/  

WF_MessageStatusE  WF_AlwaysOn_NoAck::SendToDevice(uint8_t* destMAC, WF_Message_n64_t&  msg, uint16_t size)
{
	Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::SendToDevice sending for wfid %d, to destination:",WID);
	//PrintMAC(destMAC);
	Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::Ack_Type.Size is %d\n",Ack_Type.Size());
	//do the message reference exchange first
	WF_MessageStatusE rtn;
	WF_Message_n64_t* in_msg_ptr = &msg;
	
	uint64_t mySrc = ConvertAddressToU64((unsigned char*)myIfAddress);
	in_msg_ptr->SetSrcNodeID(MY_NODE_ID);
	in_msg_ptr->SetSource(mySrc);
	in_msg_ptr->SetWaveform(WID);
	uint16_t tx_len = 0;
	
	Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::SendToDevice: My uint64_t address is %lu, msg ptr is %p\n",mySrc, &msg);
	Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::SendToDevice: show payload address %p\n",msg.GetPayload());
	//Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn_Dce:: Show payload size %d , sizeof SoftwareAcknowledgement is %ld\n",msg.GetPayloadSize(), sizeof(SoftwareAcknowledgement));
	//SoftwareAcknowledgement* data = (SoftwareAcknowledgement*) msg.GetPayload();
	//Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn:: show payload: %d %d %d \n",data->src , data->wfId , data->msgId);
	//Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn:: show payload: %ld %d %d \n",msg.GetSource() , msg.GetWaveform(), msg.GetFrameworkMessageID());
	Debug_Printf(DBG_WAVEFORM, "in_msg_ptr->payload %p , msg->payload %p, \n",in_msg_ptr->GetPayload(), msg.GetPayload());
#ifdef PLATFORM_LINUX
	memset(sendbuf, 0, BUF_SIZE);
	struct ethhdr  *eth;
	eth = (struct ethhdr*) sendbuf;
	memcpy(eth->h_dest,destMAC,ETH_ALEN);
	memcpy(eth->h_source,myIfAddress,ETH_ALEN); 
	eth->h_proto = htons(protocol);
	int etherHdr = sizeof(struct ethhdr);
	int hdr =  (etherHdr > WF_HDR_SIZE) ? etherHdr: WF_HDR_SIZE;
	tx_len+=hdr;
	Debug_Printf(DBG_WAVEFORM, "Ethernet header size is %d, WF hdr %d, using %d \n",etherHdr, WF_HDR_SIZE, hdr);
#elif PLATFORM_DCE
	//Give space for wifi header
	tx_len+=WF_HDR_SIZE;
	//Give space for dbm and snr
	tx_len+=sizeof(double)*2;
#endif
	
	uint16_t adapterHdrSize = in_msg_ptr->GetHeaderSize();
	Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn:: Show adapterHdrSize %d, tx_len %d\n",adapterHdrSize, tx_len);
	//Copy the Message header
	memcpy(&sendbuf[tx_len], (void*)in_msg_ptr, adapterHdrSize);
	tx_len+=adapterHdrSize;
	Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn:: updated tx_len to %d\n", tx_len);
	//Debugging
	Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::Showing the size of destinaton in SendToDevice: %d\n",msg.GetNumberOfDest());

	//Fill up data here
	memcpy(&sendbuf[tx_len], in_msg_ptr->payload, size);
	
	//SoftwareAcknowledgement* data1 = (SoftwareAcknowledgement*) &sendbuf[tx_len];
	//Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn:: show payload: %d %d %d \n",data1->src, data1->wfId, data1->msgId);
	tx_len+=size;
	

#if ENABLE_PIGGYBACKING==1
#if ENABLE_EXPLICIT_SENDER_TIMESTAMPING==1
	if(in_msg_ptr->IsFlagSet(WF_FLG_TIMESTAMP)){
		EventTimeStamp *ets= (EventTimeStamp *)in_msg_ptr->GetPiggyPayload();
		if(ets){
			
		}
		else {
			Debug_Printf(DBG_PIGGYBACK, "WF_AlwaysOn:: SendToDevice: ERROR: Explicit Timestamp is turned ON. But piggy payload not present. %p \n", ets);
		}
		uint64_t now = PAL::SysTime::GetEpochTimeInNanoSec();
		uint64_t event = ets->senderEventTime.GetTime();
		
		ets->senderDelay = now - event;
		Debug_Printf(DBG_PIGGYBACK, "WF_AlwaysOn:: SendToDevice: Explicit Timestamp is turned ON.Now: %lu, Event: %lu, Delay %lu\n", now, event, ets->senderDelay);
	}else {
		Debug_Printf(DBG_PIGGYBACK, "WF_AlwaysOn:: SendToDevice: Explicit Timestamp is turned ON, but the outgoing msg has NO timestamp flag set\n");
	}
	
#endif //end ENABLE_EXPLICIT_SENDER_TIMESTAMPING

	if(in_msg_ptr->IsFlagSet(WF_FLG_PIGGYBACKING) || in_msg_ptr->IsFlagSet(WF_FLG_TIMESTAMP) ){
		memcpy(&sendbuf[tx_len], in_msg_ptr->GetPiggyPayload(), in_msg_ptr->GetPiggyPayloadSize());
		tx_len+= in_msg_ptr->GetPiggyPayloadSize();
		Debug_Printf(DBG_PIGGYBACK, "WF_AlwaysOn:: SendToDevice: Piggyback is turned ON, adding piggpayload of size %u\n", in_msg_ptr->GetPiggyPayloadSize());;
	}else {
		Debug_Printf(DBG_PIGGYBACK, "WF_AlwaysOn:: SendToDevice: Piggyback is turned ON, but the outgoing msg has NO piggy flag set\n")	
	}
	

#endif //end piggyback		
	

	struct sockaddr_ll dest={0};

	dest.sll_family = AF_PACKET;
	dest.sll_halen = ETHER_ADDR_LEN;
	dest.sll_ifindex = wfAttrib.ifindex;
	dest.sll_protocol=htons(protocol);
	memcpy(dest.sll_addr,destMAC,ETHER_ADDR_LEN);

	/*#if __BYTE_ORDER == __LITTLE_ENDIAN
		dest.sll_addr[0] = destMAC[5];
		dest.sll_addr[1] = destMAC[4];
		dest.sll_addr[2] = destMAC[3];
		dest.sll_addr[3] = destMAC[2];
		dest.sll_addr[4] = destMAC[1];
		dest.sll_addr[5] = destMAC[0];
	#elif __BYTE_ORDER == __BIG_ENDIAN

		dest.sll_addr[0] = destMAC[0];
		dest.sll_addr[1] = destMAC[1];
		dest.sll_addr[2] = destMAC[2];
		dest.sll_addr[3] = destMAC[3];
		dest.sll_addr[4] = destMAC[4];
		dest.sll_addr[5] = destMAC[5];
	#endif
	*/
	Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::SendToDevice sending for wfid %d, to destination:",WID);
	if(DBG_WAVEFORM()) PrintMAC(dest.sll_addr);
	Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::Sending message to device of length %d, Ack_Type.Size is %d\n",tx_len, Ack_Type.Size());
	//PrintMsg(tx_len,0, sendbuf);
/// Send packet
	int sent=sendto(sockID, sendbuf, tx_len, 0, (struct sockaddr*)&dest, sizeof(struct sockaddr_ll));
	if (sent < 0){
		Debug_Printf(DBG_WAVEFORM,"WF_AlwaysOn:: Sending failed:: \n");
		lastMsgStatus = WF_ST_BUFFER_FULL;
		rtn=WF_ST_BUFFER_FULL;
	}else {
		Debug_Printf(DBG_WAVEFORM,"WF_AlwaysOn:: Sent a msg of size %d successfully \n", sent); fflush(stdout);
		rtn=WF_ST_SUCCESS;
	}


	if(IsTimerSet){
		Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::IsTimerSet is true\n");
	}else{ 
		Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::IsTimerSet is false\n");
	}
	if(last_packet){
		Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::last_packet is true\n");
	}else{ 
		Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::last_packet is false\n");
	}

	//first check if ackTimer is running or not.
	if(!IsTimerSet){// && last_packet){
		//Currently no ackTimer is running so start timer
		Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::Starting timer to wait for acknowledgement from NS-3\n");
		if(ackTimer == NULL){
			Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::ackTimer is NULL. Illegal!");
		}else {
			ackTimer->Start();
			IsTimerSet = true;
		}
		//Debug_Printf(DBG_WAVEFORM, "Starting acknowledgement timer \n");
	}else{
		Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::IsTimerSet is true\n");
	}    
	//store the pointer to send the acknowledgement. RAL created the packet and it will decides what to do with it.
	last_sent_msg_ptr = in_msg_ptr;
	lastMsgStatus = rtn;
	Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::Ack_Type.Size is %d\n",Ack_Type.Size());
	return rtn;
}


void WF_AlwaysOn_NoAck::RecvTimer_Handler(uint32_t flag)
{
	OnSocketSignal(sockID);
}

void WF_AlwaysOn_NoAck::AckTimer_Handler(uint32_t flag)
{
	Debug_Printf(DBG_WAVEFORM, "\nWF_AlwaysOn_NoAck acKnowledgement timer has expired. wfid %d\n",this->WID);
	//Delete first element in the list.
	uint64_t current_node = destList.GetItem(0);
	Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn_NoAck destList[0] is %ld\n",current_node);
	Debug_Printf(DBG_WAVEFORM, "WF_ALwaysOn_NoAck %ld\n",this->current_destination_);
	destList.Delete(0); 

	Debug_Printf( DBG_WAVEFORM,"WF_AlwaysOn::Deleting destList[0] \n");
	for(unsigned int i =0; i < destList.Size(); i++){
		Debug_Printf( DBG_WAVEFORM,"WF_AlwaysOn::destList[%d] is %ld \n",i, destList[i]);
	}
	if(last_sent_msg_ptr->GetNumberOfDest() >0 ){
	//This is unicast. Store NACK for this node in the future but for now it is set to ack
		//Ack_Type[last_sent_msg_dest_ptr[destination_index++]] = WDN_DST_RECV;
//		Ack_Type[current_node] = WDN_DST_RECV;
//		MessageStatus[current_node] = WF_ST_ACK_NOT_RECV;
		//Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::Setting ackType of destination %ld to WDN_DST_SENT\n",last_sent_msg_dest_ptr[destination_index-1]);
		Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::Setting ackType of destination %ld to WDN_DST_SENT\n",current_node);
		Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::Ack_Type.Size is %d\n",Ack_Type.Size());
	}else{
		Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn:: This is broadacast\n");
//		Ack_Type[0] = WDN_WF_SENT;
//		MessageStatus[0] = WF_ST_ACK_NOT_RECV;
	#if Debug_Flag
		b_summary.negative_wf_sent_b++;
	#endif
	}

	//If this is the acknowledgement for last packet sent, signal acknowledgements for each destination
	if(destList.Size() ==0){
		Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::Sending acknowledgment to Framework.\n");
		//Create DataStatusParam object to be send via Signal   
		//WF_DataStatusParam_n64_t ackParam(*last_sent_msg_ptr, lastMsgStatus);
		//set dest pointer and noOfdest
		current_messageId.Delete(0); //remove messageId I am sendinig WDN_DST_RECV 
		//WF_Buffer_FULL = false; //Now I can accept new message
		//printf("WF_AlwaysOn::Setting readToRecv to true and sending WF_ to Framework.\n");
		readyToRecv = true;
		IsTimerSet = false;

		Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn_Dce_NoAcK::AckTimer_Handler: Erased all keys? Size of Ack_Type is %d\n",Ack_Type.Size());
		if(last_sent_msg_ptr->GetNumberOfDest() == 0){ //Broaodcast
//			WF_DataStatusParam<uint64_t> ackParam(last_sent_msg_ptr->GetWaveformMessageID(),last_sent_msg_ptr->GetWaveform());
//			this->CopyToDN(ackParam);
//			ackParam.destArray[0] = 0;
//			ackParam.statusType[0] = Core::Dataflow::WDN_WF_SENT;
//			ackParam.statusValue[0] = WF_ST_SUCCESS;
//			ackParam.readyToReceive = true;
//			dataNotifierEvent->Invoke(ackParam); //Event only for SendData.
			WF_ControlResponseParam param;
			param.id = 0;
			cont = ControlStatusResponse;
			param.type = cont;
			//param.status = true;
			param.wfid = WID;
			stat = WF_BUFFER_LOW;
			param.data = &stat; //Can I guarantee this value does not change till receiver reads this value?
			param.dataSize = sizeof(WF_ControlP_StatusE); //Set to zero to be safe.
			controlResponseEvent->Invoke(param);
		}else{
			Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::This is Multiple Unicast\n");
			//WF_ControlResponseParam* param = new WF_ControlResponseParam;
			WF_ControlResponseParam param;
			param.id = 0;
			cont = ControlStatusResponse;
			param.type = cont;
			//param.status = true;
			param.wfid = WID;
			stat = WF_BUFFER_LOW;
			param.data = &stat; //Can I guarantee this value does not change till receiver reads this value?
			param.dataSize = sizeof(WF_ControlP_StatusE); //Set to zero to be safe.
			controlResponseEvent->Invoke(param);
		}

		//Clear Ack_Type map;
		BSTMapT<NodeId_t, WF_DataStatusTypeE>::Iterator it;
		it = Ack_Type.Begin();
		while(it != Ack_Type.End()){ //for loop does not work as we have not it++?
			//Debug_Printf(DBG_WAVEFORM,"Show what it is pointing %d \n", it->First());
			Ack_Type.Erase(it);
		}
		//clear MessageStatus map;
		BSTMapT<NodeId_t, WF_MessageStatusE>::Iterator it_msg = MessageStatus.Begin();
		while(it_msg != MessageStatus.End()){
			MessageStatus.Erase(it_msg);
		}
		Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn:: AckTimer_Hanlder 2: Erased all keys? Size of MessageStatus is %d\n",MessageStatus.Size());
	}else{
		Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::Pending destintion exits\n");
		IsTimerSet = false;
		//There are pending destinations. call Unicast to send them
		Unicast((uint64_t)destList[0],*last_sent_msg_ptr,message_size);
	#if Debug_Flag
		if(last_sent_msg_ptr->GetType() == Types::ACK_REQUEST_MSG){
			u_summary.positive_wf_sent++;
		}
	#endif
		this->current_destination_ = destList[0];
	}
}
   
  //This translates the nodeId to MAC address used. This is based on how ns3 configures macs address for devices
  //Not needed starting v1.1. 
  /*uint64_t WF_AlwaysOn::MapNodeIdToWaveformAddress(uint64_t nodeId)
  {
    uint64_t deviceId;
    //first increament by one, Mac ids begin from 1, while nodeIds begin at 0.
    nodeId++;
    
    //your actual mac depends on the number of interfaces configured across all radios. so increament by network size and number of radios
    deviceId = (wfAttrib.wfId * NETWORK_SIZE) + nodeId;
    return deviceId;
  }*/
  

  /************************************************************************
  * CancelData Request
  * Cancel Message send using messageId
  *
  **************************************************************************/

  void WF_AlwaysOn_NoAck::CancelDataRequest(RequestId_t _rId, WF_MessageId_t _msgId, uint64_t* _destArray, uint16_t _noOfDestinations)
  {
	    //For now, This always fails no matter what.
	/*    if(_noOfDestinations > 1){
	      delete[]_destArray;
	    }
	    else{
	      delete _destArray;
	    }*/

//	    WF_CancelDataResponse_Param rv;
//		rv.msgId = _msgId;
//		rv.status = false;
//		rv.noOfDest = _noOfDestinations;
//		for(uint16_t index =0; index < _noOfDestinations; index++){
//			rv.cancel_status[index] = false;
//			rv.destArray[index] = _destArray[index];
//		}
//		WF_ControlResponseParam param_rv;
//		param_rv.id = _rId;
//		param_rv.wfid = WID;
//		cont = CancelPacketResponse;
//		param_rv.type = cont;
//		//param.status = false; // Too late to cancel send
//		param_rv.data = &rv;//0; // Not 100%  sure what needs to be set. I do not think this filed is used fo CancelDataRequest?
//		param_rv.dataSize = sizeof(WF_CancelDataResponse_Param); //Set to zero to be safe.
//		Debug_Printf(DBG_WAVEFORM, "Invoking Event\n"); fflush(stdout);
//		controlResponseEvent->Invoke(param_rv);
//		Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn_Ack::Just Invoked event\n"); fflush(stdout);
//
//	    return;
	    //signal framework
	    Debug_Printf(DBG_WAVEFORM,"WF_AlwaysOn_Dce_NoAcK::Got CancelDataRequest for msgId %lu\n",_msgId);
	    Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn_Dce_NoAcK::Show destination to be canceled\n");
	    for(uint16_t index =0; index < _noOfDestinations; index++){
	    	Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn_Dce_NoAcK::CancelDataRequest: destination[%d] is %ld\n",index,_destArray[index]);
	    }
	    WF_CancelDataResponse_Param data;
	    data.msgId = _msgId;
	    data.noOfDest = 0;
	    data.status = true;
	    //check if request is for broadcast
	    auto msgIdPtrite = MessageIdToPointer.Find(_msgId);
	    if(msgIdPtrite == MessageIdToPointer.End()){
		    data.status = false;
		    for(uint16_t i = 0; i < _noOfDestinations; ++i){
		    	data.destArray[i] = _destArray[i];
		    	data.cancel_status[i] = false;
		    	++data.noOfDest;
		    }

	    	WF_ControlResponseParam param;
	    	param.id = _rId;
	    	param.wfid = WID;
	    	cont = CancelPacketResponse;
	    	param.type = cont;
	    	//param.status = false; // Too late to cancel send
	    	param.data = &data;//0; // Not 100%  sure what needs to be set. I do not think this filed is used fo CancelDataRequest?
	    	param.dataSize = sizeof(WF_CancelDataResponse_Param); //Set to zero to be safe.
	    	Debug_Printf(DBG_WAVEFORM, "Invoking Event\n"); fflush(stdout);
	    	controlResponseEvent->Invoke(param);
	    	Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn_Dce_NoAcK::Just Invoked event\n"); fflush(stdout);
	    	return;
	    }
	    else{
		    data.msgId = _msgId;
		    data.noOfDest = 0;
		    data.status = true;
		    if(msgIdPtrite->Second()->GetNumberOfDest() == 0){
		    	//check current status
		    	BSTMapT<NodeId_t, WF_DataStatusTypeE>::Iterator it_ack = Ack_Type.Find(0);
		    	if(it_ack == Ack_Type.End()){
		    		Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn_Dce_NoAcK:: Ack_Type for broadcast does not exist. Cannot cancel\n");
				    for(uint16_t i = 0; i < _noOfDestinations; ++i){
				    	data.destArray[i] = _destArray[i];
				    	data.cancel_status[i] = false;
				    	++data.noOfDest;
				    }

			    	WF_ControlResponseParam param;
			    	param.id = _rId;
			    	param.wfid = WID;
			    	cont = CancelPacketResponse;
			    	param.type = cont;
			    	//param.status = false; // Too late to cancel send
			    	param.data = &data;//0; // Not 100%  sure what needs to be set. I do not think this filed is used fo CancelDataRequest?
			    	param.dataSize = sizeof(WF_CancelDataResponse_Param); //Set to zero to be safe.
			    	Debug_Printf(DBG_WAVEFORM, "Invoking Event\n"); fflush(stdout);
			    	controlResponseEvent->Invoke(param);
			    	Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn_Dce_NoAcK::Just Invoked event\n"); fflush(stdout);
			    	return;
		       	}
		    	else{
		       		Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn_Dce:: Check current status of broadcast\n");
		       		switch(Ack_Type[0]){
		       		case WDN_WF_RECV:{
		       	  	    Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn_Dce_NoAcK:: Broadcast is successfully canceled\n");
		       	  		destList.DeleteItem(0);
		       	   	    Ack_Type.Erase(0);
		       	        Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn_Dce_NoAcK:: show destList\n");
		       	        break;
		       	   	}
		       	    case WDN_WF_SENT:{
		       	    	Debug_Printf(DBG_WAVEFORM,"WF_AlwaysOn_Dce_NoAcK:: Broadcast has WDN_WF_SENT. Too late to cancel\n");
		       	    	data.status = false;
		       	    	break;
		       	    }
		       	  	case WDN_DST_RECV:{
		       	    	Debug_Printf(DBG_WAVEFORM,"WF_AlwaysOn_Dce_NoAcK:: Broadcast has WDN_DST_RECV. Too late to cancel\n");
		       	    	data.status = false;
		       	    	break;
		       		}
		       		default:
		       			Debug_Printf(DBG_WAVEFORM,"WF_AlwaysOn_Dce_NoAcK:: Broadcast has Unknown status\n");
		       			data.status = false;
		       			break;
		       		}
		    	}
		    	Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn_Dce:: Broadcast is canceled\n");
		    	WF_ControlResponseParam param;
		    	param.id = _rId;
		    	param.wfid = WID;
		    	cont = CancelPacketResponse;
		    	param.type = cont;
		    	//param.status = false; // Too late to cancel send
		    	param.data = &data;//0; // Not 100%  sure what needs to be set. I do not think this filed is used fo CancelDataRequest?
		    	param.dataSize = sizeof(WF_CancelDataResponse_Param); //Set to zero to be safe.
		    	Debug_Printf(DBG_WAVEFORM, "Invoking Event\n"); fflush(stdout);
		    	controlResponseEvent->Invoke(param);
		    	Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn_Dce_NoAcK::Just Invoked event\n"); fflush(stdout);
		    	return;

		    }
		    else{
			    //check result
			    for(uint16_t index =0; index < _noOfDestinations; index++){
			        //check if given destination existed.
			    	BSTMapT<NodeId_t, WF_DataStatusTypeE>::Iterator it_ack = Ack_Type.Find(_destArray[index]);
			    	if(it_ack == Ack_Type.End() && !IsBroadcast){
			    		Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn_Dce_NoAcK:: Node %ld does not exist. Cannot cancel\n",_destArray[index]);
			    		continue;
			    	}
			    	switch(Ack_Type[_destArray[index]]){
			    	  case WDN_WF_RECV:{
			    	    Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn_Dce_NoAcK:: destination[%ld] is successfully canceled\n",_destArray[index]);
			    		destList.DeleteItem(_destArray[index]);
			    	    Ack_Type.Erase(_destArray[index]);
			            data.cancel_status[data.noOfDest] = true;
			            data.destArray[data.noOfDest] = _destArray[index];
			            ++data.noOfDest;
			            Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn_Dce_NoAcK:: show destList\n");
			            for(uint16_t k =0; k < destList.Size(); k++){
			            	Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn_Dce_NoAcK:: destList[%d] is %ld\n",k, destList.GetItem(k));
			            }
			            break;
			    	  }
			          case WDN_WF_SENT:{
			    		Debug_Printf(DBG_WAVEFORM,"WF_AlwaysOn_Dce_NoAcK:: node %ld has WDN_WF_SENT. Too late to cancel\n",_destArray[index]);
			    		break;
			          }
			    	  case WDN_DST_RECV:{
			    		Debug_Printf(DBG_WAVEFORM,"WF_AlwaysOn_Dce_NoAcK:: node %ld has WDN_DST_RECV. Too late to cancel\n",_destArray[index]);
			    		break;
			    	  }
			    	  default:
			      		Debug_Printf(DBG_WAVEFORM,"WF_AlwaysOn_Dce_NoAcK:: node %ld has Unknown status\n",_destArray[index]);
			      		data.cancel_status[data.noOfDest] = false;
			      		data.destArray[data.noOfDest] = _destArray[index];
			      		++data.noOfDest;
			      		data.status = false;
			      		break;
			    	}
			    }
			    WF_ControlResponseParam param;
			    param.id = _rId;
			    param.wfid = WID;
			    cont = CancelPacketResponse;
			    param.type = cont;
			    //param.status = false; // Too late to cancel send
			    param.data = &data;//0; // Not 100%  sure what needs to be set. I do not think this filed is used fo CancelDataRequest?
			    param.dataSize = sizeof(WF_CancelDataResponse_Param); //Set to zero to be safe.
			    Debug_Printf(DBG_WAVEFORM, "Invoking Event\n"); fflush(stdout);
			    controlResponseEvent->Invoke(param);
			    Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn_Dce_NoAcK::Just Invoked event\n"); fflush(stdout);


		    }
	    }






	 /*********************************************************************************
	 //For now, This always fails no matter what.
     //signal framework 
     Debug_Printf(DBG_WAVEFORM,"WF_AlwaysOn::Why I need this method? WF has no buffering capability...?\n");
     WF_ControlResponseParam param;
     // WF_ControlResponseParam* param = new WF_ControlResponseParam;
     param.id = _rId;
     cont = CancelPacketResponse;Debug_Printf(DBG_WAVEFORM, "1\n"); fflush(stdout);
     param.type = cont;
     //param.status = false; // Too late to cancel send
     Debug_Printf(DBG_WAVEFORM, "2\n"); fflush(stdout);
     param.data = 0; // Not 100%  sure what needs to be set. I do not think this filed is used fo CancelDataRequest?
     param.dataSize = 0; //Set to zero to be safe.
     Debug_Printf(DBG_WAVEFORM, "Invoking signel\n"); fflush(stdout);
     controlResponseEvent->Invoke(param);
     Debug_Printf(DBG_WAVEFORM, "Just Invoked signel\n"); fflush(stdout);
     ***************************************************************************************/
  }
  /************************************************************************
  * ReplacePayloadRequest
  *
  *
  **************************************************************************/
  void WF_AlwaysOn_NoAck::ReplacePayloadRequest(RequestId_t rId, WF_MessageId_t msgId, uint8_t* payload, uint16_t payloadSize)
  {
	  Debug_Printf(DBG_WAVEFORM,"WF_AlwaysOn_Dce_NoAcK::ReplacePayloadRequest. %d.. What does it mean to replace payload when wf has only one?\n", rId);
	     WF_ReplacePayloadResponse_Param rv;
	     rv.msgId = msgId;
	     rv.status = false;
	     if(IsBroadcast == true){
	    	 rv.noOfDest = 0;
	     }else{
	    	 rv.noOfDest = Ack_Type.Size();
	     }
	     BSTMapT<NodeId_t, WF_DataStatusTypeE>::Iterator it_ack_test = Ack_Type.Begin();
	     uint16_t index =0;
	     while(it_ack_test != Ack_Type.End()){
	    	rv.replace_status[index] = false;
	     	rv.destArray[index] = it_ack_test->First();
	     	index++;
	     	++it_ack_test;
	     }
	     WF_ControlResponseParam param_rv;
	     param_rv.id = rId;
	     param_rv.wfid = WID;
	     cont = ReplacePayloadResponse;
	     param_rv.type = cont;
	     //param.status = false; // Too late to cancel send
	     param_rv.data = &rv;//0; // Not 100%  sure what needs to be set. I do not think this filed is used fo CancelDataRequest?
	     param_rv.dataSize = sizeof(WF_ReplacePayloadResponse_Param); //Set to zero to be safe.
	     Debug_Printf(DBG_WAVEFORM, "Invoking Event\n"); fflush(stdout);
	     controlResponseEvent->Invoke(param_rv);
	     Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn_Ack::Just Invoked event\n"); fflush(stdout);
	     return;
	  //signal framework
	      WF_ControlResponseParam param;
	      //WF_ControlResponseParam* param = new WF_ControlResponseParam;
	      //Debug_Printf(DBG_WAVEFORM, "1\n"); fflush(stdout);
	      param.id = rId;
	      param.wfid = this->WID;
	      cont = ReplacePayloadResponse;
	      param.type = cont;

	      //Debug_Printf(DBG_WAVEFORM, "2\n"); fflush(stdout);
	      //param.status = false; // I do not no what this field supporce to mean for ControlStatusRequest
	      //for data, put
	      WF_ReplacePayloadResponse_Param data;
	      data.status = true;
	      uint16_t node_count = 0;
	      BSTMapT<NodeId_t, WF_DataStatusTypeE> ::Iterator it_ack = Ack_Type.Begin();
	      while(it_ack != Ack_Type.End()){
	        switch(it_ack->Second()){
	        case WDN_WF_RECV:
	          data.destArray[node_count] = it_ack->First();
	          data.replace_status[node_count] = true;
	          Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn_Dce_NoAcK::Node %d is at index %d, replace Payload successfully\n",data.destArray[node_count], node_count);
	          node_count++;
	          break;
	        case WDN_WF_SENT: Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn_Dce_NoAcK::Node %d could not replace Payload successfully\n", it_ack->First());
	          break;
	        case WDN_DST_RECV: Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn_Dce_NoAcK::Node %d could not replace Payload successfully\n",it_ack->First());
	          break;
	        }
	        ++it_ack;
	      }

	      if(IsBroadcast == true){
	        Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn_Dce_NoAcK:: This message is broadcast.\n")
	 	   data.noOfDest = 0;
	      }else{
	        data.noOfDest = node_count;
	      }
	      data.msgId = msgId;
	      param.data = &data;     //put list of node on waveform// Not 100%  sure what needs to be set
	      param.dataSize = sizeof(WF_ReplacePayloadResponse_Param); //number of node on waveform //Set to zero to be safe.
	      Debug_Printf(DBG_WAVEFORM,"WF_AlwaysOn_Dce_NoAcK::Node_Count is %d, address of data is %p\n",node_count, param.data);
	      Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn_Dce_NoAcK::Invoking Event\n"); fflush(stdout);
	      controlResponseEvent->Invoke(param);
	      Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn_Dce_NoAcK::Just Invoked Event\n"); fflush(stdout);
  }
  /************************************************************************
  * DataStatusRequest
  *
  *
  **************************************************************************/

  void WF_AlwaysOn_NoAck::DataStatusRequest(RequestId_t rId, WF_MessageId_t mId)
  {    
     WF_ControlResponseParam param;
     //WF_ControlResponseParam* param = new WF_ControlResponseParam;
     param.id = rId;
     param.type = DataStatusResponse;
     //check if this request is for the message AlwaysOn_Dce is currently handling
     if(mId != current_Id){
         //Request is send for different message
         //param.status = false; //Not valid response
         param.data = NULL;
         param.dataSize = 0;
         controlResponseEvent->Invoke(param);
     }else{
         //Now send ack. Q1: the below dynamic allocation make cause memory leak
         //Q2 is it ok to send signal in a row?
        /********************************************
         WF_DataStatusParam<uint64_t>* data_WF_RECV = new WF_DataStatusParam<uint64_t>(last_sent_msg_ptr->GetFrameworkMessageID(),
	    last_sent_msg_ptr->GetWaveform(), msgStat);
         WF_DataStatusParam<uint64_t>* data_WF_SENT = new WF_DataStatusParam<uint64_t>(last_sent_msg_ptr->GetFrameworkMessageID(),
	    last_sent_msg_ptr->GetWaveform(), msgStat);
         WF_DataStatusParam<uint64_t>* data_DST_RECV = new WF_DataStatusParam<uint64_t>(last_sent_msg_ptr->GetFrameworkMessageID(),
	    last_sent_msg_ptr->GetWaveform(), msgStat);
        ********************************************/
         //WF_DataStatusParam<uint64_t> data_WF_RECV(last_sent_msg_ptr->GetFrameworkMessageID(), last_sent_msg_ptr->GetWaveform(), msgStat);
         //WF_DataStatusParam<uint64_t> data_WF_SENT(last_sent_msg_ptr->GetFrameworkMessageID(), last_sent_msg_ptr->GetWaveform(), msgStat);
         WF_DataStatusParam<uint64_t> data(last_sent_msg_ptr->GetWaveformMessageID(), last_sent_msg_ptr->GetWaveform());
             this->CopyToDN(data);
             param.data = &data;
             param.dataSize = sizeof(WF_DataStatusParam<uint64_t>);
             controlResponseEvent->Invoke(param);
     }   
     // Need to know how it supporse to send status back.
     /*
     //check if AlwaysOn_Dce still keeps information for this mId or not
     if(it == Ack_Type.End()){ //mId not found
         param->status = false; //This information is no longer avilable.
         param->data = NULL;
         param->dataSize = 0;
         Debug_Printf(DBG_WAVEFORM, "DataStatusRequest is too late. Everything has been already deleted.\n");
     }else{
         param->status = true; // I do not no what this field supporce to mean for ControlStatusRequest
         //data field points to WF_DataStatusParam object which contains current message status
         WF_Message_n64_t msg;      
         WF_DataStatusParam<uint64_t>* data = new WF_DataStatusParam<uint64_t>(msg,MessageStatus[mId]);
         //Store corresponding values to WF_DataStatusPararm
         data->ackType = Ack_Type[mId]; 
         data->status = MessageStatus[mId];
         data->dest = 0; // What to do?
         data->noOfDest = 0;
         data->wfMsg = MessageIdToPointer[mId];
         Debug_Printf(DBG_WAVEFORM, "The address of this message is at %p\n",data->wfMsg);
         param->data = data; //assign address of data object. (This may causes memory leak.) 
         param->dataSize = sizeof(WF_DataStatusParam<uint64_t>); //Set to zero to be safe.
     }
     //execute Invoke method.
     controlResponseEvent->Invoke(param);     */
  }
  /************************************************************************
  * AttributesRequest
  *
  *
  **************************************************************************/
  void WF_AlwaysOn_NoAck::AttributesRequest(RequestId_t rId)
  {
		Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn:: Waveform %u, AttributesRequest received. Preparing reply, ack timer %u, attribute size %lu\n", WID, wfAttrib.ackTimerPeriod, sizeof(wfAttrib) );
		//signal framework 
		WF_ControlResponseParam param;
		//WF_ControlResponseParam* param = new WF_ControlResponseParam;
		param.id = rId;
		param.wfid = WID;
		cont = AttributeResponse;
		param.type = cont;
		//param.status = true;
		param.data = &wfAttrib; // Return address of attribute member
		param.dataSize = sizeof(WF_Attributes); //Set to zero to be safe.
		controlResponseEvent->Invoke(param);
  }

  /************************************************************************
  * ControlStatusRequest
  *
  *
  **************************************************************************/
  void WF_AlwaysOn_NoAck::ControlStatusRequest(RequestId_t rId)
  {
     //signal framework 
     WF_ControlResponseParam param;
     //WF_ControlResponseParam* param = new WF_ControlResponseParam;
     param.id = rId;
     cont = ControlStatusResponse;
     param.type = cont;
     //param.status = true;
     param.wfid = WID;
     param.data = &stat; //Can I guarantee this value does not change till receiver reads this value?
     param.dataSize = sizeof(WF_ControlP_StatusE); //Set to zero to be safe.
     controlResponseEvent->Invoke(param);
  }

void WF_AlwaysOn_NoAck::SetAck_Type(NodeId_t node, WF_DataStatusTypeE type){
  //Just in case, check if node exists in Ack_Type or not.
  BSTMapT<NodeId_t, WF_DataStatusTypeE>:: Iterator it = Ack_Type.Find(node);
  if(it != Ack_Type.End()){
      Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::Node found in Ack_Type, updating status\n");
      Ack_Type[node] = type;
  }else{
      Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::Node id not found in Ack_Type....\n");
  }
}


void WF_AlwaysOn_NoAck::ProcessIncomingMessage(WaveformId_t wfid, uint16_t msglen, unsigned char* rcvbuf)
{
	//Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::Address of rcvbuf in Process method is %p\n",rcvbuf);
	//Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn_Ack::ProcessIncoming: MessageAddress of rcvbuf in Process method is %p\n",rcvbuf);
	U64NanoTime recvTimeStamp = SysTime::GetEpochTimeInNanoSec();
	
	wrn++;
	Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn_Ack::ProcessIncomingMessage: Received a new Message %lu size is %d\n\n", wrn, msglen);
	//if waveform does not support acknowledgement. no need to do below.
	//PrintMsg(msglen, wrn, rcvbuf);

	if(msglen < WF_HDR_SIZE + rcv_msg_ptr->GetHeaderSize()){
#ifdef PLATFORM_DCE
		if(msglen == 71) {
			Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::ProcessIncomingMessage: This is a Ack message from Transceiver\n");
			ProcessAckMessages(wfid,msglen, rcvbuf);
			return;
		}
#endif
		printf("WF_AlwaysOn::ProcessIncomingMessage: Something is wrong, message size %d, is less than hdr size %d\n", 
			 msglen, WF_HDR_SIZE + rcv_msg_ptr->GetHeaderSize());
		return;
	}
		
	WF_Message_n64_t *msgToSendUp = ExtractWFMessage(wfid,msglen, rcvbuf);
	if(!msgToSendUp){
			Debug_Printf(DBG_ERROR, "WF_AlwaysOn::ProcessIncomingMessage: Error couldnt extract received msg of size : %d\n", msglen);
			return;
	}
	
	if(wfAttrib.receiverTimeStampSupport) msgToSendUp->SetRecvTimestamp(recvTimeStamp);
	
	Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::ProcessIncomingMessage: Message Type is: %d\n", msgToSendUp->GetType());
	Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::Show number of destination: %d\n", msgToSendUp->GetNumberOfDest());
	Debug_Printf( DBG_WAVEFORM, "WF_AlwaysOn::Got message from %lu, node id %d,  on wfid %d of size %d and type %d with an snr of %f and a rssi of %fdb\n",
								msgToSendUp->GetSource(),msgToSendUp->GetSrcNodeID(), wfid, msgToSendUp->GetPayloadSize(), msgToSendUp->GetType(), lastSnr, lastRss);

	//printf("WF_AlwaysOn::Got message from %lu, node id %d,  on wfid %d of size %d and type %d with an snr of %f and a rssi of %fdb\n",
	//							msgToSendUp->GetSource(),msgToSendUp->GetSrcNodeID(), wfid, msgToSendUp->GetPayloadSize(), msgToSendUp->GetType(), lastSnr, lastRss);
	//printf("WF_AlwaysOn::ProcessIncomingMessage: Message Type is: %d\n", msgToSendUp->GetType());
	
	uint16_t type = msgToSendUp->GetType();
	
	#if Debug_Flag
	//validation
	switch(type){
	case Types::DISCOVERY_MSG:
	case Types::ESTIMATION_MSG:
		b_summary.total_discovery_recv++;
		break;
	case Types::ACK_REPLY_MSG:
		u_summary.positive_dst_recv++;
		break;
	case Types::ACK_REQUEST_MSG:
		u_summary.total_unicast_recv++;
		break;
	case Types::PATTERN_MSG:
		b_summary.total_data_recv++;
	}
#endif

	if(type == ESTIMATION_MSG && wfAttrib.estType == WF_FULL_EST) {
		if(estimator){
			estimator->OnPacketReceive(msgToSendUp);
		}
		
	}else {

		WF_RecvMsgParam<uint64_t> sigParam;
		sigParam.wrn = wrn;
		sigParam.msg = msgToSendUp;
		if(recvMsgEvent){
			recvMsgEvent->Invoke(sigParam);
		} 
	}
	//printf("Deleting: the message\n");fflush(stdout);
	//delete(msgToSendUp);
	//printf("Deleted: the message\n");fflush(stdout);
}   

void WF_AlwaysOn_NoAck::AddDestinationRequest(RequestId_t _rId, WF_MessageId_t _msgId, uint64_t* _destArray, uint16_t _noOfDestinations)
{
	Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn_Dce_NoAcK::Received AddDestinationReuqest\n");
//	 WF_AddDestinationResponse_Param rv;
//	 rv.msgId = _msgId;
//	 rv.status = false;
//	 if(IsBroadcast == true){
//		 rv.noOfDest = 0;
//
//	 }else{
//		 rv.noOfDest = _noOfDestinations;
//	 }
//	 for(uint16_t index =0 ; index < _noOfDestinations; index++){
//		 rv.add_status[index] = false;
//		 rv.destArray[index] = _destArray[index];
//	 }
//	 WF_ControlResponseParam param_rv;
//	 param_rv.id = _rId;
//	 param_rv.wfid = WID;
//	 cont = ReplacePayloadResponse;
//	 param_rv.type = cont;
//	 //param.status = false; // Too late to cancel send
//	 param_rv.data = &rv;//0; // Not 100%  sure what needs to be set. I do not think this filed is used fo CancelDataRequest?
//	 param_rv.dataSize = sizeof(WF_ReplacePayloadResponse_Param); //Set to zero to be safe.
//	 Debug_Printf(DBG_WAVEFORM, "Invoking Event\n"); fflush(stdout);
//	 controlResponseEvent->Invoke(param_rv);
//	 Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn_Ack::Just Invoked event\n"); fflush(stdout);
//	 return;


	//I can assume destArray has only destination not found,
	  //check if the current message WF is handling is the one pattern is making request or not
	 for(uint16_t index =0; index < _noOfDestinations; index++){
		 Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn_Dce_NoAcK::destArray[%d] is %ld\n",index, _destArray[index]);
	 }

	  WF_AddDestinationResponse_Param data;
	  data.msgId = _msgId; // Stores message Id

	  if(current_messageId.Size() !=0){
		  //Get message I am dealing with right now
		  WF_MessageId_t current_msg = current_messageId.GetItem(0);
		  if(current_msg != _msgId){
			  //fail
			  Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn_Dce_NoAcK, too late. Message has been send out\n");
			  data.noOfDest = 0;
			  data.status = false;
			  //data.destArray = NULL;
		  }else{
			  //check if this message still waiting for ack.
			  if(destList.Size() != 0){ //There is still node waiting for acknowledgement.
				  Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn_Dce_NoAcK:: destListSize is not zero\n");
				  //now it is ok to add
				  bool not_finished = false;
				  BSTMapT<NodeId_t,  WF_DataStatusTypeE>::Iterator it_type = Ack_Type.Begin();
				  while(it_type != Ack_Type.End()){
					  Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn_Dce_NoAcK:: Show WF_AlwaysOn_Dce_NoAcK Ack_Type[%d] is %d \n",it_type->First(), it_type->Second());
					  if(it_type->Second() != WDN_WF_RECV){
						  not_finished = true;
						  break;
					  }
					  ++it_type;
				  }
				  if(not_finished){//check if last node is not WDN_DST_RECV
					  Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn_Dce_NoAcK:: Now add destinations to list\n");
					  for(uint16_t index =0 ; index < _noOfDestinations; index++){
					  //create entry in ack type
						  Ack_Type[_destArray[index]] = WDN_WF_RECV;
						  destList.InsertBack(_destArray[index]);
						  Debug_Printf(DBG_WAVEFORM, "WF_AlawaysOn_ACK:: Adding node %ld, size of destlist %d\n",_destArray[index], destList.Size());
						  //It is impossible to fail at this point. A
						  data.status = true;
						  data.noOfDest = _noOfDestinations;
						  data.destArray[index] =_destArray[index];
						  data.add_status[index] = true;
					  }
					  for(uint16_t index =0; index < _noOfDestinations; index++){
						  Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn_Dce_NoAcK:: data.destArray[%d] is %ld\n", index, data.destArray[index])
					  }

				  }
			  }else{
				  Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn_Dce_NoAcK:: Ack_Type[%ld] is %d",destList.GetItem(0), Ack_Type[destList.GetItem(0)])
			  }
			  BSTMapT<NodeId_t,  WF_DataStatusTypeE>::Iterator it_type1 = Ack_Type.Begin();
			  while(it_type1 != Ack_Type.End()){
			   	  Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn_Dce_NoAcK:: Show WF_AlwaysOn_Dce_NoAcK Ack_Type[%d] is %d \n",it_type1->First(), it_type1->Second());
			      ++it_type1;
			  }
		  }
	  }else{
		  Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn_Dce_NoAcK, too late. Message has been send out\n");
		  data.noOfDest = 0;
		  data.status = false;
		  //data.destArray = 0;
	  }

	  struct WF_ControlResponseParam param;
	  param.id = _rId;
	  param.type = AddDestinationResponse;
	  param.data = &data;
	  param.dataSize =sizeof(WF_AddDestinationResponse_Param);    //bool status;
	  param.wfid = this->WID;
	  controlResponseEvent->Invoke(param);
}

void WF_AlwaysOn_NoAck::CopyToDN( WF_DataStatusParam<uint64_t>& ackParam){
	Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn:: Copy information into DN\n");
    BSTMapT<NodeId_t, WF_MessageStatusE>::Iterator  it_msg;
	BSTMapT<NodeId_t, WF_DataStatusTypeE>::Iterator it;
    uint16_t index = 0; //I need to do this as Ack_Type.Begin works counter intuitive.
   /* for(it = Ack_Type.Begin(); it != Ack_Type.End() ; ++it){
	    Debug_Printf(DBG_WAVEFORM,"WF_AlwaysOn:: destArray[%d] : %d \n", it->First(), it->Second());
	    ackParam.destArray[index] = it->First();// ackedDest[ack_index++] = it->First();
	    ackParam.statusType[index] = it->Second();// Debug_Printf(DBG_WAVEFORM,"WF_AlwaysOn_Ack::ackedDest[%d] is %ld\n",ack_index-1,ackedDest[ack_index-1])
	    index++;
    }*/
    it = Ack_Type.Begin();
    while(it != Ack_Type.End()){
    	Debug_Printf(DBG_WAVEFORM,"WF_AlwaysOn:: destArray[%d] : %d \n", it->First(), it->Second());
    	ackParam.destArray[index] = it->First();// ackedDest[ack_index++] = it->First();
    	ackParam.statusType[index] = it->Second();// Debug_Printf(DBG_WAVEFORM,"WF_AlwaysOn_Ack::ackedDest[%d] is %ld\n",ack_index-1,ackedDest[ack_index-1])

    	index++;
    	++it;
    }
    ackParam.noOfDest = index;
    index = 0;
    for(it_msg = MessageStatus.Begin(); it_msg != MessageStatus.End(); ++it_msg){
    	  ackParam.statusValue[index] = it_msg->Second();
    	  Debug_Printf(DBG_WAVEFORM, "AlwaysOnNoACK:: Show ackParam.statusValue[%d]\n",ackParam.statusValue[index]);
    	  index++;
    }
}

void WF_AlwaysOn_NoAck::validationTimer_Handler(uint32_t flag){

    //std::stringstream ss;
    //ss << this->WID;
	//string a =  ss.str() + "u.bin";
	//string b =  ss.str() + "b.bin";
	//char filename[16];
	string a =  std::to_string(WID) + "u.bin";
	string b =  std::to_string(WID) + "b.bin";
	printf("WF_AlwaysOn::ValidationTimer: File names: %s, %s \n", a.c_str(), b.c_str());
    MemMap<AckUnicastSummary>  unicastStat(a);
    MemMap<AckBroadcastSummary> broadcastStat(b);
    unicastStat.addRecord(u_summary);
    broadcastStat.addRecord(b_summary);
    Debug_Printf(DBG_WAVEFORM,"WF_AlwaysOn_Ack:: writing to memmap. wfid is %d\n",this->WID);

}
void WF_AlwaysOn_NoAck::DebugTimer_Handler(uint32_t flag){
    //Just sent clear flag to framework.
	Debug_Printf(DBG_WAVEFORM, "Entered DebugTimer Handler\n");
	WF_ControlResponseParam param;
	param.id = 0;

	param.type = ControlStatusResponse;
	//param.status = true;
	param.wfid = WID;
	stat = WF_BUFFER_LOW;
	param.data = &stat; //Can I guarantee this value does not change till receiver reads this value?
	param.dataSize = sizeof(WF_ControlP_StatusE); //Set to zero to be safe.
	Debug_Printf(DBG_WAVEFORM, "Try to clear busy flag");
	controlResponseEvent->Invoke(param);
}

void WF_AlwaysOn_NoAck::OnSocketSignal(int32_t sockfd)
{
	//Debug_Printf(DBG_WAVEFORM, "WF_AlawaysOn_Ack::OnSocketSignal: Reading from socket %d \n", sockfd);
	int32_t readBytes = recv(sockfd,rcvbuf, BUF_SIZE, 0);
	
	while(true){
		if (readBytes > 0){
			//wrn++;
			Debug_Printf(DBG_WAVEFORM, "WF_AlawaysOn::OnSocketSignal: Read %d bytes from socket %d \n", readBytes, sockfd);
			//PrintMsg(readBytes, wrn, rcvbuf);
			ProcessIncomingMessage(WID, readBytes, rcvbuf);
			readBytes = recv(sockfd,rcvbuf, BUF_SIZE, 0);
			
		}else {
			if(readBytes ==0){
				Debug_Printf(DBG_WAVEFORM, "WF_AlawaysOn::OnSocketSignal: Got a signal on socket, but couldnt read. \n");
				//Cleanup(sockfd);
			}
			break;
		}
	}
	
}

//This must do the opposite of SendToDevice
WF_Message_n64_t* WF_AlwaysOn_NoAck::ExtractWFMessage(WaveformId_t wfid, uint16_t msglen, unsigned char* buf)
{
	WF_Message_n64_t *ret=NULL,*temp =NULL;// = new WF_Message_n64_t();
	int rx_buf_loc=0;

#ifdef PLATFORM_LINUX
	char destMAC[ETH_ALEN];
	char srcMAC[ETH_ALEN];
	struct ethhdr  *eth;
	eth = (struct ethhdr*) buf;
	memcpy(destMAC, eth->h_dest,ETH_ALEN);
	memcpy(srcMAC, eth->h_source,ETH_ALEN); 
	//rx_buf_loc+=sizeof(struct ethhdr);
	rx_buf_loc+=WF_HDR_SIZE; //use the bigger size of header
	Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn:: ExtractWFMessage: Got a message from nbr : \n");
	if(DBG_WAVEFORM()) PrintMAC((uint8_t*)srcMAC);
#elif PLATFORM_DCE
	//Give space for wifi header
	rx_buf_loc+=WF_HDR_SIZE;
	  //Extract the rss and snr
	memcpy(&lastSnr, &rcvbuf[rx_buf_loc], sizeof lastSnr);
	memcpy(&lastRss, &rcvbuf[rx_buf_loc + sizeof lastSnr], sizeof lastRss);
	rx_buf_loc += sizeof(double) * 2;
#endif
	
	int wfHdr = WF_Message_n64_t::GetHeaderSize();
	Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn:: ExtractWFMessage: WfMessage stucture size is %d: \n", wfHdr);
	temp = (WF_Message_n64_t*)&buf[rx_buf_loc];
	//memcpy( (void*)ret,&buf[rx_buf_loc], wfHdr);
	
	if(temp->GetPayloadSize() > 0){
		ret = new WF_Message_n64_t();
		uint8_t *payload = new uint8_t[temp->GetPayloadSize()];
		memcpy(ret, &buf[rx_buf_loc],sizeof(WF_Message_n64_t)); 
		rx_buf_loc+= wfHdr;
		memcpy(payload,&buf[rx_buf_loc], temp->GetPayloadSize());
		ret->SetPayload(payload);
		ret->SetPayloadSize(temp->GetPayloadSize());
		rx_buf_loc+= temp->GetPayloadSize();
		
#if ENABLE_PIGGYBACKING==1
		if( ret->IsFlagSet(WF_FLG_PIGGYBACKING) || ret->IsFlagSet(WF_FLG_TIMESTAMP)  ){
			Debug_Printf(DBG_PIGGYBACK, "WF_AlwaysOn:: ExtractWFMessage: Piggyback is turned ON, extracting piggy payload of size %u\n", temp->GetPiggyPayloadSize());fflush(stdout);
			uint8_t *piggy_payload = new uint8_t[temp->GetPiggyPayloadSize()];
			memcpy(piggy_payload,&buf[rx_buf_loc], temp->GetPiggyPayloadSize());
			ret->SetPiggyPayload(piggy_payload, temp->GetPiggyPayloadSize());
			rx_buf_loc+=temp->GetPiggyPayloadSize();
		}
		else {
			Debug_Printf(DBG_PIGGYBACK, "WF_AlwaysOn:: ExtractWFMessage: Piggyback is turned ON. But Msg has no piggy Payload\n");
		}
#endif //end piggyback	
		
		
	}else {
		Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn:: ExtractWFMessage: Something wrong a Zero payload message has arrived. %d: \n", temp->GetPayloadSize());
	}
	//ret->SetSource(ConvertToAddress_u64((uint8_t*) srcMAC));
	//ret->SetWaveform(WID);
  //ret->SetWaveformMessageID(GetNewWaveformMsgId());
	Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn:: ExtractWFMessage: Got a message of type %d with payload size %d: \n", ret->GetType(), ret->GetPayloadSize());
	return ret;
}

void WF_AlwaysOn_NoAck::ProcessAckMessages(WaveformId_t wfid,uint16_t msglen, unsigned char* buf)
{
	Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::Message Received is acknowledgement from Ns-3\n");
	if(current_messageId.Size() == 0){
		Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::No message waiting for NS-3 reply now. Timer hander is already returned reply\n");
	}
	uint8_t* nbr = new uint8_t[6];
	uint64_t tnbr;
	uint8_t* pnbr = (uint8_t*)&tnbr;

	memcpy(nbr, rcvbuf, 6);
	pnbr[0] = nbr[5];
	pnbr[1] = nbr[4];
	pnbr[2] = nbr[3];
	pnbr[3] = nbr[2];
	pnbr[4] = nbr[1];
	pnbr[5] = nbr[0];
	
	//nodeAddress is the node that sent the ack
	//wfid is the waveform id
	//status is true for success and false for failure
	uint64_t nodeAddress = tnbr;
	bool status = rcvbuf[70] == 0x9;
	delete (nbr);

	//Maybe this responce was too late and entry may have been deleted and responce might have sent to FI.
	//Check if messageId still exists in map.
	Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::Size of current_messageId is %d\n",current_messageId.Size());
	bool already_removed = true;
	if((last_sent_msg_ptr != NULL) && (current_messageId.Size() != 0)){ 
		Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn:: WF MessageId is %lu\n", last_sent_msg_ptr->GetWaveformMessageID());
		for(unsigned int index = 0;  index < current_messageId.Size(); index++){
				Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::current_messageId[%d] is %lu\n", index, current_messageId[index]);
				if(last_sent_msg_ptr->GetWaveformMessageID() == current_messageId[index]){
						already_removed = false;
				}
		}
		if(already_removed == true){
				Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::messageId is not found. Maybe getting acknowledgement for message which had been deleted\n");
				//return;
		}
	}else{
		Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::There is no message waiting for NS-3 acknowledgement right now\n");
	}
	if(status && !already_removed){
		printf("WF_AlwaysOn::The ack was received for %lu\n", nodeAddress);
		if(IsBroadcast == true){
				//Maybe I can use last_sent_msg_ptr->GetNumberOfDestination()) ?
				Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::this is responce to broadcast. \n");
				Ack_Type[0] = WDN_WF_SENT;
				MessageStatus[0] = WF_ST_SUCCESS;
				//Debug_Printf(DBG_WAVEFORM, "Setting ackType of destination 0 to WDN_WF_SENT\n");  
		}else{
				Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::This is responce to unicast\n");
				//Ack_Type[nodeAddress] = WDN_DST_RECV; //myWFPtr[wfid]->SetAck_Type(nodeAddress, WDN_DST_RECV);

				//Debug_Printf(DBG_WAVEFORM, "Setting ackType of destination %d to WDN_DST_SENT\n",nodeAddress);               

		}             
		destination_index++;  //WFPtr[wfid]->IncrementDestinationIndex(); //increment index for last_sent_msg_dest_ptr
		
	}else if( !already_removed){
		Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::The NACK from NS-3 was received for %lu\n", nodeAddress);
	}else{
		Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::It seems acktimer handler took care reply for this message\n");
	}

	if(current_messageId.Size() != 0 && !already_removed)
	{
		//AlwaysOn_Dce received ACK/NACK. So suspend ackTimier;
		Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::Suspend timer waiting for ack from NS-3\n");
		ackTimer->Suspend();
		//Get infromatioin from AlwaysOn_Dce instance
		//BSTMapT<NodeId_t, WF_DataStatusTypeE>* ackType; //Stores ackType for each destinations.  
		//ackType = myWFPtr[wfid]->GetAck_TypeAddress();  
		//Debug_Printf(DBG_WAVEFORM, "Address of Ack_Type is %p\n",&Ack_Type);
		//Debug_Printf(DBG_WAVEFORM, "Size of Ack_Type is %d\n",Ack_Type.Size());
		//ListT<uint64_t,false, EqualTo<uint64_t> >* destList = myWFPtr[wfid]->GetdestListAddress();
		//Debug_Printf(DBG_WAVEFORM, "Address of destList is %p\n",&destList);
		//WF_Message_n64_t* last_sent_msg_ptr = last_sent_msg_ptr;//myWFPtr[wfid]->getLastSentMessagePtr();
		//WF_MessageStatusE lastMsgStatus = myWFPtr[wfid]->getMessageStatus(); 
		//Delete first element in the list.
		//myWFPtr[wfid]->DeleteElement(); //destList.Delete(0);
		Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::Size of destList is %d\n",destList.Size());

		if(destList.Size() != 0){
				destList.Delete(0);
				Debug_Printf( DBG_WAVEFORM,"WF_AlwaysOn::Deleting destList[0] \n");
		}
		else {
				Debug_Printf( DBG_WAVEFORM,"WF_AlwaysOn::something wrong; destlist size is 0 but shud not be \n");
		}
		for(unsigned int i =0; i < (unsigned int) destList.Size(); i++){
				Debug_Printf( DBG_WAVEFORM,"WF_AlwaysOn::destList[%d] is %ld \n",i, destList[i]);
		}
		//If this is the acknowledgement for last packet send, signal acknowledgements for each destination
		if((destList.Size() ==0)){
			//Debug_Printf(DBG_WAVEFORM, "sending WF_Buffer_FULL false to Framework.\n");
			Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::sending readyToRecv true to framework.\n");
			//Clear Ack_Type map;
			BSTMapT<NodeId_t, WF_DataStatusTypeE>::Iterator it;
			it = Ack_Type.Begin();
			while(it != Ack_Type.End()){ //for loop does not work as we have not it++?
				Ack_Type.Erase(it);
			}

			//Create DataStatusParam object to be send via Signal
			//WF_DataStatusParam_n64_t ackParam(*last_sent_msg_ptr, lastMsgStatus);
			//set dest pointer and noOfdest
			//WF_Buffer_FULL = false;//myWFPtr[wfid]->ClearWFBusy();
			readyToRecv = true;
			current_messageId.Delete(0); //remove messageId I am sendinig WDN_DST_RECV 
			//Clear IsTimerSEt flag here,
			IsTimerSet = false;
			if(last_sent_msg_ptr->GetNumberOfDest() == 0){ //Broaodcast
				//Send WDN_WF_SENT acknowledgement
				WF_DataStatusParam_n64_t ackParam(last_sent_msg_ptr->GetWaveformMessageID(),last_sent_msg_ptr->GetWaveform());

				ackParam.statusValue[0] = MessageStatus[0];
				//ackParam.dest = NULL;
				ackParam.readyToReceive = false;
				ackParam.statusType[0] = WDN_WF_SENT;
				ackParam.wfId = this->WID;
				ackParam.noOfDest = 0;
				ackParam.destArray[0] = 0;
				myWFPtr[wfid]->DataNotification(ackParam);
#if Debug_Flag
				if(last_sent_msg_ptr->GetType() == Types::ACK_REQUEST_MSG){
					b_summary.positive_wf_sent_b++;
				}
#endif
				
				WF_ControlResponseParam param;
				param.id = 0;
				cont = ControlStatusResponse;
				param.type = cont;
				//param.status = true;
				param.wfid = WID;
				stat = WF_BUFFER_LOW;
				param.data = &stat; //Can I guarantee this value does not change till receiver reads this value?
				param.dataSize = sizeof(WF_ControlP_StatusE); //Set to zero to be safe.
				controlResponseEvent->Invoke(param);
				//clear IsBroadcast flag;
				IsBroadcast = false;
			}
			else {
				WF_ControlResponseParam param;
				param.id = 0;
				cont = ControlStatusResponse;
				param.type = cont;
				//param.status = true;
				param.wfid = WID;
				stat = WF_BUFFER_LOW;
				param.data = &stat; //Can I guarantee this value does not change till receiver reads this value?
				param.dataSize = sizeof(WF_ControlP_StatusE); //Set to zero to be safe.
				controlResponseEvent->Invoke(param);
			}
			//clear MessageStatus map;
			BSTMapT<NodeId_t, WF_MessageStatusE>::Iterator it_msg = MessageStatus.Begin();
			while(it_msg != MessageStatus.End())
			{
				MessageStatus.Erase(it_msg);
			}
			Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::ProcessAckMessages: Erased all keys? Size of MessageStatus is %d\n",MessageStatus.Size());
		}
		else 
		{//there are still pending acks for this message
			Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::Pending destination exits\n");
			//There are pending destinations. call Unicast to send them
			uint64_t destination = destList[0];//myWFPtr[wfid]->GetDistination();
			//uint16_t message_size = message_size;///myWFPtr[wfid]->GetMessageSize();
			Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::Clearing acknowledgment timer flag\n");
			IsTimerSet = false;//myWFPtr[wfid]->ClearIsTimerSet(); 
			Unicast(destination,*last_sent_msg_ptr,message_size);
			this->current_destination_ = destination;
			WF_DataStatusParam_n64_t ackParam(last_sent_msg_ptr->GetWaveformMessageID(),last_sent_msg_ptr->GetWaveform());
			//update status of destination to WDN_WF_SENT
			Ack_Type[destination] = WDN_WF_SENT;
			MessageStatus[destination] = WF_ST_SUCCESS;
			this->CopyToDN(ackParam);
			ackParam.readyToReceive = readyToRecv;
			Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn_ACK:: Showing message (ptr %p) type for this message. %d \n",last_sent_msg_ptr, last_sent_msg_ptr->GetType());
			DataNotification(ackParam);
#if Debug_Flag
			if(last_sent_msg_ptr->GetType() == Types::ACK_REQUEST_MSG){
				u_summary.total_unicast_sent++;
				u_summary.positive_wf_sent++;
			}
#endif
		} 
	}
	else
	{// close already_removed
		Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::Do nothing\n");
	}  
	          
}

void WF_AlwaysOn_NoAck::InstantiateEstimation(uint32_t period)
{
    //estimatorHash[wid] = static_cast<Periodic*>(new Periodic(this,wid, &nbrTable));
	//leSendDel = new SendEstimationMessageDelegate_t(this, &WF_AlwaysOn::SendEstimationMessage);
	leDel = new WF_LinkChangeDelegate_n64_t(this, &WF_AlwaysOn_NoAck::LinkEstimatorChangeHandler);
	estimator = new Core::Estimation::EstBase(*this,WID);
	Debug_Printf(DBG_WAVEFORM, "WF_AlwaysOn::Created Periodic link estimation %d, with estimation ptr %p\n", WID, estimator);  fflush(stdout);
	
	estimator->SetParam(DEFAULT_ESTIMATION_PERIOD, 3*DEFAULT_ESTIMATION_PERIOD);
	estimator->RegisterDelegate(*leDel);
}

void WF_AlwaysOn_NoAck::LinkEstimatorChangeHandler(WF_LinkEstimationParam_n64_t _param)
{
	//WF_LinkEstimationParam param;
	//param.changeType = _param.changeType;
	linkEstimateEvent->Invoke(_param);
}

bool WF_AlwaysOn_NoAck::SendEstimationMessage(WaveformId_t wfId, FMessage_t* msg)
{
	WF_Message_n64_t estMsg;
	estMsg.SetType(Types::ESTIMATION_MSG);
	estMsg.SetPayload(msg->GetPayload());
	estMsg.SetPayloadSize(msg->GetPayloadSize());
	BroadcastData(estMsg,estMsg.GetPayloadSize(),0);
	return true;
}

WF_Attributes WF_AlwaysOn_NoAck::GetWaveformAttributes(WaveformId_t wfId)
{
	return wfAttrib;
}

//This translates the nodeId to MAC address used. This is based on how ns3 configures macs address for devices
uint64_t WF_AlwaysOn_NoAck::NodeIdToNS3MacAddress(NodeId_t nodeId)
{
	uint64_t deviceId;
	//first increament by one, Mac ids begin from 1, while nodeIds begin at 0.
	nodeId++;

	//your actual mac depends on the number of interfaces configured across all radios. so increament by network size and number of radios
	deviceId = (wfAttrib.wfId * NETWORK_SIZE) + nodeId;
	return deviceId;
}


}//End of namespace
