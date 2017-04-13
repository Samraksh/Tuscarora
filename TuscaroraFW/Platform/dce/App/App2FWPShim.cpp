////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include <Platform/dce/App/App2FWPShim.h>

//#include "Platform/dce/Pattern/FWPFasade.h"

namespace AWI {
Fwp_I& GetApp2FWPShim(){
 return (AWI::GetFwpShim());
}


//bool App2FWPShim::ConnectToFwp(int32_t commId){};

} //End of namespace
