////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

2) for look ups. Very inefficient for large number of elements.
 */


#ifndef LISTMAPT_H_
#define LISTMAPT_H_

#include "ListT.h"

template <class KEY, class DATA>
class MapElement{
  KEY key;
  DATA data;
public:
  MapElement(KEY _key, DATA _data){key=_key; data=_data;}
  MapElement(KEY _key){key=_key;}
  MapElement(){}
  bool operator == (MapElement<KEY,DATA> B){
    if(key == B.key) {
      return true;
    }
    else return false;
  }
  DATA GetData(){return data;}
};

template <class Element>
class MapComparator{
  public:
  bool operator () (Element& A, Element& B) {
    return A == B;
  }
};

template <class KEY, class DATA>
class ListMapT : public ListT< MapElement<KEY,DATA>, false, MapComparator<MapElement<KEY,DATA> > > {
public:
  bool Insert(KEY _key, DATA _data){
    MapElement<KEY,DATA> *element = new MapElement<KEY,DATA>(_key,_data);
    return ListT< MapElement<KEY,DATA>, false, MapComparator<MapElement<KEY,DATA> > >::InsertBack (*element);
  }
  
  DATA operator [] (KEY _key){
    MapElement<KEY,DATA> onlyKeyElement(_key);
    Node< MapElement<KEY,DATA> > *node = ListT< MapElement<KEY,DATA>, false, MapComparator<MapElement<KEY,DATA> > >::FindNode(onlyKeyElement);
    if(node){ return node->data.GetData();}
    else return NULL;
  }
  
  bool Delete(KEY _key){
    MapElement<KEY,DATA> onlyKeyElement(_key);
    Node< MapElement<KEY,DATA> > *node = ListT< MapElement<KEY,DATA>, false, MapComparator<MapElement<KEY,DATA> > >::FindNode(onlyKeyElement);
    if(node){ return this->DeleteNode(node);}
    else return false;
  }
};

#endif //LISTMAPT_H_
