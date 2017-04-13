////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

//#include "FWPFasade.h"
#include "Patterns/FloodingWithPruning/FWP.h"

namespace AWI {
 
Fwp_I& GetFwpShim(){
  Fwp_I *fi = &GetFwpInterface();
  return *fi;
}




}
