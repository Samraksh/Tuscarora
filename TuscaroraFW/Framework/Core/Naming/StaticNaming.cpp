////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#include "StaticNaming.h"
#include "PatternIDs.h"

namespace Core{
namespace Naming {

//uint16_t StaticNaming::generateID=0;
PatternTypeE StaticNaming::patternIDMap[MAX_PATTERNS]={UNKNOWN_PTN};
PatternMaping StaticNaming::config[MAX_PATTERNS];

PatternId_t GetNewPatternInstanceId(PatternTypeE ptype, const char uniqueName[128]){
  return StaticNaming::GetNewPatternInstanceId(ptype, uniqueName);
}


StaticNaming::StaticNaming(){
  for (uint16_t i=0; i<MAX_PATTERNS; i++){
    config[i].pid = i+1;
    config[i].assigned = false;
    config[i].ptype = PatternsSupported[i];
  }
}

    /// Returns an unique id for use as the pattern id, takes the pattern type as parameter
PatternId_t StaticNaming::GetNewPatternInstanceId (PatternTypeE ptype, const char uniqueName[128]){
  PatternId_t newId=0;
  int i=0;
  for (i=0; i < MAX_PATTERNS; i++){
    if(config[i].ptype == ptype && !config[i].assigned){
      newId = config[i].pid;
      config[i].assigned=true;
      memcpy(config[i].uniqueName,uniqueName,128);
      patternIDMap[newId]=ptype;
      break;
    }else if(config[i].ptype == ptype && !strcmp(config[i].uniqueName,uniqueName)) {
      newId=config[i].pid;
      printf("UniqueName '%s' is already in the pattern DB, second request by the same pattern %d?\n", uniqueName,newId);
      break;
    }
  }
  if(i==MAX_PATTERNS){
    printf("Didnt find ID for the pattern, something wrong, all %d ids taken?\n", MAX_PATTERNS);
    newId=0;
  }
  return newId;
}
    
PatternTypeE StaticNaming::GetPatternType(PatternId_t id){
  return patternIDMap[id];
}

} //end Naming
} //end Core

