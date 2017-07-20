/*
 * BasicPattern.h
 *
 *  Created on: Jun 16, 2017
 *      Author: bora
 */

#ifndef PATTERNS_BASICPATTERN_BASICPATTERN_H_
#define PATTERNS_BASICPATTERN_BASICPATTERN_H_

#include <Interfaces/Pattern/PatternBase.h>

#define ReSendTimeOutInterval 2000

namespace Patterns {

class BasicPattern : public PatternBase{
protected:
	FrameworkAttributes fwAttributes;
	uint32_t nonce;
	PatternNeighborTableI *myNbrHood;
	bool hasFWRejectedPacket;

	Timer *randEvent_ReSendUpdate;	//A pointer for the event class used to reSendPreviousPacket when no response is received after a timeout period
	TimerDelegate *eventDel_ReSendUpdate;//reSendPreviousPacket when no response is received after a timeout period

	virtual void SetNeighborhoodandLinkComparator();
	virtual void RequestDataNotifications();
	virtual bool InitiateProtocol();
	virtual void ReSendHandler(uint32_t event) {};


	virtual void Handle_AttributeResponse(ControlResponseParam response);
	virtual void Handle_LinkThresholdResponse(ControlResponseParam response);
	virtual void Handle_SelectDataNofiticationResponse(ControlResponseParam response);
	virtual void Handle_AddDestinationResponse(ControlResponseParam response){};
	virtual void Handle_ReplacePayloadResponse(ControlResponseParam response){};
	virtual void Handle_CancelDataResponse(ControlResponseParam response){};




	virtual void NeighborUpdateEvent(NeighborUpdateParam nbrUpdate);
	virtual void ReceiveMessageEvent(FMessage_t& msg){};
	virtual void DataStatusEvent(DataStatusParam ntfy);
	virtual void ControlResponseEvent(ControlResponseParam response);


public:
	BasicPattern() ;
	virtual bool Start();

	virtual bool Stop();

};

} /* namespace Patterns */

#endif /* PATTERNS_BASICPATTERN_BASICPATTERN_H_ */
