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
#include <map>
#include "../../../../Platform/linux/PAL/Logs/MemMap.h"

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

Location adjustForTime(double time, Location loc) {
  Location _loc;
  if(loc.time > time) {
    _loc.time = -1;
    return _loc;
  }
  _loc.x = loc.x + loc.vx * (time - loc.time);
  _loc.y = loc.y + loc.vy * (time - loc.time);
  _loc.time = loc.time;
  _loc.vx = loc.vx;
  _loc.vy = loc.vy;
  //printf("%f,%f becomes %f,%f at %f with an acc of %f,%f and a start time of %f\n", loc.x, loc.y, _loc.x, _loc.y, time, loc.vx, loc.vy, loc.time);
  return _loc;
}

int between(double A, double X, double B) {
  if(A > B) {
    A += B;
    B = A - B;
    A -= B;
  }
  return (A <= X && X <= B);
}

Location closestpointonline(float lx1, float ly1, 
					     float lx2, float ly2, float x0, float y0){ 
  float A1 = ly2 - ly1; 
  float B1 = lx1 - lx2; 
  double C1 = (ly2 - ly1)*lx1 + (lx1 - lx2)*ly1; 
  double C2 = -B1*x0 + A1*y0; 
  double det = A1*A1 - -B1*B1; 
  double cx = 0; 
  double cy = 0; 
  if(det != 0){ 
    cx = (float)((A1*C1 - B1*C2)/det); 
    cy = (float)((A1*C2 - -B1*C1)/det); 
  }else{ 
    cx = x0; 
    cy = y0; 
  } 
  Location ret;
  ret.x = cx;
  ret.y = cy;
  return ret;
}

