////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

/* C++ Delegate definition based on http://allenchou.net/2012/04/easy-c-delegates/
  Modified by: Bora Karaoglu
   Date: July 21, 2016.
 */

#ifndef Delegate_h
#define Delegate_h

#include <assert.h>
#include "Callback.h"
#include <stdio.h>
#include "Lib/Misc/Debug.h"
#include <assert.h>

using namespace Lib;

///Definitions and types for the Platform Abstraction Layer
namespace PAL {

template <typename Ret, typename... ParamN>
class StaticFunctionCallback : public Callback<Ret, ParamN...>
{
private:
  Ret (*func_)(ParamN...);

public:
  StaticFunctionCallback(Ret (*func)(ParamN...))
    : func_(func)
  {}

  virtual Ret invoke(ParamN... param0)
  {
    return (*func_)(param0...);
  }
  virtual ~StaticFunctionCallback(){}
};



template <typename Ret, typename... ParamN>
class Delegate
{
  private:
  int DelegateCount;
  Callback<Ret, ParamN...> *callback_;

public:

  //Default constructor
  Delegate()
  {DelegateCount=0;}

  Delegate(Ret (*func)(ParamN...))
    :callback_(new StaticFunctionCallback<Ret, ParamN...>(func))
  {
    DelegateCount=1;
  }

  template <typename T, typename Method>
  Delegate(T *object, Method method)
    :callback_(new MethodCallback<Ret, T, Method, ParamN...>(object, method))
  {
    DelegateCount=1;
  }

  ~Delegate(void) {
    delete callback_;
  }

  void AddCallback(Ret (*func)(ParamN...))
  {
    callback_=new StaticFunctionCallback<Ret, ParamN...>(func);
    DelegateCount++;
  }

  template <typename T, typename Method>
  void AddCallback(T *object, Method method)
  {
    callback_=new MethodCallback<Ret, T, Method, ParamN...>(object, method);
    DelegateCount++;
    //printf("Delegate Count, after Addcallback: %d", DelegateCount);
  }


  Ret operator()(ParamN... param0)
  {
    if(DelegateCount > 1){
      printf("%d functions registered for callback for same delegate. This is unexpected behavior\n",DelegateCount);fflush (stdout);
    }
    
    if(DelegateCount) {
      //printf("delegate firing\n");
      return callback_->invoke(param0...);
    }else {
      Debug::PrintTimeMicro();
      printf("Delegate:: Delegate fired, but no callback registered\n");
      assert(0);
    }
  }
};




}

#endif //Delegate_h
