////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef WF_I_H_
#define WF_I_H_

#include <Base/FrameworkTypes.h>
#include <Interfaces/Core/DataFlowI.h>
#include "WF_Types.h"
#include "WF_Event.h"
//#include <Interfaces/Core/LinkEstimatorI.h>
#include <string.h>

using namespace PAL;
using namespace Core;
using namespace Core::Dataflow;


///Waveform Interface namespace
namespace Waveform {

//Typedef delegates 
typedef WF_Event<WF_CONT_RES_EVT, WF_ControlResponseParam> WF_ControlResponseEvent_t;
typedef WF_Event<WF_SCHD_UPD_EVT, WF_ScheduleUpdateParam> WF_ScheduleUpdateEvent_t;
  

class WaveformBase {
   
public:
  WaveformBase() {}
  
  /**
   * @brief Requests the replacement the payload of a message, passed in an earlier call to ‘SendData’ or ‘BroadcastData’ method with the given MessageId.
   * The previous payload will be discarded.
   * 
   * <b>Semantic Behavior:</b> Waveform should replace the payload of a message passed in an earlier call to ‘SendData’ or ‘BroadcastData’ method with the 
   * given message ID, if the message has not been sent out.  A response message should be generated through the Control Response Event, with the same request
   * ID, that should  include a status which will be set to false if the message has been sent out or if the waveform is unable to replace the payload of a 
   * message. Otherwise, the status should be set to true in the response message.
   * 
   * @param rId Specifies the ID of the request, which the waveform will use to in its response message.
   * @param msgId specifies the message ID of the packet.
   * @param payload pointer to the new payload.
   * @param payloadSize size of the new payload.
   * @return void.
   */
  virtual void ReplacePayloadRequest (RequestId_t rId, WF_MessageId_t msgId, uint8_t *payload, uint16_t payloadSize) = 0;  
  
  
  /**
   * @brief Request the attributes of the waveform.
   * 
   * <b>Semantic Behavior:</b> Waveform should return its attributes to the Framework by generating a response through the Control Response Event, with the same request ID. 
   * The status of the response message should be true, unless otherwise the attributes are not available to be sent to the Framework. The response contains 
   * attributes in the WF_Attributes structure, which is copied into the data portion of the WF_ControlResponseParam.
   * 
   * @param rId Specifies the ID of the request, which the waveform will use to in its response message.
   * @return void.
   */
  virtual void AttributesRequest (RequestId_t rId) = 0;
  
  /**
   * @brief Requests the control status of waveform. A WF_StatusE enum type is sent as the data of the response message, to indicate the status of the variable.
   *
   * <b>Semantic Behavior:</b> The Framework expects the waveform to be in one of the data plane states defined by the WF_ControlP_StatusE enum. The waveform 
   * should return the current status of its data plane by generating a response through the Control Response Event, with the same RequestId. The status that
   * response message should be true. The actual data plane status should be copied into the data part of the response 
   * 
   * @param rId Specifies the ID of the request, which the waveform will use to in its response message.
   * @return void.
   */
  virtual void ControlStatusRequest (RequestId_t rId) = 0;
  
