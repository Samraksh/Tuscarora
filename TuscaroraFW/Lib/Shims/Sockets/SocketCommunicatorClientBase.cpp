////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#include "SocketCommunicatorClientBase.h"
#include <Sys/SocketShim.h>
#include <sys/ioctl.h>

namespace Lib {
namespace Shims {
extern IO_Signals ioSignals;

SocketCommunicatorClientBase::SocketCommunicatorClientBase(string _server, int32_t _port, SocketTypeE _type) {
	  sc = NULL;
	  socketID = 0;

	  server = _server;
	  port = _port;
	  type = _type;


}
SocketCommunicatorClientBase::~SocketCommunicatorClientBase() {
	// TODO Auto-generated destructor stub
}

int32_t SocketCommunicatorClientBase::GetSocketID(){
	return socketID;
}

bool SocketCommunicatorClientBase::IsConnected(){
	return (socketID > 0);
}

bool SocketCommunicatorClientBase::Connect(int32_t commId)
{
	Debug_Printf(DBG_SHIM, "SocketCommunicatorClientBase::ConnectToFramework \n");
	if(IsConnected()) {return true;}
	if(sc) { delete(sc);}
	sc = new SocketClient(server, port, type);
	sc->Connect();
	socketID = sc->GetSocketID();
	if(socketID > 0){
		Debug_Printf(DBG_SHIM, "SocketCommunicatorClientBase::ConnectToFramework connected! \n");
		SocketSignalDelegate_t* del = new SocketSignalDelegate_t(this, &SocketCommunicatorClientBase::OnSignal);
		ioSignals.Register(socketID, del);
	}
	else{
		Debug_Printf(DBG_SHIM, "SocketCommunicatorClientBase::ConnectToFramework failed\n");
	}
	return false;
}

void SocketCommunicatorClientBase::OnSignal(int32_t sockfd){
	if (sockfd == socketID) {
		printf("PatternShim::OnSignal: Received a signal on the Command socket %d \n", sockfd);
		OnRecv(sockfd);

	}else {
		printf("PatternShim::OnSignal: Received a signal for unknown socketsocket %d \n", sockfd);

	}
}

void SocketCommunicatorClientBase::OnRecv(int sockfd){
	Debug_Printf(DBG_SHIM, "SocketCommunicatorClientBase::OnRecv: Reading from socket %d \n", sockfd);

	bool rv = true;
	while(rv ){
		rv = Deserialize ();
		Debug_Printf(DBG_SHIM, "SocketCommunicatorClientBase::So far Deserialized");
	}
	if(rv){
		Debug_Printf(DBG_SHIM, "SocketCommunicatorClientBase::OnRecv: finished successfully\n");
	}
	else{
		Debug_Printf(DBG_SHIM, "SocketCommunicatorClientBase::OnRecv: finished unsuccessfully\n");
	}
//    }
//	else {
//        if(readBytes ==0){
//            Debug_Printf(DBG_SHIM, "SocketCommunicatorClientBase::Deserialize: Got a signal on socket, but couldnt read. Probably the pattern terminated \n");
//            //Cleanup(sockfd);
//        }
//    }
}

bool SocketCommunicatorClientBase::Read(void * buf, uint32_t maxSize){
	// If connection is established then start communicating
	bzero(buf,maxSize);
	int32_t n = read( socketID,buf,maxSize );

	if (n < 0) {
		Debug_Printf(DBG_SHIM,"ERROR reading from socket");
	}
	return n > 0;
}

void SocketCommunicatorClientBase::PrintMsg(char* buf, uint32_t size)
{
    for(uint32_t i=0; i<size; i++){
        printf("%X ", buf[i]);
    }
    printf("\n");
}

bool SocketCommunicatorClientBase::SendMsg2Server(uint8_t* buf, int32_t size){
    int32_t w = sc->Write(reinterpret_cast<char*>(buf), size);
    return (w > 0);
}





}
} /* namespace Lib */
