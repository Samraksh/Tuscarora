////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include "Common.h"
#include "WF_Wrapper_I.h"
#include <math.h>



//extern bool DBG_WAVEFORM;

namespace Waveform {
  
//unsigned char sendbuf[16][BUF_SIZE];  //Each wavefrom needs separate buffer.
//unsigned char rcvbuf[BUF_SIZE]; //Each wavefrom needs separate buffer.
//int32_t sockfd[MAX_WAVEFORMS];
WF_Wrapper_I *myWFPtr[MAX_WAVEFORMS];

struct ifreq if_idx;
//struct ifreq if_mac;
uint32_t protocol;
//uint32_t rcvNumber;

//signal stuff
//sigset_t recv_mask;
//struct sigaction recv_sa;

uint32_t x(){return (uint32_t)LocationService::GetLocation().x;}
uint32_t y(){return (uint32_t)LocationService::GetLocation().y;}

UFixed_2_8_t MilliWattToLog2PicoJoules(double mWperPacket, double avgPacketSizeBytes, double bitTimeInNanoSec)
{
	//double mWPerBit = mWperPacket/(8*avgPacketSizeBytes);
	double packetTime = 8*avgPacketSizeBytes*bitTimeInNanoSec/pow((double)10,9);
	double joulesPerSec = mWperPacket/(1000*packetTime);
	double picoJoules = joulesPerSec*pow((double)10,12);
	double log2PicoJoules = log2(picoJoules);
	printf("MillWatt= %f, PacketSize=%f, PacketTime=%f s, J/s= %f, PJ/s= %f, Log2(PJ/s)= %f \n", mWperPacket, avgPacketSizeBytes, packetTime, joulesPerSec,
				 picoJoules, log2PicoJoules);
	return log2PicoJoules;
}

UFixed_2_8_t DBmToLog2PicoJoules(double dbm, double bitTimeInNanoSec){

	return dbm * 25.35;
}

void PrintMsg(int msglen, int msgNumber, uint8_t * msg) {
	Debug_Printf(DBG_WAVEFORM,"Waveform:: Total Message size: %d, hdr %d, payload %d \n", msglen, WF_HDR_SIZE, msglen-WF_HDR_SIZE);
	printf("Header: ");
	for (int i = 0; i< WF_HDR_SIZE; i++){
		printf("%X ",msg[i]);
	}
	printf("\n");
	printf("Payload: ");
	for (int i = WF_HDR_SIZE; i< msglen; i++){
		printf("%X ",msg[i]);
	}
	printf("\n");
}

uint64_t ConvertAddressToU64(unsigned char *a){
	uint64_t ret=0;
	//uint8_t *l8 = (uint8_t*)ret;
	int s=6;
	for (int i=0; i< s; i++){
		//l8[s-i]= a[i];
		ret = ret | ((0xffffffffffffffff&a[i])<<((s-i-1)*8));
		//hprintf(" %02X: ", a[i]);
	}
	//printf("ConvertToAddress_u64:: Input: %s, Ouput: %lu \n",a, ret);
	return ret;
}

void ConvertU64ToAddress(uint64_t x, unsigned char* address)
{
	int s=6;
	uint8_t *tPtr = (uint8_t *)&x;
	for (int i=0; i< s; i++){
		address[i]= tPtr[s-1-i];
		//hprintf(" %02X: ", a[i]);
	}
	address[s]='\0';
	//printf("ConvertU64ToAddress:: %s \n", (char*)address);

}


int32_t InitPacSoc(uint16_t wfid, char *device, struct ifreq *ret_ifreq, char *ifAddress)
{
		protocol= ETH_P_C2E1;
	//protocol= FIXED_SAM_PAC_TYPE;
	int32_t sockfd=0;
	
	if ((sockfd = socket(PF_PACKET, SOCK_RAW, htons(protocol))) == -1) {
		perror("Cannot open socket");
	}
	else {
		printf("Created socket successfully with id %d\n", sockfd);
	}
		
	//Set the device name	
	memset(&if_idx, 0, sizeof(struct ifreq));
	strncpy(if_idx.ifr_name, device, IFNAMSIZ-1);
	if (ioctl(sockfd, SIOCGIFINDEX, &if_idx) < 0){
			perror("SIOCGIFINDEX");
	}

#ifdef PLATFORM_DCE
	char tdevice[5] = "eth";
	tdevice[3] = '0'+wfid;
	tdevice[4] = '\0';
	if(setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE, tdevice, sizeof(tdevice))< 0){
		perror ("Sock Opts Bind error:");
		exit (1);
	}
#elif defined(PLATFORM_LINUX)
	//Get the device MAC address of the device and store it
	memset(ret_ifreq, 0, sizeof(struct ifreq));
	strncpy(ret_ifreq->ifr_name, device, IFNAMSIZ-1);
	if (ioctl(sockfd, SIOCGIFHWADDR, ret_ifreq) < 0){
		perror("SIOCGIFHWADDR");
		fflush(stderr);fflush(stdout);
		exit(1);
	}
	
	memcpy(ifAddress, ret_ifreq->ifr_hwaddr.sa_data, IFNAMSIZ);
	const unsigned char* mac=(unsigned char*)ret_ifreq->ifr_hwaddr.sa_data;
	printf("MY MAC for device %s is :%02X:%02X:%02X:%02X:%02X:%02X\n",
		device, mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
	
	ret_ifreq->ifr_ifindex = if_idx.ifr_ifindex;
	
	fflush(stdout);
	struct sockaddr_ll sll;
	sll.sll_family = AF_PACKET;
	sll.sll_ifindex = if_idx.ifr_ifindex;
	sll.sll_protocol = htons (protocol);
	if(bind ( sockfd, (struct sockaddr *) &sll, sizeof (sll) ) < 0){
		perror("Bind error:");
		exit(1);
	}  
#endif
	return sockfd;
}

}//End namespace
