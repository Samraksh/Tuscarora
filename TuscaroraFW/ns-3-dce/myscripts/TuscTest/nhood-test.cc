////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

// standard includes
#include "ns3/packet-socket-helper.h"
#include "ns3/core-module.h"
#include "ns3/dce-module.h"
#include "ns3/wifi-module.h"
#include "ns3/wifi-helper.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/nqos-wifi-mac-helper.h"
#include "ns3/mobility-module.h"
//#include "ns3/random-variable.h"
#include "ns3/mobility-helper.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/log.h"
#include <string.h>
#include <sstream>
#include "sys/mman.h"
#include <unistd.h>
#include <fcntl.h>

using namespace ns3;

template < class T >
std::string stringify(const T& t) {
  std::stringstream ss;
  ss << t;
  return ss.str();
}

int main (int argc, char *argv[])
{
  // setup commandline options
  std::string asnp;
  int runTime=21;
  std::string mobilityModel="RandomWalk2dMobilityModel";
  std::string capacity="DsssRate11Mbps";
  int numNodes=10;
  double range = 100;
  int density=10;
  double speedMin = 2.0;
  double speedMax = 4.0;
  int rate = 500000;
  int dead = 1;
  bool verbose=false;
  int discovery = 1;
  int leType = 1;
  int mobilityModelInt = 0;
  
  CommandLine cmd;
  cmd.AddValue("ASNP", "ASNP to run", asnp);
  cmd.AddValue("RunTime", "Total simulation run time", runTime);
  cmd.AddValue("Density", "Average number of neighbors", density);
  cmd.AddValue("Mobility", "Mobility model to use", mobilityModel);
  cmd.AddValue("Capacity", "Network Capacity", capacity);
  cmd.AddValue("LinkEstimationType", "LinkEstimationType", leType);
  cmd.AddValue("Verbose", "Display ns3 data", verbose);
  cmd.AddValue("Nodes", "Number of nodes", numNodes);
  cmd.AddValue("Range", "Radio range", range);
  cmd.AddValue("SpeedMin", "Minimum speed of nodes", speedMin);
  cmd.AddValue("SpeedMax", "Maximum speed of nodes", speedMax);
  cmd.AddValue("DiscoveryType", "Network Discovery Type", discovery);
  cmd.AddValue("LinkEstimationPeriod", "Beaconing rate in microseconds", rate);
  cmd.AddValue("DeadNeighborPeriod", "Number of missed beacons to count a neighbor as dead", dead);
  cmd.Parse(argc, argv);
  
  runTime += 1;//Added to insure simulations run to completion - timers are off by a handful of microseconds sometimes. This lets us get full memory maps
  
  // create physical world
  NodeContainer nodes;
  nodes.Create (numNodes);
  
  PacketSocketHelper packetHelper;
  packetHelper.Install(nodes);
  
  Config::SetDefault("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue("2200"));
  Config::SetDefault("ns3::WifiRemoteStationManager::NonUnicastMode", StringValue(capacity));
  
  WifiHelper wifi = WifiHelper::Default();
  wifi.SetStandard(WIFI_PHY_STANDARD_80211b);
  
  if(verbose){
    wifi.EnableLogComponents ();
  }
  
  YansWifiPhyHelper phy = YansWifiPhyHelper::Default();
  phy.SetPcapDataLinkType(YansWifiPhyHelper::DLT_IEEE802_11_RADIO);
  
  YansWifiChannelHelper wifiChannel;
  wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
  wifiChannel.AddPropagationLoss("ns3::RangePropagationLossModel", "MaxRange", DoubleValue(range));

  phy.SetChannel(wifiChannel.Create());

  NqosWifiMacHelper mac = NqosWifiMacHelper::Default();
  wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager", "ControlMode", StringValue(capacity), "DataMode", StringValue(capacity));
  mac.SetType("ns3::AdhocWifiMac");
  
  NetDeviceContainer devices = wifi.Install(phy, mac, nodes);
  if(verbose) {
    for (int i=0; i<numNodes; i++){
      int index=devices.Get (i)->GetIfIndex ();
      printf("Node: %d, Index: %d, ", i,index);
      std::cout << "MAC: "<< devices.Get (i)->GetAddress() <<"\n";
      //printf("Node: %d, Index: %d\n", i,index);
  }
 }

  // set mobility
  MobilityHelper mobility;
  int gridWidth = static_cast<int>(sqrt(numNodes * range * range * M_PI / density)); 
  std::string gridLayout = "0|" + stringify(gridWidth) + "|0|" + stringify(gridWidth);
  if(verbose) printf("Grid layout: %s\n", gridLayout.c_str());
  mobility.SetPositionAllocator ("ns3::RandomRectanglePositionAllocator",
				 "X", StringValue("ns3::UniformRandomVariable[Min=0.0|Max=" + stringify(gridWidth) + "]"),
				 "Y", StringValue("ns3::UniformRandomVariable[Min=0.0|Max=" + stringify(gridWidth) + "]"));
  
  if(speedMin != 0 && speedMax != 0) {
    if(strcmp(mobilityModel.c_str(), "RandomWalk2dMobilityModel") == 0) {
      mobilityModelInt = 1;
      mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
				 "Speed", StringValue("ns3::UniformRandomVariable[Min=" + stringify(speedMin) + "|Max=" + stringify(speedMax) + "]"),
				 "Mode", StringValue("Distance"),
				 "Distance", StringValue(stringify(gridWidth)),
				 "Bounds", StringValue(gridLayout)
				 );
    } else if(strcmp(mobilityModel.c_str(), "GaussMarkovMobilityModel") == 0) {
      mobilityModelInt = 2;
      mobility.SetMobilityModel ("ns3::GaussMarkovMobilityModel",
				 "MeanVelocity", StringValue("ns3::UniformRandomVariable[Min=" + stringify(speedMin) + "|Max=" + stringify(speedMax) + "]"),
				 "Bounds", BoxValue(Box (0, gridWidth, 0, gridWidth, 0, 0))
				 );
    } else if(strcmp(mobilityModel.c_str(), "RandomWaypointMobilityModel") == 0) {
      ObjectFactory pos;
      mobilityModelInt = 3;
      pos.SetTypeId ("ns3::RandomRectanglePositionAllocator");
      pos.Set ("X", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max="+stringify(gridWidth)+"]"));
      pos.Set ("Y", StringValue ("ns3::UniformRandomVariable[Min=0.0|Max="+stringify(gridWidth)+"]"));
      
      Ptr<PositionAllocator> taPositionAlloc = pos.Create ()->GetObject<PositionAllocator> ();
      mobility.SetMobilityModel ("ns3::RandomWaypointMobilityModel",
				 "Speed", StringValue("ns3::UniformRandomVariable[Min=" + stringify(speedMin) + "|Max=" + stringify(speedMax) + "]"),
				 "PositionAllocator", PointerValue(taPositionAlloc)
				 );
    }
  }
  mobility.Install (nodes);

  // dce
  //==================Done Setting up NS3 network and Wifi Device, Setup up DCE================
  DceManagerHelper dceManager;
  dceManager.Install (nodes);
  
  DceApplicationHelper dce;
  ApplicationContainer apps;

  dce.SetStackSize (1<<20);
  // dce.SetBinary ("ASNPTest-dce");
  dce.SetBinary (asnp);
  dce.ResetArguments ();
  dce.ResetEnvironment ();

  dce.AddArgument(stringify(asnp));
  dce.AddArgument(stringify(rate));//2
  dce.AddArgument(stringify(dead));//3
  dce.AddArgument(stringify(range));//4
  dce.AddArgument(stringify(leType));//5
  dce.AddArgument(stringify(numNodes));//6
  dce.AddArgument(stringify(runTime));//7
  dce.AddArgument(stringify(density));//8
  dce.AddArgument(stringify(mobilityModelInt));//9
  dce.AddArgument(stringify(speedMin));//10
  dce.AddArgument(stringify(speedMax));//11
  
  apps = dce.Install (nodes);
  apps.Start (Seconds (0.0));

  // run simulation
  //Start simulation
  //Simulator::Stop (Seconds(120.0));
  Simulator::Stop (Seconds(runTime));
  Simulator::Run ();
  if(verbose) printf("Simulator done\n");

  Simulator::Destroy ();

}
