////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef CALLBACK_H_
#define CALLBACK_H_

#include <stdio.h>

namespace PAL {

template <typename Ret, typename... ParamN>
class Callback
{
public:
  virtual Ret invoke(ParamN&... param0)=0;
  virtual ~Callback(){}
};


template <typename Ret, typename T, typename Method,  typename... ParamN>
class MethodCallback : public Callback<Ret, ParamN...>
{
private:
  void *object_;
  Method method_;

public:
  MethodCallback(void *object, Method method)
    : object_(object)
    , method_(method)
  {}

  virtual Ret invoke(ParamN&... param0)
  {
	//printf("Inside callback.invoke\n");
    T *obj = static_cast<T *>(object_);
    return (obj->*method_)(param0...);
  }
  virtual ~MethodCallback(){}
};



}//End of namespace


#endif //CALLBACK_H_
