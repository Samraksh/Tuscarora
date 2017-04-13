////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include <PAL/Utils.h>
#include <cstdlib>
#include <string.h>

float GetEnv(const char* name)
{
  
  if(!strcmp("CURX", name) || !strcmp("CURY", name)){
    char *val = getenv(name);
    float ret = atof(val);
    delete[] val;
    return ret;
  }
  return 0;
}
