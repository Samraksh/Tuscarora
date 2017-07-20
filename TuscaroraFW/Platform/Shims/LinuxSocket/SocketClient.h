////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef SOCKET_CLIENT_H_
#define SOCKET_CLIENT_H_

#include "../../../Platform/Shims/LinuxSocket/Common.h"
#include "Lib/Misc/Debug.h"

namespace Lib {
namespace Shims {


class SocketClient {
	int32_t socketID=0;
	int32_t portNumber=0;
	char serverString[128];
	SocketTypeE socketType;
	
public:
	SocketClient(string server, int32_t port, SocketTypeE type ){
		socketID=0;
		portNumber=port;
		socketType=type;
		strcpy(serverString, server.c_str());
	}
	
	bool Reconnect(){
		return true;
	}
	
	int32_t GetSocketID(){
		return socketID;
	}
	
	bool IsConnected(){
		return (socketID > 0);	
	}
	
	bool Connect(){
		int sockfd=0;
		struct sockaddr_in serv_addr;
		struct hostent *hserver;
		if(socketType == TCP_SOCK) {
		  sockfd = socket(AF_INET, SOCK_STREAM, 0);
		}else {
		  sockfd = socket(AF_INET, SOCK_DGRAM, 0);	
		}
		if (sockfd < 0) {
			Debug_Printf(DBG_SHIM,"ERROR opening socket");
			return false;
		}
		hserver = gethostbyname(serverString);
		if (hserver == NULL) {
			Debug_Printf(DBG_SHIM,"ERROR, no such host\n");
			return false;
		}
		bzero((char *) &serv_addr, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		bcopy((char *)hserver->h_addr, 
			(char *)&serv_addr.sin_addr.s_addr,
			hserver->h_length);
		serv_addr.sin_port = htons(portNumber);
		if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
			Debug_Printf(DBG_SHIM,"ERROR connecting");
			return false;
		}
		socketID=sockfd;
		Debug_Printf(DBG_SHIM,"SocketClient: Socket ID %d Connected to Server Successfully at %s:%d \n\n", socketID,serverString, portNumber);
		return true;
	}
	
	int32_t Write(char* buf, uint32_t size){
		printf("Writing %d bytes to socket %d \n", size, socketID);
		PrintMsg(buf,size);
		
		int32_t n = write(socketID,buf,size);
		//n = write(socketID,buf,size);
		
		//fflush(socketID);
		
		printf("Wrote %d bytes\n\n", n);
		
		if (n < 0) {
			Debug_Printf(DBG_SHIM,"ERROR writing to socket");
		}
		else if(n < (int32_t)size){
			Debug_Printf(DBG_SHIM,"Wrote %d bytes, but not all\n", n);
		}
		Flush(socketID);
		
		return n;
	}
	
	int32_t Read(char * buf, uint32_t maxSize){
		// If connection is established then start communicating 
		bzero(buf,maxSize);
		int32_t n = read( socketID,buf,maxSize );
		
		if (n < 0) {
			Debug_Printf(DBG_SHIM,"ERROR reading from socket");
		}
		return n;
	}
	
	void PrintMsg (char* buf, uint32_t size){
		for (uint i=0; i< size; i++){
			printf("%X ", buf[i]);
		}
		printf("\n");
	}
	void Flush(int32_t sock){
		int flag = 1; 
		setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int));
		//send(sock, "important data or end of the current message", ...);
		flag = 0; 
		setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int));
			
	}
};

} //end namespace
}

#endif //SOCKET_CLIENT_H_