////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

// This program is used to calculate the Packet reception rate with distance for various channel models
// Currently it used the YansWifi and the Friis propagation model
// This example configures a single sender (node 0) and x-1 receivers on the same Same Channel
// The sender sends broadcast packets periodically and the receivers count the received packets

// The orientation is:

//     source  ---------> n1 -->n2 ...... --------- n_(x-1)
//       receiver       
//
// ./runOrDebug --pure-ns3 receiver-range --size 101
//
// When you are done, you will notice a pcap trace file in your directory.

#include "standard-includes.h"


NS_LOG_COMPONENT_DEFINE ("WifiSimpleInterference");

using namespace ns3;

#define MAX_NODES 300
uint16_t recievedPkts[MAX_NODES];

//static inline std::string PrintReceivedPacket (Ptr<Socket> socket)
static inline void PrintReceivedPacket (Ptr<Socket> socket)
{
  Address addr;
  uint32_t node=socket->GetNode()->GetId();
  socket->GetSockName (addr);
  InetSocketAddress iaddr = InetSocketAddress::ConvertFrom (addr);

  std::ostringstream oss;
  //oss << "Received one packet! At Node "<< node << "On Socket: " << iaddr.GetIpv4 () << " port: " << iaddr.GetPort ();

  recievedPkts[node]++;
  //return oss.str ();
}

static void ReceivePacket (Ptr<Socket> socket)
{
  PrintReceivedPacket (socket);
  //NS_LOG_UNCOND (PrintReceivedPacket (socket));
}

static void GenerateTraffic (Ptr<Socket> socket, uint32_t pktSize, 
                             uint32_t pktCount, Time pktInterval )
{
 NS_LOG_UNCOND ("Scheduling pkt: " << pktCount); 
  if (pktCount > 0)
    {
      socket->Send (Create<Packet> (pktSize));
      Simulator::Schedule (pktInterval, &GenerateTraffic, 
                           socket, pktSize,pktCount-1, pktInterval);
    }
  else
    {
      socket->Close ();
    }
}


