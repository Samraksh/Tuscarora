////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/csma-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"
#include "ns3/netanim-module.h"
#include "ns3/basic-energy-source.h"
#include "ns3/simple-device-energy-model.h"
#include "ns3/slim-module.h"
#include "ns3/propagation-module.h"




using namespace ns3;

//This creates a tx vector closely resembling 802.11 DSSS 1Mbps rate tx with long preamble
SlimTxVector CreateSlimTxVector(){
	SlimTxVector txvector, txvector2;
	txvector.SetUniqueName("Tuscarora1MbpsMode");


	SlimMode slimheader, slimpaylod;
	slimheader.SetUniqueName("TuscaroraHeaderMode");
	slimheader.SetBandwidth(22000000);
	slimheader.SetDataRate(MicroSeconds(1)); // Set time to transmit one bit of information
	slimheader.SetCodeRate(0.01);
	slimheader.SetSlimModeError(new DsssDbpskModeError()); //BK: Potentially dangereous. Carefull //ToDO: Think about a safer method if time permits.
	slimheader.SetDuration(MicroSeconds(144));
	txvector.GetListofModes().push_back(slimheader);

	slimpaylod = slimheader;
	slimpaylod.SetDuration(MicroSeconds(1));//This is Irrelevant since the packet payload size will be determined at the time of transmission
	slimheader.SetUniqueName("TuscaroraPayloadMode");
	txvector.GetListofModes().push_back(slimpaylod);

	txvector.SetPayloadModePosition(1);

	txvector2 = txvector;

	return txvector;

}

Ptr<TuscaroraSlimPhy> CreateTuscaroraSlimPhy(
		Ptr<TuscaroraSlimChannel> channel
		, Ptr<MobilityModel> mobilitymodel
		, Ptr<AntennaModel>  antenna
		, uint16_t channelnumber
	){
	Ptr<TuscaroraSlimPhy> phy = CreateObject<TuscaroraSlimPhy> ();

	NS_ASSERT(mobilitymodel);
	phy->SetMobility(mobilitymodel);
	phy->SetAntenna(antenna);

	phy->SetChannel (channel);
	phy->SetChannelNumber(channelnumber);

	phy->GetTxVectorList().push_back(CreateSlimTxVector());

	return phy;
}

NS_LOG_COMPONENT_DEFINE ("SlimWaveformExample");

void EventToSetNumberofSlotsinAFrame (uint16_t node, slotNumber_t ns)
{
  std::cout << "EventToSetNumberofSlotsinAFrame called";
  std::ostringstream oss;
  oss << "/NodeList/"<< node <<"/DeviceList/0/ContentionWindowSize";
  Config::Set (oss.str(), UintegerValue (ns));
}


