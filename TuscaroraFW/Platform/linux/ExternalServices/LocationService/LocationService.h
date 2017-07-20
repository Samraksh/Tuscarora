////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef LOCATIONSERVICE_H_
#define LOCATIONSERVICE_H_

#include <Interfaces/ExternalServices/LocationServiceI.h>

namespace ExternalServices {

  class LocationService : public LocationServiceI<Location2D> {
  public:
    static Location2D GetLocation();
  };

} //End of namespace

#endif
