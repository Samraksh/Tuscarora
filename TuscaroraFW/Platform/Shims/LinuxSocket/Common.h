////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef COMMON_H_
#define COMMON_H_

#include <unistd.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <netinet/tcp.h>

using namespace std;

namespace Lib {
namespace Shims {

enum SocketTypeE {
		TCP_SOCK,
		UDP_SOCK
};

}
}

#endif //COMMON_H_
