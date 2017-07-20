////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef WAVEFORM_FASADE_H_
#define WAVEFORM_FASADE_H_

#include <Interfaces/Waveform/WF_I.h>
#include "Lib/KernalSignals/IO_Signals.h"

using namespace Waveform;
//using namespace Lib::Shims;


namespace Waveform {
	
class WaveformFasade {
	Waveform_I<uint64_t> *wi;
	WaveformId_t WID;
	SocketSignalDelegate_t *del;
	int32_t calllSocketID;
	char readBuf[2048];
	//int32_t eventSocketID;
	//EventSocket eventSocket;
	
public:
	WaveformFasade(WaveformId_t wid, Waveform_I<uint64_t> *_wi);
	void OnRecv (int sockfd);
	void OnSignal(int32_t sockfd);
	void Deserialize (char * buf, uint32_t size, int32_t sockfd);
	void PrintMsg(char* buf, uint32_t size);
};

}

#endif //WAVEFORM_FASADE_H_