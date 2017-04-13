////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef WF_ALWAYSON_ACK_H_
#define WF_ALWAYSON_ACK_H_

#include "WF_AlwaysOn_NoAck.h"

namespace Waveform { 

	class WF_AlwaysOn_Ack : public  WF_AlwaysOn_NoAck{
		//double lastSnr;
		//double lastRss;
		
		//void ProcessNS3Acks(WaveformId_t wfid, uint16_t msglen, unsigned char* rcvbuf);
	public:
		using WF_AlwaysOn_NoAck::WF_AlwaysOn_NoAck;
		void AckTimer_Handler(uint32_t flag);
  };
}
#endif /* WF_ALWAYSON_ACK_H_ */
