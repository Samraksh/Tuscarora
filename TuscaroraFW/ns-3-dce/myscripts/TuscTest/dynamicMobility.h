////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



  MobilityHelper mobility;
  int gridWidth = static_cast<int>(sqrt(numNodes * range * range * M_PI / density));
  std::string gridLayout = "0|" + stringify(gridWidth) + "|0|" + stringify(gridWidth);
  if(verbose) printf("Grid layout: %s\n", gridLayout.c_str());

  mobilityModelInt = 2;
  mobility.SetMobilityModel ("ns3::GaussMarkovMobilityModel", 
	"MeanVelocity", StringValue("ns3::UniformRandomVariable[Min=" + stringify(speedMin) + "|Max=" + stringify(speedMax) + "]"),
	"MeanDirection", StringValue ("ns3::UniformRandomVariable[Min=0|Max=6.283185307]"),
	"MeanPitch", StringValue ("ns3::UniformRandomVariable[Min=0.05|Max=0.05]"),
	"NormalVelocity", StringValue ("ns3::NormalRandomVariable[Mean=0.0|Variance=0.0|Bound=0.0]"),
	"NormalDirection", StringValue ("ns3::NormalRandomVariable[Mean=0.0|Variance=0.2|Bound=0.4]"),
	"NormalPitch", StringValue ("ns3::NormalRandomVariable[Mean=0.0|Variance=0.02|Bound=0.04]"),
	"Bounds", BoxValue(Box (0, gridWidth, 0, gridWidth, 0, gridWidth)),
	"TimeStep", TimeValue (Seconds (0.5)),
  	"Alpha", DoubleValue (0.85)
	 );


  mobility.SetPositionAllocator ("ns3::RandomBoxPositionAllocator",
  "X", StringValue ("ns3::UniformRandomVariable[Min=0|Max=" +stringify(gridWidth) + "]"),
  "Y", StringValue ("ns3::UniformRandomVariable[Min=0|Max=" +stringify(gridWidth) + "]"),
  "Z", StringValue ("ns3::UniformRandomVariable[Min=0|Max=" +stringify(gridWidth) + "]"));

  mobility.Install (nodes);
