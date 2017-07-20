
#include <Framework/PWI/FrameworkBase.h>
#include "../Shims/DirectBinding/Framework/PatternEventDispatch.h"

void FrameworkBase :: InitializeFrameworkObjects(FrameworkBase *fb){
	// This random number generator is used get small delays with packet Event.
	fb->rnd = new UniformRandomInt(400,600);
	Debug_Printf(DBG_CORE," FrameworkBase:: InitializeFWObjects:: Created random integter with period 500 microsecs\n"); fflush(stdout);

	//create delegate that can be called back by link estimation on changes
	fb->leDel = new WF_LinkChangeDelegate_n64_t(fb,
			&FrameworkBase::LinkEstimatorChangeHandler);
	//leSendDel = new SendEstimationMessageDelegate_t (fb, &FrameworkBase::SendToWF2);
	Debug_Printf(DBG_CORE,
			"FrameworkBase:: InitializeFWObjects:: Created delegate for link updates %p\n",
			fb->leDel);

	//delegate to pass to packet handler for link estimation
	fb->LES.linkEstimationDelegate = new WaveformMessageDelegate_t(fb,
			&FrameworkBase::ProcessLinkEstimationMsg);

	//delegate to pass to packet handler for discovery
	fb->LES.discoveryDelegate = new WaveformMessageDelegate_t(fb,
			&FrameworkBase::ProcessDiscoveryMsg);

	//create object to dispatch events to patterns
	fb->eventDispatcher = new PatternEventDispatch();

	fb->wfAttributeMap = new WF_AttributeMap_t();

	//Generic Non-Pattern Message Handler
	fb->u64SendDelegate = new U64AddressSendDataDelegate_t(fb,
			&FrameworkBase::GenericSendMessage);

	fb->PB = new PiggyBacker<uint64_t>(*(fb->u64SendDelegate));

	//pktHandler = new PacketHandler(&wfMap, &wfMsgAdaptorMap, &patternClientsMap, linkEstimationDelegate, discoveryDelegate, nbrTable, eventDispatcher);
	//pktHandler = new PacketHandler(&wfMap,wfAttributeMap, &wfMsgAdaptorHash, &patternClientsMap, linkEstimationDelegate, discoveryDelegate, nbrTable);

	//Add object for keep track of linkestimation accounting
	PatternClient *obj = new PatternClient();
	fb->patternClientsMap.Insert(0, obj);

	Debug_Printf(DBG_CORE,"FrameworkBase:: InitializeFWObjects::Instantiating PacketHandler object\n");

	fb->pktHandler = new PacketHandler();
	fb->pktHandler->Initialize(&fb->wfMap, fb->wfAttributeMap, &fb->addressMap,
			&fb->patternClientsMap, fb, fb->nbrTable, fb->attribute_request_pending,
			fb->eventDispatcher, fb->PB);

	//Create an adpator for 64-bit waveform packet address
	//fb->pktAdaptor64 = new PacketAdaptor<uint64_t>();

	fb->inqueryTimerDelegate = new TimerDelegate(fb,
			&FrameworkBase::inqueryTimer_Handler);
	fb->inqueryTimer = new Timer(40000, PERIODIC, *fb->inqueryTimerDelegate);
	//inqueryTimer->Start();
}
