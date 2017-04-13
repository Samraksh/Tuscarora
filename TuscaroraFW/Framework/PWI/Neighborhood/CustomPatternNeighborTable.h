////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef CUSTOM_PATTERN_NEIGHBOR_TABLE_H_
#define CUSTOM_PATTERN_NEIGHBOR_TABLE_H_
#include <Interfaces/PWI/PatternNeighborTableI.h>
#include "Framework/Core/Neighborhood/NeighborTable.h"
#include <Interfaces/PWI/PatternEventDispatch_I.h>
#include "Lib/DS/SortedListT.h"
#include "Framework/PWI/Neighborhood/LinkComparators.h"
#include <assert.h>

using namespace Core::Estimation;
using namespace Core::Neighborhood;

namespace PWI{
namespace Neighborhood {

class FW_To_Ptable_I {
  
public:
  virtual void NeighborUpdateSignal()=0;
  virtual ~FW_To_Ptable_I(){}
};


class PNbrHood : public SortedListT<LinkMap*, false, QualityComparator> {
 
public:
  bool DeleteLink(LinkId id){
    LinkMap *keyOnly = new LinkMap(id);
    Node* node= FindNode(keyOnly);
    if(node) {
      DeleteNode(node);
      return true;
    }
    return false;
  }
  Link GetLink(LinkId& id){
    LinkMap *keyOnly = new LinkMap(id);
    Link ret;
    Node* node= FindNode(keyOnly);
    if(node) {
      ret= node->data->link;
    }
    return ret;
  }
};


//class CustomPatternNeighborTable : public PatternNeighborTableI{
class CustomPatternNeighborTable {
  //declear Iterator has friend class
  //friend class PatternNeighborIterator;
  
  PatternId_t patternId;
  //PatternClient *pattern;
  PatternEventDispatch_I *eventDispatch;
  
  LinkMetrics threshold;
  //LinkEstimatorI *asnpLinkQualities;
  //static QualityComparator defualtLinkComparator;
  LinkMapComparatorI *comparator;
  Core::Neighborhood::NeighborTable *coreNbrTable;
  uint8_t noOfWfs;
  
  //SortedListT<LinkMap*, false, QualityComparator> *nbrhood;
  PNbrHood *nbrhood;
  LinkComparatorTypeE currentLCType;
private:
  uint16_t PopulateNeighbors ();
  
public:
  CustomPatternNeighborTable (Core::Neighborhood::NeighborTable *_coreTable, PatternId_t _patternId, PatternEventDispatch_I *_eventDispatcher);
  //void ApplyFilter ();
  void SetLinkComparatorType(LinkComparatorTypeE compType);
  LinkComparatorTypeE GetLinkComparatorType();
  //void SetLinkComparator (LinkComparatorI &comparator);
  void SetNeighborThreshold (LinkMetrics threshold);
  uint16_t GetNumberOfNeighbors ();
  
  uint8_t GetNumberOfWaveform();
  uint8_t GetWaveformIds(WaveformId_t *wIdArray);
  void SetNeighborThreshold(LinkMetrics *threshold);
  
  //uint16_t GetBestNeighbor ();
  
  Link* GetNeighborLink (NodeId_t nodeId);
  NodeId_t GetNeighborID (uint16_t table_index);
  //uint16_t GetPendingPackets (uint16_t neighbor);
  
  //PatternNeighborIterator Begin ();

  //PatternNeighborIterator End ();
  
  void NeighborUpdate(NeighborUpdateParam _param);
  
  //FindLink(NodeId_t id);
  //void Sort ();
  
};

}
}

#endif /* CUSTOM_PATTERN_NEIGHBOR_TABLE_H_ */
