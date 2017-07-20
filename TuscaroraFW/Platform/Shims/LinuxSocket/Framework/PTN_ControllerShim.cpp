////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include "PTN_ControllerShim.h"
#include "../Serialization.h"

extern IO_Signals ioSignals;



PTN_ControllerShim::PTN_ControllerShim() : SocketCommunicatorServerBase<PatternId_t>(FRAMEWORK_HOST, FRAMEWORK_PATTERN_PORT, FRAMEWORK_PATTERN_PORT_TYPE, IS_SINGLE_PATTERN_ENFORCED){
    Debug_Printf(DBG_SHIM, "PTN_ControllerShim::PTN_ControllerShim Initializing \n");
    fi =  &GetFrameworkInterface();

//	del = new SocketSignalDelegate_t(this, &PTN_ControllerShim::OnSignal);
//	master = new SocketServer(FRAMEWORK_HOST, FRAMEWORK_PATTERN_PORT, FRAMEWORK_PATTERN_PORT_TYPE);
//	ptnMasterSocket = master->GetSocketID();
//	//ioSig = new IO_Signals();
//	ioSignals.Register(ptnMasterSocket, del);
	Debug_Printf(DBG_SHIM, "PTN_ControllerShim::PTN_ControllerShim Initializing \n");
}

void PTN_ControllerShim::SetFrameworkPtr(Framework_I *_fi){
    fi = _fi;
}


