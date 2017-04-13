////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <vector>

struct Received {
  long time;
  int receiver;
  int sender;
};

struct Loc {
  long time;
  int node;
  float x;
  float y;
};
struct Sent {
  long time;
  int node;
  float x;
  float y;
};

using namespace std;

int main(int argc, char *argv[]) {
  ifstream sin(argv[1]);
  ifstream rin(argv[2]);
  ifstream lin(argv[3]);

  vector<Sent> vsent;
  vector<Received> vreceived;
  vector<Loc> vloc;

  for(int x = 0; sin.good(); x++) {
    Sent sent;
    sin >> sent.time >> sent.node >> sent.x >> sent.y;
    vsent.push_back(sent);
  }

  for(int x = 0; rin.good(); x++) {
    Received received;
    rin >> received.sender >> received.time >> received.receiver;
    vreceived.push_back(received);
  }

  for(int x = 0; lin.good(); x++) {
    Loc loc;
    lin >> loc.time >> loc.node >> loc.x >> loc.y;
    vloc.push_back(loc);
  }

  for(int x = 0; x < vsent.size(); x++) {
    Sent sent = vsent.at(x);
    //cout << "Node " << sent.node << " sent a hb at " << sent.time << " " << x << " " << sent.x << "," << sent.y << endl;

    for(int y = 0; y < vloc.size(); y++) {
      //Received received = vreceived.at(y);
      Loc loc = vloc.at(y);
      if(sent.time + 100000 > loc.time && sent.time < loc.time && loc.node != sent.node) {
	float dx = sent.x - loc.x;
	float dy = sent.y - loc.y;
	if(dx * dx + dy * dy < 10000) {
	  Loc locloc = vloc.at(y - 1);
	  if(sent.time - 100000 < locloc.time && sent.time > locloc.time && loc.node == locloc.node) {
	    float ddx = sent.x - locloc.x;
	    float ddy = sent.y - locloc.y;
	    if(ddx * ddx + ddy * ddy < 10000) {
	      bool isreceived = false;
	      if(sent.node == 0 && loc.node == 89) printf("Here is 0/80 with a delta of %f sent at %lu\n", sqrt(dx * dx + dy * dy), sent.time);
	      if(sent.node == 0 && loc.node == 89) printf("%f-%f, %f-%f at %lu\n", sent.x, loc.x, sent.y, loc.y, loc.time);
	      for(int z = 0; z < vreceived.size(); z++) {
		Received received = vreceived.at(z);
		if(received.receiver == loc.node && received.sender == sent.node && received.time > sent.time && received.time < sent.time + 1000) {
		  if(sent.node == 0 && loc.node == 89) printf("it was received by %d at %lu\n", received.receiver, received.time);
		  isreceived = true;
		}
	      }
	      if(!isreceived) {
		cout << sent.node << " " << loc.node << " " << sent.time << " " << sqrt(dx * dx + dy * dy) << endl;
	      }
	    }
	  }
	}
      }
    }
  }
}

