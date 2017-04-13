////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef LIB_SHIMS_SOCKETS_SocketCommunicatorServerBase_H_
#define LIB_SHIMS_SOCKETS_SocketCommunicatorServerBase_H_

#include <Lib/Shims/Sockets/SocketServer.h>
#include <Lib/KernalSignals/IO_Signals.h>
#include "SocketMessages.h"




namespace Lib {
namespace Shims {


extern IO_Signals ioSignals;

template<typename CLIENTIDTYPE>
class SocketCommunicatorServerBase {
private:
	class ClientInitializer{
		SocketCommunicatorServerBase* socket_com_ptr;
	public:
		ClientInitializer(SocketCommunicatorServerBase* _socket_com_ptr){
			socket_com_ptr = _socket_com_ptr;
		}
		void InitClient(CLIENTIDTYPE pid, int32_t sock) const{
			socket_com_ptr->InitClient(pid, sock);
		}
	};
	SocketId_t ptnMasterSocket;
	SocketServer *master;
	bool m_IsSingleClientEnforced;
	SocketSignalDelegate_t *del;

	string server;
	int32_t port;
	SocketTypeE type;




	bool Cleanup (int sockfd){
		return (ioSignals.DeRegister(sockfd));
	}

	SocketId_t NewConnection(){
		SocketId_t newsock=0;
		newsock = master->Accept();

		if (newsock > 0){
			//Register this socket with the IO_Signal handler and store this in the list of waveforms
			//First read from socket and figure out the Waveform ID;
			//WaveformId_t nptnd =0;
			Debug_Printf(DBG_SHIM, "SocketCommunicatorServerBase::NewConnection: Master socket has received a new connection on socket %d \n", newsock);

		}else {
			Debug_Printf(DBG_SHIM, "Error: SocketCommunicatorServerBase: Something wrong with accepting connections\n");

		}
		return newsock;
	};

	void OnRecv (int sockfd){
		//int32_t readBytes = read(sockfd,readBuf, MAX_SOC_READ_SIZE);
		Debug_Printf(DBG_SHIM, "PTN_ControllerShim::OnRecv: Reading from socket %d \n", sockfd);

		bool rv = true;
		while(rv ){
			rv = Deserialize (sockfd);
			Debug_Printf(DBG_SHIM, "PTN_ControllerShim::So far Deserialized:");
		}
		if(rv){
			Debug_Printf(DBG_SHIM, "PTN_ControllerShim::OnRecv: finished successfully\n");
		}
		else{
			Debug_Printf(DBG_SHIM, "PTN_ControllerShim::OnRecv: finished unsuccessfully\n");
		}

		Debug_Printf(DBG_SHIM, "PTN_ControllerShim::OnRecv:EXITING \n");
	};

	void PrintMsg(char* buf, uint32_t size)
	{
		for(uint32_t i=0; i<size; i++){
			//printf("%u: %c, ", i, buf[i]);
			printf("%X ", buf[i]);
		}
		printf("\n");
	}

	void OnSignal(int32_t sockfd){
		//You should read the socket here

		//this is the master socket, a waveform is (re)connecting
		//printf("PTN_ControllerShim::OnSignal: Received a signal for socket %d \n", sockfd);
		if (sockfd == ptnMasterSocket) {
			printf("PTN_ControllerShim::OnSignal: Received a signal for Master socket %d \n", sockfd);
			SocketId_t nwPTN = NewConnection();
			if (nwPTN > 0) {
				ioSignals.Register(nwPTN, del);
			}
		}
		else {
			//This is one of the client sockets, read message and process
			printf("PTN_ControllerShim::OnSignal: Received a signal for a pattern socket %d \n", sockfd);
			OnRecv(sockfd);
		}
	};

protected:
	typedef BSTMapT<CLIENTIDTYPE, SocketId_t> SocketMap_t;
	mutable SocketMap_t ptnToSockIdMap;

	virtual bool Deserialize (int32_t sockfd) = 0;

