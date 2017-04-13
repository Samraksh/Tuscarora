////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef PLATFORM_LINUX_MULTIPROCESS_SocketCommunicatorClientBase2_H_
#define PLATFORM_LINUX_MULTIPROCESS_SocketCommunicatorClientBase2_H_
#include <Lib/Shims/Sockets/SocketClient.h> //TODO: BK: This should be eliminated
#include "Lib/KernalSignals/IO_Signals.h"
#include "SocketMessages.h"

namespace Lib {
namespace Shims {



class SocketCommunicatorClientBase {
private:


    SocketClient *sc;

    string server;
    int32_t port;
    SocketTypeE type;

    void OnSignal(int32_t sockfd);
    void OnRecv(int sockfd);
    void PrintMsg(char* buf, uint32_t size);
    int32_t GetSocketID();
    bool IsConnected();
protected:

    int32_t socketID;

    bool SendMsg2Server(uint8_t* buf, int32_t size);
    bool Connect(int32_t commId);
    bool Read(void * buf, uint32_t maxSize);


    virtual bool Deserialize() = 0;



public:

    template<typename T1,typename T2=void*, typename... TNs>
    bool SendMsg2Server(const T1& i1, const T2& i2, const TNs&... ins){
    	SocketSerializer socketser(socketID);
    	return socketser.Write<T1, T2, TNs...>(i1,i2, ins...);
    }

    template<typename T1>
    bool SendMsg2Server(const T1& i1){
    	SocketSerializer socketser(socketID);
    	return socketser.Write<T1>(i1);
    }

	template<typename T1,typename T2, typename... TNs>
	uint32_t Read(  T1& i1, T2& i2, TNs&... ins) const{
		SocketDeSerializer scktreader(socketID, true);
		scktreader.Read<T1, T2, TNs...>(i1,i2,ins...);
		return scktreader.GetNumVarsRead();
	};

	template<typename T1>
	uint32_t Read(  T1& i1) const {
		SocketDeSerializer scktreader(socketID, true);
		scktreader.Read<T1>(i1);
		return scktreader.GetNumVarsRead();
	};


	template<typename T1>
	bool Peek(  T1& i1) const{
		SocketDeSerializer scktreader(socketID, false);
		bool rv = scktreader.Peek<T1>(i1);
		return rv;
	};


	SocketCommunicatorClientBase(string _server, int32_t _port, SocketTypeE _type);
	virtual ~SocketCommunicatorClientBase();

};
}
} /* namespace Apps */

#endif /* PLATFORM_LINUX_MULTIPROCESS_APP_SOCKETCOMMUNICATORBASE_H_ */
