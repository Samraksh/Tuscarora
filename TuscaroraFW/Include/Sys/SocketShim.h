////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef PLATFORM_SOCKETSHIM_H_
#define PLATFORM_SOCKETSHIM_H_

//constants for Framework
#define WF_SOCKET 7777
#define PATTERN_SOCKET 9999

//constants for Waveform, note the port should match with framework defines
#define FRAMEWORK_HOST "localhost"
#define FRAMEWORK_WF_PORT 7777
#define FRAMEWORK_WF_PORT_TYPE TCP_SOCK

//constants for Patterns,, note the port should match with framework defines
//#define FRAMEWORK_HOST "localhost"
#define FRAMEWORK_PATTERN_PORT 9999
#define FRAMEWORK_PATTERN_PORT_TYPE TCP_SOCK

//
#define FWP_HOST "localhost"
#define FWP_APP_PORT 6999
#define FWP_APP_PORT_TYPE TCP_SOCK


#define GOSSIP_HOST "localhost"
#define GOSSIP_APP_PORT 5999
#define GOSSIP_APP_PORT_TYPE TCP_SOCK

#define MAX_SOC_BUF_SIZE 2048

#endif /* PLATFORM_SOCKETSHIM_H_ */
