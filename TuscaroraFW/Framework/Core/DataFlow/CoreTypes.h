////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef CORE_TYPES_H_
#define CORE_TYPES_H_


#include <Base/BasicTypes.h>
#include <Base/FrameworkTypes.h>
#include <Interfaces/PWI/Framework_I.h>
#include <Interfaces/Waveform/WF_I.h>
#include "Framework/PWI/PatternClient.h"
#include "Framework/Core/Estimation/EstimationLogging.h"
//#include "Framework/Core/Estimation/EstimationManager.h"

using namespace PWI;
using namespace Waveform;
using namespace Core;

namespace Core {
  
namespace Dataflow {

typedef Delegate<void, WF_MessageBase&> WaveformMessageDelegate_t;
typedef BSTMapT<WaveformId_t, Waveform_I<uint64_t>* > WaveformMap_t;
typedef BSTMapT<WaveformId_t, WF_Attributes> WF_AttributeMap_t;
typedef BSTMapT<WaveformId_t, PacketAdaptor<uint64_t>*> WF_MsgAdaptorMap_t;
typedef BSTMapT<PatternId_t, PatternClient*> PatternClientMap_t;
typedef BSTMapT<WaveformId_t, LinkEstimatorI<uint64_t> *> EstimatorHash_t;
typedef BSTMapT<WF_MessageId_t, MessageId_t> WFMsgIdToFmsgIdMap_t;

struct LinkEstimationStore{
	EstimatorTypeE linkEstimationType;
	WaveformMessageDelegate_t *linkEstimationDelegate;
	WaveformMessageDelegate_t *discoveryDelegate;
	EstimationLogging logger;
	//EstimationManager linkManager;
	EstimatorHash_t estimatorHash;
	int discoveryType;
	bool longLinkActive;
};	
	

struct GenericSendDataDelegateParam {
	WaveformId_t wid;
	uint16_t dataSize;
	FMessage_t &msg;
};


typedef  Delegate<bool, WaveformId_t, uint64_t, MessageTypeE, FMessage_t& , bool > U64AddressSendDataDelegate_t;

} //End namespace
}

#endif // CORE_TYPES_H_