  /**
   * @brief Requests the status of a data message already sent to the waveform
   * 
   * <b>Semantic Behavior:</b> If the Framework is uncertain about the status of a message that it sent to the waveform, it can send the DataStatusRequest 
   * command. A response message should be generated through the Control Response Event, with the same RequestId, that should have the status set as ‘true’ if
   * the waveform received the particular message and a WF_DataNotifierParam object must be created that reflects the current status of the message and should
   * be copied into the data part of the WF_ControlResponseParam.
   * 
   * @param rId Specifies the ID of the request, which the waveform will use to in its response message.
   * @param mId Specified the ID of the message, for which status is requested.
   * @return void.
   */
  virtual void DataStatusRequest (RequestId_t rId, WF_MessageId_t mId) = 0;
  
  
  /**
   * @brief Sends information about a schedule for one of 3 kinds of functions: link estmation, sending or receiving to the waveform
   * 
   * <b>Semantic Behavior:</b> This method enables the Framework to pass on some scheduling information to the waveform. The expectation is that this 
   * method will be implemented only if the waveform needs scheduling information from the Framework or an external third party provider. The method enables 
   * receiving scheduling information specific to a neighbor specified by the _nodeId parameter. The schedule information is sent as abstract schedule object
   * represented by the _schedule object and the size of this object is specified by the _scheduleSize parameter. The expectation is that, if a waveform needs
   * scheduling information, it will derive from the ScheduleBaseI class and will implement its own scheduling class. The Framework defines three types of 
   * waveform schedules as the WF_ScheduleTypeE enum (one each for link estimation, sending, and receiving messages).
   * 
   * The waveform should regenerate a response to this request through the Control Response Event, with the same RequestId. The status of that response 
   * message should be true if the waveform able to successfully use the scheduler information or false otherwise. 
   * 
   * Since this is an optional feature for waveforms, if it choose to implement it, the waveform should set the schedulerInfoSupport field in its Attribute
   * as true whenever it responds to the AttributeRequest. 
   * 
   * @param rId Specifies the ID of the request, which the waveform will use to in its response message.
   * @param nodeid Specifies the node ID for which the schedule should be used.
   * @param type Enum that specifies the type of the schedule. There are 3 types of schedules, sending, receiving and link estimation.
   * @param schedule Reference to an abstract schedule object. It is expected that this class will be derived and implemented by different waveforms.
   * @param scheduleSize Size of the schedule object.
   * @return void.
   */
  virtual void SetScheduleRequest (RequestId_t rId, NodeId_t nodeId, WF_ScheduleTypeE type, ScheduleBaseI &schedule, uint16_t scheduleSize) {}
   
  /*
   * @brief Request a particular type of schedule from the waveform
   * 
   * <b>Semantic Behavior:</b> This method enables the waveform to share its schedule information with the framework. This is an optional method. If the 
   * waveform has no schedule or if the waveform does not want to share this information with the Framework, this method can be ignored. The parameters 
   * specify the RequestId, the neighbor for which the schedule information is requested and the type of the schedule.
   * 
   * The waveform should regenerate a response to this request through the Control Response Event, with the same RequestId. The status of that response 
   * message should be true. A WF_ScheduleUpdateParam structure with the schedule information should be created and copied into the data part of the response
   * and the size of the data should be set as the size of WF_ScheduleUpdateParam object being sent to the Framework. 
   * 
   * Since this is an optional feature for waveforms, if it choose to implement it, the waveform should set the schedulerInfoSupport field in its Attribute 
   * as true whenever it responds to the AttributeRequest. 

   * 
   * @param nodeid Specifies the neighbor for which the schedule is requested
   * @param type Enum that specifies the type of schedule. There are 3 types of schedules, sending, receiving and link estimation.
   * @return void.
   *
   */
  virtual void ScheduleRequest (RequestId_t rId, NodeId_t nodeid, WF_ScheduleTypeE type) {}
};


  //\image latex WF_CommandResponse.eps "Summary of Commands and their corresponding responses" width=10cm
/**
 * @brief Defines the generic iteractions for waveforms. 
 * 

 * The interface defines a asynchornous message passing interface for interacting with a waveform running
 * either as part of same process or as seperate process on the same node. When running as two seperate processes, a shim-layer will be implementation that will translate the calls between the two
 * processes.
 *  
 * The control flow in general consist of a command/request orginating from the framework to the waveform and the waveform
 * responding to the command/request using a event response. There four types of events that the waveform can send to the framework :
 * 1. Response Event (WF_ControlResponseEvent_t): This is called in response to one of the *Request message, with an appropriate response type, response status (sucuss/failure), any additional data and its size
 * 2. Received Message Event (WF_RcvMessageEvent_t) : This delegate is called whenever a message is received on the waveform, with the message as the parameter
 * 3. Data notification delegate (WF_DataNotifierEvent_t): This is called to convey the status of the SendData and BroadcastData calls. The delegates are might be called upto three types for each message, once to acknowledge
 * the message has been received by the waveform, once when it is sent out and once when the message is received at the destination.
 * 4. Link estimation delegate (WF_LinkEstimatesEvent_t): This is called by waveform to provide link estiamtions about neighbors to the framework, if the waveform can provide link estimates. 
 */

template <class WF_AddressType>
class Waveform_I : public WaveformBase {
protected:
  WaveformId_t WID;
  WF_TypeE wfType;
  char deviceName[32];
  WF_EstimatorTypeE estType;
  
public:
  
  typedef WF_Event<WF_RECV_MSG_EVT,WF_RecvMsgParam<WF_AddressType> > WF_RcvMessageEvent_t;
  typedef WF_Event<WF_DATA_NTY_EVT,WF_DataStatusParam<WF_AddressType> > WF_DataStatusEvent_t;
  typedef WF_Event<WF_LINK_EST_EVT,WF_LinkEstimationParam<WF_AddressType> > WF_LinkEstimateEvent_t;
  
