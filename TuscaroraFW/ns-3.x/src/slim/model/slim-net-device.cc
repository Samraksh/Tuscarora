/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/* Copyright: The Samraksh Company
 * SLIM-WF:Synchronous Lightweight Interfernce-aware Model-driven Wavefrm
 * The usage of this module is governed by the associated license file.
 * Authors: Bora Karaoglu, Mukundan Sridharan
 */
#include "slim-net-device.h"

#include "ns3/node.h"
#include "ns3/packet.h"
#include "ns3/log.h"
#include "ns3/pointer.h"
//#include "ns3/error-model.h"
#include "ns3/trace-source-accessor.h"
#include <netinet/in.h>  // needed for noth for protocol # in sniffer
#include "ns3/uinteger.h"

NS_LOG_COMPONENT_DEFINE ("SlimNetDevice");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (SlimNetDevice);

//********************************************************
//  SLIM_TimestampTag used to store a timestamp with a packet
//  when they are placed in the queue
//********************************************************
TypeId SLIM_TimestampTag::GetTypeId (void)
{
	static TypeId tid = TypeId ("SLIM_TimestampTag")
    						.SetParent<Tag> ()
							.AddConstructor<SLIM_TimestampTag> ()
							.AddAttribute ("Timestamp",
									"Some momentous point in time!",
									EmptyAttributeValue (),
									MakeTimeAccessor (&SLIM_TimestampTag::GetTimestamp),
									MakeTimeChecker ())
									;
	return tid;
}

TypeId SLIM_TimestampTag::GetInstanceTypeId (void) const
{
	return GetTypeId ();
}

uint32_t SLIM_TimestampTag::GetSerializedSize (void) const
{
	return 8;
}

void SLIM_TimestampTag::Serialize (TagBuffer i) const
{
	int64_t t = m_timestamp.GetNanoSeconds ();
	i.Write ((const uint8_t *)&t, 8);
}

void SLIM_TimestampTag::Deserialize (TagBuffer i)
{
	int64_t t;
	i.Read ((uint8_t *)&t, 8);
	m_timestamp = NanoSeconds (t);
}

void SLIM_TimestampTag::SetTimestamp (Time time)
{
	m_timestamp = time;
}

Time SLIM_TimestampTag::GetTimestamp (void) const
{
	return m_timestamp;
}

void SLIM_TimestampTag::Print (std::ostream &os) const
{
	os << "t=" << m_timestamp;
}

//********************************************************



TypeId 
SlimNetDevice::GetTypeId (void)
{
	static TypeId tid = TypeId ("ns3::SlimNetDevice")
    						.SetParent<NetDevice> ()
							.AddConstructor<SlimNetDevice> ()
							.AddAttribute ("TxVectorIndex",
									"The index of the SlimTxVector to be used for transmitting packets in the list of supported SlimTxVectors of the attached phy layer.",
									TypeId::ATTR_SGC, // allow both getting and setting it
									UintegerValue (0),
									MakeUintegerAccessor (&SlimNetDevice::txmodenum),
									MakeUintegerChecker <uint32_t> ())
							.AddAttribute("AverageFrameSize",
									"Average number of slots in between two  transmission slots. ",
									UintegerValue (10),
									MakeUintegerAccessor (&SlimNetDevice::m_ContentionWindowSize),
									MakeUintegerChecker <uint8_t> (0,64))
							.AddAttribute("SlotDuration",
									"The size of each slot(in ns3::Time type)",
									TypeId::ATTR_CONSTRUCT, // allow setting only at the construction time
									TimeValue (MicroSeconds (SLOTDURATION_IN_MICROSECONDS)),
									MakeTimeAccessor (&SlimNetDevice::m_SlotDuration),
									MakeTimeChecker ())
							// Transmit queueing discipline for the device which includes its own set
							// of trace hooks.
							.AddAttribute ("TxQueue",
								   "A pointer to a queue object used as the transmit queue in the device.",
								   PointerValue (CreateObject<DropTailQueue> ()),
								   MakePointerAccessor (&SlimNetDevice::m_queue),
								   MakePointerChecker<Queue> ())
							.AddTraceSource ("PhyTxBegin",
									"Trace source indicating a packet has begun transmitting",
									MakeTraceSourceAccessor (&SlimNetDevice::m_TxBeginTrace),
									 "ns3::Packet::TracedCallback")
							.AddTraceSource ("PhyRxDrop",
									"Trace source indicating a packet has been dropped by the device during reception",
									MakeTraceSourceAccessor (&SlimNetDevice::m_phyRxDropTrace),
									 "ns3::Packet::TracedCallback")
									// Trace sources designed to simulate a packet sniffer facility (tcpdump).
							.AddTraceSource ("PromiscSniffer",
									"Trace source simulating a promiscuous packet sniffer attached to the device",
									MakeTraceSourceAccessor (&SlimNetDevice::m_promiscSnifferTrace),
									"ns3::Packet::TracedCallback")
							.AddTraceSource ("QueueLatency",
									"Trace source to report the latency of a packet in the queue. Datatype returned is Time.",
									MakeTraceSourceAccessor (&SlimNetDevice::m_QueueLatencyTrace),
									"ns3::Time::TracedCallback")
									;
	return tid;
}

