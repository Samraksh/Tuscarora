////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef LOCATIONSERVICEI_H_
#define LOCATIONSERVICEI_H_


namespace ExternalServices {
  
  template <class LocationType>
  class LocationServiceI {
  public:
    LocationType GetLocation();
  };


struct GPSLocation {
  double latitude;
  double longitude;
};

struct Location2D {
  double x;
  double y;
};

struct Location3D {
  double x;
  double y;
  double z;
};

}//End of namespace

#endif /* LOCATIONSERVICEI_H_ */
