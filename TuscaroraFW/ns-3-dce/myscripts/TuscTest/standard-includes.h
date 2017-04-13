////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include <sys/stat.h>
#include "ns3/packet-socket-helper.h"
#include "ns3/core-module.h"
#include "ns3/dce-module.h"
#include "ns3/wifi-module.h"
#include "ns3/wifi-helper.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/nqos-wifi-mac-helper.h"
#include "ns3/mobility-module.h"
#include "ns3/mobility-helper.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/log.h"
#include "ns3/simple-net-device.h"
#include <ns3/constant-position-mobility-model.h>

#include <string.h>
#include <sstream>
#include "sys/mman.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>

using namespace ns3;

template < class T >
std::string stringify(const T& t) {
  std::stringstream ss;
  ss << t;
  return ss.str();
}

#include "SimulationOpts.h"

using namespace ns3;
