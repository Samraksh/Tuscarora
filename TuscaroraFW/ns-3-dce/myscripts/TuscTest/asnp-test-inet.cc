////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include "ns3/packet-socket-helper.h"
#include "ns3/core-module.h"
#include "ns3/dce-module.h"
#include "ns3/wifi-module.h"
#include "ns3/wifi-helper.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/nqos-wifi-mac-helper.h"
#include "ns3/mobility-helper.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/log.h"

using namespace ns3;

int main (int argc, char *argv[])
{
  int numNodes=2;
  double distance=20;//m
  bool verbose=true;
  //bool verbose=false;

  NodeContainer nodes;
  nodes.Create (numNodes);

  //PacketSocketHelper packetHelper;
  //packetHelper.Install(nodes);

  InternetStackHelper internet;
  internet.Install(nodes);


  Config::SetDefault("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue("2200"));
  Config::SetDefault("ns3::WifiRemoteStationManager::NonUnicastMode", StringValue("DsssRate11Mbps"));
  
  WifiHelper wifi = WifiHelper::Default();
  wifi.SetStandard(WIFI_PHY_STANDARD_80211b);
  //Turn on Wifi Logging
  if(verbose){
    wifi.EnableLogComponents ();
  }
  
  //Create physical layer
  YansWifiPhyHelper phy = YansWifiPhyHelper::Default();
  phy.SetPcapDataLinkType(YansWifiPhyHelper::DLT_IEEE802_11_RADIO);

  //Create the channel;
  YansWifiChannelHelper wifiChannel;
  wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
  wifiChannel.AddPropagationLoss("ns3::RangePropagationLossModel", "MaxRange", DoubleValue(180));
  
  phy.SetChannel(wifiChannel.Create());

  //create the mac
  NqosWifiMacHelper mac = NqosWifiMacHelper::Default();
  wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager", "ControlMode", StringValue("DsssRate11Mbps"), "DataMode", StringValue("DsssRate11Mbps"));
  mac.SetType("ns3::AdhocWifiMac");

  //Set the network device interface
  //wifi.SetAttribute("DeviceName", StringValue ("wlan0"));
  //wifi.SetIfIndex(1);
  NetDeviceContainer devices = wifi.Install(phy, mac, nodes);

  Ipv4AddressHelper address;
  address.SetBase ("10.0.0.0", "255.255.255.0");
  Ipv4InterfaceContainer interfaces = address.Assign (devices);


  for (int i=0; i<numNodes; i++){
  	//devices.Get(i)->SetIfIndex(1);
  	//devices.Get(i)->SetAttribute("DeviceName", StringValue ("wlan0"));
  }
  //Setup Mobility
  MobilityHelper mobility;
  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (distance),
                                 "DeltaY", DoubleValue (distance),
                                 "GridWidth", UintegerValue (5),
                                 "LayoutType", StringValue ("RowFirst"));
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (nodes);


  //==================Done Setting up NS3 network and Wifi Device, Setup up DCE================
  DceManagerHelper dceManager;
  dceManager.Install (nodes);
  
  DceApplicationHelper dce;
  ApplicationContainer apps;

  dce.SetStackSize (1<<20);

  //Launch node 0
  dce.SetBinary ("ASNPTest-dce");
  dce.ResetArguments ();
  dce.ResetEnvironment ();
  apps = dce.Install (nodes.Get (0));
  apps.Start (Seconds (0.1));
  //apps.Stop (Seconds (20.0));
	
  //Launch node 1
  dce.SetBinary ("ASNPTest-dce");
  dce.ResetArguments ();
  dce.ResetEnvironment ();
  apps = dce.Install (nodes.Get (1));
  apps.Start (Seconds (0.2));
  //apps.Stop (Seconds (22.0));

  //Start simulation
  Simulator::Stop (Seconds(30.0));
  Simulator::Run ();
  Simulator::Destroy ();
}
