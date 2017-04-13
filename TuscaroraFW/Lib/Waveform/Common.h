////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef WF_COMMOM_H_
#define WF_COMMOM_H_

#include "Lib/Misc/Debug.h"
#include "Base/FixedPoint.h"

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
#include <netinet/in.h>
#include <endian.h>
#include <netpacket/packet.h>
#include <netinet/ether.h>

#include <netdb.h>
#include <ifaddrs.h>

//These are experimental types that are reserved
#define ETH_P_C2E1	0x88b5		//We define a custom type to easily filter out packets
#define ETH_P_C2E2	0x88b6		//We define a custom type to easily filter out packets

#ifdef PLATFORM_LINUX
#include "Platform/linux/ExternalServices/LocationService/LocationService.h"
#else
#include "Platform/dce/ExternalServices/LocationService/LocationService.h"
#endif

using namespace ExternalServices;

#define WF_HDR_SIZE 64
#define MAX_DCE_PKT_SIZE (1024+WF_HDR_SIZE) //This seems to be some peculier dce limit
#define MAX_WF_PKT_SIZE MAX_DCE_PKT_SIZE
#define MAX_AO_WF_PACKET_SIZE (MAX_DCE_PKT_SIZE - WF_HDR_SIZE)
#define BUF_SIZE ((MAX_DCE_PKT_SIZE+64)*2)

namespace Waveform {
	void PrintMsg(int msglen, int rcvNumber, uint8_t * msg);
	int32_t InitPacSoc(uint16_t wfid, char *device, struct ifreq *ret_if_mac, char *ifAddress);
	uint64_t ConvertAddressToU64(unsigned char *address);
	void ConvertU64ToAddress(uint64_t x, unsigned char *address);
	UFixed_2_8_t MilliWattToLog2PicoJoules(double mWperPacket, double avgPacketSizeBytes, double bitTimeInNanoSec);
	UFixed_2_8_t DBmToLog2PicoJoules(double dbm, double bitTimeInNanoSec);
//	void RegisterSigIO();
//	void MakeSocketNonBlocking(int32_t sockId);
//	void ReceiveMsgSimple(int sig);
//	void ReceiveMsg(int sig, siginfo_t *si, void *uc);
  uint32_t x();
  uint32_t y();
}

#endif //WF_COMMOM_H_
