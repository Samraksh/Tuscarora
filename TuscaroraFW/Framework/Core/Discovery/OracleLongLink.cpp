////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include "OracleLongLink.h"

/*Counts nodes as potential neighbors when they are within 200 meters of the current node*/

extern NodeId_t MY_NODE_ID;

// extern bool DBG_CORE; // set in Lib/Misc/Debug.cpp

extern RuntimeOpts* RUNTIME_OPTS;

using namespace PAL;
using namespace PWI;

namespace Core {
namespace Discovery {

  void OracleLongLink::Start() {
    TimerDelegate *state_ = new TimerDelegate(this, &OracleLongLink::UpdateNeighborList);
    longTimer = new Timer(1000000, PERIODIC, *state_);
    longTimer->Start();
    UpdateNeighborList(0);//This calls UpdateNeighborList every 10 seconds.
  }

  void OracleLongLink::UpdateNeighborList(uint32_t event) {
    double x = atof(getenv("CURX"));
    double y = atof(getenv("CURY"));
    char* posses = getenv("CURPOS");//This is in the format of x1-y1-x2-y2-....-xn-yn. This line is getting knowlege of information from DCE that it would not be privy to in a real world installation
    char *p = strtok(posses, "-");
    NodeId_t id = 0;

    MemMap<Header> map("configuration.bin");
    map.openForRead();
    Header* header = map.getNext();
    while (p) {
      double x2 = atof(p);
      p = strtok(NULL, "-");
      double y2 = atof(p);
      p = strtok(NULL, "-");

      if(id != MY_NODE_ID) {
	double dx = x2-x;
	double dy = y2-y;

	if(dx*dx+dy*dy < (header->range * 2) * (header->range * 2)) {
	  //Add a potential neighbor since it is in the 200 meter radius
	  LinkId link; link.nodeId = id;
	  for(int w = 2; w < MAX_WAVEFORMS; w++) {
	    link.waveformId = w;
	    if(nbrs.GetNode(link) == 0 && NetworkDiscovery::hasWaveform(w, id)) AddLink(id, w, header, dx, dy);
	  }
	} else {
	  //Remove a potential neighbor since it is no longer within range
	  LinkId link; link.nodeId = id; link.waveformId = 0;
	  for(int w = 2; w < MAX_WAVEFORMS; w++) {
	    link.waveformId = 0;
	    if(nbrs.GetNode(link) != 0 && NetworkDiscovery::hasWaveform(w, id)) RemoveLink(id, w);
	  }
	}
      }	
      id++;
    }
    map.closeForRead();
  }

  void OracleLongLink::RemoveLink(NodeId_t node, WaveformId_t waveform) {
    LinkId link;
    link.nodeId = node;
    link.waveformId = waveform;
    PotentialNeighbor *i = nbrs.GetNode(link);
    if(del != 0) {
      //Sending the event to the delegate, this should be the link estimation method
      PotentialNeighborDelegateParam _param;
      _param.changeType = PNBR_INVALID;
      _param.pnbr = i;
      del->operator ()(_param);	
      nbrs.RemoveNode(link);
    }
  }

  void OracleLongLink::AddLink(NodeId_t node, WaveformId_t waveform, Header* header, double dx, double dy) {
    LinkId link;
    link.nodeId = node;
    link.waveformId = waveform;
    PotentialNeighbor *i = nbrs.AddNode(link, BEYOND_COMM_PNODE, 0, 1 - (((header->range * 2) * (header->range * 2)) / (dx*dx+dy*dy)));
    if(del != 0) {
      //Sending the event to the delegate, this should be the link estimation method
      PotentialNeighborDelegateParam _param;
      _param.changeType = PNBR_UPDATE;
      _param.pnbr = i;
      del->operator ()(_param);
    }
  }
}
}
