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
    char *str= getenv("CURX");
    if (str) location.x = atof(str); else location.x=0.0;
    str= getenv("CURY");
    if (str) location.y = atof(str); else location.y=0.0;
    return location;
  }

  
}
