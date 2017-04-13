////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include "sys/mman.h"
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <vector>
#include "BasicTypes.h"
#include "MemMap.h"
#include <map>

const uint16_t MAX_NBR=256;

enum Status {
  UnknownNeighbor,
  Neighbor,
  FalseNeighbor,
  NonNeighbor
};

using namespace std;
using namespace PAL;

struct Location { Location(EstimationLogElement l) { x = l.x; y = l.y; } Location(){} double x; double y; double vx; double vy; double time; };

double getDelta(Location a, Location b) {
  double x = a.x - b.x;
  double y = a.y - b.y;

  return sqrt(x * x + y * y);
}

Location getLocation(uint node, long time, map<uint, vector<Location> > locations) {
  Location location;

  time -= 1262304000000000;
  
  for(uint x=0; x < locations[node].size(); x++) {
    //printf("Node %d is at %f, %f for time %f\n", node, locations[node].at(x).x, locations[node].at(x).y, locations[node].at(x).time);
    if(locations[node].at(x).time > 1.0f * time / 1000000) { //WRONG WRONG WRONG
      //printf("skipping time of %f when at %ld\n", locations[node].at(x).time, time);
    } else {
      float ftime = (time / 1000000.0f) - locations[node].at(x).time;
      //printf("Calculating distance after %f seconds of travel\n", ftime);
      location.x = locations[node].at(x).x + locations[node].at(x).vx * ftime;
      location.y = locations[node].at(x).y + locations[node].at(x).vy * ftime;
      return location;
    }
  }
  printf("CANNOT CALCULATE LOCATION\n");
  return location;
}

int main(int argc, char *argv[]) {
  char filename[300];
  sprintf(filename, argv[1], 0);
  PAL::MemMap<PAL::Header> headerMap(filename);
  headerMap.openForRead();
  Header* header = headerMap.getNext();

  map<uint, vector<Location> > locations;
  FILE * fp = fopen(argv[3], "r");
  if(fp == NULL) { printf("ERROR opening file\n"); return 1; }
  while(!feof(fp)) {
    int node;
    Location l;
    int cnt = fscanf(fp, "CourseChange at t = %lf /NodeList/%d/$ns3::MobilityModel/CourseChange Vx=%lf, Vy=%lf x=%lf, y=%lf, z=0\n", &l.time, &node, &l.vx, &l.vy, &l.x, &l.y);
    if(cnt != 6) { printf("ERROR READING file\n"); return 1; }
    locations[node].insert(locations[node].begin(), l);
  }
  
  int cnt = 0;
  int numReceived = 0;
  vector<EstimationLogElement> vsent;
  map<long, vector<EstimationLogElement> > vreceived;
  for(uint x = 0; x < header->numNodes; x++) {
    char filename[300];
    sprintf(filename, argv[2], x);
    MemMap<EstimationLogElement> estimation(filename);
    estimation.openForRead();
    while(estimation.hasNext()) {
      EstimationLogElement* elm = estimation.getNext();
      if(elm->type == LINK_SENT) {
	vsent.push_back(*elm);
      } else if(elm->type == LINK_ADDED || elm->type == LINK_UPDATED) {
	vreceived[(elm->time / header->beaconPeriod) + 1].push_back(*elm);
	vreceived[(elm->time / header->beaconPeriod) - 1].push_back(*elm);
	vreceived[elm->time / header->beaconPeriod].push_back(*elm);
	numReceived++;
      } else if(elm->type == LINK_SKIPPED) {
      } else if(elm->type == LINK_REMOVED) {
      } else if(elm->type == LINK_POTENTIAL_ADD) {
      } else if(elm->type == LINK_POTENTIAL_DEL) {
      }
      cnt++;
    }
  }

  map<int, Location> deltas;
  for(uint x=0; x < vsent.size(); x++) {
    EstimationLogElement sent = vsent.at(x);
    vector<EstimationLogElement> vrec = vreceived[sent.time / header->beaconPeriod];
    for(int nn = 0; nn < header->numNodes; nn++) {
      if(nn == sent.id) continue;
      bool gotit = false;
      for(uint z = 0; z < vrec.size(); z++) {
	EstimationLogElement received = vrec.at(z);
	if(received.id == nn && received.nbr == sent.id && received.time > sent.time && received.time < sent.time + 1000) {
	  //printf("message received from %d by %d at %f,%f\n", sent.id, received.id, received.x, received.y);
	  double delta = getDelta(Location(sent), Location(received));
	  deltas[((int)delta/atoi(argv[4])) * atoi(argv[4])].x++;
	  gotit = true;
	}
      }

      if(!gotit) {
	Location loc = getLocation(nn, sent.time, locations);
	double delta = getDelta(Location(sent), loc);
	deltas[((int)delta/atoi(argv[4])) * atoi(argv[4])].y++;
	//printf("message not received from %d by %d at %f,%f with a delta of %f\n", sent.id, nn, loc.x, loc.y, getDelta(Location(sent), loc));
      }
    }
  }

  map<int, Location>::iterator iter;
  for(iter = deltas.begin(); iter != deltas.end(); iter++) {
    if(iter->second.x != 0)
      printf("%d %f %f %f\n", iter->first, iter->second.x, iter->second.y, iter->second.x / (iter->second.x + iter->second.y) * 100);
  }
  
  return 0;
}
