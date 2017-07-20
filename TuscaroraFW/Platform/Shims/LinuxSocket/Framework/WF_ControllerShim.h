////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef WF_CONTROLLER_SHIM_H_
#define WF_CONTROLLER_SHIM_H_

#include <Base/FrameworkTypes.h>
#include <Sys/SocketShim.h>
#include <Lib/KernalSignals/IO_Signals.h>
#include <Framework/Core/DataFlow/CoreTypes.h>
#include "Platform/Shims/LinuxSocket/SocketServer.h"

using namespace Types;
using namespace Lib::Shims;
using namespace Core::Dataflow;

#define MAX_FW_SOC_READ_SIZE 4096

class WF_ControllerShim {
	int32_t wfMasterSocket;
	SocketServer *master;
	//IO_Signals *ioSig;
	int16_t noOfWaveforms;
	char readBuf[MAX_FW_SOC_READ_SIZE];
	SocketSignalDelegate_t *del;
	BSTMapT<WaveformId_t, int32_t> wfToSockIdMap;
	WaveformMap_t *wfIdMap;
	BSTMapT<int32_t, Waveform_I<uint64_t>* > *wfSockMap;
public:
	WF_ControllerShim(WaveformMap_t *wfMap);
	void OnSignal(int32_t sockfd);
	uint32_t NewConnection();
	void OnRecv (int sockfd);
	void Cleanup (int sockfd);
	bool Deserialize (char * buf, uint32_t size, int32_t sockfd);
	void InitWaveform(WaveformId_t wid, int32_t sock);
private:
	void PrintMsg(char *buf, uint32_t size);
};

#endif //WF_CONTROLLER_SHIM_H_
