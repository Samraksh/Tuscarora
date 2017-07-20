////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef FRAMEWORK_I_H_
#define FRAMEWORK_I_H_

#include <Base/FrameworkTypes.h>
#include <Base/Delegate.h>
#include <Interfaces/Core/LinkEstimatorI.h>
#include <Interfaces/Core/DataFlowI.h>
#include <Interfaces/Core/PatternNamingI.h>
#include "PatternNeighborTableI.h"
#include <Interfaces/Pattern/PatternEvent.h>
#include "FrameworkResponces.h"

using namespace Types;
using namespace PAL;
using namespace Core;
using namespace Core::Estimation;
using namespace Core::Dataflow;
using namespace Core::Naming;
using namespace PWI;
using namespace PWI::Neighborhood;

namespace PWI {


  /**
   * @brief Describtes the attributes of the framework.
   * 
   * Any pattern should first query the FrameworkAttributes before it starts messaging.
   */
  struct FrameworkAttributes {
  uint8_t numberOfWaveforms; 			///Number of waveforms supported/accessible to the pattern on the node
  WaveformId_t waveformIds[MAX_WAVEFORMS];	///Waveform IDs of the waveforms.
  uint16_t maxFrameworkPacketSize;		///Maximum size of a packet that can be sent unfragmented on all of the existing waveforms
public:
  FrameworkAttributes(){
    numberOfWaveforms=0; 
    maxFrameworkPacketSize=0;
  }
};

/**
 * @brief Enum that describes the types of constrol responses sent by the framework to the pattern
 * 
 */
enum ControlResponseTypeE {
  PTN_RegisterResponse=2,
  PTN_AttributeResponse=3,
  PTN_SetLinkThresholdResponse=4,
  PTN_AddDestinationResponse=5,
  PTN_ReplacePayloadResponse=6,
  PTN_CancelDataResponse=7,
  PTN_SelectDataNotificationResponse=8
};


/**
 * @brief Structure used as parameter by the framework when sending the Control Response Event
 * 
 */
struct ControlResponseParam {
  uint32_t sequenceNo; //4b
  ControlResponseTypeE type;//1b  
  uint16_t dataSize;//2b
  void *data;
};

///Event sent to Pattern as a response to control requests
typedef PatternEvent<PTN_CONT_RES_EVT, ControlResponseParam> PTN_ControlResponseEvent_t; 

///Event sent to Pattern to notify it of updates in the neighborhood
typedef PatternEvent<PTN_NBR_CHG_EVT, NeighborUpdateParam> PTN_NeighborUpdateEvent_t; 

///Event sent to Pattern when a new message is received on one of the waveforms for the pattern
typedef PatternEvent<PTN_RECV_MSG_EVT, FMessage_t&> PTN_ReceivedMessageEvent_t;

///Event sent to the pattern to notify about the status of message the pattern sent to the framework
typedef PatternEvent<PTN_DATA_NTY_EVT, DataStatusParam> PTN_DatNotificationEvent_t;


typedef Delegate<void, FMessage_t&> RecvMessageDelegate_t;
typedef Delegate<void, ControlResponseParam> ControlResponseDelegate_t; 


/**
 * @brief Defines the generic interations between the Pattern and the Framework
 * 
 * Interface class that defines a asynchornous message passing
 * interface for interactions between the Pattern and the framework. A
 * pattern may either be built as part of same process or as seperate
 * process on the same node. When the pattern and the framework are
 * running as two seperate processes, a shim-layer should be
 * implementation that will translate the calls between the two
 * processes.
 *  
 * The interactions in general consist of a command/request orginating
 * from the Pattern to the framework and the framework responding to
 * the commands using an Event response. There are 12 Commands that
 * the pattern can issue to framework and 5 types of Events that the
 * framework sends to the pattern. A command is a message in a
 * specific format (based on a the command being invoked) sent from
 * the pattern to the framework. Event is a message in a specific
 * format sent from the framework to the pattern. While Commands and
 * Events are messages, they are invoked using function syntax, just
 * like invoking and providing functions. To send a command to the
 * framework, a pattern uses a FRAMEWORK reference provided by the
 * shim layer and to handle a event from the framework a pattern
 * implements 4 methods one for each type of event. The shim layer
 * takes care of invoking the correct method when the corresponding
 * event message arrives.
 * 
 * The 4 event types are:
 * 
 * 1. Control Response Event (PTN_ControlResponseEvent_t): This is
 * dispatched in response to one of the *Request message, with an
 * appropriate response type, response status (sucuss/failure), any
 * additional data and its size.
 *
 * 2. Received Message Event (PTN_RcvMessageEvent_t) : This event is
 * dispatched whenever a message is received for the pattern, with the
 * message as the parameter
 *
 * 3. Data Notification Event (PTN_DatNotificationEvent_t): This is
 * dispatched to convey the status of the SendData and BroadcastData
 * calls. The event provides the cummulative status of a message. Each
 * notification contains an array called `statusType' that specifies
 * the status type corresponding to each destination and another
 * boolen array called the `statusValue' that specifies if that
 * operation was a success or failure. For each destination, the
 * status progresses in the following sequence:
 *   PDN_FW_RECV (framework received the packet), 
 *   PDN_WF_RECV (waveform received the packet), 
 *   PDN_WF_SENT (waveform sent the messsage out) and finally
 *   PDN_DST_RECV (destination received the message). 
 * The sequence stops at PDN_WF_SENT for messages sent using
 * BroadcastData api, for which destination received notification is
 * not supported. Also if the status type has progressed to the next
 * in sequence, the previous type is implicitly successfull. For
 * example, for a given destination the status type is 'PDN_WF_SENT',
 * then it also means that PDN_FW_RECV and PDN_WF_RECV are true.
 *
 * 4. Neighbor Update Event (PTN_NeighborUpdateEvent_t): This event is
 * dispatched to provide updates about the status of the nodes
 * neighborhood. A neighbor update can either be about a new neighbor,
 * a dead neighor or change in the quality of an existing neighbor.
 *  
 */
class Framework_I {
  
public:
  