SlimNetDevice::SlimNetDevice ()
: m_phy (0),
  m_node (0),
  m_mtu (0xffff),
  m_ifIndex (0),
  m_txMachineState (READY),
  m_pktRcvTotal(0),
  m_pktRcvDrop(0),
  m_pktCollisionLoss(0),
  m_pcapEnabled(false)

{
	if(m_queue == NULL) {
		m_queue = CreateObject<DropTailQueue> ();
	}
	m_ContentionWindowSize = InitialNumberofSlotsinAFrame;
	m_txSlotScheduled = false;
	m_nextSelectedSlot = 0;
	m_rng = CreateObject<UniformRandomVariable> ();
}



void 
SlimNetDevice::Receive (Ptr<Packet> packet, uint16_t protocol,
		Mac48Address to, Mac48Address from)
{
	NS_LOG_FUNCTION (packet << protocol << to << from);
	NetDevice::PacketType packetType;

	m_pktRcvTotal++;

	if (m_pcapEnabled)
	{
		// Add the ethernet header to the packet for sniffer
		// For some reason the Ethernet header is STRIPPED from the packet
		// by the time we get here so we need to reconstruct it for the
		// sniffer.
		// allocate buffer for packet and mac addresses
		uint8_t buffer[8192];
		uint8_t macBuffer[6];
		// get the dest and src addresses and copy into the buffer
		to.CopyTo(macBuffer);
		memcpy(&buffer[0], macBuffer, 6);
		from.CopyTo(macBuffer);
		memcpy(&buffer[6], macBuffer, 6);
		//copy the protocol and the actual packet data in the the buffer
		uint16_t tempProt = ntohs(protocol);
		memcpy(&buffer[12], &tempProt, 2);
		packet->CopyData(&buffer[14], packet->GetSize());
		// Now make a new temp packet to write to the sniffer
		Ptr<Packet> tempPacket = Create<Packet> (buffer, packet->GetSize() + 14);
		m_promiscSnifferTrace (tempPacket);
	}

	if (to == m_address)
	{
		packetType = NetDevice::PACKET_HOST;
	}
	else if (to.IsBroadcast ())
	{
		packetType = NetDevice::PACKET_BROADCAST;
	}
	else if (to.IsGroup ())
	{
		packetType = NetDevice::PACKET_MULTICAST;
	}
	else
	{
		packetType = NetDevice::PACKET_OTHERHOST;
	}

	if (packetType != NetDevice::PACKET_OTHERHOST)
	{
		m_rxCallback (this, packet, protocol, from);
	}

	if (!m_promiscCallback.IsNull ())
	{
		m_promiscCallback (this, packet, protocol, from, to, packetType);
	}
	NS_LOG_DEBUG ("<m_address:"<< m_address << "> - Total Rcvd: " << m_pktRcvTotal << " Total Dropped: " << m_pktRcvDrop);
}

void SlimNetDevice::SetPhy (Ptr<SlimPhy> phy)
{
	m_phy = phy;
	m_phy->SetReceiveOkCallback (MakeCallback (&SlimNetDevice::RxFromPhyOK, this));
	m_phy->SetReceiveErrorCallback (MakeCallback (&SlimNetDevice::RxFromPhyError, this));
	//SetupPhyMacLowListener (phy);
}

