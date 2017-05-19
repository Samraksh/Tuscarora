/* Copyright: The Samraksh Company
 * SLIM-WF:Synchronous Lightweight Interfernce-aware Model-driven Wavefrm
 * The usage of this module is governed by the associated license file.
 * Authors: Bora Karaoglu, Mukundan Sridharan
 */

#ifndef SLIM_NET_DEVICE_H
#define SLIM_NET_DEVICE_H

#include <stdint.h>
#include <string>
#include "ns3/traced-callback.h"
#include "ns3/net-device.h"
#include "ns3/mac48-address.h"
#include "ns3/trace-helper.h"
#include "ns3/data-rate.h"
#include "ns3/drop-tail-queue.h"
#include "ns3/ethernet-header.h"
#include "ns3/double.h"
//#include "ns3/random-variable-stream.h"
#include "ns3/ampdu-tag.h"

#include "ns3/slim-module.h"

#define InitialNumberofSlotsinAFrame 100
#define SLOTDURATION_IN_MICROSECONDS 1000
typedef uint64_t slotNumber_t;

namespace ns3 {

//class SimpleTdmaChannel;
class Node;

//********************************************************
//  SLIM_TimestampTag used to store a timestamp with a packet
//  when they are placed in the queue
//********************************************************
class SLIM_TimestampTag : public Tag {
public:
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;

  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (TagBuffer i) const;
  virtual void Deserialize (TagBuffer i);

  // these are our accessors to our tag structure
  void SetTimestamp (Time time);
  Time GetTimestamp (void) const;

  void Print (std::ostream &os) const;

private:
  Time m_timestamp;

  // end class SLIM_TimestampTag
};




/**
 * \ingroup netdevice
 *
 * This device does not have a helper and assumes 48-bit mac addressing;
 * the default address assigned to each device is zero, so you must 
 * assign a real address to use it.  There is also the possibility to
 * add an ErrorModel if you want to force losses on the device.
 * 
 * \brief simple net device for simple things and testing
 */
class SlimNetDevice : public NetDevice
{
public:
  static TypeId GetTypeId (void);
  SlimNetDevice ();

  void Receive (Ptr<Packet> packet, uint16_t protocol, Mac48Address to, Mac48Address from);
  void SetPhy (Ptr<SlimPhy> phy);

  void RxFromPhyOK (Ptr<Packet> aggregatedPacket, double rxSnr, SlimTxVector txVector);
  void RxFromPhyError (Ptr<const Packet> packet, double rxSnr);


    /**
   * Attach a queue to the PointToPointNetDevice.
   *
   * The PointToPointNetDevice "owns" a queue that implements a queueing 
   * method such as DropTail or RED.
   *
   * @see Queue
   * @see DropTailQueue
   * @param queue Ptr to the new queue.
   */
  void SetQueue (Ptr<Queue> queue);

  /**
   * Get a copy of the attached Queue.
   *
   * @returns Ptr to the queue.
   */
  Ptr<Queue> GetQueue (void) const;

  bool IsPhyReady2Tx();
  
  void EnablePcapAll(std::string filename);


  // inherited from NetDevice base class.
  virtual void SetIfIndex(const uint32_t index);
  virtual uint32_t GetIfIndex(void) const;
  virtual Ptr<Channel> GetChannel (void) const;
  virtual void SetAddress (Address address);
  virtual Address GetAddress (void) const;
  virtual bool SetMtu (const uint16_t mtu);
  virtual uint16_t GetMtu (void) const;
  virtual bool IsLinkUp (void) const;
  virtual void AddLinkChangeCallback (Callback<void> callback);
  virtual bool IsBroadcast (void) const;
  virtual Address GetBroadcast (void) const;
  virtual bool IsMulticast (void) const;
  virtual Address GetMulticast (Ipv4Address multicastGroup) const;
  virtual bool IsPointToPoint (void) const;
  virtual bool IsBridge (void) const;
  virtual bool Send(Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber);
  virtual bool SendFrom(Ptr<Packet> packet, const Address& source, const Address& dest, uint16_t protocolNumber);
  virtual Ptr<Node> GetNode (void) const;
  virtual void SetNode (Ptr<Node> node);
  virtual bool NeedsArp (void) const;
  virtual void SetReceiveCallback (NetDevice::ReceiveCallback cb);

  virtual Address GetMulticast (Ipv6Address addr) const;

