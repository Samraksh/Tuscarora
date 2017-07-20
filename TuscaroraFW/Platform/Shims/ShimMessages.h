////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef SHIM_MESSAGES_H_
#define SHIM_MESSAGES_H_

#include <Base/FrameworkTypes.h>

/*typedef struct {
	WaveformId_t wid0;
	char greet[24];
	WaveformId_t wid1;
}__attribute__((packed, aligned(1))) WaveformGreetings_t;
*/

typedef struct {
	PatternId_t pid0;
	char greet[16];
	PatternId_t pid1;
}__attribute__((packed, aligned(1))) PatternGreetings_t;

#endif //SHIM_MESSAGES_H_