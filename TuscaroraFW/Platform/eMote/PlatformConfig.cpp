
#include <Framework/PWI/FrameworkBase.h>

namespace PWI {
UniformRandomInt rndInt(400,600);
WF_LinkChangeDelegate_n64_t lkDel();
WaveformMessageDelegate_t wfMsgDel(), diskDel();
PatternEventDispatch evntDispatch;
WF_AttributeMap_t() wfMap;
U64AddressSendDataDelegate_t u64SendDel();
PiggyBacker<uint64_t> pbService(u64SendDel);
PatternClient patternBookkeep();
PacketHandler pktHandler();
PacketAdaptor<uint64_t> pktAdapter64();
TimerDelegate timDel();
Timer inTim(40000, PERIODIC, &inqueryTimerDelegate);

void FrameworkBase :: InitializeFrameworkObjects(FrameworkBase *fb){

	// This random number generator is used get small delays with packet Event.
	fb->rnd = &rndInt;
	Debug_Printf(DBG_CORE,"FrameworkBase:: Contructor:: Created random integter with period 500 microsecs\n"); fflush(stdout);

	//create delegate that can be called back by link estimation on changes
	fb->leDel = &lkDel;
	lkDel.AddCallback(fb,	&FrameworkBase::LinkEstimatorChangeHandler);

	Debug_Printf(DBG_CORE, "FrameworkBase:: Contructor:: Created delegate for link updates %p\n", leDel);

	//delegate to pass to packet handler for link estimation
	fb->LES.linkEstimationDelegate = &wfMsgDel;
	wfMsgDel.AddCallback(fb,&FrameworkBase::ProcessLinkEstimationMsg);

	//delegate to pass to packet handler for discovery
	fb->LES.discoveryDelegate = &diskDel;
	diskDel.AddCallback(fb,&FrameworkBase::ProcessDiscoveryMsg);

	//create object to dispatch events to patterns
	fb->eventDispatcher = &evntDispatch;

	fb->wfAttributeMap = &wwfMap;

	//Generic Non-Pattern Message Handler
	fb->u64SendDelegate = &u64SendDel;
	u64SendDel.AddCallback(fb,&FrameworkBase::GenericSendMessage);

	fb->PB = &pbService;

	fb->patternClientsMap.Insert(0, &patternBookkeep);


	fb->pktHandler = &pktHandler;
	pktHandler.Initialize(&fb->wfMap, fb->wfAttributeMap, &fb->addressMap,
			&fb->patternClientsMap, fb, fb->nbrTable, fb->attribute_request_pending,
			fb->eventDispatcher, fb->PB);

	//Create an adpator for 64-bit waveform packet address
	//fb->pktAdaptor64 = &pktAdapter64;

	timDel.AddCallback(fb, &FrameworkBase::inqueryTimer_Handler);
	fb->inqueryTimerDelegate = &timDel;
	fb->inqueryTimer = &inTim;
	fb->inqueryTimer->Start();

	Debug_Printf(DBG_CORE,"FrameworkBase:: Instantiating Framework objects done for eMote Platform\n");
}

} //End namespace
