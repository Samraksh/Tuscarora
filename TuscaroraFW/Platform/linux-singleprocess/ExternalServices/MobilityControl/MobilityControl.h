////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef MOBILITY_CONTROL_H_
#define MOBILITY_CONTROL_H_


namespace ExternalServices {
  
  struct Location3D {
    double x;
    double y;
    double z;
  };
  
  class MobilityControl {
  public:
    static bool SetLocation(Location3D _loc);
    //static bool SetVelocity(double _vel);
  };

  
  
} //End of namespace

#endif
