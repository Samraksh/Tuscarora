////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#include "Lib/Pattern/NeighborTable/PatternNeighborTable.h"

namespace Patterns{

PatternNeighborIterator::PatternNeighborIterator()
{

  //nbrhood = new SortedListT<LinkMetrics*, false, QualityComparator>();
}

PatternNeighborIterator::PatternNeighborIterator(PatternNeighborTable* tbl, uint16_t _curNodeIndex)
{
  curNodeIndex = _curNodeIndex;
  myNbrTable = tbl;
  curNode = &(myNbrTable->nbrhood->GetItem(curNodeIndex)->link);
}


const Link* PatternNeighborIterator::operator -> (){

  return curNode;  
}

const Link& PatternNeighborIterator::operator * (){

  return *curNode;  
}

PatternNeighborIterator PatternNeighborIterator::operator ++ (){
  curNode = &(myNbrTable->nbrhood->GetItem(++curNodeIndex)->link);
  return *this;
}

PatternNeighborIterator PatternNeighborIterator::operator -- (){
  curNode = &(myNbrTable->nbrhood->GetItem(--curNodeIndex)->link);
  return *this; 
}

PatternNeighborIterator PatternNeighborIterator::GetNext() {
  LinkMap* nextNode = myNbrTable->nbrhood->GetItem(++curNodeIndex);
  if(nextNode) {
    curNode= &(nextNode->link);
  }
  return *this; 
}

/*bool PatternNeighborIterator::operator == (Link& _node) {
  
  return false;
}


bool PatternNeighborIterator::operator == (Link& _node) {
  return false;
}*/

bool PatternNeighborIterator::operator == (PatternNeighborIterator _iter) {

  if(_iter.operator->() == curNode) {
    //printf("\n\nThe iterators point ot same node\n\n");fflush(stdout);
    return true;
  }
  return false;  
}
  
bool PatternNeighborIterator::operator != (PatternNeighborIterator _iter) {

  return !(this->operator==(_iter));  
}

  
} //End namespace