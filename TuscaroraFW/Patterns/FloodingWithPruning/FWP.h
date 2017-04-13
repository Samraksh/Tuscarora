////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef FWP_H_
#define FWP_H_

#include <Interfaces/Pattern/PatternBase.h>
#include <Interfaces/AWI/Fwp_I.h>
#include "Lib/PAL/PAL_Lib.h"
#include "Lib/Buffers/FifoList.h"
#include "Framework/PWI/StandardFI.h"
#include "Lib/Pattern/NeighborTable/PatternNeighborTable.h"


using namespace PAL;

using namespace AWI;
namespace Patterns {

class FWP : public PatternBase , public Fwp_I{
	PWI::StandardFI *fi;
	uint16_t seqNumber;
	FMessage_t *sendMsg;
	uint32_t nonce;
	PatternNeighborTableI *myNeighborTable;
	Timer *msgTimer;

	//  TimerDelegate *timerDelegate;
	//FMessageDelegate_t *recvDelegate;
	//DataflowDelegate_t *dataNotifierDelegate;

	PatternNeighborTableI* nbrTable;
	FIFOList *sendBuffer;
	FMessage_t *outgoingPacket;
	BSTMapT<MessageId_t, FMessage_t*> sentMap;
	bool active;
	FrameworkAttributes fwAttributes;
	FWP2AppShim_I* eventdispatcher;
private:
	void Handle_AttributeResponse(ControlResponseParam response);
	void NeighborUpdateEvent (NeighborUpdateParam nbrUpdate);
	void ControlResponseEvent (ControlResponseParam response);
	void DataStatusEvent (DataStatusParam notification);
	void ReceiveMessageEvent (FMessage_t& msg);
	void InserBackToSendBuffer(MessageId_t id);
	void CleanUpBuffers(MessageId_t id);
	void DequeAndSend();
	bool InitiateProtocol();
public:
	FWP(FWP2AppShim_I* pattern2appshimptr);
	bool Start();
	bool Stop();
	virtual ~FWP();

	//void SendData (AppId_t _app_id, AppMessage_t& msg, uint16_t nonce);
	void Send (void *data, uint16_t size);
	void RegisterAppReceiveDelegate(AppId_t _app_id, AppRecvMessageDelegate_t* _gvu_del);
};

}

#endif /* FWP_H_ */
