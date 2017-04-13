////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef PTN_NBR_HOOD_H
#define PTN_NBR_HOOD_H

#include <Interfaces/Core/Link.h>
#include "Lib/DS/SortedListT.h"
#include "Framework/PWI/Neighborhood/LinkComparators.h"

//using namespace Core;
using namespace PWI::Neighborhood;

namespace Patterns {
/*  
struct LinkMap{
  LinkId id;
  Link link;
  
  LinkMap(LinkId &_id, Link _link){id=_id; link=_link;}
  LinkMap(LinkId &_id){id=_id; }
};
  
class LinkMapComparatorI {
    ///Returns true if A is better than B, false otherwise
  public:
    virtual bool BetterThan (LinkMap* A, LinkMap* B) = 0;
    virtual bool DuplicateOf (LinkMap* A, LinkMap* B) = 0;
    bool LessThan (LinkMap* A, LinkMap* B) { return BetterThan(A,B);}  
};

class QualityMapComparator:public LinkMapComparatorI{
public:
  bool LessThan (LinkMap *A, LinkMap *B) {return BetterThan(A,B);}
  bool BetterThan (LinkMap *A, LinkMap *B) {
    return A->link.metrics.quality > B->link.metrics.quality;
  }
  bool DuplicateOf (LinkMap *A, LinkMap *B) {
    return A->id == B->id;
  }
};  

class CostMapComparator:public LinkMapComparatorI{
public:
  bool LessThan (LinkMap *A, LinkMap *B) {return BetterThan(A,B);}
  bool BetterThan (LinkMap *A, LinkMap *B) {
    return A->link.metrics.cost < B->link.metrics.cost;
  }
  bool DuplicateOf (LinkMap *A, LinkMap *B) {
    return A->id == B->id;
  }
};  

class QualAndXmitDelayMapComparator:public LinkMapComparatorI{
public:
  bool LessThan (LinkMap *A, LinkMap *B) {return BetterThan(A,B);}
  bool BetterThan (LinkMap *A, LinkMap *B) {
    if(A->link.metrics.quality > B->link.metrics.quality) {
      return true;
    }else if(A->link.metrics.quality > B->link.metrics.quality){
      return false;
    }else {
      return A->link.metrics.avgLatency < B->link.metrics.avgLatency;
    }
  }
  bool DuplicateOf (LinkMap *A, LinkMap *B) {
    return A->id == B->id;
  }
}; 

*/

class PtnNbrHoodI {
public:
  virtual bool DeleteLink(LinkId id)=0;
  virtual Link* GetBestLink(WaveformId_t wid) =0;
  virtual Link* GetLink(LinkId& id)=0;
  virtual LinkMap* GetItem (uint32_t index)=0;
  virtual bool Insert(LinkMap*& object)=0;
  virtual uint32_t Size() =0;
  //virtual bool DeleteNode( *_node);
};


template <class comparator>
class PtnNbrHood : public SortedListT<LinkMap*, false, comparator>, public PtnNbrHoodI {
  typedef typename SortedListT<LinkMap*, false, comparator>::Node MyNode;
    
public:
  LinkMap* GetItem (uint32_t index){
    return SortedListT<LinkMap*, false, comparator>::GetItem(index);
  }
  bool Insert(LinkMap*& object){
    return SortedListT<LinkMap*, false, comparator>::Insert(object);
  }
  uint32_t Size(){
    return SortedListT<LinkMap*, false, comparator>::Size();
  }   
  bool DeleteLink(LinkId id){
    LinkMap *keyOnly = new LinkMap(id);
    MyNode *node =  SortedListT<LinkMap*, false, comparator>::FindNode(keyOnly);
    if(node) {
      SortedListT<LinkMap*, false, comparator>::DeleteNode(node);
      return true;
    }
    return false;
  }
  
  Link* GetBestLink(WaveformId_t wid){
    uint16_t index=0;
    for (index=0; index < SortedListT<LinkMap*, false, comparator>::Size(); index++)
    {
      LinkMap *lm = SortedListT<LinkMap*, false, comparator>::GetItem(index);
      if(lm->id.waveformId == wid) {
	return &(lm->link);
      }
    }
    return 0;
  }
  
  Link* GetLink(LinkId& id){
    LinkMap *keyOnly = new LinkMap(id);
    Link *ret=0;
    MyNode* node= SortedListT<LinkMap*, false, comparator>::FindNode(keyOnly);
    if(node) {
      ret= &(node->data->link);
    }
    return ret;
  }
};

  
  
} //End namespace

#endif //PTN_NBR_HOOD_H