////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef PLATFORM_SELECTOR_H_
#define PLATFORM_SELECTOR_H_

/* PlatformSelector.h */
#pragma once
#if PLATFORM_DCE==1
#include "Platform/dce/PlatformConfig.h"
#define PLATFORM Platform/dce
#elif PLATFORM_LINUX==1
#include "Platform/linux/PlatformConfig.h"
#define PLATFORM Platform/linux
#elif PLATFORM_EMOTE==1
#include "Platform/eMote/PlatformConfig.h"
#define PLATFORM Platform/eMote
#endif



#endif /* PLATFORM_SELECTOR_H_ */