   Waveform_I (){}
  
  /**
   * @brief Constructor. Creates a waveform object.
   * 
   * @detail <b> Semantic Behavior:</b> 
   * The constructor is expected to create and initialize the waveform object.  The constructor is also expected to “connect” to the Framework process. 
   * The mechanism of connecting may be platform specific.  The data plane of the waveform is expected to be in WF_NORMAL state at the end of this call.
   * 
   * The WaveformId is allocated statically during compile time in the current version of Tuscarora. Other parameters of the constructor are decided by 
   * the waveform provider.

   * 
   * @param _wId Unique ID of the waveform. The WaveformId is allocated when the Tuscarora is initialized. A waveform writter usually doesnot have to worry
   * about how they are generated. A waveform is expected to store this ID and use it to uniquely identify itself when communicating with the framework.
   * @param _type Type of the waveform. This is one of the types of the waveform the framework supports.
   * @param _estType Type of the estimator provided by the waveform. Indicated weather or not the waveform supports link estimation.
   * @param _deviceName Name of the device which will be used by the waveform, if device names exist on the platform used to deploy the waveform.
   * 
   */
  Waveform_I (WaveformId_t _wId, WF_TypeE _type, WF_EstimatorTypeE _estType, char* _deviceName) ;
  
  /**
   * @brief Send message indicated by the MessageId to a set of neighbors indicated by destArray, with a given payload size.
   *  
   * @detail <b> Semantic Behavior:</b> This is the primary method the Framework uses to request that a message be transmitted by the waveform. 
   * The first two parameters are self-explanatory.  The third parameter is an array of one or more neighbors to which the message is to be sent, 
   * each of which are identified by the neighbor’s waveform address. The Framework may use the given MessageId to cancel or replace the payload 
   * in future .  The parameter _noAck specifies whether a notification is requested when this message has reached its 
   * intended destination(s).  A true value for this field means the Framework does not need notifications for this message.  If data 
   * notifications/acknowledgements are not supported by the waveform, this parameter may be ignored. 
   * 
   * <b>Data Notifications:</b> It is recommended that Waveforms support acknowledgements from destinations for unicast and multi-cast (in local neighborhood)
   * messages. However this is optional.  If the waveform choose to implement receiver acks, it should set the ‘destReceiveAckSupport’ field in the Attribute 
   * record as true whenever it responds to the AttributeRequest. If the waveform does not support this feature, then the framework will implement destination
   * acknowledgements by replying to the source node on receiving messages from the waveform. The MessageId may be used by the Framework to cancel or replace 
   * the payload in future.  The waveform is expected to generate at least 2 notifications through the Data Notification Event, for every message sent to it, 
   * the first indicating that the waveform has received the message and the second indicating that it has sent out the message.  The waveform can generate a 
   * third notification indicating that the destination(s) has received the message if it chooses to support destination acknowledgement.
   * 
   * <b>Performance:</b> A request to send to multiple nodes should be fulfilled via a technique that is no less efficient than a sequence of transmissions 
   * to each node.  The intent is that more advanced waveforms may internally optimize beyond this base performance level, by combining transmissions when 
   * possible and not unduly difficult.  Any optimizations that:  1) are not naturally supported by the hardware and the waveform, 2) require non-local 
   * information, or 3) consume an order of magnitude more computational resources than required for the core send operation, should not be supported
   * 
   * @param msg Reference to the packet be sent. 
   * @param payloadSize Size of the payload in the packet.
   * @param destArray Array of destinations to which the packet should be sent.
   * @param noOfDest Number of destinations in the destArray.
   * @param msgId Specifies the packet id the framework will use to identify this packet in future calls.
   * @param noAck Specifies if acks should not be generated. Defaults option generates acks
   * @return void.
   */
  virtual void SendData (WF_MessageT<WF_AddressType>& _msg, uint16_t _payloadSize, WF_AddressType *_destArray, uint16_t _noOfDest, WF_MessageId_t _msgId, bool _noAck=false) = 0;
  
  
  /**
   * @brief Sends a broadcast on a waveform. Destination received data notification will not be generated
   * If the waveform does not support physical layer broadcasting, the implementation is upto the waveform or the waveform might simply not implement this interface. 
   * 
   * <b> Semantic Behavior:</b> Broadcast is an optional feature for waveforms.  They can choose not to implement it.  If implemented, the waveform should set 
   * the ‘broadcastSupport’ field in its Attribute as true whenever it responds to the AttributeRequest. When the Framework makes a broadcast request, it is 
   * expected that the waveform will transmit the message without any specific addressing and without any expectation of destination acknowledgements.  The 
   * waveform should perform sufficient effort such that neighboring nodes as well as nodes that could be a neighbor might sometimes receive the packet under 
   * representative operating conditions. 
   * 
   * Efficiency is the key intent of the broadcast method.  For example, if a waveform has neighbors across 7 different spectral-temporal channels and to 
   * reach all of the known neighbors it has to send the same message 7 times, once on each channel, then the nominal implementation would randomly select a 
   * channel and send one message on that channel.  The randomization ensures that any given neighbor will be reached at least sometimes.
   * 
   * The MessageId may be used by the pattern to cancel or replace the payload in future.  The waveform is expected to generate 2 notifications through the 
   * Data Notification Event, for every message sent to it, the first indicating that the waveform has received the message and the second indicating that it 
   * has sent out the message.  The waveform can generate a third notification indicating that the destination(s) has received the message if it can support 
   * destination acknowledgements.
   * 
   * @param msg Reference to the packet to be broadcasted
   * @param payloadSize Size of the payload in the packet.
   * @param msgId Specifies the id of the framework will use to identify this packet in future calls.
   * @return void.
   */
  virtual void BroadcastData (WF_MessageT<WF_AddressType>& _msg, uint16_t _payloadSize, WF_MessageId_t _msgId) {}
  
  
  /**
   * @brief Requests the Cancellation of a data message already sent to the waveform for one or more destinations.
   * 
   * <b> Semantic Behavior:</b> Waveform should cancel the transmission of the message, passed in an earlier call to SendData or BroadcastData method with the 
   * given message ID, if the message has not been sent out. The message transmission should be cancelled only for the destinations passed as parameters. If 
   * the noOfDestinations parameter is 0, then the entire message should be cancelled.  If the message ID is for a Broadcast message the whole message should 
   * be cancelled.
   * 
   * A response message should be generated through the Control Response Event, with the same RequestID, that should include a status which will be set to 
   * false if the message has been sent out or if the waveform is otherwise unable to cancel the transmissions.  Otherwise the status should be set to true in 
   * the response message.
   * 
   * @param rId Specifies the ID of the request, which the waveform will use to in its response message.
   * @param msgId ID of the message that needs to be cancelled. 
   * @param destArray Array of destinations to which the packet should be cancelled.
   * @param noOfDestinations Number of destinations in the destArray. If the number of destinations is 0, the entire message is cancelled.
   * @return void
   */
  virtual void CancelDataRequest (RequestId_t _rId, WF_MessageId_t _msgId, WF_AddressType *_destArray, uint16_t _noOfDestinations) = 0;
  
  
  /**
   * @brief Adds one or more destinations to a packet, that has already been sent to the waveform. Waveform will send a Control Response Event as reply to this 
   * command.
   * 
   * <b> Semantic Behavior:</b> Waveform should add a destination to the outgoing message if it has not already sent out the message identified by the message
   * ID and send a positive response. If the waveform has already sent out the message to all the destinations and has cleaned its local buffers of the message,
   * it should send a negative response to this request. Once destination has been added to a message, the waveform should communicate the result of the 
   * operation using Data Status Response Events as usual.
   * 
   * A response to this request should be generated through the Control Response Event, with the same RequestID, that should include a status which will be set to 
   * false if the message has been sent out or if the waveform is otherwise unable to add destinations to the message.  Otherwise the status should be set to true in 
   * the response message.
   * 
   * @param _rId Specifies the ID of the request, which the waveform will use in its response message
   * @param _msgId ID of the message that to which the destinations needs to be added
   * @param _destArray New destinations to be added to the message
   * @param _noOfDestinations Number of destinations in the destArray. If this is 0, the request will be ignored by waveform.
   * @return void
   */
  virtual void AddDestinationRequest (RequestId_t _rId, WF_MessageId_t _msgId, WF_AddressType *_destArray, uint16_t _noOfDestinations) = 0;
  
  /// virtual destructor for interface
  virtual ~Waveform_I() {} 

};

  
} //end of namespace

#endif // WF_I_H_
