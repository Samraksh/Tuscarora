////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef ADDRESS_MAP_H_
#define ADDRESS_MAP_H_

//#include <unistd.h>

#include <Interfaces/Core/Link.h>
#include <Base/Delegate.h>
//#include "Lib/DS/BSTMapT.h"
#include <string>
#include "Lib/DS/BiBSTMapT.h"

using namespace Types;
using namespace std;
using namespace Core;

namespace Core {
	

class LinkIdComparator{
public:
	static bool LessThan (const LinkId& A, const LinkId& B) {
		return A < B;
	}
	static bool EqualTo (const LinkId& A, const LinkId& B) {
		return A == B;
	}
};
	
class StringComparator{
public:
	static bool LessThan (const string& A, const string& B) {
		if (A.compare(B) < 0) return true;
		return false;
	}
	static bool EqualTo (const string& A, const string& B) {
		if (A.compare(B) == 0) return true;
		return false;
	}
};

typedef BiBSTMapT<LinkId, string, StringComparator, LinkIdComparator> BiMapAddrNodeId_t;

class AddressMap {
	BiMapAddrNodeId_t map;

public:

	LinkId LookUpLinkId(string wfAddress);
	bool AddNodeID(string wfAddress, NodeId_t nid, WaveformId_t wid);
	string LookUpWfAddress(LinkId link);
	//uint64_t LookUpWFAddress(NodeId_t nid);
	NodeId_t GenerateNodeId (string wfAddress, WaveformId_t wid);
	
};

} //End namespace


#endif //ADDRESS_MAPPER_H_
