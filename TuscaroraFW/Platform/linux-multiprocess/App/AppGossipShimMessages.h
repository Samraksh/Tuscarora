////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef APP_SHIM_MESSAGES_H
#define APP_SHIM_MESSAGES_H

//#include <Interfaces/PWI/FrameworkResponces.h>
//#include <Interfaces/PWI/PatternEventDispatch_I.h>

#define MAX_PAYLOAD 2048
#define GREETINGMSGSIZE 22

enum APP2GOSSIP_Call_MethodE {
	APP2GOSSIP_Call_Greet = 1,
	APP2GOSSIP_Call_Send,
	APP2GOSSIP_Call_RegisterApp,

};

enum GOSSIP2APP_Call_MethodE {
	GOSSIP2APP_Event_RcvMsg = 1,
};



#endif //PTN_SHIM_MESSAGES_H //PTN_API_CALLS_H
