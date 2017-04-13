////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef _LOGSTRING_H_
#define _LOGSTRING_H_
// for stat
#include <sys/types.h>
// #include <sys/stat.h>
// #include <unistd.h>

#include <cstdio> // fopen, fprintf, fclose, 
#include <string>
using std::string;

// Open filename, print the header if necessary, and print the line
class logString {
 public:
  logString ( string filename, string header, string line ) {
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

#define logPrintf(filename, header, args...) {			\
  struct stat s;							\
  int err = stat ( filename, &s );					\
  bool no_header_yet = err || s.st_size == 0;				\
  FILE *file = fopen ( filename, "a" );					\
  if ( file == NULL ) {							\
  printf ( "Error opening text file %s\n", filename );			\
  exit (1);								\
  }									\
  if ( no_header_yet ) {						\
    fprintf ( file, "%s\n", header );					\
  }									\
  fprintf ( file, args );						\
  fclose ( file );							\
  }

#endif //  _LOGSTRING_H_