  /**
   * @brief Sends message provided as parameter to the set of
   * neighbors indicated by destArray
   * 
   * This is the primary method a pattern uses to request that a
   * message be transmitted to its neighbors.  The first two
   * parameters are self-explanatory.  The third parameter is an array
   * of one or more neighbors to which the message is to be sent, each
   * of which are identified by the neighbor’s waveform address. The
   * Framework may use the given MessageId to cancel or replace the
   * payload in future (via calls to the next two methods).  The
   * parameter noDestAck specifies whether a notification is requested
   * when this message has reached its intended destination(s).  A
   * true value for this field means the Framework does not need
   * notifications for this message.  If data
   * notifications/acknowledgements are not supported by the waveform,
   * this parameter may be ignored.
   * 
   * <b>Data Notifications:</b> Framework returns one or more data
   * notification events corresponding to each message sent. The Data
   * Notifications are cummulative in nature. Each notification
   * contains an array called `statusType' that specifies the status
   * type corresponding to each destination and another boolen array
   * called the `statusValue' that specifies if that operation was a
   * success or failure.
   * 
   * <b>Nonce:</b> Nonce is an unsigned integer that the pattern uses
   * to identify the message, before the framework can return a
   * message Id. Framework uses the nonce in the data notification and
   * also returns a Message ID generated by the framework for the
   * message. The nonce need not be unique for every message, but must
   * be different for consequent messages. If two messages have the
   * same once, then the framework assumes that the data notificaiton
   * it sent to the pattern for the previous message is lost and hence
   * the pattern is resending the same message with the same nonce.
   * 
   * 
   * @param pid Id of the pattern sending the message.
   * 
   * @param destArray List of destinations.
   * 
   * @param noOfDest Number of destinations specified in the
   * destination array.
   * 
   * @param lcType The link comparator type to be used to differentiate between multiple links to the same neighbor
   *
   * @param msg Reference of the message to be send out. Tuscarora
   * uses `handover' semantics for parameters passed as referece. That
   * is the Pattern SHOULD NOT deallocate the message that is sent
   * out. The shim layer will deallocate the message once it has been
   * sent out.
   *
   * @param nonce An unsigned integer specified by the pattern to
   * differentiate this message from previous message. It is used for
   * error recovery between the pattern and the framework.
   *
   * @param noDestAck Indicates if the pattern is not requesting
   * destination acknowledgement for this message. A true value means
   * no destination ack is requested. Default value is false.
   *
   * @return void
   */
  virtual void SendData (PatternId_t pid, NodeId_t destArray[MAX_DEST], uint16_t noOfDest, FMessage_t& msg, uint16_t nonce, bool noDestAck=false) = 0;
  virtual void SendData (PatternId_t pid, NodeId_t destArray[MAX_DEST], uint16_t noOfDest, LinkComparatorTypeE lcType, FMessage_t& msg, uint16_t nonce, bool noDestAck=false) = 0;
 
  
#if ENABLE_FW_BROADCAST==1  
  /**
   * @brief Sends the data provided as parameter to all the
   * destinations provided as an unaddressed broadcast message on a
   * given waveform.
   * 
   * <b>Data Notifications:</b> Framework returns one or more data
   * notification events corresponding to each message
   * sent. Notification for broadcast are returned in the index `0' of
   * the `statusType' and `statusValue' arrays. BroadcastData follows
   * the same sequence statys progressions, except it stops at
   * PDN_WF_SENT.
   * 
   * <b>Nonce:</b> Nonce is an unsigned integer that the pattern uses
   * to identify the message, before the framework can return a
   * message Id. Framework uses the nonce in the data notification and
   * also returns a Message ID generated by the framework for the
   * message. The nonce need not be unique for every message, but must
   * be different for consequent messages. If two messages have the
   * same once, then the framework assumes that the data notificaiton
   * it sent to the pattern for the previous message is lost and hence
   * the pattern is resending the same message with the same nonce.
   *     
   * @param pid Id of the pattern sending the message.
   *
   * @param msg Reference of the message to be send out. Tuscarora
   * uses `handover' semantics for parameters passed as referece. That
   * is the Pattern SHOULD NOT deallocate the message that is sent
   * out. The shim layer will deallocate the message once it has been
   * sent out.
   *
   * @param wid Waveform ID of the waveform on which the broadcast
   * should be sent.
   *
   * @return void.
   */
   virtual void BroadcastData(PatternId_t pid, FMessage_t& msg, WaveformId_t wid,  uint16_t nonce )=0;
#endif   
   
