////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef FIFOLIST_H_
#define FIFOLIST_H_

#include <Interfaces/Core/MessageT.h>
#include "Lib/DS/ListT.h"


class DummyCompare {
public:
  bool operator () (Core::FMessage_t*& A, Core::FMessage_t*& B){
    return LessThan(A,B);
  }
  bool LessThan (Core::FMessage_t*& A, Core::FMessage_t*& B) {
    return false;
  }
  bool DuplicateOf (Core::FMessage_t*& A, Core::FMessage_t*& B) {
    return A == B;
  }
};


class FIFOList : public  ListT<Core::FMessage_t*, false, DummyCompare >{
public:
  bool InsertMessage(Core::FMessage_t *msg){
    return InsertBack(msg);
  }
  
  Core::FMessage_t*  RemoveMessage(){
    Core::FMessage_t *ret=NULL;
    if(Size() >0){
     ret = GetItem(0);
     Delete(0);
    }
    return ret;
  }
  
};

#endif //FIFOLIST_H_