bool PTN_ControllerShim::Deserialize (int32_t sockfd) {
//	Debug_Printf(DBG_SHIM, "PTN_ControllerShim::Deserialize ... ReadBytes / TotalBytes = %lu / %lu \n", ptn2fwmsg.GetBytesRead(), ptn2fwmsg.GetPayloadSize() );
	//Debug_Printf(DBG_SHIM, "PTN_ControllerShim::Deserialize: Message %s\n", buf);
//	PTN2FWMessages ptn2fwmsg(size, buf);
//	Generic_VarSized_Msg *ptn2fwmsg2_ptr = new Generic_VarSized_Msg(size, (uint8_t*)buf);
//	delete ptn2fwmsg2_ptr;
//	int calltype = ptn2fwmsg.GetType();
	int calltype = 0;
    bool rv = Peek<int>(sockfd, calltype) == 1;

    if(!rv){
     	Debug_Printf(DBG_SHIM, "FWP2AppShim::Deserialize No data to deserialize ... sockfd = %d \n", sockfd);
     	return false;
     }



    Debug_Printf(DBG_SHIM, "PTN_ControllerShim::Deserialize: Got a message msg.GetType() = %d \n", calltype);

//    if(calltype == PTN2FW_Call_Greet) {
//        PatternId_t patternId;
//        GenericMsgPayloadSize_t totalsize;
//        char greet[22];
//        void* greet_ptr = (void*)greet;
//        GenericDeSerializer< PatternId_t, GenericMsgPayloadSize_t, void* >
//            gs(&ptn2fwmsg,  patternId, totalsize, greet_ptr);
//
//        Debug_Printf(DBG_SHIM, "PTN_ControllerShim::Deserialize: Got a greeting message patternId = %d \n", patternId );
//        PrintMsg(greet,totalsize/sizeof(char));
//        InitPattern(patternId, sockfd);
//    }
    if (calltype == PTN2FW_Call_RegisterPatternRequest){
        PatternId_t patternId;
         PatternTypeE type;
         GenericMsgPayloadSize_t sizeofchararray;
         //char* uniqueName = NULL;
         char uniqueName[128];
         char* uniqueNamePtr = uniqueName;
         Read<int,   PatternId_t, GenericMsgPayloadSize_t, char*, PatternTypeE >
			(sockfd, calltype,   patternId, sizeofchararray, uniqueNamePtr, type);

        Debug_Printf(DBG_SHIM, "PTN_ControllerShim::Deserialize: PTN2FW_Call_RegisterPatternRequest patternId = %d type = %d \n", patternId, type);

        if(patternId == 0){
            patternId =  (static_cast<FrameworkBase*>(fi))->NewPatternInstanceRequest(type, uniqueName );
            Debug_Printf(DBG_SHIM, "PTN_ControllerShim::Deserialize: PTN2FW_Call_RegisterPatternRequest got new PID patternId = %d type = %d \n", patternId, type);

        }
        //static_cast<FrameworkBase*>(fi)->InitPattern(patternId, sockfd);
        client_init_ptr->InitClient(patternId, sockfd);

        static_cast<FrameworkBase*>(fi)->RegisterPatternRequest(patternId,  type);
        //free((void*)uniqueName); //TODO: BK: This is against the general memory rules, this should have been consumed by NewPatternInstanceRequest
    }

    else if (calltype == PTN2FW_Call_Send){
		PatternId_t pid = 0;
		 NodeId_t destArray[MAX_DEST];
		 void* destArrayptr = (void*)destArray;
		 uint16_t noOfDest;
		 GenericMsgPayloadSize_t totalsize;
		 FMessage_t msg;
          GenericMsgPayloadSize_t serial_msg_size;
          void* serial_msg = NULL;
		 uint16_t nonce;
		 bool noAck;


		Read<int,   PatternId_t, GenericMsgPayloadSize_t, void*, GenericMsgPayloadSize_t, void*, uint16_t, bool >
			(sockfd, calltype, pid, totalsize, destArrayptr, serial_msg_size, serial_msg, nonce, noAck);


		Shim::Serialization::DeSerialize(&msg, serial_msg_size, serial_msg);


		noOfDest = totalsize / sizeof(NodeId_t);

	fi->SendData (pid, destArray,  noOfDest,  msg, nonce, noAck);
//	free(serial_msg); //This is destructed by the msg.DeSerialize no need to destruct again
	}

	else if (calltype == PTN2FW_Call_SendwComp){
		PatternId_t pid;
		 NodeId_t destArray[MAX_DEST];
		 void* destArrayptr = (void*)destArray;
		 uint16_t noOfDest;
	        GenericMsgPayloadSize_t totalsize;
		 LinkComparatorTypeE lcType;
		 FMessage_t msg;
         GenericMsgPayloadSize_t serial_msg_size;
         void* serial_msg = NULL;
		 uint16_t nonce;
		 bool noAck;
		Read<int,   PatternId_t, GenericMsgPayloadSize_t, void*, LinkComparatorTypeE, GenericMsgPayloadSize_t, void*, uint16_t, bool >
			(sockfd, calltype,  pid, totalsize, destArrayptr, lcType, serial_msg_size, serial_msg, nonce, noAck);

		Shim::Serialization::DeSerialize(&msg, serial_msg_size, serial_msg);
		noOfDest = totalsize / sizeof(NodeId_t);


	fi->SendData (pid, destArray, noOfDest, lcType, msg, nonce, noAck);
	//	free(serial_msg); //This is destructed by the msg.DeSerialize no need to destruct again
	}

	else if (calltype == PTN2FW_Call_Broadcast){
//		PatternId_t pid;
//		 FMessage_t msg;
//         GenericMsgPayloadSize_t serial_msg_size;
//         void* serial_msg = NULL;
//		 WaveformId_t wid;
//		 uint16_t nonce;
//		Read<int,   PatternId_t, GenericMsgPayloadSize_t, void*, WaveformId_t, uint16_t >
//			(sockfd, calltype,  pid, serial_msg_size, serial_msg, wid, nonce);
//
//		msg.DeSerialize( serial_msg_size, serial_msg);
//
//	fi->BroadcastData(pid,  msg, wid, nonce);
	}

	else if (calltype == PTN2FW_Call_ReplacePayload){
		PatternId_t patternId;
		 FMessageId_t  msgId;
		 void* payload = NULL;
		 GenericMsgPayloadSize_t sizeOfPayload;
		Read<int,   PatternId_t, FMessageId_t , GenericMsgPayloadSize_t, void* >
			(sockfd, calltype,  patternId, msgId, sizeOfPayload, payload);
	fi->ReplacePayloadRequest(patternId, msgId, payload, sizeOfPayload);
	}

	else if (calltype == PTN2FW_Call_AddDest){
		PatternId_t patternId;
		 FMessageId_t  msgId;
		 NodeId_t destArray[MAX_DEST];
		 uint16_t noOfNbrs;

		 void* destArrayptr = (void*)destArray;
		 GenericMsgPayloadSize_t totalsize;

		Read<int,   PatternId_t, FMessageId_t , GenericMsgPayloadSize_t, void* >
			(sockfd, calltype,  patternId, msgId, totalsize, destArrayptr);

		noOfNbrs = totalsize / sizeof(NodeId_t);

	fi->AddDestinationRequest(patternId, msgId, destArray, noOfNbrs);
	}

	else if (calltype == PTN2FW_Call_AddDestwComp){
		PatternId_t patternId;
		 FMessageId_t  msgId;
		 NodeId_t destArray[MAX_DEST];
		 uint16_t noOfNbrs;

	        void* destArrayptr = (void*)destArray;
	         GenericMsgPayloadSize_t totalsize;

		 LinkComparatorTypeE lcType;
		Read<int,   PatternId_t, FMessageId_t , GenericMsgPayloadSize_t, void*, LinkComparatorTypeE >
			(sockfd, calltype,  patternId, msgId, totalsize, destArrayptr, lcType);

		noOfNbrs = totalsize / sizeof(NodeId_t);

	fi->AddDestinationRequest(patternId, msgId, destArray, noOfNbrs, lcType);
	}

	else if (calltype == PTN2FW_Call_Cancel){
		PatternId_t patternId;
		 FMessageId_t  msgId;
		 NodeId_t destArray[MAX_DEST];
		 uint16_t noOfDest;

         void* destArrayptr = (void*)destArray;
         GenericMsgPayloadSize_t totalsize;

		Read<int,   PatternId_t, FMessageId_t , GenericMsgPayloadSize_t, void* >
			(sockfd, calltype,  patternId, msgId, totalsize, destArrayptr);

		noOfDest = totalsize / sizeof(NodeId_t);

	fi->CancelDataRequest(patternId, msgId, destArray, noOfDest);
	}

	else if (calltype == PTN2FW_Call_DataStatus){
		PatternId_t patternId;
		 FMessageId_t  msgId;
		Read<int,   PatternId_t, FMessageId_t  >
			(sockfd, calltype,  patternId, msgId);
	fi->DataStatusRequest(patternId, msgId);
	}

//	else if (calltype == PTN2FW_Call_RegisterPattern){
//		PatternId_t patternId;
//		 PatternTypeE type;
//		GenericDeSerializer< PatternId_t, PatternTypeE >
//			    gs(&ptn2fwmsg,  patternId, type);
//
//		Debug_Printf(DBG_SHIM, "PTN_ControllerShim::Deserialize: PTN2FW_Call_RegisterPattern patternId = %d type = %d \n", patternId, type);
//
//	fi->RegisterPatternRequest(patternId, type);
//	}

	else if (calltype == PTN2FW_Call_FrameworkAttributes){
		PatternId_t patternId;
		Read<int,   PatternId_t >
			(sockfd, calltype,  patternId);
	fi->FrameworkAttributesRequest(patternId);
	}

	else if (calltype == PTN2FW_Call_SelectDataNotification){
		PatternId_t patternId;
		 uint8_t notifierMask;
		Read<int,   PatternId_t, uint8_t >
			(sockfd, calltype,  patternId, notifierMask);
	fi->SelectDataNotificationRequest(patternId, notifierMask);
	}


	else if (calltype == PTN2FW_Call_SelectLinkComparator){
		PatternId_t patternId;
		 LinkComparatorTypeE lcType;
		Read<int,   PatternId_t, LinkComparatorTypeE >
			(sockfd, calltype,  patternId, lcType);
	fi->SelectLinkComparatorRequest(patternId, lcType);
	}

	else if (calltype == PTN2FW_Call_SetLinkThreshold){
		PatternId_t patternId;
		 LinkMetrics threshold;
		Read<int,   PatternId_t, LinkMetrics >
			(sockfd, calltype,  patternId, threshold);
	fi->SetLinkThresholdRequest(patternId, threshold);
	}


//	else if (calltype == PTN2FW_Call_NewPatternInstance){
//		PatternTypeE type;
//		 char uniqueName[128];
//		 void* uniqueNameptr = (void*)uniqueName;
//		 GenericMsgPayloadSize_t totalsize;
//		GenericDeSerializer< PatternTypeE, GenericMsgPayloadSize_t, void* >
//			    gs(&ptn2fwmsg,  type, totalsize, uniqueNameptr);
//		static_cast<FrameworkBase*>(fi)->NewPatternInstanceRequest(type, uniqueName);
//	}




	else{
			Debug_Printf(DBG_SHIM, "PTN_ControllerShim::Deserialize: Unknown message \n");
			//PrintMsg(buf, size);
			rv = false;
	}

    if(!rv){
    	Debug_Printf(DBG_SHIM, "FWP2AppShim::Deserialize Failed to Deserialize ... sockfd = %d \n", sockfd);
    	return false;
    }
	
    Debug_Printf(DBG_SHIM, "PTN_ControllerShim::Deserialize:EXITING \n");
	return true;
}

