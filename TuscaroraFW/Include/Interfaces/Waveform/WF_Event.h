////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef _WF_EVENT_H_
#define _WF_EVENT_H_
//specilization of the AsyncDelegate class for the Mulitprocess waveform implementation
#include <Base/FrameworkTypes.h>
#include <Base/Delegate.h>


using namespace Types;

namespace Waveform {

  enum WF_EventNameE {
	WF_GREET_EVT,
    WF_LINK_EST_EVT,
    WF_RECV_MSG_EVT,
    WF_DATA_NTY_EVT,
    WF_CONT_RES_EVT,
    WF_SCHD_UPD_EVT
  };
 
/**
  # Received Message Event #
 ## Invoke Method Definition ##
 * bool WF_RcvMessageEvent_t::Invoke (WF_RecvMsgParam param);
 * 
 * The Received Message Event class has a single public method called Invoke, which can be use do send the event to the framework, with WF\_RecvMsgParam as
 * the parameter.
 ## Expected Behavior: ##
 * When the waveform receives a valid data message through its radio interface, the waveform should event the Framework using the ``Receive Message Event''.
 * Upon receipt of a message, waveform will evaluate metadata (RSSI, SINR, Receive Timestamp) associated with the message and the metadata will be added to
 * the packet structure and passed in the parameter to the event.
 * An operating mode wherein the waveform forwards up to the Framework each and every packets that it receives and decodes correctly irrespective of whether
 * the packet was destined for the node is commonly referred to as ``promiscuous mode''. The API does not take a position on whether or not waveforms should
 * support promiscuous mode. (Hence, no API is currently defined to configure such a mode). If a future programmatic decision is made to support promiscuous
 * mode of waveforms, a policy manager (through the Framework) will explicitly configure the waveform to operate in such a mode. Unless explicitly configured
 * a waveform should not send packets not intended for the node to the Framework.
 * Note to Pattern designers: If a waveform does send packets to the Framework that are not meant for a particular node, the Framework has no way of identifying
 * such packets. The Framework cannot guarantee to a Pattern that it will not receive such “unintentional” packets. If a Pattern does receive such packets they would have originated from the same Pattern instance on a neighboring node, but are not intended to be received on the current node (i.e., right pattern, wrong node). We recommend that Pattern designers consider the possibility of receiving some unintentional packets and make their patterns robust enough to handle them.
 ## Event creation and invocation: ##
 * The details of the WF_RecvMsgParam parameter, and the code for creation and invocation of the Received Message are shown below. The waveform provider
 * creates the event object and the parameter object. Further, it initializes the parameter object with the received data message, the rcvMsgId is assigned
 * a MessageId generated by the waveform for the messages received by it. Once the parameter is initialized, the waveform can simply invoke the event with the parameter. The event class will convert this into a message of the appropriate format and send it using the shim layer to the Framework,
 * where it will be routed to the particular module that will process the received waveform data message.
 
 #  Data Notification Event #
 ## Invoke Method Definition ##
 * bool WF_DataNotifierEvent_t::Invoke (WF_DataNotifierParam param);
 * The Data Notification class has a single public method called Invoke, which can be use do send the event to the framework, with WF_DataNotifierParam as
 * the parameter.
 ## Expected Behavior:
 * When a ‘SendData’ or ‘BroadcastData’ command is received by the Waveform, the Framework expects a Data Notification Event in response. The ackType in the parameter of the Data Notification Event indicates the type of notification. For each SendData command received with a particular MessageId, the waveform should send back at least two events, one of the ACK_WF_RECV type, indicating that the waveform received the message successfully and one of the ACK_WF_SENT type, indicating the waveform sent the message out through its radio. A third type named ACK_DST_RECV can also be sent if the waveform supports acks from the destination. For the BroadcastData command, the first two types are required and the third is not required. The ‘dest’ field in the param is an array that determines for which destinations that notification is being sent. For example, if the Waveform chooses to send multiple physical messages corresponding to the same “MessageId” once for each destination, then a notification event can be sent once per destination or it can all be grouped together and sent as a single notification.
 ## Event Parameters:
 * The parameter creation and invocation is quite similar to that of the other events. The waveform needs to create an object of WF_DataNotifierEvent_t,
 * create an object of the WF_DataNotifierParam and invoke the event with the param. The parameter for this event is show below.
 
 # Link Estimation Event
 ## Invoke Method Definition
 * bool WF_LinkEstimateEvent_t::Invoke (WF_LinkEstimateParam param);
 * The Link Estimation Event class has a single public method called Invoke, which can be use do send the event to the framework, with WF_LinkEstimateParam as the parameter.
 ## Expected Behavior:
 * Link estimation of known neighbors is an optional function that the waveforms can choose to provide. If supported, the waveform should set the ‘estType’ field in its Attribute to WF_EST_FULL whenever it responds to the AttributeRequest.
 * Waveform that provide link estimation should provide link estimation events, and follow a standard format. The Framework defines a link metric structure that the waveform should compute for each neighbor. The WF\_LinkMetrics structure is show below, it consist of four metrics that are defined as follows:
 * 1. Quality: Abstract quality metric expressed as a real number between [0,1].
 * 2. Data rate: Link data rate expressed as log_2(bps).
 * 3. Average Latency: Average latency in sending a packet to the destination expressed as log_2(seconds).
 * 4. Energy: Average energy to transmit a packet expressed as log_2(pica-joules).
 * 
 The waveform is expected to generate a event for each neighbor whenever one of the three events happens:
 * 1. New Neighbor: Whenever the waveform detects a new neighbor and has an initial estimate of its link metrics. It uses the value NBR_NEW for the ‘changeType’ field of the event parameter.
 * 2. Dead Neighbor: Whenever a previous neighbor no longer exist in the neighbor. Use the value NBR_DEAD for the ‘changeType’ field of the event parameter
 * 3. Change in Metrics: Whenever the metrics has changed enough to notify the Framework. It uses the value NBR_UPDATE for the ‘changeType’ field of the event parameter.
 ## Event Parameters:
 * The  WF_LinkEstimateParam structure (shown above) is used as the parameter for the Link Estimation Event. The structure contains the waveform address, the link type, its link metrics and the type of change in neighborhood being conveyed by the message.
 # Control Response Event
 ## Invoke Method Definition
 * bool WF_ControlResponseEvent_t::Invoke (WF_ControlResponseParam param);
 * The Control Response Event class has a single public method called Invoke, which can be use do send the event to the framework, with WF_ControlResponseParam as the parameter.
 ## Expected Behavior:
 * Whenever a waveform receives any request command, i.e., a command whose name has the suffix Request, it should use the Control Response Event to send a response. The response should use the same “RequestId” as in the request command. The Framework tracks the RequestId that it uses for each waveform and generates a new RequestId for every request command. If the Framework fails to receive a response for a request, the Framework may resend the request using the same RequestId.
 ## Event Parameters:
 * The WF_ControlResponseParam parameter associated with the Control Response is shown below. The key field is ‘type’, which is an enum of type WF_ControlResponseE. This enumerator defines a response for each of the Request commands. The waveform should set this type appropriately based on the command it is responding too. The parameter also contains a generic ‘data’ field, whose interpretation by the Framework depends upon on the type of the response. Based on the type of Request command that is being responded to, an appropriate structure is copied into the ‘data’ field, and the number of bytes sent through the data field should be set appropriately in the dataSize  field.
 
 # Schedule Update Event
 ## Invoke Method Definition
 * bool WF_ScheduleUpdateEvent_t::Invoke (WF_ScheduleUpdateParam param);
 * The Schedule Update Event class has a single public method called Invoke, which can be use do send the event to the framework, with WF_ScheduleUpdateParam as the parameter.
 ## Expected Behavior:
 * Schedule updates are an optional feature by which waveforms that maintain schedules for functions such as sending, receiving or link estimation can share information about the schedules with the Framework. If the feature is supported, the waveform should set as true the ‘schedulerInfoSupport’ field in its Attribute whenever it responds to the AttributeRequest.
 * This event is expected to be implemented by waveforms that have timing characteristics or TDMA schedules. The eventing should be implemented if the knowledge of a waveform's slot schedules will help the Framework in improving the overall data efficiency.
 * Note: If the waveform is responding to the ‘ScheduleRequest’ command, then it should use the Control Response Event; and, if it is sending ongoing schedule updates, it should use the Schedule Updates Event.
 ## Event Parameter
 * The WF_ScheduleUpdateParam structure that is used as the parameter of the Schedule Update Event is shown below. The parameter contains information about one of three types of schedules,  send, receive or link estimation, which is indicated by setting the ‘type’ field appropriately. The ‘data’ field is a pointer of type ScheduleBaseI which points to the object that contains the actual schedule information for the neighbor indicated by ‘nodeid’. 
 * The dataSize field contains the size of the actual schedule object that is pointed to by the data field.
 */
  template <WF_EventNameE evtName, typename Param0>
  class WF_Event{
    
