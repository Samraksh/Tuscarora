////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include "WF_AlwaysOn_Ack.h"


void WF_AlwaysOn_Ack::AckTimer_Handler(uint32_t flag)
{
	Debug_Printf(DBG_WAVEFORM, "\nWF_AlwaysOn_NoAck acKnowledgement timer has expired\n");          
	//Delete first element in the list.
	destList.Delete(0); 

	Debug_Printf( DBG_WAVEFORM,"Deleting destList[0] \n");
	for(unsigned int i =0; i < destList.Size(); i++){
		Debug_Printf( DBG_WAVEFORM,"destList[%d] is %ld \n",i, destList[i]);  
	}
	if(last_sent_msg_ptr->GetNumberOfDest() >0 ){
		//This is unicast. Store NACK for this node in the future but for now it is set to ack
		Ack_Type[last_sent_msg_dest_ptr[destination_index++]] = WDN_DST_RECV;
		Debug_Printf(DBG_WAVEFORM, "Setting ackType of destination %ld to WDN_DST_SENT\n",last_sent_msg_dest_ptr[destination_index-1]);

		Debug_Printf(DBG_WAVEFORM, "Ack_Type.Size is %d\n",Ack_Type.Size());
	}

	//If this is the acknowledgement for last packet send, signal acknowledgements for each destination
	if(destList.Size() ==0){
		Debug_Printf(DBG_WAVEFORM, "Sending acknowledgement to Framework.\n");
		//Create DataStatusParam object to be send via Event   
		//WF_DataStatusParam_n64_t ackParam(*last_sent_msg_ptr, lastMsgStatus);
		WF_DataStatusParam<uint64_t>* ackParam = new WF_DataStatusParam<uint64_t>(last_sent_msg_ptr->GetWaveformMessageID(),
			last_sent_msg_ptr->GetWaveform());
		
		//set dest pointer and noOfdest
		if(last_sent_msg_ptr->GetNumberOfDest() == 0){ //Broaodcast
			Debug_Printf(DBG_WAVEFORM, "This is Broadcast\n");
			ackParam->noOfDest = 0;
			ackParam->statusType[0] = WDN_WF_SENT;
			ackParam->statusValue[0] = WF_ST_SUCCESS;
		}else{
			Debug_Printf(DBG_WAVEFORM, "This is Multiple Unicast\n"); 
			int numberOfAck = 0;
			//go through ackTypeList to find node which received Ack.
			/*for(unsigned int i=0; i <ackTypeList.Size(); i++){
				if(ackTypeList[i] == 1){
					numberOfAck++;
				}
			}*/
			numberOfAck = Ack_Type.Size();   
			Debug_Printf(DBG_WAVEFORM, "numberOfAck is %d\n",numberOfAck);
			//Debug_Printf(DBG_WAVEFORM, "Ack_Type.Size is %d\n",Ack_Type.Size());
			uint32_t index = 0;
			BSTMapT<NodeId_t, WF_DataStatusTypeE>::Iterator it;
			index = numberOfAck -1; //I need to do this as Ack_Type.Begin works counter intuitive.
			for(it = Ack_Type.Begin(); it != Ack_Type.End() ; ++it){
				//Debug_Printf(DBG_WAVEFORM,"Show what it is pointing %d \n", it->First());
				if(it->Second() == WDN_DST_RECV){
					Debug_Printf(DBG_WAVEFORM,"Ack_Type[%d] is WDN_DST_RECV\n",it->First());
					ackParam->destArray[index--] = it->First();
					ackParam->statusType[index--] = WDN_DST_RECV;
					ackParam->statusValue[index--] = WF_ST_SUCCESS;
					//Debug_Printf(DBG_WAVEFORM,"Ack_Type[%ld] is WDN_DST_RECV\n",ackedDest[index-1])
				}
			}
			ackParam->noOfDest = numberOfAck;
		}

		//Clear Ack_Type map;
		BSTMapT<NodeId_t, WF_DataStatusTypeE>::Iterator it;
		it = Ack_Type.Begin();
		while(it != Ack_Type.End()){ //for loop does not work as we have not it++?
			//Debug_Printf(DBG_WAVEFORM,"Show what it is pointing %d \n", it->First());
			Ack_Type.Erase(it);
		}   
		Debug_Printf(DBG_WAVEFORM, "Erased all keys? Size of Ack_Type is %d\n",Ack_Type.Size());
		//size of destList is already zero.
		//Debug_Printf(DBG_WAVEFORM, "Ack handler, clear WF_Buffer_FULL\n");
		Debug_Printf(DBG_WAVEFORM, "Ack handler, set readyToRecv\n");
		//WF_Buffer_FULL = false; //Now I can accept new message
		readyToRecv = true; // Now waveform can accept message
		IsTimerSet = false;
		if(current_messageId.Size() != 1){
			Debug_Printf(DBG_WAVEFORM, "current_messageId size is not 1. This means I am handling two messages\n");
		}
		Debug_Printf(DBG_WAVEFORM, "Deleting messageId %lu from current_messageId\n",current_messageId.GetItem(0));
		current_messageId.Delete(0); //remove messageId I am sendinig WDN_DST_RECV 
		Debug_Printf(DBG_WAVEFORM, "Invoking datanotification from Ack timer handler\n");
		//ackParam->WF_Buffer_FULL = WF_Buffer_FULL;
		ackParam->readyToReceive = readyToRecv;
		dataNotifierEvent->Invoke(*ackParam); //Event only for SendData.
	}else{
		Debug_Printf(DBG_WAVEFORM, "Pending destintion exits\n");
		//There are pending destinations. call Unicast to send them
		Unicast((uint64_t)destList[0],*last_sent_msg_ptr,message_size);
		//start very short timer for acknowledgement from MAC. 
		Debug_Printf(DBG_WAVEFORM, "Starting timer to wait for acknowledgement from NS-3\n");
		if(ackTimer == NULL){
			Debug_Printf(DBG_WAVEFORM, "ackTimer is NULL. Illegal!");
		} 
		ackTimer->Start();

		//signal WDN_WF_RECV to framework. For now, it is always success
		Debug_Printf(DBG_WAVEFORM, "Sending WDN_WF_SENT for destination %ld\n",destList[0]);
		//WF_DataStatusParam_n64_t ackParam(*last_sent_msg_ptr, lastMsgStatus);
		WF_DataStatusParam<uint64_t>* ackParam = new WF_DataStatusParam<uint64_t>(last_sent_msg_ptr->GetWaveformMessageID(),
			last_sent_msg_ptr->GetWaveform());    
		//ackParam->wfMsg = last_sent_msg_ptr;
		ackParam->statusValue[0] = lastMsgStatus;
		ackParam->statusType[0] = WDN_WF_SENT;
		ackParam->destArray[0] = destList[0];
		ackParam->noOfDest = 1; 
		//ackParam->WF_Buffer_FULL = WF_Buffer_FULL;
		ackParam->readyToReceive = readyToRecv;
		dataNotifierEvent->Invoke(*ackParam);   
	}

}