  /**
   * @brief Replaces the payload of a packet already sent to the
   * framework. The previous payload will be discarded and destroyed.
   * 
   * Replaces the payload of a packet already sent to the
   * framework. The previous payload will be discarded and
   * destroyed. In response to this command one or more Control
   * Response Event(s) may be generated by the framework. If the
   * message has already been sent to the waveform, but not sent out
   * by the waveform, the framework will issue commands to the
   * waveform to replace the payload, however the waveform may return
   * a failure. Hence, if the original message was sent to more than
   * one destination, it is possible that this operation can fail for
   * some destination and can succeed for others.
   *     
   * @param patternId Id of the pattern sending the message.
   * @param msgId ID  of the message in which the payload should be replaced 
   * @param payload Pointer to new payload.
   * @param sizeOfPayload Size of the new payload.
   * @return void.
   */
  virtual void ReplacePayloadRequest (PatternId_t patternId, FMessageId_t  msgId, void *payload, uint16_t sizeOfPayload) = 0 ;
  



  /**
   * @brief Generates a pattern ID, based on the pattern type and an
   * optional unique string.
   *
   * The method is used to get a unique pattern ID for each pattern
   * using the framework. The ID itself is generated by a Pattern Name
   * Service. The framework acts as an pass through for this
   * service. The ID is return through a Control Response Event. The
   * uniqueName parameter can be used for fail-safe reasons. For
   * example, if a pattern which was running crashes and reboot, it
   * could get the ID for the previous instance by using the same
   * uniqueName of the previous instance.
   *
   * @param ptype Pattern type.
   *
   * @param uniqueName An optional parameter using which a pattern can
   * identify itself to the framework. The Pattern Name Service will
   * rememeber the string the pattern provides and will generate the
   * same ID, if both the Pattern type and the uniqueName string
   * match.
   *
   * @return void.

  virtual void NewPatternInstanceRequest(PatternTypeE ptype, char uniqueName[128] = NULL)= 0;
  */
  
