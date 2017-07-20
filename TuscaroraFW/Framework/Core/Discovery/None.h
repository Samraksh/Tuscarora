////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 


#ifndef NONE_H_
#define NONE_H_

#include "Framework/Core/Discovery/NetworkDiscovery.h"
#include <Interfaces/Core/MessageT.h>

namespace Core {
namespace Discovery {

class None : public NetworkDiscovery{
 public:
    virtual ~None() {}
    void Start() override;
    void ProcessMessage(FMessage_t& msg) override {}
};

}
}
#endif
