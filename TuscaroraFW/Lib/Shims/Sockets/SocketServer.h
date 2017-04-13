////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef SOCKET_SERVER_H_
#define SOCKET_SERVER_H_

#include "Lib/Misc/Debug.h"
#include "Common.h"

namespace Lib {
namespace Shims {

typedef int32_t SocketId_t;

class SocketServer {
    SocketId_t socketID=0;
	SocketTypeE socketType;
	int32_t portNumber=0;
	char address[128];
public:
	SocketServer(){
		/*strcpy(address,"localhost");
		portNumber = 8888;
		socketType=TCP_SOCK;
		Init();*/
		SocketServer("localhost", 8888, TCP_SOCK);
	}
	SocketServer (string ipaddress, int32_t port, SocketTypeE type){
		strcpy(address, ipaddress.c_str());
		//strcpy(address, ipaddress);
		socketID=0;
		socketType=type;
		portNumber=port;
		Init();
	}
	
	bool IsConnected(){
		if(socketID >0) {return true;}
		return false;
	}
	
	bool Reconnect(){
		return false;
	}
	
	SocketId_t GetSocketID(){
		return socketID;
	}
	
	bool Init(){
		struct sockaddr_in serv_addr;

		if(socketType == TCP_SOCK) {
			socketID = socket(AF_INET, SOCK_STREAM, 0);
		}else {
			socketID = socket(AF_INET, SOCK_DGRAM, 0);
		}
   
		if (socketID < 0) {
				Debug_Printf(DBG_SHIM,"ERROR opening socket");
				return false;
		}
		
		/* Initialize socket structure */
		bzero((char *) &serv_addr, sizeof(serv_addr));
		
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = INADDR_ANY;
		serv_addr.sin_port = htons(portNumber);
		
		/* Enable address reuse */
		int on = 1;
		setsockopt(socketID, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) );
		
		/* Now bind the host address using bind() call.*/
		if (bind(socketID, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
				perror("ERROR on binding");
				return false;
		}
        
		// Now start listening for the clients, here process will
		// go in sleep mode and will wait for the incoming connection 
		if (listen(socketID,5) >= 0 ){
			Debug_Printf(DBG_SHIM,"SocketServer: Server Listening on  %s:%d \n", address, portNumber);
		}	
		
		return true;
	}
	
	SocketId_t Accept(){
		int newsockfd;
		socklen_t clilen;
		struct sockaddr_in cli_addr;
		clilen = sizeof(cli_addr);
		// Accept actual connection from the client 
		newsockfd = accept(socketID, (struct sockaddr *)&cli_addr, &clilen);
	
		if (newsockfd < 0) {
			Debug_Printf(DBG_SHIM,"ERROR on accept");
		}
		
		Debug_Printf(DBG_SHIM,"SocketServer: New connection received, socket is %d \n", newsockfd);
		return newsockfd;
	}
	
	int32_t Write(char * buf, uint32_t size){
		
		int32_t n = write(socketID,buf,size);
   
		if (n < 0) {
				Debug_Printf(DBG_SHIM,"ERROR writing to socket");
		}
		else if(n < (int32_t)size){
			Debug_Printf(DBG_SHIM,"Wrote %d bytes, but not all\n", n);
		}
		return n;
	}
	
	
	
	int32_t Read(char *buf, uint32_t  maxSize){
		// If connection is established then start communicating 
		bzero(buf,maxSize);
		int32_t n = read( socketID,buf,maxSize );
		
		if (n < 0) {
			Debug_Printf(DBG_SHIM,"ERROR reading from socket");
		}
		return n;
	}
};

} //end namespace
}
#endif //SOCKET_SERVER_H_