   /**
    * @brief Registers a pattern with the framework.
    * 
    * Patterns can use this method to register with the framework and optionally request for a pattern ID.
    * A pattern will need to register first to be able to access the
    * services of the framework. A Control Response Event will be
    * generate to notify the pattern of the result of the
    * registration.
    * If the pattern does not have an ID, it sets the input variable "patternId" to zero and uses
    * Framework generates a new ID for the pattern in this case. The Control Response Event will include the new pattern ID in this case.
    * 
    * If the pattern already has an ID either generated through an external service or obtained previously, it can use the existing patternID.
    * In this case, the input uniqueName is ignored by the Framework.
    *
    * @param patternId Unique ID of the Pattern
    * @param uniqueName Unique string indicating the pattern instance used in pattern id generation
    * @param type Pattern type
    * @return void.
    */
  virtual void RegisterPatternRequest (PatternId_t patternId, const char 
uniqueName[128], PatternTypeE type) = 0;
   //virtual void RegisterPatternRequest (PatternId_t patternId, PatternTypeE type) = 0;
   
  /**
   * @brief Adds more destinations to a message already handed over to
   * the framework. If the original message was sent using
   * BroadcastData method, this new request is ignored.
   * 
   * Adds destinations to a message already sent to the framework. If
   * the new destinations specifed using is destArray parameter is
   * split across two or more waveforms, it is possible that the
   * request can succeed for some of the new destinations and can fail
   * for others. A Control Response Event will be generated by the
   * framework with the result of the operation.
   * 
   * @param patternId Pattern's instance ID.
   * @param msgId Unique message ID.
   * @param destArray Array of destinations to be added.
   * @param noOfNbrs Number of new destinations.
   * @return void.
   */
  virtual void AddDestinationRequest (PatternId_t patternId, FMessageId_t  msgId, NodeId_t destArray[MAX_DEST], uint16_t noOfNbrs) = 0;
  virtual void AddDestinationRequest (PatternId_t patternId, FMessageId_t  msgId, NodeId_t destArray[MAX_DEST], uint16_t noOfNbrs, LinkComparatorTypeE lcType ) = 0;
  
    
  /**
   * @brief Cancels the sending of a message to one or more
   * destinations of a message previously handed over to the framework
   * 
   * Cancels the sending of a message previously handed over to the
   * framework. If no destination is specified or if the original
   * message was sent using BroadcastData method, entire message is
   * cancelled and destroyed. Otherwise sending is cancelled only to
   * the desitnations specified in the destArray parameter. A Control
   * Response Event is generated with the result of the operation.
   * 
   * @param patternId Pattern's instance ID.
   * @param msgId Unique message ID.
   * @param destArray Array of destinations that need to be cancelled
   * @param noOfDest Number of destinations to be cancelled.
   * @return void.
   */
  virtual void CancelDataRequest (PatternId_t patternId, FMessageId_t  msgId, NodeId_t destArray[MAX_DEST], uint16_t noOfDest) = 0;

  /**
   * @brief Request the status of a data message sent to the framework.
   * 
   * If for any reason a pattern has ambiguity about the status of a message that was sent to the framework, the pattern can use this method to request the status of the message. The 
   * framework will generate a Data Notification Event in response to this command, with the cummulative status of the message till that point.
   * 
   * @param patternId Pattern's instance ID.
   * @param msgId ID of the message for which status is being requested.
   * @return void
   */ 
  virtual void DataStatusRequest (PatternId_t patternId, FMessageId_t  msgId) = 0; 
 
