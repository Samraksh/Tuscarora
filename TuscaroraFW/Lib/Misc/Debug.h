////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>
#include <iostream>
#include <time.h>
#include <sys/time.h>
#include <Base/BasicTypes.h>

//#include "Lib/Misc/ElapsedTime.h"


/// DBG_* flags are functions defined elsewhere. If the definition of
/// a DBG_* flag changes, we don't need to recompile every file that
/// includes this header file (Debug.h).

bool DBG_SIMULATION(); // returns true if this is running in a simulation.
bool DBG_TRUE();
bool DBG_ERROR();
bool DBG_ERR();
bool DBG_SHOW_LOCATION();
bool DBG_FW_REPORT();
bool DBG_CORE();
bool DBG_CORE_API();
bool DBG_CORE_DATAFLOW();
bool DBG_CORE_DISCOVERY();
bool DBG_CORE_ESTIMATION();
bool DBG_CORE_WFCONTROL();
bool DBG_MAIN();
bool DBG_PATTERN();
bool DBG_PTN_EVENT();
bool DBG_SIGNAL();
bool DBG_TEST();
bool DBG_PIGGYBACK();
bool DBG_WAVEFORM();
bool DBG_WF_EVENT();
bool DBG_SHIM();
bool DBG_TIMESYNC();

namespace Lib{
  struct Debug {
    static bool showTextOutput;
    static void PrintSeconds();
    static void PrintTime();
    static void PrintTimeMilli();
    static void PrintTimeMicro();
    //static uint64_t GetTimeMicro();
    static void TurnOnDebugging(char *mod);
    inline static void SetTextOutput(bool enable) { showTextOutput = enable; }
  }; 
}

#define ASSERT_MSG(condition, message) do { \
		if (!(condition)) { printf((message)); } \
		assert ((condition)); \
	} while(false)

#define Debug_Error(...) {Lib::Debug::PrintTimeMicro(); printf(__VA_ARGS__); printf("ERROR: Going to quit in 100 msec");fflush(stdout);usleep(100000);fflush(stdout); exit(1);}
#define Debug_Warning(...) {Lib::Debug::PrintTimeMicro(); printf("DBG_WARNING: "); printf(__VA_ARGS__); fflush(stdout);}

#define PrintWithTime(...) {Lib::Debug::PrintTimeMicro(); printf(__VA_ARGS__);}

#define Debug_Printf(mod,  ...) if(mod () && Lib::Debug::showTextOutput){ Lib::Debug::PrintTimeMicro(); printf("%s:", #mod); if (DBG_SHOW_LOCATION ()) printf("%s:%d:", __FILE__, __LINE__); printf(" "); printf(__VA_ARGS__); fflush(stdout);}

// Don't print a newline. Continue with Debug_Printf_Cont.
#define Debug_Printf_Start(mod,  ...) if(mod () && Lib::Debug::showTextOutput){if (DBG_SIMULATION()) Lib::Debug::PrintSeconds(); else Lib::Debug::PrintTimeMicro(); printf("%s:", #mod); if (DBG_SHOW_LOCATION ()) printf("%s:%d:", __FILE__, __LINE__); printf(" "); printf(__VA_ARGS__); fflush(stdout);}

// Don't print a newline or a prefix.
#define Debug_Printf_Cont(mod,  ...) if(mod () && Lib::Debug::showTextOutput){printf(__VA_ARGS__); fflush(stdout);}

// Don't print the time.
#define DebugNoTime_Printf(mod,  ...) if(mod () && Lib::Debug::showTextOutput){printf("%s: in file %s line %d: ", #mod, __FILE__, __LINE__);printf(__VA_ARGS__);}

// Don't print the time.
#define DebugNoTimeNoMod_Printf(mod,  ...) if(mod () && Lib::Debug::showTextOutput){printf(__VA_ARGS__);}


#endif //DEBUG_H