  virtual void SetPromiscReceiveCallback (PromiscReceiveCallback cb);
  virtual bool SupportsSendFrom (void) const;


protected:
  virtual void DoDispose (void);
private:
  Ptr<SlimPhy> m_phy; //!< Pointer to SlimPhy (actually send/receives frames)
  //Ptr<SimpleTdmaChannel> m_channel;
  NetDevice::ReceiveCallback m_rxCallback;
  NetDevice::PromiscReceiveCallback m_promiscCallback;
  Ptr<Node> m_node;
  uint16_t m_mtu;
  uint32_t m_ifIndex;
  Mac48Address m_address;
  
  uint32_t txmodenum;

  Ptr<Packet> m_currentPkt;
  
  uint8_t m_ContentionWindowSize;
  Time m_SlotDuration;

  // BK
  slotNumber_t CurrentSlotNumber();
  void SelectNextSlot(slotNumber_t curslot);
  bool CanITXInCurrentSlot();
  void ScheduleNextSlot();
  void ExecuteCurrSlot(void);
  Time TimeTilltheEndofCurrentSlot();

  void SetContentionWindowSize(uint8_t ns);
  uint8_t GetContentionWindowSize();


    /**
   * Start Sending a Packet Down the Wire.
   *
   * The TransmitStart method is the method that is used internally in the
   * NetDevice to begin the process of sending a packet out on
   * the channel.  The corresponding method is called on the channel to let
   * it know that the physical device this class represents has virtually
   * started sending signals.  An event is scheduled for the time at which
   * the bits have been completely transmitted.
   */
  void TransmitStart ();
  bool Decide2TxorSchedule();
  SlimTxVector PrepareTxVector (Ptr<const Packet> packet);

  /**
   * Stop Sending a Packet Down the Wire and Begin the Interframe Gap.
   *
   * The TransmitComplete method is used internally to finish the process
   * of sending a packet out on the channel.
   */
  void TransmitComplete (void);

  /**
   * Enumeration of the states of the transmit machine of the net device.
   */
  enum TxMachineState
  {
    READY,   /**< The transmitter is ready to begin transmission of a packet */
    BUSY     /**< The transmitter is busy transmitting a packet */
  };
  /**
   * The state of the Net Device transmit state machine.
   * @see TxMachineState
   */
  TxMachineState m_txMachineState;

  
    /**
   * The Queue which this PointToPointNetDevice uses as a packet source.
   * Management of this Queue has been delegated to the PointToPointNetDevice
   * and it has the responsibility for deletion.
   * @see class Queue
   * @see class DropTailQueue
   */
  Ptr<Queue> m_queue;
  
  /**
   * The trace source fired when the phy layer drops a packet it has received
   * due to the error model being active.  Although SlimNetDevice doesn't
   * really have a Phy model, we choose this trace source name for alignment
   * with other trace sources.
   *
   * \see class CallBackTraceSource
   */
  TracedCallback<Ptr<const Packet> > m_phyRxDropTrace;

  /**
   * A trace source that emulates a promiscuous mode protocol sniffer connected
   * to the device.  This trace source fire on packets destined for any host
   * just like your average everyday packet sniffer.
   *
   * The trace is captured on send and receive.
   *
   * \see class CallBackTraceSource
   */
  TracedCallback<Ptr<const Packet> > m_promiscSnifferTrace;
  
  /**
   * The trace source fired when a packet begins the transmission process on
   * the medium.
   *
   * \see class CallBackTraceSource
   */
  TracedCallback<Ptr<const Packet>, Mac48Address, uint16_t> m_TxBeginTrace;
  
  /**
   * The trace source fired when a packet is dequeued to be sent.
   * It does NOT track queue latency for packets that get dropped
   * in the queue only that that are actually sent from the queue
   *
   * \see class CallBackTraceSource
   */
  TracedCallback<Time> m_QueueLatencyTrace;

  
  uint32_t  m_pktRcvTotal;
  uint32_t  m_pktRcvDrop;
  uint32_t  m_pktCollisionLoss;
  bool       m_pcapEnabled;


  //
  slotNumber_t m_nextSelectedSlot;
  Ptr<UniformRandomVariable> m_rng;
  bool m_txSlotScheduled;
};

} // namespace ns3

#endif /* SLIM_WF_NET_DEVICE_H */
