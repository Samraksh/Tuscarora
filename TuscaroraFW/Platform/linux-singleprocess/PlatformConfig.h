////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef _PLATFORM_CONFIG_LINUX_SP_
#define _PLATFORM_CONFIG_LINUX_SP_

#warning "Compiling for Linux single process deployment! Using Platform Config under linux-singleprocess"

#define ENABLE_PIGGYBACKING 1
#define ENABLE_EXPLICIT_SENDER_TIMESTAMPING 1
#define ENABLE_IMPLICIT_SYNC_TIMESTAMPING 0

#define ENABLE_FW_BROADCAST 0

#endif //