int main(int argc, char *argv[]) {
  char filename[300];
  sprintf(filename, argv[1], 0);
  PAL::MemMap<PAL::Header> headerMap(filename);
  headerMap.openForRead();
  Header* header = headerMap.getNext();

  map<uint, vector<Location> > locations;
  FILE * fp = fopen(argv[2], "r");
  if(fp == NULL) { printf("ERROR opening file\n"); return 1; }
  while(!feof(fp)) {
    int node;
    Location l;
    int cnt = fscanf(fp, "CourseChange at t = %lf /NodeList/%d/$ns3::MobilityModel/CourseChange Vx=%lf, Vy=%lf x=%lf, y=%lf, z=0\n", &l.time, &node, &l.vx, &l.vy, &l.x, &l.y);
    if(cnt != 6) { printf("ERROR READING file\n"); return 1; }
    locations[node].insert(locations[node].begin(), l);
    //if(node <= 1)
    //printf("%d, %f, %f, %f, %f, %f\n", node, l.time, l.vx, l.vy, l.x, l.y);
  }

  //for(uint x = 0; x < header->numNodes; x++) {
  //for(uint y = 0; y < header->numNodes; y++) {
  int linkChanges = 0;
  for(uint x = 0; x < header->numNodes; x++) {
    for(uint y = 0; y < header->numNodes; y++) {
      double time = 0;
      bool within = false;
      bool pwithin = false;
      if(x==y) continue;
      while(time < 120) {
	int idxA = 0;
	int idxB = 0;
	Location v;//pt vector
	Location a; a.time = -1;
	Location nextA;
	while(a.time < 0) {
	  a = adjustForTime(time, locations[x].at(idxA));//pt
	  if(a.time < 0) idxA++;
	}
	if(idxA == 0) {
	  nextA.time = 120;
	} else {
	  nextA = locations[x].at(idxA - 1);
	}
	Location b; b.time = -1;
	Location nextB;
	while(b.time < 0) {
	  b = adjustForTime(time, locations[y].at(idxB));//pt
	  if(b.time < 0) idxB++;
	}
	if(idxB == 0) {
	  nextB.time = 120;
	} else {
	  nextB = locations[y].at(idxB - 1);
	}
	within = ((a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y)) < header->range*header->range;
	if(time != 0.0 && pwithin != within) {
	  linkChanges++;
	}
	pwithin = within;
	//printf("Testing %d.%d vs %d.%d at %f ", x, idxA, y, idxB, time);
	if(within) {
	  //printf("WITHIN ");
	  //printf("Orig Distance from pt: %f\n", sqrt((a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y)));
	  //printf("A:\t%f,%f\nB:\t%f,%f\nntime:\t%f\n", a.x, a.y, b.x, b.y, time);
	} else {
	  //printf("______ Orig Distance from pt: %f\n", sqrt((a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y)));
	}
	
	//A=ball1-moving
	//B=ball2-static
	Location P;//location
	//velocity a is closing on a static b
	v.vx = a.vx - b.vx;
	v.vy = a.vy - b.vy;
	float small = nextA.time - time;
	if(small > nextB.time - time)
	  small = nextB.time - time;
	//printf("Small is %f\nNextA is %f\nNextB is %f\n", small, nextA.time - time, nextB.time - time);
	
	Location aEnd;
	aEnd.x = a.x + v.vx * small;
	aEnd.y = a.y + v.vy * small;
	
	if(within) {
	  double distance = header->range-sqrt((a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y));
	  double ttl = distance/sqrt(v.vx*v.vx+v.vy*v.vy);
	  //printf("Distance to edge is %f in %f seconds\n", distance, ttl);
	  small = ttl;
	  //printf("Orig Distance from pt: %f\nNew Distance from pt: %f\n", sqrt((a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y)), sqrt((a.x+v.vx*small-b.x)*(a.x+v.vx*small-b.x) + (a.y+v.vy*small-b.y)*(a.y+v.vy*small-b.y)));
	} else {
	  Location d = closestpointonline(a.x, a.y, aEnd.x, aEnd.y, b.x, b.y);
	  double closestdistsq = (b.x - d.x)*(b.x-d.x) + (b.y-d.y)*(b.y-d.y);
	  //printf("closesdistdq: %f\n", closestdistsq);
	  if(closestdistsq <= header->range*header->range){ 
	    // a collision has occurred
	    double backdist = sqrt(header->range*header->range - closestdistsq); 
	    double movementvectorlength = sqrt(v.vx*v.vx + v.vy*v.vy); 
	    Location c;
	    c.x = d.x - backdist * (v.vx / movementvectorlength); 
	    c.y = d.y - backdist * (v.vy / movementvectorlength);
	    float distanceToC = sqrt((a.x-c.x)*(a.x-c.x)+(a.y-c.y)*(a.y-c.y));
	    if(between(a.x, c.x, aEnd.x) && between(a.y, c.y, aEnd.y)) {
	      //printf("A:\t%f,%f\naEnd:\t%f,%f\nB:\t%f,%f\nd:\t%f,%f\nc:\t%f,%f\ntime:\t%f\n", a.x, a.y, aEnd.x, aEnd.y, b.x, b.y, d.x, d.y, c.x, c.y, small);
	      float distanceInOne = sqrt(v.vx*v.vx + v.vy*v.vy);
	      small=distanceToC/distanceInOne;
	      //printf("%f,%f\n%f,%f\n%f,%f\n%f,%f\n%f,%f\ntime:\t%f\n", a.x, a.y, aEnd.x, aEnd.y, b.x, b.y, d.x, d.y, c.x, c.y, small);
	      //printf("Orig Distance from pt: %f\nNew Distance from pt: %f\n", sqrt((a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y)), sqrt((a.x+v.vx*small-b.x)*(a.x+v.vx*small-b.x) + (a.y+v.vy*small-b.y)*(a.y+v.vy*small-b.y)));
	    } else {
	      small = 1.0;
	    }
	  }
	}
	time += small + 0.00001;
      }
    }
  }
  printf("%d\n", linkChanges);
  return 0;
}
