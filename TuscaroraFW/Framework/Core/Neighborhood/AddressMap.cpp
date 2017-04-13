////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include "AddressMap.h"


bool AddressMap::AddNodeID(string wfAddress, NodeId_t nid, WaveformId_t wid)
{
	LinkId link(nid, wid);
	return map.Insert(link,wfAddress);
	
}


NodeId_t AddressMap::GenerateNodeId(string wfAddress, WaveformId_t wid)
{
	const char *addr_c = wfAddress.c_str();
	int size = wfAddress.size();
	
	NodeId_t *id = (NodeId_t *) &addr_c[size-2];
	
	return *id;
}


LinkId AddressMap::LookUpLinkId(string wfAddress)
{
	BiMapAddrNodeId_t::Iterator2 it= map.Find2(wfAddress);
	LinkId ret = LinkId(NULL_NODE_ID,NULL_WF_ID);
	if(it != map.End2()) {ret = it->First();}
	return ret;
}


string AddressMap::LookUpWfAddress(LinkId link)
{
	BiMapAddrNodeId_t::Iterator1 it= map.Find1(link);
	if(it !=map.End1()) return it->Second();
	return string("NULL");
}
