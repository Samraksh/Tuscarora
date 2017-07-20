////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include <Lib/Misc/Debug.h>

#include <stdio.h>
#include <iostream>
#include <time.h>
#include <sys/time.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

#include <inttypes.h>
#include <Base/BasicTypes.h>

// Debug flags used in Debug_Printf. Check if they have already been
// defined because these can be set from the commandline.

//set this to 1 to turn on all debugging; generally a very bad idea


/// The DBG_* flags are defined here as functions, returning the macro
/// values defined above or on the commandline. When the macro values
/// change, we don't have to recompile anything except this file and
/// relink it to the rest of the code. This is much faster than
/// defining the DBG_* flags directly in Debug.h.

///location should be turn on explicity, setting DBG_SET_ALL_ will not turn this on.


namespace Lib {

bool DBG_SET_ALL_;

bool DBG_SHOW_LOCATION_ =0;
bool DBG_TRUE_ =0;
bool DBG_ERROR_=0;
bool DBG_SIMULATION_=0;
bool DBG_FW_REPORT_=1;
bool DBG_CORE_=0;
bool DBG_CORE_API_=0;
bool DBG_CORE_DATAFLOW_=0;
bool DBG_CORE_DISCOVERY_=0;
bool DBG_CORE_ESTIMATION_=0;
bool DBG_CORE_WFCONTROL_=0;
bool DBG_MAIN_=1;
bool DBG_PATTERN_=1;
bool DBG_PTN_EVENT_=0;
bool DBG_SIGNAL_=0;
bool DBG_TEST_=1;
bool DBG_PIGGYBACK_=0;
bool DBG_WAVEFORM_=0;
bool DBG_WF_EVENT_=0;
bool DBG_SHIM_=0;
bool DBG_TIMESYNC_=0;


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


typedef bool (ModFunc)();




bool IsModTurnedOn(bool mod){
	return (mod || DBG_SET_ALL_) && Debug::ShowTextOutput;
}


bool Debug::ShowTextOutput = true;
  
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



}//End namespace

//This function has been moved to SysTime
/*
 uint64_t Debug::GetTimeMicro(){
	struct timeval now;
	gettimeofday(&now, NULL);
	return ((now.tv_sec *1000000)+ now.tv_usec);
}*/

#define FUNC_printf(args,format) \
va_start(args, format); \
vprintf(format, args); \
va_end(args)

#define FUNC_fprintf(args,format, filehandle) \
  va_start(args, format); \
  vfprintf(filehandle,format, args); \
  va_end(args)


void PRINTF (int x, ... ){
	va_list args;
	va_start(args, x);
	char *format=va_arg(args,char*);
	vprintf(format, args);
	va_end(args);
}


/*void PRINTF_FORMAT(char *format, ...){
	va_list args;
	FUNC_printf(args,format);
}
*/
void Debug_Error(char *format, ...) {
	va_list args;

	Lib::Debug::PrintTimeMicro();
	FUNC_printf(args, format);

	//printf(__VA_ARGS__);
	printf("ERROR: Going to quit in 100 msec");
	fflush(stdout);
	usleep(100000);
	fflush(stdout);
	exit(1);
}


void FileLogPrintf (char *filename, char *header, char *format, ...) {
	va_list args;

	struct stat s;
	int err = stat ( filename, &s );
	bool no_header_yet = err || s.st_size == 0;
	FILE *file = fopen ( filename, "a" );
	if ( file == NULL ) {
		printf ( "Error opening text file %s\n", filename );
		exit (1);
	}
	if ( no_header_yet ) {
		fprintf ( file, "%s\n", header );
	}
	FUNC_fprintf ( args , format, file);
	fclose ( file );
}

void Debug_Warning(char *format, ...) {
	va_list args;
	Lib::Debug::PrintTimeMicro(); printf("DBG_WARNING: ");
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
	fflush(stdout);
}

void PrintWithTime(char *format, ...) {
	va_list args;
	Lib::Debug::PrintTimeMicro();
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
	fflush(stdout);
}