int main (int argc, char *argv[])
{


	uint32_t nSlim = 20;
	CommandLine cmd;
	cmd.AddValue ("nSlim", "Number of devices", nSlim);

	  //------------------------------------------------------------
	  //-- Create nodes
	  //--------------------------------------------
	cmd.Parse (argc,argv);
	NodeContainer node_cont;
	node_cont.Create (nSlim);

	  //------------------------------------------------------------
	  //-- Setup physical layout and mobility
	  //--------------------------------------------
	MobilityHelper mobility;
	mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
			"MinX", DoubleValue (10.0),
			"MinY", DoubleValue (10.0),
			"DeltaX", DoubleValue (5.0),
			"DeltaY", DoubleValue (2.0),
			"GridWidth", UintegerValue (5),
			"LayoutType", StringValue ("RowFirst"));
	//mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
	//                           "Bounds", RectangleValue (Rectangle (0, 50, 0, 50)));
	mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
	mobility.Install (node_cont);

	  //------------------------------------------------------------
	  //-- Create Channel
	  //--------------------------------------------
	Ptr<TuscaroraSlimChannel> channel = CreateObject<TuscaroraSlimChannel> ();
	Ptr<PropagationLossModel> prop_loss =  CreateObject<FriisPropagationLossModel> ();
	Ptr<PropagationDelayModel>  prop_delay = CreateObject<ConstantSpeedPropagationDelayModel> ();
	channel->SetPropagationLossModel(prop_loss);
	channel->SetPropagationDelayModel (prop_delay);

	  //------------------------------------------------------------
	  //-- Create network stacks
	  //--------------------------------------------

	NetDeviceContainer devices_cont;
	NodeContainer::Iterator it;

	Config::SetDefault ("ns3::SlimNetDevice::TxMode", UintegerValue (0)); //Set SlimNetDevice to use the first tx mode available in its list
	Config::SetDefault ("ns3::SlimNetDevice::ContentionWindowSize", UintegerValue (10)); // This sets the frequency of random transmission slots for the net device.

    for (it = node_cont.Begin (); it != node_cont.End (); ++it) {

    //-- Create Net Device and set address
		Ptr<ns3::Node> node = *it;
		Ptr<SlimNetDevice> device = CreateObject<SlimNetDevice> ();
		device->SetNode(node);
		device->SetAddress(Mac48Address::Allocate ());

		std::stringstream strs;
		strs << "pcap-" << node->GetId() << "-"  << ".tr";
		std::string file = strs.str();
		device->EnablePcapAll(file);

	//-- Create PHY
		Ptr<TuscaroraSlimPhy> phy = CreateTuscaroraSlimPhy(
				  channel
				, node->GetObject<MobilityModel> ()
				, CreateObject<IsotropicAntennaModel> ()
				, 0
			);

		phy->SetDevice (device);
		device->SetPhy(phy);



		Ptr<DropTailQueue> queue;
		Config::SetDefault ("ns3::DropTailQueue::Mode", StringValue ("QUEUE_MODE_PACKETS"));
		Config::SetDefault ("ns3::DropTailQueue::MaxPackets", UintegerValue (100));
		queue = CreateObject<DropTailQueue> ();
		device->SetQueue(queue);
		device->SetIfIndex(1);

		//Attach device to the node
		node->AddDevice(device);
		devices_cont.Add(device);
		device->SetIfIndex(1);
	}


	  //------------------------------------------------------------
	  //-- Internet stacks
	  //--------------------------------------------
	// Install internet stack
	InternetStackHelper stack;
	stack.Install (node_cont);

	// Install Ipv4 addresses

	Ipv4AddressHelper address;
	address.SetBase ("10.1.3.0", "255.255.255.0");
	Ipv4InterfaceContainer ipInterfaces;
	ipInterfaces = address.Assign (devices_cont);
	Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

	  //------------------------------------------------------------
	  //-- Applications
	  //--------------------------------------------
	// Install applications: Attach a server to the first node and attach clients to all nodes
	UdpEchoServerHelper echoServer (9);
	ApplicationContainer serverApps = echoServer.Install (node_cont.Get (1));
	serverApps.Start (Seconds (1.0));
	serverApps.Stop (Seconds (15.0));
	UdpEchoClientHelper echoClient (ipInterfaces.GetAddress (1), 9);
	echoClient.SetAttribute ("MaxPackets", UintegerValue (10));
	echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.)));
	echoClient.SetAttribute ("PacketSize", UintegerValue (1024));
	ApplicationContainer clientApps = echoClient.Install (node_cont);
	clientApps.Start (Seconds (2.0));
	clientApps.Stop (Seconds (15.0));

	  //------------------------------------------------------------
	  //-- Net Anim
	  //--------------------------------------------

	AnimationInterface anim ("wireless-animation.xml"); // Mandatory
	anim.EnablePacketMetadata (); // Optional
	anim.EnableIpv4RouteTracking ("routingtable-wireless.xml", Seconds (0), Seconds (5), Seconds (0.25)); //Optional

	  //------------------------------------------------------------
	  //-- Logging
	  //--------------------------------------------
	LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
	//LogComponentEnable("SlimNetDevice", LOG_LEVEL_INFO);

	  //------------------------------------------------------------
	  //-- Simulation
	  //--------------------------------------------
	for (uint32_t i=0; i<nSlim/2 ; ++i){ //Set mean channel access time of the nodes independently from one another
		Simulator::Schedule (Seconds(i), &EventToSetNumberofSlotsinAFrame,i,2) ;
	}

	Simulator::Stop (Seconds (15.0));
	Simulator::Run ();
	Simulator::Destroy ();
	return 0;
}


