////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include "LinkEstimationTable.h"
//#include "NodeSchedule.h"
#include <Misc/ScheduleI.h>
#include "Lib/PAL/PAL_Lib.h"

//#include <map>
using namespace std;

// extern bool DBG_CORE; // set in Lib/Misc/Debug.cpp

namespace Core {
namespace Estimation {

template <class WFAddressType>
LinkEstimationTable<WFAddressType>::LinkEstimationTable() 
{
	nbrs = new Estimation_t<WFAddressType>();
}

template <class WFAddressType>
EstimationInfo<WFAddressType>* LinkEstimationTable<WFAddressType>::GetEstimationInfo(WFAddressType nodeId, WaveformId_t wfId) 
{
	WF_LinkId<WFAddressType> linkid(nodeId, wfId);
	typename Estimation_t<WFAddressType>::iterator it = nbrs->find(linkid);
	if(it!=nbrs->end()) return &(it->second);
	else return 0;
}

template <class WFAddressType>
void LinkEstimationTable<WFAddressType>::SetLink(WFAddressType nodeId, WaveformId_t wfId, Link *link) 
{
	//printf("setting for %u\n", id);
	WF_LinkId<WFAddressType> linkid(nodeId, wfId);
	nbrs->operator[](linkid).link = link;
}

template <class WFAddressType>
EstimationInfo<WFAddressType>* LinkEstimationTable<WFAddressType>::AddNeighbor(WFAddressType nodeId, WaveformId_t wfId) 
{
	WF_LinkId<WFAddressType> linkid(nodeId, wfId);
	nbrs->operator[](linkid).linkAddress = nodeId;
	//printf("Adding for %u\n", id);
	logger.LogEvent(PAL::LINK_ADDED, nodeId);
	return &(nbrs->at(linkid));
}
    
template <class WFAddressType>
int32_t LinkEstimationTable<WFAddressType>::CheckExpiration(Delegate<void, WF_LinkEstimationParam<WFAddressType> > &del) 
{
	struct timeval curtime;
	gettimeofday(&curtime,NULL);
	uint64_t cur = (uint64_t)curtime.tv_sec * 1000000 + curtime.tv_usec;
	typename Estimation_t<WFAddressType>::iterator ii = nbrs->begin();
	//printf("LinkEstiamtionTable: my ptr is %p No of elements in LE Table %d\n", this, (int)nbrs->size()); fflush(stdout);
	while(ii != nbrs->end()) {
		//printf("Checking the expiry of %d, %d\n", ii->first, ii->second.linkAddress); fflush(stdout);
		//if(cur > (nbrs->at(ii->first).expiryTime) && ii->second.expiryTime != 0) {
		if(cur >  ii->second.expiryTime ) {
			//Invalidating the link because it has expired, and no new beacons have been observed
			WF_LinkEstimationParam_n64_t _param;
			//NeighborUpdateParam _param;
			_param.changeType = NBR_DEAD;
			_param.linkAddress = ii->first.nodeId;
			_param.wfid = ii->first.waveformId;
			_param.metrics = ii->second.link->metrics;
			_param.type = ii->second.link->type;
			
			//_param.link = *(ii->second.link);
			//_param.linkAddress = ii->first.linkAddress;
			(&del)->operator ()(_param);
			RemoveNeighbor(ii->first.nodeId, ii->first.waveformId);
			//printf("%u shoudll have expured\n", ii->first);fflush(stdout);
		} else if(cur > ii->second.expiryTime) 
		{
			//printf("Skipped %u since its an empty expiry\n", ii->first);
		}
		//printf("Iterating %d\n", ii->second.linkAddress);fflush(stdout);
		ii++;
	}
	return -1;
}

template <class WFAddressType>
void LinkEstimationTable<WFAddressType>::UpdateExpiration(WFAddressType nodeId, WaveformId_t wfId, uint64_t expiry) {
	//printf("updating for table %p for nbr %u\n", nbrs, id); fflush(stdout);
	WF_LinkId<WFAddressType> linkid(nodeId, wfId);
	typename Estimation_t<WFAddressType>::iterator ii = nbrs->find(linkid);
	if (ii!=nbrs->end()){
		nbrs->operator[](linkid).expiryTime = expiry;
	}else {
		printf("Neigbhor not found\n");fflush(stdout);
	}

	logger.LogEvent(PAL::LINK_UPDATED, nodeId);
}

template <class WFAddressType>
void LinkEstimationTable<WFAddressType>::RemoveNeighbor(WFAddressType nodeId, WaveformId_t wfId) {
	WF_LinkId<WFAddressType> linkid(nodeId, wfId);
	logger.LogEvent(PAL::LINK_REMOVED, nodeId);
	nbrs->erase(linkid);
}

template class LinkEstimationTable<uint64_t>;

}//End of namespace
}
