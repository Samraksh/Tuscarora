////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef PTN_CONTROLLER_SHIM_H_
#define PTN_CONTROLLER_SHIM_H_

#include <Base/FrameworkTypes.h>
#include <Sys/SocketShim.h>
#include <Lib/KernalSignals/IO_Signals.h>
#include <Framework/Core/DataFlow/CoreTypes.h>
#include <Platform/Shims/LinuxSocket/Pattern/PatternShimMessages.h>
#include <Framework/PWI/FrameworkBase.h>

#include "../SocketCommunicatorServerBase.h"
#include "../SocketServer.h"


using namespace Types;
using namespace Lib::Shims;
using namespace Core::Dataflow;

#define MAX_FW_SOC_READ_SIZE 4096
#define IS_SINGLE_PATTERN_ENFORCED false

class PTN_ControllerShim : public SocketCommunicatorServerBase<PatternId_t> {
    Framework_I *fi;

//    SocketId_t ptnMasterSocket; //Inherited from SocketCommunicatorServerBase
//	SocketServer *master;//Inherited from SocketCommunicatorServerBase
	//IO_Signals *ioSig;
	int16_t noOfWaveforms;
//	char readBuf[MAX_FW_SOC_READ_SIZE];//Inherited from SocketCommunicatorServerBase
//	SocketSignalDelegate_t *del;//Inherited from SocketCommunicatorServerBase
//	BSTMapT<PatternId_t, SocketId_t> ptnToSockIdMap; //Inherited from SocketCommunicatorServerBase
	//WaveformMap_t *ptnIdMap;
	//BSTMapT<int32_t, Waveform_I<uint64_t>* > *ptnSockMap;
	bool Deserialize (int32_t sockfd);
public:
	PTN_ControllerShim();
	//~PTN_ControllerShim();
	void SetFrameworkPtr(Framework_I *_fi);
//	PTN_ControllerShim();
//	void OnSignal(int32_t sockfd);
//	SocketId_t NewConnection();
	//void OnRecv (int sockfd);
//	bool Cleanup (int sockfd);
	//bool Deserialize (char * buf, uint32_t size, int32_t sockfd);


//	void InitPattern(PatternId_t wid, int32_t sock);
//	bool SendMsg2Pattern(PatternId_t pid, uint8_t* buf, int32_t size);
private:
//	void PrintMsg(char *buf, uint32_t size);
};


#endif //PTN_CONTROLLER_SHIM_H_
