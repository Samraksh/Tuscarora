////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef _PLATFORM_CONFIG_LINUX_
#define _PLATFORM_CONFIG_LINUX_

//#warning "Compiling for Linux multi process deployment! Using Platform Config under linux-multiprocess"

//#define ENABLE_PIGGYBACKING 1
//#define ENABLE_FW_BROADCAST 0

#define FW_LOWER_SHIM SOCKET_SHIM

#define ENABLE_PIGGYBACKING 1
#define ENABLE_EXPLICIT_SENDER_TIMESTAMPING 1
#define ENABLE_IMPLICIT_SYNC_TIMESTAMPING 0

#define ENABLE_FW_BROADCAST 0

#endif //
