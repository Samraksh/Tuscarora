////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef EVENT_SOCKET_H_
#define EVENT_SOCKET_H_

class EventSocket {
	int32_t socketID;
public:
	EventSocket() {
	}
	
	int32_t GetSocketID(){
		return socketID;
	}
	
	bool IsConnected(){
		return (socketID > 0);	
	}
	
	bool Connect(){
		return true;
	}
	
	bool Invoke (char * buf, int32_t size){
		return true;	
	}
};

#endif //EVENT_SOCKET_H_