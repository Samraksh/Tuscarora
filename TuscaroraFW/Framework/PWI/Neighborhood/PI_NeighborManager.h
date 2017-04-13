////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef PI_NEIGHBORMANAGER_H_
#define PI_NEIGHBORMANAGER_H_

#include "CustomPatternNeighborTable.h"
#include "Framework/Core/Neighborhood/NeighborTable.h"
#include "Framework/PWI/PatternClient.h"

namespace PWI {
namespace Neighborhood {

typedef BSTMapT<PatternId_t, CustomPatternNeighborTable *>  PatternToPNT_Map_t;
	
class PI_NeighborManager {
private:
  NeighborTable *corNbrTable;
  PatternToPNT_Map_t  patternTables;
  //BSTMapT<PatternId_t, PatternClient> *clients;
  PatternEventDispatch_I *eventDispatcher;
public:
  
  //NeighborManager();
  void SetLinkComparator(PatternId_t pid, LinkComparatorTypeE compType);
  LinkComparatorTypeE GetLinkComparator(PatternId_t pid);
  void SetPatternEventDispatcher(PatternEventDispatch_I *_eventDispatcher){eventDispatcher = _eventDispatcher;}
  void InitializeNeighborhood(PatternId_t pid);
  //void SetLinkEstimator(uint8_t asnpID, LinkEstimatorI& linkEstimator);
  void SetCoreNbrTable(NeighborTable &table);
  void SetPatternNbrTable(uint16_t asnpId, CustomPatternNeighborTable &table);
  CustomPatternNeighborTable& GetNeighborTable(uint16_t patternId);
  void SetNeighborThreshold(PatternId_t pid, LinkMetrics *threshold);
  void CleanUp();
  void EvaluateQualities();
  void EvaluteNbr(uint16_t node);
  void UpdatePatternTables(NeighborUpdateParam _param);
};

}
}


#endif /* PI_NEIGHBORMANAGER_H_ */
