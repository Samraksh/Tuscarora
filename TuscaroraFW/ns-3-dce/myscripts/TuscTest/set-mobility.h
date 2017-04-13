////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



  MobilityHelper mobility;
  int gridWidth = static_cast<int>(sqrt(numNodes * range * range * M_PI / density));
  std::string gridLayout = "0|" + stringify(gridWidth) + "|0|" + stringify(gridWidth);
  if(verbose) printf("Grid layout: %s\n", gridLayout.c_str());

  if(strcmp(mobilityModel.c_str(), "ConstantPositionMobilityModel") == 0) {
            //ObjectFactory pos;
            mobilityModelInt = 4;
            Ptr<ListPositionAllocator> positionAlloc = CreateObject <ListPositionAllocator>();
            for (int i=0; i<numNodes; i++){
            	positionAlloc ->Add(Vector(0, range*i, 0));
            }
            mobility.SetPositionAllocator(positionAlloc);
            mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  }else {
	  mobility.SetPositionAllocator("ns3::RandomRectanglePositionAllocator",
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
  }
  mobility.Install (nodes);