	ClientInitializer* client_init_ptr;


	void InitClient(CLIENTIDTYPE pid, int32_t sock) {
		Debug_Printf(DBG_SHIM, "SocketCommunicatorServerBase::InitPattern: Initializing shim for pattern %d on sock %d\n", pid, sock);
		if(ptnToSockIdMap.Find(pid) == ptnToSockIdMap.End()){
			Debug_Printf(DBG_SHIM, "SocketCommunicatorServerBase::InitWaveform: registering socket for pattern  %d new sock %d\n", pid, sock);
			if(m_IsSingleClientEnforced && ptnToSockIdMap.Size() > 1) {
				Debug_Printf(DBG_SHIM, "SocketCommunicatorServerBase::InitWaveform: Cannot Register socket for pattern due to m_IsSingleClientEnforced = %d  (pattern  %d new sock %d)\n", m_IsSingleClientEnforced, pid, sock);
			}
			else{
				ptnToSockIdMap.Insert(pid,sock);
			}
		}
		else{
			Debug_Printf(DBG_SHIM, "PTN_ControllerShim::InitWaveform: Overwriting socket for pattern %d old sock %d new sock %d\n", pid, ptnToSockIdMap[pid] ,sock);
			ptnToSockIdMap[pid] = sock;
		}
	};
	SocketCommunicatorServerBase(string _server, int32_t _port, SocketTypeE _type, bool _isSingleClientEnforced ){
		client_init_ptr = new ClientInitializer(this);
		Debug_Printf(DBG_SHIM, "SocketCommunicatorServerBase::SocketCommunicatorServerBase Initializing \n");
		m_IsSingleClientEnforced =  _isSingleClientEnforced;
		server = _server;
		port = _port;
		type = _type;

		del = new SocketSignalDelegate_t(this, &SocketCommunicatorServerBase<CLIENTIDTYPE>::OnSignal);
		master = new SocketServer(server, port, type);
		ptnMasterSocket = master->GetSocketID();

		ioSignals.Register(ptnMasterSocket, del);
		Debug_Printf(DBG_SHIM, "PTN_ControllerShim::PTN_ControllerShim Initializing \n");
	};


//	bool Read(int32_t sockfd, void * buf, uint32_t maxSize);
//	bool Write(int32_t sockfd, void * buf, uint32_t maxSize);
//



public:

	virtual ~SocketCommunicatorServerBase(){};

	template<typename T1,typename T2=void*, typename... TNs>
	bool SendMsg2Client(CLIENTIDTYPE pid, const T1& i1, const T2& i2, const TNs&... ins){
		if(m_IsSingleClientEnforced)
			return SendMsg2Client(i1, i2);
		else{
			typename SocketMap_t::Iterator iter =  ptnToSockIdMap.Find(pid);
			if(iter != ptnToSockIdMap.End()){
				SocketId_t socketID = iter->Second();
				Debug_Printf(DBG_SHIM, "SocketCommunicatorServerBase::SendMsg2Client Found a socket %d for the pattern %d \n", socketID, pid);
//				int32_t n = write(socketID, buf, size);
//				Debug_Printf(DBG_SHIM, "SocketCommunicatorServerBase:: Write : Sent %d bytes on socket %d \n", n, socketID);
//				PrintMsg((char*)buf,size);
//				return (n>0);
				SocketSerializer socketser(socketID);
				return socketser.Write<T1, T2, TNs...>(i1,i2, ins...);
			}
			else{
				Debug_Printf(DBG_SHIM, "Cannot find the sockket for the pattern %d \n", pid);
				return false;
			}
		}
	}