int main (int argc, char *argv[])
{
//  LogComponentEnable ("InterferenceHelper", LOG_LEVEL_ALL);
  SimulationOpts cmd;

  //std::string phyMode ("DsssRate11Mbps");
  double Prss = -80;  // -dBm
  double Irss = -95;  // -dBm
  double delta = 0;  // microseconds
  uint32_t PpacketSize = 100; // bytes
  uint32_t IpacketSize = 100; // bytes
  bool verbose = false; // note that there is already a "Verbose" option
  
  
  // these are not command line arguments for this version
  uint32_t numPackets = 131;
  double interval = 1; // seconds
  double startTime = 10.0; // seconds
  double firstDistance = 500.0; // meters
  double nextDistance = 5.0; // meters
  double offset = 91;  // This is a magic number used to set the 
                       // transmit power, based on other configuration
  int numNodes = 50;
  //CommandLine cmd;

  //cmd.AddValue ("phyMode", "Wifi Phy mode", phyMode);
  
  cmd._opts
    .addValue ("Prss",
	       "Intended primary received signal strength (dBm)",
	       &Prss)
    .addValue ("Irss",
	       "Intended interfering received signal strength (dBm)",
	       &Irss)
    .addValue ("delta",
	       "time offset (microseconds) for interfering signal",
	       &delta)
    .addValue ("PpacketSize",
	       "size of application packet sent",
	       &PpacketSize)
    .addValue ("IpacketSize",
	       "size of interfering packet sent",
	       &IpacketSize)
    .addValue ("verbose",
	       "turn on all WifiNetDevice log components",
	       &verbose);
  
  cmd._parse ( argc-1, argv+1 );
  
  string DSS_RATE;
  switch(cmd.dataRate){
    default:
      DSS_RATE = "DsssRate11Mbps";
  }
  
  
  // Convert to time object
  Time interPacketInterval = Seconds ( interval );

  // disable fragmentation for frames below 2200 bytes
  Config::SetDefault (
    "ns3::WifiRemoteStationManager::FragmentationThreshold",
    StringValue ("2200"));
  // turn off RTS/CTS for frames below 2200 bytes
  Config::SetDefault (
    "ns3::WifiRemoteStationManager::RtsCtsThreshold",
    StringValue ("2200"));
  // Fix non-unicast data rate to be the same as that of unicast
  Config::SetDefault (
    "ns3::WifiRemoteStationManager::NonUnicastMode",
    StringValue ( DSS_RATE ));

  cmd.nodes = numNodes;
  NodeContainer nodes;
  nodes.Create (cmd.nodes);
    
  WifiHelper wifi = WifiHelper::Default();
  wifi.SetStandard(WIFI_PHY_STANDARD_80211b);

  if(verbose){
    wifi.EnableLogComponents ();
  }
    
  YansWifiPhyHelper phy = YansWifiPhyHelper::Default();
  phy.SetPcapDataLinkType(YansWifiPhyHelper::DLT_IEEE802_11_RADIO);

  YansWifiChannelHelper wifiChannel;
  wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
  //wifiChannel.AddPropagationLoss("ns3::RangePropagationLossModel", "MaxRange", DoubleValue(range));
  wifiChannel.AddPropagationLoss("ns3::FriisPropagationLossModel");
  phy.SetChannel(wifiChannel.Create());

  NqosWifiMacHelper mac = NqosWifiMacHelper::Default();
  wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager", "ControlMode", StringValue ( DSS_RATE ), "DataMode", StringValue ( DSS_RATE ));
  mac.SetType("ns3::AdhocWifiMac");

  NetDeviceContainer devices = wifi.Install(phy, mac, nodes);

  // Note that with FixedRssLossModel, the positions below are not 
  // used for received signal strength. 
  MobilityHelper mobility;
  Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
  positionAlloc->Add (Vector (0.0, 0.0, 0.0));
  
  for(int i=1; i< cmd.nodes ; i++){
    positionAlloc->Add (Vector (firstDistance+i*nextDistance, 0.0, 0.0));
  }
  //positionAlloc->Add (Vector (distanceToRx, 0.0, 0.0));
  //positionAlloc->Add (Vector (-1*distanceToRx, 0.0, 0.0));
  mobility.SetPositionAllocator (positionAlloc);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (nodes);

  InternetStackHelper internet;
  internet.Install (nodes);

  Ipv4AddressHelper ipv4;
  NS_LOG_INFO ("Assign IP Addresses.");
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer ipv4Container = ipv4.Assign (devices);

  //Set up sender
  int sender=0;
  TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
  
  Ptr<Socket> source = Socket::CreateSocket (nodes.Get (sender), tid);
  InetSocketAddress remote = InetSocketAddress (Ipv4Address ("255.255.255.255"), 80);
  source->SetAllowBroadcast (true);
  source->Connect (remote);
  
  
  for(int i=1; i< cmd.nodes ; i++){
    Ptr<Socket> recvSink = Socket::CreateSocket (nodes.Get (i), tid);
    Ptr<Ipv4> ipv4 = nodes.Get(i)->GetObject<Ipv4> (); // Get Ipv4 instance of the node
    Ipv4Address addr = ipv4->GetAddress (1, 0).GetLocal ();
    InetSocketAddress local = InetSocketAddress (addr, 80);
    recvSink->Bind (local);
    recvSink->SetRecvCallback (MakeCallback (&ReceivePacket));
  }
  
/*  
  // Interferer will send to a different port; we will not see a
  // "Received packet" message
  Ptr<Socket> interferer = Socket::CreateSocket (nodes.Get (2), tid);
  InetSocketAddress interferingAddr = InetSocketAddress (Ipv4Address ("255.255.255.255"), 49000);
  interferer->SetAllowBroadcast (true);
  interferer->Connect (interferingAddr);
*/

  // Tracing
  phy.EnablePcap ("receiver-range", devices.Get (sender));

  // Output what we are doing
  //NS_LOG_UNCOND ("Primary packet RSS=" << Prss << " dBm and interferer RSS=" << Irss << " dBm at time offset=" << delta << " ms");
  NS_LOG_UNCOND ("Numboer of Nodes=" << cmd.nodes << "Total packets=" << numPackets);
  
  Simulator::ScheduleWithContext (source->GetNode ()->GetId (),
                                  Seconds (startTime), &GenerateTraffic, 
                                  source, PpacketSize, numPackets, interPacketInterval);

  //Simulator::ScheduleWithContext (interferer->GetNode ()->GetId (),
  //                                Seconds (startTime + delta/1000000.0), &GenerateTraffic, 
      //                               interferer, IpacketSize, numPackets, interPacketInterval);

  Simulator::Run ();
  Simulator::Destroy ();
  
  NS_LOG_UNCOND ("|Node|Distance|Pkts Received|Total pkts|PRR(%)|");
  //NS_LOG_UNCOND ("Nodes " << i << "Received "<< recievedPkts[i] <<"/" << numPackets << ". PRR (%): " << (float) recievedPkts[i]*100/numPackets);
  for(int i=1; i< cmd.nodes ; i++){
    uint32_t distance=firstDistance+i*nextDistance;
    NS_LOG_UNCOND (""<< i << "|"<< distance<< "|"<< recievedPkts[i] <<"|" << numPackets << "|" << (float) (recievedPkts[i]*100/numPackets) <<"|");
  }
  
  return 0;
}