  /**
   * @brief Lets the Pattern Select which type of Data Notification
   * Updates the Pattern wants to recieve.
   * 
   * This methods lets the pattern to customize data notifications
   * that it wishes to receive. The customization is achived by
   * specifying the mask of the data status types that pattern is
   * interested in receiving. While this could decrease the number and
   * frequency of notifications the pattern receives, the framework
   * cannot completely eliminate the status types that the pattern is
   * not interested in since the data notification is an cummulative.
   * 
   * @param patternId Pattern's instance ID.
   * @param notifierMask Mask of the notifications that are requested.
   * @return void
   */
   virtual void SelectDataNotificationRequest (PatternId_t patternId, uint8_t notifierMask) = 0;
  
  
  /**
    * @brief Sets the link metric threshold to qualify as pattern
    * neighbors
    * 
    * This method lets a Pattern customize its neighbor and the
    * neighborhood updates. A pattern can specify the minimum metric
    * that a link should meet to qualify as its neighbor.  This
    * simplifies neighbor maintanence and look up.Also, neighborhood
    * updates will be sent to patterns only about neighbors who meet
    * this threshold, hence the number of Neighbor Update Events sent
    * to the pattern will decrease. If a pattern does not set this
    * threshold, all neighbors know to the framework will be published
    * to the pattern. A Control Response Event is generated in
    * response to this command to indicate the status of the
    * operation.
    * 
    * @param patternId Pattern's instance ID.
    * @param threshold Link metric threshold to be set.
    * @return void. 
    */
  virtual void SetLinkThresholdRequest (PatternId_t patternId, LinkMetrics threshold) = 0;
    
  /**
   * @brief Requests the attributes of the framework.
   * 
   * The method is used to know the current attributes of the
   * framework. The addritbutes are defined in the FrameworkAttributes
   * struct. A pattern should first querry this method to find out the
   * attributes of the framework before starting messaging. Critical
   * information such as Max packet size, number of waveforms in the
   * system, their IDs, etc. are returned through the
   * FrameworkAttributes struct. A Control Response Event is generated
   * in response to this command, with the FrameworkAttributes copied
   * into the data section of the event. It is possible for the
   * framework to generate a Control Response Event with the type set
   * to `PTN_AttributeResponse', even without a pattern requesting it,
   * if the framework attributes changes while a pattern is active.
   * 
   * @param patternId Pattern's instance ID. 
   * @return void.
   */
  virtual void FrameworkAttributesRequest(PatternId_t patternId) = 0;
  
  /**
   * @brief Selects the link comparator to use while comparing link
   * qualities. Methods lets patterns to specify a link comparator
   * from a list of existing library of comparators.
   * 
   * This method lets a pattern to customize the comparison of the
   * link metrics between two links. While there are 5 standard link
   * metrics, how to select one link as better than another can be
   * customized using this method, by selecting one of the many
   * pre-approved comparators. Please see documentation for
   * `LinkComparatorTypeE' for available comparators.
   * 
   * @param patternId Pattern's instance ID.
   * @param lcType Type of the link comparator. Enum. 
   * @return void.
   */
  virtual void SelectLinkComparatorRequest (PatternId_t patternId, LinkComparatorTypeE  lcType) = 0;
  
  ///Virtual destructor
  virtual ~Framework_I (){}
};

///Returns a reference to an implentation of the pattern interface.
Framework_I& GetFrameworkInterface();
Framework_I& GetFrameworkShim();
void SetFrameworkInterface(Framework_I &fi);

void GetWaveformIDs(WaveformId_t *idArray, uint16_t noOfWaveforms);
uint16_t GetNumberOfWaveforms();

} //End of namespace

#endif /* FRAMEWORK_I_H_ */