  private:
    //int EventCount;
    Delegate<void, Param0> *delegate;
	WaveformId_t WID;

  public:
    WF_Event(WaveformId_t id);
    ~WF_Event(void);

    //bool operator()(Param0 param0);
    bool Invoke(Param0 param0);
    
  };
  //CancelDataRequest
  struct WF_CancelDataResponse_Param{
      MessageId_t msgId; // Stores message Id
      bool        status;  //Set to true when cancel was successful for all destinations
      NodeId_t    destArray[16]; //Copy of destination array passed from Pattern
      bool        cancel_status[16];// cancel_status[i] is set to true if destArray[i] is canceled successfully.
      uint16_t noOfDest;
  };
  //CancelDataRequest
  struct WF_ReplacePayloadResponse_Param{
      MessageId_t msgId; // Stores message Id
      bool        status;  //Set to true when cancel was successful for all destinations
      NodeId_t    destArray[16]; //Copy of destination array passed from Pattern
      bool        replace_status[16];// cancel_status[i] is set to true if destArray[i] is canceled successfully.
      uint16_t noOfDest;
  };
  struct WF_AddDestinationResponse_Param{
        MessageId_t msgId; // Stores message Id
        bool        status;  //Set to true when cancel was successful for all destinations
        uint64_t    destArray[MAX_DEST]; //Copy of destination array passed from Pattern
        bool        add_status[MAX_DEST];// cancel_status[i] is set to true if destArray[i] is canceled successfully.
        uint16_t noOfDest;
    };
}

#endif // _WF_EVENT_H_
