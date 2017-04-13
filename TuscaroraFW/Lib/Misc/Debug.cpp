////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 




#include <Lib/Misc/Debug.h>

// Debug flags used in Debug_Printf. Check if they have already been
// defined because these can be set from the commandline.

//set this to 1 to turn on all debugging; generally a very bad idea
#define DBG_SET_ALL_ 0

// this flag is always true
#define DBG_TRUE_ 1

// Use this flag to report errors.
#define DBG_ERROR_ 1

// True if running in simulation.
// Affects time display in Debug_Printf: in a simulation, time is
// elapsed seconds, if not, time is system time.
#ifndef DBG_SIMULATION_
#define DBG_SIMULATION_ 1
#endif

// Show the location of the file in the debug message
#ifndef DBG_SHOW_LOCATION_
#define DBG_SHOW_LOCATION_ 0
#endif

#ifndef DBG_FW_REPORT_
#define DBG_FW_REPORT_ 1
#endif


#ifndef DBG_CORE_
#define DBG_CORE_ 0
#endif

#ifndef DBG_CORE_API_
#define DBG_CORE_API_ 0
#endif

#ifndef DBG_CORE_DATAFLOW_
#define DBG_CORE_DATAFLOW_ 0
#endif

#ifndef DBG_CORE_DISCOVERY_
#define DBG_CORE_DISCOVERY_ 0
#endif

#ifndef DBG_CORE_ESTIMATION_
#define DBG_CORE_ESTIMATION_ 0
#endif

#ifndef DBG_CORE_WFCONTROL_
#define DBG_CORE_WFCONTROL_ 0
#endif

#ifndef DBG_MAIN_
#define DBG_MAIN_ 0
#endif

// Debug patterns.
#ifndef DBG_PATTERN_
#define DBG_PATTERN_ 1
#endif

#ifndef DBG_PTN_EVENT_
#define DBG_PTN_EVENT_ 0
#endif

#ifndef DBG_SIGNAL_
#define DBG_SIGNAL_ 0
#endif

#ifndef DBG_TEST_
#define DBG_TEST_ 1
#endif

#ifndef DBG_PIGGYBACK_
#define DBG_PIGGYBACK_ 0
#endif


// Debug waveform
#ifndef DBG_WAVEFORM_
#define DBG_WAVEFORM_ 0
#endif

#ifndef DBG_WF_EVENT_
#define DBG_WF_EVENT_ 0
#endif

#ifndef DBG_SHIM_
#define DBG_SHIM_ 0
#endif

#ifndef DBG_TIMESYNC_
#define DBG_TIMESYNC_ 0
#endif


/// The DBG_* flags are defined here as functions, returning the macro
/// values defined above or on the commandline. When the macro values
/// change, we don't have to recompile anything except this file and
/// relink it to the rest of the code. This is much faster than
/// defining the DBG_* flags directly in Debug.h.

///location should be turn on explicity, setting DBG_SET_ALL_ will not turn this on.
bool DBG_SHOW_LOCATION(){ return DBG_SHOW_LOCATION_ ; }


bool DBG_TRUE(){ return DBG_TRUE_ || DBG_SET_ALL_; }
bool DBG_ERROR(){ return DBG_ERROR_ || DBG_SET_ALL_; }
bool DBG_SIMULATION() { return DBG_SIMULATION_ || DBG_SET_ALL_; }
bool DBG_FW_REPORT(){ return DBG_FW_REPORT_ || DBG_SET_ALL_; }
bool DBG_CORE(){ return DBG_CORE_ || DBG_SET_ALL_; }
bool DBG_CORE_API(){ return DBG_CORE_API_ || DBG_SET_ALL_; }
bool DBG_CORE_DATAFLOW(){ return DBG_CORE_DATAFLOW_ || DBG_SET_ALL_; }
bool DBG_CORE_DISCOVERY(){ return DBG_CORE_DISCOVERY_ || DBG_SET_ALL_; }
bool DBG_CORE_ESTIMATION(){ return DBG_CORE_ESTIMATION_ || DBG_SET_ALL_; }
bool DBG_CORE_WFCONTROL(){ return DBG_CORE_WFCONTROL_ || DBG_SET_ALL_; }
bool DBG_MAIN(){ return DBG_MAIN_ || DBG_SET_ALL_; }
bool DBG_PATTERN(){ return DBG_PATTERN_ || DBG_SET_ALL_; }
bool DBG_PTN_EVENT(){ return DBG_PTN_EVENT_ || DBG_SET_ALL_; }
bool DBG_SIGNAL(){ return DBG_SIGNAL_ || DBG_SET_ALL_; }
bool DBG_TEST(){ return DBG_TEST_ || DBG_SET_ALL_; }
bool DBG_PIGGYBACK(){ return DBG_PIGGYBACK_ || DBG_SET_ALL_; }
bool DBG_WAVEFORM(){ return DBG_WAVEFORM_ || DBG_SET_ALL_; }
bool DBG_WF_EVENT(){ return DBG_WF_EVENT_ || DBG_SET_ALL_; }
bool DBG_SHIM(){ return DBG_SHIM_ || DBG_SET_ALL_; }

bool DBG_TIMESYNC(){ return DBG_TIMESYNC_ || DBG_SET_ALL_; }

namespace Lib {
  bool Debug::showTextOutput = true;
  
  void Debug::PrintSeconds(){
    PrintTime();
  }
  
  void Debug::PrintTime(){
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    printf(" %d-%d-%d %d:%d:%d :: ", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
  }

  void Debug::PrintTimeMilli(){
    struct timeval now;
    gettimeofday(&now, NULL);
    //printf("Elapsed time: %ld milliseconds\n", mtime);
    printf(" %ld:%03ld :: ",now.tv_sec, (now.tv_usec/1000));
    fflush(stdout);
  }
  
  void Debug::PrintTimeMicro(){
    struct timeval now;
    gettimeofday(&now, NULL);
    //printf("Elapsed time: %ld milliseconds\n", mtime);
    printf(" %ld:%06ld :: ", now.tv_sec, (now.tv_usec));
    fflush(stdout);
  }
  
  
  /*This function has been moved to SysTime
   * uint64_t Debug::GetTimeMicro(){
    struct timeval now;
    gettimeofday(&now, NULL);
    return ((now.tv_sec *1000000)+ now.tv_usec);
  }*/
  
  void Debug::TurnOnDebugging(char *mod){
    
  }
}
