////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef NS3_WAVEOFRM_SPEC_H_
#define NS3_WAVEOFRM_SPEC_H_

#include <Sys/RuntimeOpts.h>

extern NodeId_t MY_NODE_ID;

namespace ExternalServices {

class NS3WaveformSpec {
public:
	static bool HasWaveform(WaveformId_t wfid) {
		return HasWaveform(wfid, MY_NODE_ID);
	}

	static void GetLine(std::ifstream &file, string &type, string &itype, string &estType, string &cost,string &energy,string &range){
		getline(file, type, ' ');
		getline(file, itype, ' ');
		getline(file, estType, ' ');
		getline(file, cost, ' ');
		getline(file, energy, ' ');
		getline(file, range);
	}
	
	static bool HasWaveform(WaveformId_t wfid, NodeId_t nodeid) {
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
				return InRange(range, nodeid);
			}
			getline(file, tmp, ' ');
		}
		file.close();
		return false;
	}

	static const char* GetWaveformType(WaveformId_t wfid) {
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

	static const char* GetWaveformEstimationType(WaveformId_t wfid) {
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

	static const char* GetWaveformCost(WaveformId_t wfid) {
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

	static const char* GetWaveformEnergy(WaveformId_t wfid) {
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
	
	
	static bool InRange(string init, NodeId_t id) {
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

	uint16_t NetworkSize(uint16_t waveformId) { return 0; }

};

}//End of namespace

#endif
