////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef IO_SIGNALS_H_
#define IO_SIGNALS_H_

#include <stdio.h>
#include <signal.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <Base/Delegate.h>
#include "Lib/DS/BSTMapT.h"

using namespace PAL;

//Signals the individual socket provided as the parameter
typedef Delegate<void, int32_t > SocketSignalDelegate_t;

void ReceiveSigIO(int sig);
void ReceiveIOSignal(int sig, siginfo_t *si, void *uc);


class IO_Signals {
//	static uint16_t noOfObjects;
	static BSTMapT<int32_t, SocketSignalDelegate_t*> sockMap;
	static bool lock;
	static bool pending;
	static uint16_t registered;

	fd_set readset;
public:
	IO_Signals(){
		
		lock=false;
		pending=false;
		registered =0;
	}

	static bool RegisterWithKernel(){
	//sigset_t recv_mask;
		struct sigaction recv_sa;
		memset (&recv_sa, '\0', sizeof(recv_sa));
		recv_sa.sa_flags = SA_SIGINFO;
		recv_sa.sa_sigaction = &ReceiveIOSignal;
		sigemptyset(&recv_sa.sa_mask);
		sigaddset(&recv_sa.sa_mask, SIGIO);
	#if defined(PLATFORM_LINUX)
		if (sigaction(SIGIO, &recv_sa, NULL) == -1){
			perror("sigaction");exit(1);
		}else {
			printf ("SIGIO action handler registration SUCCESS!\n");
		}
	#elif defined(PLATFORM_DCE)	
		signal(SIGIO, ReceiveSigIO);
		printf ("SIGIO action handler registration SUCCESS!\n");
	#endif	
		registered++;
		return true;
	}
	
	static void MakeSocketNonBlocking(int32_t sockfd){
		/*int status;
		status = fcntl (sockId, F_GETFL, 0);
		if (status == -1)
		{
				perror ("fcntl get:");
				exit (1);
		}
		status = fcntl (sockId, F_SETFL, status | O_NONBLOCK);
		if (status == -1)
		{
				perror ("fcntl set:");
				exit (1);
		}
		status = fcntl (sockId, F_GETFL, 0);
		status = fcntl(sockId, F_SETFL, status| O_ASYNC );
		if(status < 0) {
			perror("fcntl set asunc"), exit(1);
		}*/

		printf("Setting socket %d in async non blocking mode\n", sockfd);
	#if defined(PLATFORM_LINUX)
		fcntl(sockfd, F_SETOWN, (int) getpid());
		fcntl(sockfd, F_SETSIG, SIGIO);
	#endif
		/*int flags = fcntl(sockfd, F_GETFL, 0);
		if (fcntl(sockfd, F_SETFL, flags| O_ASYNC | O_NONBLOCK) < 0) {
			perror("fcntl"), exit(1);
		}*/
		int status;
		status = fcntl (sockfd, F_GETFL, 0);
		if (status == -1)
		{
			perror ("fcntl get:");
			exit (1);
		}
		status = fcntl (sockfd, F_SETFL, status | O_NONBLOCK);
		if (status == -1)
		{
			perror ("fcntl set:");
			exit (1);
		}
		status = fcntl (sockfd, F_GETFL, 0);
		status = fcntl(sockfd, F_SETFL, status| O_ASYNC );
		if(status < 0) {
			perror("fcntl set asunc"), exit(1);
		}

	}

	/*static void CheckSocketsAndSignal()
	{
		if(!lock){
			lock=true;
			startOver: 
			for( BSTMapT<int32_t, SocketSignalDelegate_t*>::Iterator i = sockMap.Begin(); i!=sockMap.End() ; ++i) {
				int32_t sockfd = i->First();
				if(sockfd == 0) {continue;}
				int cont = 1;
				while(cont == 1){
					int count;
					int ret=ioctl(sockfd, FIONREAD, &count);
					if(ret==EINVAL){
						Debug_Printf(DBG_SHIM, "CheckSocketsAndSignal: This is a listening socket\n");
					}
					if(count < 0 ){
						cont=0;
						continue;
					}else{
						Debug_Printf(DBG_SHIM, "CheckSocketsAndSignal: Socket ID %d got some stuff\n",sockfd);
						if(ret==EINVAL){
							Debug_Printf(DBG_SHIM, "CheckSocketsAndSignal: This is a listening socket\n");
							cont=0;
						}
						(i->Second())->operator()(sockfd);
					}
				}//end while
			};//end for
			
			if (pending){
				Debug_Printf(DBG_SHIM, "CheckSocketsAndSignal: Setting pending to false\n");
				pending=false;
				goto startOver;
			}
			lock=false;
		}
		else {
			//We are already in a Signal handler
			pending=true;
			Debug_Printf(DBG_SHIM, "CheckSocketsAndSignal: Already in handler, setting pending to true\n");
		}
	}//end method
	*/

	static bool CheckListenSocket(int fd){
		fd_set readset;
		int result= -1;
		struct timeval tv;

		// Initialize the set
		FD_ZERO(&readset);
		FD_SET(fd, &readset);
		
		// Initialize time out struct
		tv.tv_sec = 0;
		tv.tv_usec = 0;
		// select()
		result = select(fd+1, &readset, NULL, NULL, &tv);
		
		if(result < 0) return false;
		if(result > 0 && FD_ISSET(fd, &readset)) return true;

		return false;
	}
	
	static void CheckSocketsAndSignal()
	{
		if(!lock){
			lock=true;
			startOver: 
			for( BSTMapT<int32_t, SocketSignalDelegate_t*>::Iterator i = sockMap.Begin(); i!=sockMap.End() ; ++i) {
				int32_t sockfd = i->First();
				if(sockfd == 0) {continue;}
				
				//int ret=ioctl(sockfd, FIONREAD, &count);
				//if(count > 0 ){
				bool status = CheckListenSocket(sockfd);
				if(status){
					//printf("IO_Signals::CheckSocketsAndSignal: Socket ID %d got some stuff\n",sockfd);
					Debug_Printf(DBG_SHIM, "IO_Signals::CheckSocketsAndSignal: Socket ID %d got some stuff\n",sockfd);
					/*if(ret==EINVAL){
						Debug_Printf(DBG_SHIM, "IO_Signals:: CheckSocketsAndSignal: This is a listening socket\n");
					}*/
					(i->Second())->operator()(sockfd);
				}
				
			};//end for
			
			if (pending){
				Debug_Printf(DBG_SHIM, "CheckSocketsAndSignal: Setting pending to false\n");
				pending=false;
				goto startOver;
			}
			lock=false;
		}
		else {
			//We are already in a Signal handler
			pending=true;
			Debug_Printf(DBG_SHIM, "CheckSocketsAndSignal: Already in handler, setting pending to true\n");
		}
	}//end method
	
	static bool Register(int32_t sockfd, SocketSignalDelegate_t *del){
		if(registered < 2){
			RegisterWithKernel();
		}
		if(del && sockfd) {
			MakeSocketNonBlocking(sockfd);
			sockMap.Insert(sockfd, del);
			CheckSocketsAndSignal();
			printf("IO_Signals::Register: SUCCESS\n");
			return true;
		}
		printf("IO_Signals::Register: Failed\n");
		return false;
	}

	static bool DeRegister(int32_t sockfd){
		if(sockMap[sockfd]) {
			//MakeSocketNonBlocking(sockfd);
			sockMap.Erase(sockfd);
			return true;
		}
		return false;
	}
};

#endif //IO_SIGNALS_H_
