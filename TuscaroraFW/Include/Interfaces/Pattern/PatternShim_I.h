////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef PATTERN_SHIM_I_H_
#define PATTERN_SHIM_I_H_

#include <Interfaces/PWI/Framework_I.h>

using namespace PWI;

namespace Patterns {
  
  class PatternShim_I : public Framework_I {
  
    //Framework_I *fi;
    
  public:
    
    //virtual bool ConnectToFramework(int32_t commId=0) = 0;
    virtual void RegisterDelegates (PatternId_t _pid, RecvMessageDelegate_t *_recvDelegate, NeighborDelegate *_nbrDelegate, 
			    DataflowDelegate_t *_dataNotifierDelegate, ControlResponseDelegate_t *_controlDelegate) =0;
    virtual ~PatternShim_I(){}
  };
  
  Framework_I& GetPatternShim();
  
}//End of namespace



#endif //PATTERN_SHIM_H_
