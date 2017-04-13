////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef GETENV_H_
#define GETENV_H_

#include <cstdlib>    /* getenv, atof, atoi */

struct Getenv {
  static uint32_t uint32_t_ ( const char *s) {
    double x = atof ( getenv ( s ));
    return static_cast<uint32_t>(x);
  };
  static int int_ ( const char *s) {
    return atoi ( getenv ( s ));
  };
};

#endif
