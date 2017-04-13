////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include "EventSocket.h"

namespace Waveform {
	
int32_t EventSocket::socketID;
SocketClient* EventSocket::sc;

bool EventInitialized=false;
EventSocket eventSocket;

bool EventSocket::Connect(WaveformId_t wid){
	if(sc) { delete(sc);}
	sc = new SocketClient(FRAMEWORK_HOST, FRAMEWORK_WF_PORT, FRAMEWORK_WF_PORT_TYPE);
	sc->Connect();
	socketID = sc->GetSocketID();
	if(socketID > 0){
		int32_t w=WriteGreeting(wid);
		if( w > 0) {			
			printf("EventSocket: Wrote %d byte message \n", w);
			return true;
		}
	}
	return false;
}


int32_t EventSocket::WriteGreeting(WaveformId_t wid){
	WF_GreetEventMsg g;
	g.eventType = WF_GREET_EVT;
	g.wid0 =wid;
	g.wid1 =wid;
	strcpy(g.greet, "HELLLO FROM WAVEFORM");
	printf("Sending greetings message to framework of size: %lu \n", sizeof(g));
	return sc->Write((char *)&g, sizeof(g));
}

bool EventSocket::Invoke(char* buf, int32_t size){
	int32_t w = sc->Write(buf, size);
	return (w > 0);	
}

} //End namespace