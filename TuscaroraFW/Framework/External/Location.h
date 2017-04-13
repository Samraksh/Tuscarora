////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef LOCATION_H_
#define LOCATION_H_

#ifdef PLATFORM_LINUX
#include "Platform/linux/ExternalServices/LocationService/LocationService.h"
#else
#include "Platform/dce/ExternalServices/LocationService/LocationService.h"
#endif

using namespace ExternalServices;

#define x() LocationService::GetLocation().x
#define y() LocationService::GetLocation().y


#endif //LOCATION_H_