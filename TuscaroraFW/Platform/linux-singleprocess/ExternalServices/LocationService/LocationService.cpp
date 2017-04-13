////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#include "LocationService.h"
#include <PAL/PAL.h>
#include <stdlib.h>

namespace ExternalServices {

  Location2D LocationService::GetLocation() {
    Location2D location;
    location.x = atof(getenv("CURX"));
    location.y = atof(getenv("CURY"));
    return location;
  }

  
}