void SlimNetDevice::RxFromPhyOK (Ptr<Packet> packet, double rxSnr, SlimTxVector txVector) {
	AmpduTag ampdu;
	bool isInAmpdu = packet->RemovePacketTag(ampdu);
	if (isInAmpdu){
		NS_FATAL_ERROR ("Received A-MPDU in SlimNetDevice. Aggregation is not supported!!");
	}

	NS_LOG_FUNCTION (this << packet << rxSnr << txVector);
	SlimDbmHeader dbm_;
	packet->RemoveHeader(dbm_);

	//Remove Mac Header
	EthernetHeader ethHeader;
	packet->RemoveHeader (ethHeader);
	Mac48Address to = ethHeader.GetDestination ();
	Mac48Address from = ethHeader.GetSource ();
	uint16_t protocol = ethHeader.GetLengthType ();
	Receive(packet,protocol,to,from);
}


void SlimNetDevice::RxFromPhyError (Ptr<const Packet> packet, double rxSnr){
	  NS_LOG_FUNCTION (this << packet << rxSnr);
	  //NS_LOG_DEBUG ("rx failed ");
	  NS_LOG_DEBUG ("<m_address:"<< m_address << "Total Collision: "<< m_pktCollisionLoss);
}


void 
SlimNetDevice::SetIfIndex(const uint32_t index)
{
	m_ifIndex = index;
}
uint32_t 
SlimNetDevice::GetIfIndex(void) const
{
	return m_ifIndex;
}
Ptr<Channel> SlimNetDevice::GetChannel (void) const
{
	return m_phy->GetChannel ();
}
void
SlimNetDevice::SetAddress (Address address)
{
	m_address = Mac48Address::ConvertFrom(address);
}
Address 
SlimNetDevice::GetAddress (void) const
{
	//
	// Implicit conversion from Mac48Address to Address
	//
	return m_address;
}
bool 
SlimNetDevice::SetMtu (const uint16_t mtu)
{
	m_mtu = mtu;
	return true;
}
uint16_t 
SlimNetDevice::GetMtu (void) const
{
	return m_mtu;
}
bool 
SlimNetDevice::IsLinkUp (void) const
{
	return true;
}
void 
SlimNetDevice::AddLinkChangeCallback (Callback<void> callback)
{}
bool 
SlimNetDevice::IsBroadcast (void) const
{
	return true;
}
Address
SlimNetDevice::GetBroadcast (void) const
{
	return Mac48Address ("ff:ff:ff:ff:ff:ff");
}
bool 
SlimNetDevice::IsMulticast (void) const
{
	return false;
}
Address 
SlimNetDevice::GetMulticast (Ipv4Address multicastGroup) const
{
	return Mac48Address::GetMulticast (multicastGroup);
}

Address SlimNetDevice::GetMulticast (Ipv6Address addr) const
{
	return Mac48Address::GetMulticast (addr);
}

bool 
SlimNetDevice::IsPointToPoint (void) const
{
	return false;
}

bool 
SlimNetDevice::IsBridge (void) const
{
	return false;
}



void SlimNetDevice::TransmitStart ()
{
  NS_LOG_DEBUG("Starting transmit");
	Ptr<Packet> p = m_queue->Dequeue ()->GetPacket();
	if(p == 0 ){
		return;
	}

	NS_LOG_FUNCTION (this << p);

	// This function is called to start the process of transmitting a packet.
	// We need to tell the channel that we've started wiggling the wire and
	// schedule an event that will be executed when the transmission is complete.
	NS_ASSERT_MSG (!(m_phy->IsStateTx ()), "Must be READY to transmit");
	m_txMachineState = BUSY;
	m_currentPkt = p;

	if (m_pcapEnabled)
	{
		m_promiscSnifferTrace (p);
	}

	// Remove ethernet header since it is not sent over the air //BK: Header(whatever header is used) should be sent over the air.
	EthernetHeader ethHeader;
	p->PeekHeader(ethHeader);
	Mac48Address to = ethHeader.GetDestination ();
	//Mac48Address from = ethHeader.GetSource ();
	uint16_t protocol = ethHeader.GetLengthType ();

	// Remove the tag. calculate queue latency and peg trace
	SLIM_TimestampTag timeEnqueued;
	p->RemovePacketTag (timeEnqueued);
	Time curtime = Simulator::Now();
	Time latency = curtime - timeEnqueued.GetTimestamp();
	m_QueueLatencyTrace(latency);
	NS_LOG_DEBUG (Simulator::Now() << " Getting packet with timestamp: " << timeEnqueued.GetTimestamp() );

	//****BK START: Depreciate these and use PHY level functions instead
	SlimTxVector txVector = PrepareTxVector(p);
	Time txTime = m_phy->CalculateTxDuration (txVector);
	// TO DO: do we need interframe gap??
	//Time txCompleteTime = txTime + m_tInterframeGap;
	Time txCompleteTime = txTime;

	NS_LOG_DEBUG ("Schedule TransmitCompleteEvent in " << txCompleteTime.GetMicroSeconds () << "usec");
	Simulator::Schedule (txCompleteTime, &SlimNetDevice::TransmitComplete, this);
	//****BK END: Depreciate these and use PHY level functions instead

	m_TxBeginTrace (p, to, protocol);

	//BK m_channel->Send (p, protocol, to, from, this, txTime);
	m_phy->SendPacket (p, txVector, 0);
}

