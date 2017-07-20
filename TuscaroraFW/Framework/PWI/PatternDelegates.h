////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef PATTERN_DELEGATES_H_
#define PATTERN_DELEGATES_H_

#include <Interfaces/Pattern/PatternEvent.h>
#include <Interfaces/PWI/Framework_I.h>
#include <Interfaces/Pattern/PatternBase.h>
#include <Lib/DS/BSTMapT.h>

using namespace PAL;
using namespace PWI;
using namespace Patterns;

namespace PWI {

  class PatternDelegates{
  public:
    NeighborDelegate *nbrDelegate;
    RecvMessageDelegate_t *recvDelegate;
    DataflowDelegate_t *dataNotifierDelegate;
    ControlResponseDelegate_t *controlDelegate;
    
  public:
    PatternDelegates(){
      dataNotifierDelegate= NULL;
      recvDelegate=NULL;
      nbrDelegate=NULL;
    }
    
    void RegisterDelegates (RecvMessageDelegate_t *_recvDelegate, NeighborDelegate *_nbrDelegate, DataflowDelegate_t *_dataNotifierDelegate, ControlResponseDelegate_t *_controlDelegate)
    {
      dataNotifierDelegate = _dataNotifierDelegate;
      recvDelegate = _recvDelegate;
      nbrDelegate = _nbrDelegate;
      controlDelegate = _controlDelegate;
    }
  };
  
  typedef BSTMapT<PatternId_t, PatternDelegates> PTN_DelegatesMap_t; 
  
} // End namespace

#endif //PATTERN_DELEGATES_H_