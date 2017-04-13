////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef _COUNTER_H_
#define _COUNTER_H_

#include <cstdio> // printf

struct Counter {
  double total;
  Counter(): total(0.0) {};
  void Reset() { total = 0; };
  double operator() () { return total; };
  double operator() ( double x ) { total += x; return (*this)(); };
};
#endif //  _COUNTER_H_
