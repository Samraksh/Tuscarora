////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#include "MobilityControl.h"
//#include "mobility.h"
#include <cstdlib>
#include <sstream>

namespace ExternalServices {

  ///Set the location of a this node
  bool MobilityControl::SetLocation(Location3D _loc){
    std::stringstream strs;
    strs << "LOCATION=" << _loc.x << "," << _loc.y << "," << _loc.z;
    //char loc[]=1.0,1.0,1.0";
    putenv((char*)(strs.str().c_str()));
    return true;
  }
  
  ///Change the nodes  velocity
  /*bool MobilityControl::SetVelocity(double _vel){
    char vel[]="VELOCITY=1.0";
    putenv(vel);
    return true;
  }*/


} //End of namespace