	template<typename T1>
	bool SendMsg2Client(CLIENTIDTYPE pid, const T1& i1){
		if(m_IsSingleClientEnforced)
			return SendMsg2Client(i1);
		else{
			typename SocketMap_t::Iterator iter =  ptnToSockIdMap.Find(pid);
			if(iter != ptnToSockIdMap.End()){
				SocketId_t socketID = iter->Second();
				Debug_Printf(DBG_SHIM, "SocketCommunicatorServerBase::SendMsg2Client Found a socket %d for the pattern %d \n", socketID, pid);
//				int32_t n = write(socketID, buf, size);
//				Debug_Printf(DBG_SHIM, "SocketCommunicatorServerBase:: Write : Sent %d bytes on socket %d \n", n, socketID);
//				PrintMsg((char*)buf,size);
//				return (n>0);
				SocketSerializer socketser(socketID);
				return socketser.Write<T1>(i1);
			}
			else{
				Debug_Printf(DBG_SHIM, "Cannot find the sockket for the pattern %d \n", pid);
				return false;
			}
		}
	}


	template<typename T1,typename T2=void*, typename... TNs>
	bool SendMsg2Client( const T1& i1, const T2& i2, const TNs&... ins){
		typename SocketMap_t::Iterator iter = ptnToSockIdMap.Begin();
		if(iter != ptnToSockIdMap.End()){
			SocketId_t socketID = iter->Second();
			Debug_Printf(DBG_SHIM, "SocketCommunicatorServerBase::SendMsg2Client Found a socket %d for the pattern %d \n", socketID,  iter->First());
			SocketSerializer socketser(socketID);
			return socketser.Write<T1, T2, TNs...>(i1,i2, ins...);
		}
		else{
			Debug_Printf(DBG_SHIM, "Cannot find any sockket for the pattern \n");
			return false;
		}
	}

	template<typename T1>
	bool SendMsg2Client( const T1& i1){
		typename SocketMap_t::Iterator iter = ptnToSockIdMap.Begin();
		if(iter != ptnToSockIdMap.End()){
			SocketId_t socketID = iter->Second();
			Debug_Printf(DBG_SHIM, "SocketCommunicatorServerBase::SendMsg2Client Found a socket %d for the pattern %d \n", socketID,  iter->First());
			SocketSerializer socketser(socketID);
			return socketser.Write<T1>(i1);
		}
		else{
			Debug_Printf(DBG_SHIM, "Cannot find any sockket for the pattern \n");
			return false;
		}
	}

	template<typename T1,typename T2, typename... TNs>
	uint32_t Read( const int32_t sockfd, T1& i1, T2& i2, TNs&... ins) const{
		SocketDeSerializer scktreader(sockfd, true);
		scktreader.Read<T1, T2, TNs...>(i1,i2,ins...);
		return scktreader.GetNumVarsRead();
		//return scktreader.GetNumVarsRead();
	};
	template<typename T1>
	uint32_t Read( const int32_t sockfd, T1& i1) const {
		SocketDeSerializer scktreader(sockfd, true);
		scktreader.Read<T1>(i1);
		return scktreader.GetNumVarsRead();
	};
	template<typename T1>
	uint32_t Peek( const int32_t sockfd, T1& i1) const{
		SocketDeSerializer scktreader(sockfd, false);
		return scktreader.Peek<T1>(i1);
	};


	template<typename T1,typename T2, typename... TNs>
	uint32_t Read(  T1& i1, T2& i2, TNs&... ins) const{
		if(ptnToSockIdMap.Size() == 0){
			return 0;
		}
		else{
			return Read <T1, T2, TNs...> ( ptnToSockIdMap.Begin()->Second(), i1, i2, ins...);
		}
	};
	template<typename T1>
	uint32_t Read( T1& i1) const {
		if(ptnToSockIdMap.Size() == 0){
			return 0;
		}
		else{
			return Read <T1> ( ptnToSockIdMap.Begin()->Second(), i1);
		}
	};
	template<typename T1>
	bool Peek( T1& i1) const {
		if(ptnToSockIdMap.Size() == 0){
			return 0;
		}
		else{
			return Peek <T1> ( ptnToSockIdMap.Begin()->Second(), i1);
		}
	};

};

}} /* namespace Shims */

#endif /* LIB_SHIMS_SOCKETS_SocketCommunicatorServerBase_H_ */
