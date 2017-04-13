////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 


#ifndef NONE_H_
#define NONE_H_

#include "NetworkDiscovery.h"

namespace Core {
namespace Discovery {

class None : public NetworkDiscovery{
 public:
    ~None() {}
    void Start();
    void ProcessMessage(FMessage_t& msg) {}
};

}
}
#endif
