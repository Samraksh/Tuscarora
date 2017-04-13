////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include "Global.h"

extern NodeId_t MY_NODE_ID;
extern RuntimeOpts* RUNTIME_OPTS;

using namespace PAL;
using namespace PWI;

namespace Core {
namespace Discovery {

  void Global::Start() {
    int size = atoi(getenv("NETWORK_SIZE"));
    
    MemMap<Header> map("configuration.bin");
    map.openForRead();
    Header* header = map.getNext();
    if(header->discoveryType == LONG_LINK_DISCOVERY) size--;

    //Add every node in the network as potential neighbors
    for(int x = 0; x < size; x++) {
      if(x == MY_NODE_ID) continue;
      char nodetok[7];
      sprintf(nodetok, ",%d,", x);
      for(int w = 2; w < MAX_WAVEFORMS; w++) {
	if(NetworkDiscovery::hasWaveform(w, x)) {
	 Debug_Printf(DBG_CORE_DISCOVERY, "Adding node %d for waveform %d\n", x, w);
	  AddLink(x, w);
	} else {
	 Debug_Printf(DBG_CORE_DISCOVERY, "Not adding node %d for waveform %d\n", x, w);
	}
      }
    }
  }

  void Global::AddLink(NodeId_t node, WaveformId_t waveform) {
      LinkId link;
      link.nodeId = node;
      link.waveformId = waveform;
      PotentialNeighbor *nbr = nbrs.AddNode(link, BEYOND_COMM_PNODE, 0, 1);
      if(del != 0) {
	PotentialNeighborDelegateParam _param;
	_param.changeType = PNBR_UPDATE;
	_param.pnbr = nbr;
	del->operator ()(_param);
      }
  }
}
}
