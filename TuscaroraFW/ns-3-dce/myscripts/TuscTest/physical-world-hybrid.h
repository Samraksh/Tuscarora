////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include "ns3/netanim-module.h"
#include "ns3/slim-module.h"
#include "ns3/wifi-module.h"
//#include "ns3/wifi-module.h"
#include "ns3/propagation-delay-model.h"
#include "ns3/propagation-loss-model.h"
#include "ns3/isotropic-antenna-model.h"

#include "tuscarora-slim-creator.h"

enum WF_TYPES {
SLIM_FULL,
WIFI_FULL,
WIFI_NOEST,
SLIM_NOEST
};

#define MAX_WAVEFORMS 16
//#define DEFAULT_WF SLIM_NOEST
#define DEFAULT_WF WIFI_NOEST

struct physicalWorld_hybrid_t {
	NodeContainer nodes;
	SimulationOpts *args;
	std::string queueType; 
	double dataRate; 
	std::string DSS_RATE;
	bool trace;

	physicalWorld_hybrid_t ( SimulationOpts *opts ) : args(opts) {
		queueType = "DropTail";
		dataRate = 11000000.0;
		this->init ();
	};
  
	bool init () 
	{
		//Initialze variables
		trace=true; 
		switch(args->dataRate){
			case 1000000:
				Config::SetDefault ("ns3::SlimNetDevice::TxVectorIndex", UintegerValue (0));
				Config::SetDefault ("ns3::SlimMode::ErrorModPtr", PointerValue (Create<DsssDbpskModeError>()));
				Config::SetDefault ("ns3::SlimMode::BitTime", TimeValue (MicroSeconds (1)));
				DSS_RATE = "DsssRate1Mbps";
				break;
			case 2000000:
//				Config::SetDefault ("ns3::SlimNetDevice::TxMode", UintegerValue (1));
				NS_ASSERT_MSG(0,"The selected data rate is not defined for SLIM");
				DSS_RATE = "DsssRate2Mbps";
				break;
			case 5500000:
//				Config::SetDefault ("ns3::SlimNetDevice::TxMode", UintegerValue (2));
				NS_ASSERT_MSG(0,"The selected data rate is not defined for SLIM");
				DSS_RATE = "DsssRate5_5Mbps";
				break;
			case 11000000:
//				Config::SetDefault ("ns3::SlimNetDevice::TxMode", UintegerValue (3));
				Config::SetDefault ("ns3::SlimMode::ErrorModPtr", PointerValue (Create<DsssDqpskCCk11ModeError>()));
				Config::SetDefault ("ns3::SlimMode::BitTime", TimeValue (PicoSeconds (90909)));
				DSS_RATE = "DsssRate11Mbps";
				break;
			default:
				NS_ASSERT_MSG(0,"The selected data rate is not defined for SLIM");
				Config::SetDefault ("ns3::SlimNetDevice::TxVectorIndex", UintegerValue (0));
				DSS_RATE = "DsssRate1Mbps";
		}
		dataRate = args->dataRate;
		// see http://www.nsnam.org/docs/manual/html/random-variables.html#seeding-and-independent-replications

		RngSeedManager::SetSeed ( args->rngSeed );
		RngSeedManager::SetRun ( args->rngRun );

		if ( args->discoveryType == "longlink2hop" ){
		nodes.Create (args->nodes + 1);
		}else {
		nodes.Create (args->nodes);
		}

		PacketSocketHelper packetHelper;
		packetHelper.Install(nodes);
		Config::SetDefault ("ns3::SlimNetDevice::SlotDuration", TimeValue (MicroSeconds (13000)));
		//Config::SetDefault ("ns3::TuscaroraSlimPhy::CenterFrequency", DoubleValue (2407));

		Config::SetDefault ("ns3::WifiRemoteStationManager::MaxSlrc",StringValue("0"));
		Config::SetDefault (
		"ns3::WifiRemoteStationManager::RtsCtsThreshold",
		StringValue("2200" ));
		Config::SetDefault (
		"ns3::WifiRemoteStationManager::NonUnicastMode",
		StringValue(DSS_RATE ));
		
		Config::SetDefault (
		"ns3::WifiRemoteStationManager::RtsCtsThreshold",
		StringValue("2200" ));
		Config::SetDefault (
		"ns3::WifiRemoteStationManager::NonUnicastMode",
		StringValue(DSS_RATE));  
	
		
		//create the default waveform configuration if none was provided
		if(args->waveformConfig == "") {
			remove("defaultconfig.cnf");
			std::ofstream  dst("defaultconfig.cnf",   std::ios::binary);
			
			//Format of the waveform config file
			//dst << "#WFID WaveformType NetDeviceType EstimationType Cost Energy NodeRange<< "\n";

			WF_TYPES dft_wf=DEFAULT_WF;
			
			switch (dft_wf) {
				case WIFI_FULL:
					dst << "2 WF_AlwaysOn_NoAck WifiNetDevice WF_FULL_EST 2 2 0-" << (args->nodes - 1) << "\n";
					break;
				
				case SLIM_FULL:
					dst << "2 WF_AlwaysOn_NoAck SlimNetDevice WF_FULL_EST 2 2 0-" << (args->nodes - 1) << "\n";
					break;
				
				case WIFI_NOEST:
					dst << "2 WF_AlwaysOn_NoAck WifiNetDevice WF_NO_EST 2 2 0-" << (args->nodes - 1) << "\n";
					break;
				
				case SLIM_NOEST:
				default:
						dst << "2 WF_AlwaysOn_NoAck SlimNetDevice WF_NO_EST 2 2 0-" << (args->nodes - 1) << "\n";
						break;
			}
			args->waveformConfig = "defaultconfig.cnf";
		}
		
		//install the waveform configuration on all node's filesystems
		for(int x = 0; x < args->nodes + ((args->discoveryType == "longlink2hop") ? 1 : 0); x++) {
			char dirname[25];
			sprintf(dirname, "files-%d", x);
			DIR* dir = opendir(dirname);
			if(dir == 0) {
				mkdir(dirname, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
			} else {
				closedir(dir);
			}

			sprintf(dirname, "files-%d/waveform.cnf", x);
			remove(dirname);
			std::ifstream  src(args->waveformConfig.c_str(), std::ios::binary);
			std::ofstream  dst(dirname,   std::ios::binary);

			dst << src.rdbuf();      
		}
		setMobility();
		/*************Create and initialize the 1st Local waveform radio/phy/mac *********/
		for(int x = 0; x < MAX_WAVEFORMS; x++)
		{
			// Create container to hold devices
			NetDeviceContainer devices;
			char wfType[32];
			GetWaveformType(x, wfType);
			if(!strcmp (wfType, "SlimNetDevice")) {
				CreateSlimRadio(x, &devices);
			}else if(!strcmp (wfType, "WifiNetDevice")) {
				CreateWifiRadio(x);
			}else if(!strcmp (wfType,"IdNotFound")) {
				CreateSlimRadio(x, &devices);
			}
			else {
				std::cout << "PhysicalWorldHybrid:: Waveform " << x << " has an invalid type";
				exit (1);
			}
		}
	}; // init done

/*	bool waveformIs(int wfid, const char* waveformType) {
		std::ifstream file;
		file.open (args->waveformConfig.c_str());
		if (!file.is_open()) {
			std::cout << "Cannot find waveform config file";
			exit(1);
			return false;
		}
		int id;
		while (file >> id) {
			string type, itype, estType, cost, energy, range;
			file >> type >> itype >> estType >> cost >> energy>> range;
			if(id == wfid) {
				return strcmp(waveformType, itype.c_str()) == 0;
			}
		}

		return strcmp(waveformType, "empty") == 0;
	};
*/

	bool GetWaveformType(int wfid, char* waveformType) {
		std::ifstream file;
		strcpy(waveformType, "IdNotFound");
		file.open (args->waveformConfig.c_str());
		if (!file.is_open()) {
			std::cout << "Cannot find waveform config file";
			exit(1);
			return false;
		}
		int id;
		while (file >> id) {
			string type, itype, estType, cost, energy, range;
			file >> type >> itype >> estType >> cost >> energy>> range;
			if(id == wfid) {
				strcpy(waveformType,itype.c_str());
				return true;
			}
		}
		return false;
	};


	void setMobility () {
		//args->verbose = 1;
		MobilityHelper mobility;
		int gridWidth = static_cast<int>(sqrt ( args->nodes * args->range * args->range * M_PI / args->density));
		std::string gridLayout = "0|" + stringify(gridWidth) + "|0|" + stringify(gridWidth);
		//if(args->verbose) printf("Grid layout: %s\n", gridLayout.c_str());
		printf("Nodes: %d, Range: %f, Density: %f, Grid layout: %s\n", args->nodes, args->range, args->density, gridLayout.c_str());
		// compare a string to a char*; the compiler will cast the char* to a string.
		if ( args->mobilityModelInt() == TracefileMobilityModelInt ) {
			std::string traceFile = args->tracefile;
			Ns2MobilityHelper ns2mobility = Ns2MobilityHelper(traceFile);
			ns2mobility.Install();
			return;
		}
		if ( args->mobilityModelInt() == ConstantPositionMobilityModelInt ) {
			//ObjectFactory pos;
			//            mobilityModelInt = 4;
			Ptr<ListPositionAllocator> positionAlloc =
				CreateObject <ListPositionAllocator>();
			for ( int i=0; i<args->nodes; i++ ){
				positionAlloc ->Add(Vector(0, args->range*i, 0));
			}
			mobility.SetPositionAllocator(positionAlloc);
			mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
		}
		else {
			mobility.SetPositionAllocator(
				"ns3::RandomRectanglePositionAllocator",
				"X", StringValue("ns3::UniformRandomVariable[Min=0.0|Max="
					+ stringify(gridWidth) + "]"),
				"Y", StringValue("ns3::UniformRandomVariable[Min=0.0|Max="
					+ stringify(gridWidth) + "]"));

		if ( args->speedMin != 0 && args->speedMax != 0 ) {
			if ( args->mobilityModelInt() == RandomWalk2dMobilityModelInt ) {
				mobility.SetMobilityModel (
					"ns3::RandomWalk2dMobilityModel",
					"Speed", StringValue("ns3::UniformRandomVariable[Min="
								+ stringify(args->speedMin)
								+ "|Max=" + stringify(args->speedMax) + "]"),
					"Mode", StringValue("Distance"),
					"Distance", StringValue(stringify(gridWidth)),
					"Bounds", StringValue(gridLayout));
			}
			else if ( args->mobilityModelInt() == GaussMarkovMobilityModelInt ) {
				mobility.SetMobilityModel (
					"ns3::GaussMarkovMobilityModel",
					"MeanVelocity", StringValue(
					"ns3::UniformRandomVariable[Min="
					+ stringify(args->speedMin)
					+ "|Max=" + stringify(args->speedMax) + "]"),
					"Bounds", BoxValue(Box (0, gridWidth, 0, gridWidth, 0, 0))
								);
			}
			else if ( args->mobilityModelInt() == RandomWaypointMobilityModelInt) {
				ObjectFactory pos;
				//		  mobilityModelInt = 3;
				pos.SetTypeId ("ns3::RandomRectanglePositionAllocator");
				pos.Set ("X", StringValue (
						"ns3::UniformRandomVariable[Min=0.0|Max="+stringify(gridWidth)+"]"));
				pos.Set ("Y", StringValue (
						"ns3::UniformRandomVariable[Min=0.0|Max="+stringify(gridWidth)+"]"));

				Ptr<PositionAllocator> taPositionAlloc = pos.Create ()->GetObject<PositionAllocator> ();
				mobility.SetMobilityModel (
					"ns3::RandomWaypointMobilityModel",
					"Speed", StringValue("ns3::UniformRandomVariable[Min=" + stringify(args->speedMin) + "|Max=" + stringify(args->speedMax) + "]"),
					"PositionAllocator", PointerValue(taPositionAlloc)
								);
			}
		}
		}
		mobility.Install (nodes);
	}; // setMobility done

	void setDce () {
		this->setDce ("");
	};
  
	void CreateWifiRadio (int index){
		WifiHelper wifi_wf = WifiHelper::Default();
		wifi_wf.SetStandard(WIFI_PHY_STANDARD_80211b);

		YansWifiPhyHelper phy_wf = YansWifiPhyHelper::Default();
		phy_wf.SetPcapDataLinkType(YansWifiPhyHelper::DLT_IEEE802_11_RADIO);

		YansWifiChannelHelper yansChannel_wf;
		yansChannel_wf.SetPropagationDelay (
							"ns3::ConstantSpeedPropagationDelayModel" );
		if ( 0 == args->interferenceModel.compare( "RangePropagationLossModel") || index == 1)
			yansChannel_wf.AddPropagationLoss (
							"ns3::RangePropagationLossModel", 
							"MaxRange", DoubleValue ( index == 1 ? 10000000 : args->range ));
		else if(args->interferenceModel.compare( "FriisPropagationLossModel" ) == 0)
			yansChannel_wf.AddPropagationLoss("ns3::FriisPropagationLossModel");
		else {
			printf("Define a loss model\n");
			exit;
		}

		Ptr<YansWifiChannel> channel_wf = yansChannel_wf.Create();
		phy_wf.SetChannel(channel_wf);

		NqosWifiMacHelper mac_wf = NqosWifiMacHelper::Default();
		wifi_wf.SetRemoteStationManager("ns3::ConstantRateWifiManager", 
							"ControlMode", StringValue(DSS_RATE), 
							"DataMode", StringValue(DSS_RATE));
		mac_wf.SetType ( "ns3::AdhocWifiMac" );
		NetDeviceContainer devices_wf = wifi_wf.Install ( phy_wf, mac_wf, nodes );

		for ( NetDeviceContainer::Iterator i = devices_wf.Begin();
			i != devices_wf.End(); i++ ) 
		{
			Ptr<WifiNetDevice> device = DynamicCast<WifiNetDevice>(*i);
			device->SetIfIndex(index);
			device->GetPhy()->SetChannelNumber(index);
		}

		if ( args->verbose ) {
			for ( int i = 0; i < args->nodes; i++ ){
				int index_wf=devices_wf.Get (i)->GetIfIndex ();

				printf("Node: %d, WF%d Index: %d", i, index, index_wf);
				std::cout << "WF" << index << " MAC: "<< devices_wf.Get (i)->GetAddress() <<"\n";
			}
		}

		bool trace=false;
		if(trace){
			phy_wf.EnableAsciiAll("wifi.tr");
		}  

	}

	void CreateSlimRadio (int index, NetDeviceContainer *net_device_cont){

		//YansWifiPhyHelper phy_helper = YansWifiPhyHelper::Default();
		//phy_helper.SetPcapDataLinkType(YansWifiPhyHelper::DLT_IEEE802_11_RADIO);
			
		/*ObjectFactory prop_factory;
		if ( 0 == args->interferenceModel.compare( RangePropagationLossModel) || index == 1) {
			prop_factory.SetTypeId ("ns3::RangePropagationLossModel");
			prop_factory.Set ("MaxRange", DoubleValue ( index == 1 ? 10000000 : args->range ));
			Ptr<PropagationLossModel> prop_loss =  prop_factory.Create<PropagationLossModel> ();
		}
		else if(args->interferenceModel.compare( FriisPropagationLossModel ) == 0) {
			prop_factory.SetTypeId ("ns3::FriisPropagationLossModel");
		}
		else {
			printf("Define a loss model\n");
			exit;
		}*/

		//printf("CreateSlimRadio: Creating Slim radios for the simulation scenario for device  Index: %d \n",index);	
		
		Ptr<TuscaroraSlimChannel> channel = CreateObject<TuscaroraSlimChannel> ();
		Ptr<PropagationLossModel> prop_loss =  CreateObject<FriisPropagationLossModel> ();
		Ptr<PropagationDelayModel>  prop_delay = CreateObject<ConstantSpeedPropagationDelayModel> ();
		channel->SetPropagationLossModel(prop_loss);
		channel->SetPropagationDelayModel (prop_delay);

		NetDeviceContainer devices_cont;
		NodeContainer::Iterator it;
		NodeContainer const & n = NodeContainer::GetGlobal ();

		//Create the desired tx vectors
		for (it = n.Begin (); it != n.End (); ++it) {
			
			Ptr<ns3::Node> node = *it; 
			
			Config::SetDefault ("ns3::SlimMode::ErrorModPtr", PointerValue (Create<DsssDqpskCCk11ModeError>()));
			Config::SetDefault ("ns3::SlimMode::BitTime", TimeValue (PicoSeconds (90909)));
			
			Ptr<SlimNetDevice> device = CreateObject<SlimNetDevice> ();
			device->SetNode(node);
			device->SetAddress(Mac48Address::Allocate ());

			std::stringstream strs;
			strs << "pcap-" << node->GetId() << "-"  << ".tr";
			std::string file = strs.str();
			//device->EnablePcapAll(file);


	//		Ptr<TuscaroraSlimPhy> phy = CreateObject<TuscaroraSlimPhy> ();

			Ptr<MobilityModel> mobilitymodel = node->GetObject<MobilityModel> ();
			Ptr<AntennaModel>  antenna = CreateObject<IsotropicAntennaModel> ();


			Ptr<TuscaroraSlimPhy> phy = CreateTuscaroraSlimPhy(
					channel
					, mobilitymodel
					, antenna
					, index
				);

			device->SetPhy(phy);
			phy->SetDevice (device);
			
			Ptr<DropTailQueue> queue;
			Config::SetDefault ("ns3::DropTailQueue::Mode", StringValue ("QUEUE_MODE_PACKETS"));
			Config::SetDefault ("ns3::DropTailQueue::MaxPackets", UintegerValue (100));
			queue = CreateObject<DropTailQueue> ();
			device->SetQueue(queue);
			
			//Attach device to the node
			node->AddDevice(device);
			devices_cont.Add(device);
			device->SetIfIndex(index);

			if(trace){
				//phy_helper.EnableAsciiAll("slim-wf.tr");
				//EnableAsciiImpl (Ptr<OutputStreamWrapper> (), "slim-wf.tr", NodeContainer::GetGlobal ());
			}
		}

		if ( args->verbose ) {
			for ( int i = 0; i < args->nodes; i++ ){
				int index_wf=devices_cont.Get (i)->GetIfIndex ();
				printf("Node: %d, WF: %d, Index: %d", i, index, index_wf);
				std::cout << "WF" << index << " MAC: "<< devices_cont.Get (i)->GetAddress() <<"\n";
			}
		}
	}

	void setDce ( string extraArgs ) {
	if ( args->pattern.size() == 0 ) {
		printf ( "No DCE pattern specified. Not setting DCE.\n" );
		return;
	}
	//======Done Setting up NS3 network and Wifi Device, Setup up DCE=====
	DceManagerHelper dceManager;
	dceManager.Install (nodes);

	DceApplicationHelper dce;
	ApplicationContainer apps;

	dce.SetStackSize (1<<20);
	dce.SetBinary (args->pattern);
	dce.ResetArguments ();
	dce.ResetEnvironment ();

	//#include "dce-pre.h"
	std::stringstream ss;
	//   ss << args->_str() << " simdir=" << args->simulationDirectory();
	ss << args->_str(); // add extra arguments here.
	if ( extraArgs.size() ) ss << " " << extraArgs;
	/* printf ( "args = %s\n", ss.str().c_str() ); */
	dce.ParseArguments(ss.str());
	apps = dce.Install (nodes);
	apps.Start (Seconds (0.0));
	}; // setDce done

	void anim () {
	AnimationInterface anim ("wireless-animation.xml");
	for (uint32_t i = 0; i < nodes.GetN (); i++) {
		anim.UpdateNodeDescription (nodes.Get (i), stringify(i));
		anim.UpdateNodeColor (nodes.Get (i), 255, 0, 0);
	}
	anim.EnablePacketMetadata ();
	};

}; // end class physicalWorld_t args
