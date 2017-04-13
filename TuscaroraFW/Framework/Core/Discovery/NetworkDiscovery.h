////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef NETWORKDISCOVERY_H_
#define NETWORKDISCOVERY_H_

#include "Framework/Core/Neighborhood/NeighborTable.h"
#include <Interfaces/Core/NetworkDiscoveryI.h>
#include "Lib/Misc/MemMap.h"
#include "../Neighborhood/PotentialNeighborRegistry.h"
#include <Sys/RuntimeOpts.h>
#include <stdlib.h>

extern NodeId_t MY_NODE_ID;

using namespace PAL;
using namespace PWI;

namespace Core {
namespace Discovery {
  
enum NetworkDiscoveryTypeE {
GLOBAL_DISCOVERY, 
ORACLE_DISCOVERY, 
LONG_LINK_DISCOVERY, 
ZSC_DISCOVERY,
FLOOD_DISCOVERY,
NULL_DISCOVERY
};

class NetworkDiscovery : public NetworkDiscoveryI{
protected:
PotentialNeighborRegistry nbrs; ///< Potential Neighbor Table
PotentialNeighborDelegate *del;  ///< Delegate to be informed when nodes become a new potential neighbor and are no longer potential neighbors

public:
~NetworkDiscovery() {};

///Processing incoming network discovery messages
virtual void ProcessMessage(FMessage_t& msg)=0;

	static bool hasWaveform(WaveformId_t wfid) {
		return hasWaveform(wfid, MY_NODE_ID);
	}

	static void GetLine(std::ifstream &file, string &type, string &itype, string &estType, string &cost,string &energy,string &range){
		getline(file, type, ' ');
		getline(file, itype, ' ');
		getline(file, estType, ' ');
		getline(file, cost, ' ');
		getline(file, energy, ' ');
		getline(file, range);
	}
	
	static bool hasWaveform(WaveformId_t wfid, NodeId_t nodeid) {
		std::ifstream file;
		file.open ("waveform.cnf");
		int id;
		string tmp;
		getline(file, tmp, ' ');
		while(!file.eof()) {
			id = atoi(tmp.c_str());
			string type, itype, estType, cost, energy, range;
			GetLine(file, type, itype, estType, cost, energy, range);
			if(id == wfid) {
				file.close();
				return inRange(range, nodeid);
			}
			getline(file, tmp, ' ');
		}
		file.close();
		return false;
	}

	static const char* getWaveformType(WaveformId_t wfid) {
		std::ifstream file;
		file.open ("waveform.cnf");
		int id;
		string tmp;
		getline(file, tmp, ' ');
		while(!file.eof()) {
			id = atoi(tmp.c_str());
			string type, itype, estType, cost, energy, range;
			GetLine(file, type, itype, estType, cost, energy, range);
			if(id == wfid) {
				file.close();
				return type.c_str();
			}
			getline(file, tmp, ' ');
		}
		file.close();
		return 0;
	}

	static const char* getWaveformEstimationType(WaveformId_t wfid) {
		std::ifstream file;
		file.open ("waveform.cnf");
		int id;
		string tmp;
		getline(file, tmp, ' ');
		while(!file.eof()) {
			id = atoi(tmp.c_str());
			string type, itype, estType, cost, energy, range;
			GetLine(file, type, itype, estType, cost, energy, range);
			if(id == wfid) {
				file.close();
				return estType.c_str();
			}
			getline(file, tmp, ' ');
		}
		file.close();
		return 0;
	}

	static const char* getWaveformCost(WaveformId_t wfid) {
		std::ifstream file;
		file.open ("waveform.cnf");
		int id;
		string tmp;
		getline(file, tmp, ' ');
		while(!file.eof()) {
			id = atoi(tmp.c_str());
			string type, itype, estType, cost, energy, range;
			GetLine(file, type, itype, estType, cost, energy, range);
			if(id == wfid) {
				file.close();
				return cost.c_str();
			}
			getline(file, tmp, ' ');
		}
		file.close();
		return 0;
	}

	static const char* getWaveformEnergy(WaveformId_t wfid) {
		std::ifstream file;
		file.open ("waveform.cnf");
		int id;
		string tmp;
		getline(file, tmp, ' ');
		while(!file.eof()) {
			id = atoi(tmp.c_str());
			string type, itype, estType, cost, energy, range;
			GetLine(file, type, itype, estType, cost, energy, range);
			if(id == wfid) {
				file.close();
				return energy.c_str();
			}
			getline(file, tmp, ' ');
		}
		file.close();
		return 0;
	}
	
	
	static bool inRange(string init, NodeId_t id) {
		char cinit_[init.length()];
		char cinit[init.length()];
		
		init.copy(cinit, init.length());
		cinit[init.length()] = '\0';
		strcpy(cinit_, cinit);
		
		uint start = 0;
		for(uint x = 0; x <= init.length(); x++) {
			if(cinit[x] == ' ' || cinit[x] == ',' || cinit[x] == '\0' || x == init.length()) {
				uint end = x;
				int first = 0;
				int second = 0;
				char buffer[10];
				memset(buffer, '\0', 10);
				memcpy(buffer, &cinit[start], end - start);
				for(uint xx = start; xx < end; xx++) {
				if(cinit[xx] == '-') {
					memset(buffer, '\0', 10);
					memcpy(buffer, &cinit[start], xx - start);
					first = atoi(buffer);
					memset(buffer, '\0', 10);
					memcpy(buffer, &cinit[xx + 1], end - xx);
					second = atoi(buffer);
					if(first <= id && second >= id) return true;
				} else if(xx + 1 == end) {
					memset(buffer, '\0', 10);
					memcpy(buffer, &cinit[start], end - start);
					int first = atoi(buffer);
					if(first == id) return true;
				}
				}
				start = end + 1;
			}
		}
		return false;
	}

	///Delegate registration for Network Change information. 
	bool RegisterDelegate (PotentialNeighborDelegate& delegate) { del = &delegate; return true; }
	uint16_t NetworkSize(uint16_t waveformId) { return 0; }
	/// Returns the number of potential neighbors
	uint16_t NumberOfPotentialNeighbors(NodeId_t node) { return nbrs.GetNumberOfNodes(); }
	/// Begins the network discovery module
	virtual void Start()=0;
	uint16_t PotentialNeighborList(NodeId_t node, NodeInfo *buf, uint16_t bufSize) { return 0;}
}; //end of class

}//End of namespace
}

#endif	//NETWORKDISCOVERY_H_