SlimTxVector SlimNetDevice::PrepareTxVector (Ptr<const Packet> packet){
	//std::string txMode = "DsssRate2Mbps";
	uint8_t txPowerLevel = 0;
	SlimTxVector txVector = (m_phy->GetObject<TuscaroraSlimPhy> ())->GetTxVector(txmodenum); //Copy the chosen tx vector into a new one

	txVector.SetPayloadSize (packet->GetSize() ); // Modify size and power level
	txVector.SetTxPowerLevel (txPowerLevel);

	//ToDo:
	//BK: Things to be GetMode(), GetTxPowerLevel(),


	return txVector;
}


void
SlimNetDevice::TransmitComplete (void)
{
	NS_LOG_FUNCTION_NOARGS ();

	// This function is called to when we're all done transmitting a packet.
	// We try and pull another packet off of the transmit queue.  If the queue
	// is empty, we are done, otherwise we need to start transmitting the
	// next packet.
	NS_ASSERT_MSG (m_txMachineState == BUSY, "Must be BUSY if transmitting");
	m_txMachineState = READY;

	NS_ASSERT_MSG (m_currentPkt != 0, "SlimNetDevice::TransmitComplete(): m_currentPkt zero");
	m_currentPkt = 0;

	NS_LOG_DEBUG ("Tx complete. Packets in queue: " <<  m_queue->GetNPackets() << " Bytes in queue: " << m_queue->GetNBytes());

	ExecuteCurrSlot(); // Check if we can process more packets before the end of current slot.
}


bool 
SlimNetDevice::Send(Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber)
{
	NS_LOG_FUNCTION (packet << dest << protocolNumber);
	Mac48Address to = Mac48Address::ConvertFrom (dest);

	// add ethernet header to the packet in the queue so we can
	// retrieve the to, from and protocol. Also Ethernet header is
	// needed so we can apply a pcap filter in priority queues
	EthernetHeader ethHeader;
	ethHeader.SetSource (m_address);
	ethHeader.SetDestination (to);
	ethHeader.SetLengthType (protocolNumber);
	packet->AddHeader (ethHeader);


	// Add a timestamp tag for latency
	SLIM_TimestampTag timestamp;
	timestamp.SetTimestamp (Simulator::Now ());
	packet->AddPacketTag (timestamp);

	NS_LOG_DEBUG ("Received packet to send. Protocol "<<  protocolNumber << " Current state is: " << m_txMachineState);

	// We should enqueue and dequeue a queueitem contaiing the packet to hit the tracing hooks.
	Ptr<QueueItem> p_qi(new QueueItem(packet));
	m_queue->Enqueue (p_qi);

	Decide2TxorSchedule();
	// TO DO: do we return true or false here??
	return true;
}


bool 
SlimNetDevice::SendFrom(Ptr<Packet> packet, const Address& source, const Address& dest, uint16_t protocolNumber)
{
	NS_LOG_FUNCTION (packet << dest << protocolNumber);
	Mac48Address to = Mac48Address::ConvertFrom (dest);
	Mac48Address from = Mac48Address::ConvertFrom (source);

	// add ethernet header to the packet in the queue so we can
	// retrieve the to, from and protocol. Also Ethernet header is
	// needed so we can apply a pcap filter in priority queues
	EthernetHeader ethHeader;
	ethHeader.SetSource (from);
	ethHeader.SetDestination (to);
	ethHeader.SetLengthType (protocolNumber);
	packet->AddHeader (ethHeader);

	// Add a timestamp tag for latency
	SLIM_TimestampTag timestamp;
	timestamp.SetTimestamp (Simulator::Now ());
	packet->AddPacketTag (timestamp);

	NS_LOG_DEBUG ("Received packet to send. Current state is: " <<  m_phy->IsStateTx ());

	Decide2TxorSchedule();

	// TO DO: do we return true or false here??
	return true;
}

bool SlimNetDevice::IsPhyReady2Tx(){
	if(!(m_phy->IsStateTx ()) && !(m_phy->IsStateSwitching())){
		return true;
	}
	return false;
}

