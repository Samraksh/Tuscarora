////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <signal.h>

#include <Sys/Run.h>

/*#include <netpacket/packet.h>
#include <net/if.h>
#include <net/ethernet.h> // the L2 protocols 
#include <netinet/ether.h>
#include <netinet/in.h>
*/

#include <linux/netdevice.h>
//#include <linux/etherdevice.h>
//#include "../../test.h"



#define DEFAULT_IF	"eth0"
#define BUF_SIZ		1024

uint8_t mac1[6]= {0x38,0x1C,0x4A,0x1,0xA2,0xEA};
uint8_t mac2[6]= {0x9c,0x20,0x7b,0x35,0x1d,0xcc};
uint8_t *myDest;

int sockfd;
int sendNumber=0;
int rcvNumber=0;
char sendbuf[BUF_SIZ];
unsigned char rcvbuf[BUF_SIZ];
char ifName[IFNAMSIZ];
bool sender=false;

struct ifreq if_idx;
struct ifreq if_mac;
int protocol;
int recvSigNum=43;
sigset_t recv_mask;
struct sigaction recv_sa;


void PrintMsg(int len, int msgid,unsigned char* buf){
  printf("Msg %d: Length: %d\n", msgid, len);
  for (int i=0;i<len; i++){
    printf("%x ", buf[i]);
  }
  printf("\n");
}

static 
void ReceiveMsg(int sig, siginfo_t *si, void *uc)
//void ReceiveMsg(int sig)
{
  printf ("Handling recv signal: %d\n", sig);
  socklen_t saddr_size;
  struct sockaddr saddr;
  saddr_size = sizeof saddr;
  int msglen=recvfrom(sockfd, rcvbuf, BUF_SIZ, 0, &saddr,  &saddr_size);
  
  if (msglen < 0)
  {
    printf("recv failed: Got a signal but reading failed\n");
  }else {
    PrintMsg(msglen, rcvNumber, rcvbuf);
    rcvNumber++;
  }
  
}

bool CheckMAC(uint8_t * mac1, uint8_t* mac2){
	bool rtn=true;
	for(int i=0;i<6;i++){
		if(mac1[i]!=mac2[i]){
			rtn=false;
			break;
		}
	}

	return rtn;
}

void PrintMAC(uint8_t *mac){
  for (int i=0; i<6; i++){
        printf("%X:", mac[i]);
  }     
  printf("\n");
}

void InitializePacSoc(){
  /// Open RAW socket to send on 
  /*if ((sockfd = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW)) == -1) {
      perror("socket");
  }*/
  
  protocol= ETH_P_ALL;
  //protocol= FIXED_SAM_PAC_TYPE;
  
  if ((sockfd = socket(PF_PACKET, SOCK_RAW, protocol)) == -1) {
    perror("Cannot open socket");
  }
    
  memset(&if_idx, 0, sizeof(struct ifreq));
  strncpy(if_idx.ifr_name, ifName, IFNAMSIZ-1);
  if (ioctl(sockfd, SIOCGIFINDEX, &if_idx) < 0){
      perror("SIOCGIFINDEX");
  }

  memset(&if_mac, 0, sizeof(struct ifreq));
  strncpy(if_mac.ifr_name, ifName, IFNAMSIZ-1);
  if (ioctl(sockfd, SIOCGIFHWADDR, &if_mac) < 0){
    perror("SIOCGIFHWADDR");
  }

  printf("I am using MAC :: ");
  PrintMAC((unsigned char*)if_mac.ifr_hwaddr.sa_data);

  if(CheckMAC(mac1,(unsigned char*)if_mac.ifr_hwaddr.sa_data)){
  	myDest=mac2;
  }else {
  	myDest=mac1;
  }
  
  printf("My destination  MAC is :: ");
  PrintMAC(myDest);
  
  ///bind the socket to interface 
  struct sockaddr_ll sll;
  sll.sll_family = AF_PACKET;
  sll.sll_ifindex = if_idx.ifr_ifindex;
  sll.sll_protocol = htons (protocol);
  bind ( sockfd, (struct sockaddr *) &sll, sizeof (sll) ) ;
  
  ///set the interface to promiscous mode
 /* struct packet_mreq      mr;
  memset (&mr, 0, sizeof (mr));
  mr.mr_ifindex = if_idx.ifr_ifindex;
  mr.mr_type = PACKET_MR_PROMISC;
  setsockopt (sockfd, SOL_PACKET,PACKET_ADD_MEMBERSHIP, &mr, sizeof (mr));
  */
 
 ///Initialize signalling and callback
  printf("Establishing handler for signal %d\n", SIGIO);
  memset (&recv_sa, '\0', sizeof(recv_sa));
  recv_sa.sa_flags = SA_SIGINFO;
  recv_sa.sa_sigaction = &ReceiveMsg;
  sigemptyset(&recv_sa.sa_mask);
  sigaddset(&recv_sa.sa_mask, SIGIO);
  if (sigaction(SIGIO, &recv_sa, NULL) == -1){
      perror("sigaction");exit(1);
  }
  
  /* Mask off SIGIO and the signal you want to use. */
  //sigemptyset(&recv_mask);
  //sigaddset(&recv_mask, recvSigNum);
  //sigaddset(&recv_mask, SIGIO);
  //sigprocmask(SIG_BLOCK, &recv_mask, NULL);
  /* For each file descriptor, invoke F_SETOWN, F_SETSIG, and set O_ASYNC. */
  fcntl(sockfd, F_SETOWN, (int) getpid());
  fcntl(sockfd, F_SETSIG, SIGIO);
  int flags = fcntl(sockfd, F_GETFL, 0);
  if (fcntl(sockfd, F_SETFL, flags| O_ASYNC | O_NONBLOCK) < 0) {
    perror("fcntl"), exit(1);
  }
  
 
}

int main ()
{
	strcpy(ifName,DEFAULT_IF);
	InitializePacSoc();
	
	//RunMasterLoopNeverExit(); //never exists on dce
	RunTimedMasterLoop();
	return 0;
}