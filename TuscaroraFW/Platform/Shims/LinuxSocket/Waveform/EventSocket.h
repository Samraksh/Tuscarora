////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef EVENT_SOCKET_H_
#define EVENT_SOCKET_H_

#include <Sys/SocketShim.h>
#include <Base/FrameworkTypes.h>
#include "Platform/Shims/LinuxSocket/SocketClient.h"
#include "Platform/Shims/WF_API_Calls.h"

using namespace Types;
using namespace Lib::Shims;

namespace Waveform {

class EventSocket {
	static int32_t socketID;
	static SocketClient *sc;
public:
	EventSocket() {
		sc = 0; socketID=0;
	}
	
	int32_t GetSocketID(){
		return socketID;
	}
	
	bool IsConnected(){
		return (socketID > 0);	
	}
	
	bool Connect(WaveformId_t wid);
	
	bool Invoke (char * buf, int32_t size);
	
	int32_t WriteGreeting(WaveformId_t wid);
};

} //End namespace

#endif //EVENT_SOCKET_H_
