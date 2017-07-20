////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef _LOGPACKET_H_
#define _LOGPACKET_H_
// for stat
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstdio> // fopen, fprintf, fclose, 
#include <string>
using std::string;

#include <sstream>
using std::stringstream;

// #include <sys/time.h> // timeval

class PacketLogger {
 public:
  void LogSend ( unsigned packetSize ) { this->Log ( false, packetSize ); };
  void LogRecv ( unsigned packetSize ) { this->Log ( true, packetSize ); };
  void LogReceive ( unsigned packetSize ) { this->LogRecv( packetSize ); };
  PacketLogger() {
    this->time0 = this->now(); // save the starting time, used to compute elapsed time.
  };

  void Log ( bool received, unsigned packetSize ) {
      stringstream ss;
      ss << this->seconds() << " "
	 << ( received? "1 ": "0 " )
	 << packetSize;
      this->LogString ( "packets.log", "seconds received bytes", ss.str() );
  };

 private:
  double time0; // starting time in seconds
  double now() {
    // current time in seconds
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec + t.tv_usec / 1000000.0;
  };
  double seconds() {
    // seconds since instantiation
    return this->now() - this->time0;
  };
  void LogString ( string filename, string header, string line ) {
    // Open filename, print the header if necessary, and print the line
    struct stat s;
    int err = stat ( filename.c_str(), &s );
    bool no_header_yet = err || s.st_size == 0;
    FILE *file = fopen ( filename.c_str(), "a" );
    if ( file == NULL ) {
      printf ( "Error opening text file %s\n", filename.c_str() );
      exit (1);
    }
    if ( no_header_yet ) {
      fprintf ( file, "%s\n", header.c_str() );
    }
    fprintf ( file, "%s\n", line.c_str() );
    fclose ( file );
  };
}; 
#endif //  _LOGPACKET_H_