bool SlimNetDevice::Decide2TxorSchedule(){
	// We should enqueue and dequeue the packet to hit the tracing hooks.
	// If the channel is ready for transition we send the packet right now

	if(!m_txSlotScheduled && CurrentSlotNumber() == m_nextSelectedSlot && CanITXInCurrentSlot()) {
		if(IsPhyReady2Tx()){
			NS_LOG_DEBUG("TransmitStart called");
			TransmitStart ();
			return true;
		}
		else{
			  NS_LOG_DEBUG("Not ready to transmit");
			  NS_LOG_DEBUG("Scheduling for next slot");
			  ScheduleNextSlot();
			  return false;
		}
	}
	else{
		  NS_LOG_DEBUG("Scheduling for next slot");
		  ScheduleNextSlot();
		  return false;
	}
}

Ptr<Node> 
SlimNetDevice::GetNode (void) const
{
	return m_node;
}
void 
SlimNetDevice::SetNode (Ptr<Node> node)
{
	m_node = node;
}
bool 
SlimNetDevice::NeedsArp (void) const
{
	return true;
}
void 
SlimNetDevice::SetReceiveCallback (NetDevice::ReceiveCallback cb)
{
	m_rxCallback = cb;
}

void
SlimNetDevice::DoDispose (void)
{
	m_node = 0;
	NetDevice::DoDispose ();
}


void
SlimNetDevice::SetPromiscReceiveCallback (PromiscReceiveCallback cb)
{
	m_promiscCallback = cb;
}

bool
SlimNetDevice::SupportsSendFrom (void) const
{
	return true;
}


void
SlimNetDevice::SetQueue (Ptr<Queue> q)
{
	NS_LOG_FUNCTION (this << q);
	m_queue = q;
}

Ptr<Queue>
SlimNetDevice::GetQueue (void) const
{ 
	NS_LOG_FUNCTION_NOARGS ();
	return m_queue;
}


void SlimNetDevice::EnablePcapAll (std::string filename)
{
	PcapHelper pcapHelper;
	Ptr<PcapFileWrapper> file = pcapHelper.CreateFile (filename, std::ios::out, PcapHelper::DLT_EN10MB);
	pcapHelper.HookDefaultSink<SlimNetDevice> (this, "PromiscSniffer", file);
	m_pcapEnabled = true;
}



slotNumber_t SlimNetDevice::CurrentSlotNumber(){
	slotNumber_t dummy = Simulator::Now()/ m_SlotDuration;
	return(dummy);
}

void SlimNetDevice::SelectNextSlot(slotNumber_t curslot) {
	while(m_nextSelectedSlot <=  curslot) {
		m_nextSelectedSlot += (slotNumber_t) (m_rng->GetInteger (1, m_ContentionWindowSize));
	}
}

void SlimNetDevice::SetContentionWindowSize(uint8_t ns){
	NS_LOG_FUNCTION (this << static_cast<uint32_t> (ns));
	m_ContentionWindowSize = ns;
}
uint8_t SlimNetDevice::GetContentionWindowSize(){
	NS_LOG_FUNCTION (this);
	return m_ContentionWindowSize;
}


bool SlimNetDevice::CanITXInCurrentSlot() {
	Time txTime = m_phy->CalculateTxDuration (PrepareTxVector(m_queue->Peek()->GetPacket() ));
	if (txTime < TimeTilltheEndofCurrentSlot()) return true;
	return false;
}

Time SlimNetDevice::TimeTilltheEndofCurrentSlot() {
	return (m_SlotDuration * (int64_t)(CurrentSlotNumber()+1) - Simulator::Now());
}

void SlimNetDevice::ScheduleNextSlot(){
	if(!m_txSlotScheduled) {
		if (m_nextSelectedSlot <= CurrentSlotNumber()) SelectNextSlot(CurrentSlotNumber());
		Time txTime = Seconds ( (((double)(m_nextSelectedSlot - CurrentSlotNumber())) * ((double)(SLOTDURATION_IN_MICROSECONDS)/1000000))  );
		Simulator::Schedule (txTime, &SlimNetDevice::ExecuteCurrSlot, this);
		m_txSlotScheduled = true;
	}
}

void SlimNetDevice::ExecuteCurrSlot(void) {
	m_txSlotScheduled = false;
	if (!(m_queue->IsEmpty())){
		Decide2TxorSchedule();
	}
}
} // namespace ns3
