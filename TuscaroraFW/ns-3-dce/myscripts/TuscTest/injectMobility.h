////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

void InjectMobility(NodeContainer &nodes,int _node,double _x, double _y, double _z){
  Ptr<ns3::Node> node = nodes.Get(_node);

	Ptr<MobilityModel> mobility = node->GetObject<MobilityModel>  ();
	mobility->SetPosition (Vector(_x,_y,_z));
}

