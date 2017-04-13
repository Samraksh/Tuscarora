////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef PATTERNNEIGHBORTABLE_H_
#define PATTERNNEIGHBORTABLE_H_
#include <Interfaces/PWI/PatternNeighborTableI.h>
#include "PtnNbrHood.h"
#include <Interfaces/PWI/PatternEventDispatch_I.h>
#include "Lib/DS/SortedListT.h"

using namespace Core::Estimation;
using namespace Patterns;
using namespace PWI;

namespace Patterns {
    
class PatternNeighborTable : public PatternNeighborTableI{
	//declear Iterator has friend class
	friend class PatternNeighborIterator;

	PatternId_t patternId;
	//PatternClient *pattern;
	PatternEventDispatch_I *eventDispatch;

	LinkMetrics threshold;
	//LinkEstimatorI *asnpLinkQualities;
	static QualityComparator defualtLinkComparator;
	LinkMapComparatorI *comparator;
	uint8_t noOfWfs;

	//SortedListT<LinkMap*, false, QualityComparator> *nbrhood;
	PtnNbrHoodI *nbrhood;
  
private:
  //uint16_t PopulateNeighbors ();
  
public:
  PatternNeighborTable (LinkComparatorTypeE  lcType);
  //void ApplyFilter ();
  
  //void SetLinkComparator (LinkComparatorI &comparator);
  //void SetNeighborThreshold (LinkMetrics threshold);
  uint16_t GetNumberOfNeighbors ();
  
  //uint8_t GetNumberOfWaveform();
  //uint8_t GetWaveformIds(WaveformId_t *wIdArray);
 // void SetNeighborThreshold(LinkMetrics *threshold);
  
  //uint16_t GetBestNeighbor ();
  
  Link* GetNeighborLink (NodeId_t nodeId);
  Link* GetBestLink(WaveformId_t wid);
  NodeId_t GetNeighborID (uint16_t table_index);
  //uint16_t GetPendingPackets (uint16_t neighbor);
  
  PatternNeighborIterator Begin ();

  PatternNeighborIterator End ();
  
  void UpdateTable(NeighborUpdateParam _param);
  
  //FindLink(NodeId_t id);
  //void Sort ();
  
  void PrintTable();

};


//Switching to a simple link based structure for neighborhood
class PatternNeighborIterator {
	Link *curNode;
	uint16_t curNodeIndex;
	PatternNeighborTable *myNbrTable;
	//SortedListT<PatternNeighborInfo*, false> *list;
  
public:
	PatternNeighborIterator();
	PatternNeighborIterator(PatternNeighborTable* tbl, uint16_t _curNodeIndex);
	const Link* operator -> ();
	const Link& operator * ();
	PatternNeighborIterator operator ++ ();
	PatternNeighborIterator operator -- ();
	PatternNeighborIterator GetNext();
	//bool operator == (Link& _link);
	bool operator == (PatternNeighborIterator _iter);
	bool operator != (PatternNeighborIterator _iter);
};


} //End namespace

#endif /* PATTERNNEIGHBORTABLE_H_ */
