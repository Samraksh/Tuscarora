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

enum APP2FWP_Call_MethodE {
	APP2FWP_Call_Greet = 1,
	APP2FWP_Call_Send,
	APP2FWP_Call_RegisterApp,

};

enum FWP2APP_Call_MethodE {
	FWP2APP_Event_RcvMsg = 1,
};



#endif //PTN_SHIM_MESSAGES_H //PTN_API_CALLS_H
