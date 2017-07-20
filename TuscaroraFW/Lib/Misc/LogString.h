////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef _LOGSTRING_H_
#define _LOGSTRING_H_
// for stat
#include <sys/types.h>

#include <cstdio> // fopen, fprintf, fclose, 
#include <string>
using std::string;

// Open filename, print the header if necessary, and print the line
class logString {
 public:
  logString ( string filename, string header, string line );
};

#endif //  _LOGSTRING_H_
