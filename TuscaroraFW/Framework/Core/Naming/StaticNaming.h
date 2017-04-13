////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef STATICNAMING_H
#define  STATICNAMING_H

#include <Interfaces/Core/PatternNamingI.h>
#include <Base/BasicTypes.h>

#include <stdio.h>
#include <string.h>

namespace Core{
namespace Naming {

#define MAX_PATTERNS 16
  
struct PatternMaping {
  PatternTypeE ptype;
  PatternId_t pid;
  bool assigned;
  char uniqueName[128];
};

  //PatternMaping config[MAX_PATTERNS];

  class StaticNaming: public PatternNameServiceI {
    static PatternMaping config[MAX_PATTERNS]; 
    static PatternTypeE patternIDMap[MAX_PATTERNS];
  
  public:
    StaticNaming();
    static PatternId_t GetNewPatternInstanceId (PatternTypeE ptype, const char uniqueName[128]);
    static PatternTypeE GetPatternType(PatternId_t id);
    
  };

} //end Naming
} //end Core

#endif	// STATICNAMING_H
