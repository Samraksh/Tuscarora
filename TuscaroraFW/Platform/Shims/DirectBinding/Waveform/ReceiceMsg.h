////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef RECEIVE_MSG_H_
#define RECEIVE_MSG_H_

#include "Lib/Misc/Debug.h"
#include <stdio.h>


#define WF_HDR_SIZE 64
#define MAX_DCE_PKT_SIZE (1024+WF_HDR_SIZE) //This seems to be some peculier dce limit
#define MAX_AO_WF_PACKET_SIZE (MAX_DCE_PKT_SIZE - WF_HDR_SIZE)
#define BUF_SIZE ((MAX_DCE_PKT_SIZE+64)*2)

namespace Waveform {
  

  void PrintMsg(int msglen, int rcvNumber, uint8_t * msg);
  void ReceiveMsg(int sig);
  uint32_t x();
  uint32_t y();
}

#endif //RECEIVE_MSG_H_
