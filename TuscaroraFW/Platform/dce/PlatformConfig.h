////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef _PLATFORM_CONFIG_DCE_
#define _PLATFORM_CONFIG_DCE_

//#warning "Compiling for DCE deployment! Using Platform Config under DCE"

#define ENABLE_FRAGMENTATION 1
#define ENABLE_PIGGYBACKING 1
#define ENABLE_EXPLICIT_SENDER_TIMESTAMPING 1
#define ENABLE_IMPLICIT_SYNC_TIMESTAMPING 0

#define ENABLE_FW_BROADCAST 0


//define the shims you want to built for this platform
//Options currently supported are SOCKET_SHIM or DIRECT_BINDING_SHIM



//pattern shims
#define FWP_LOWER_SHIM DIRECT_BINDING_SHIM

#define PTN_UPPER_SHIM DIRECT_BINDING_SHIM
#define PTN_LOWER_SHIM DIRECT_BINDING_SHIM

//framework shims
#define FW_UPPER_SHIM DIRECT_BINDING_SHIM
#define FW_LOWER_SHIM DIRECT_BINDING_SHIM


//waveform shims
#define WF_UPPER_SHIM DIRECT_BINDING_SHIM



#endif //_PLATFORM_CONFIG_DCE_
