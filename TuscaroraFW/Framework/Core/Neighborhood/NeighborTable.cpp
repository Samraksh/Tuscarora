////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include "NeighborTable.h"
#include "../Discovery/NodeSchedule.h"
#include <Misc/ScheduleI.h>
#include "Lib/PAL/PAL_Lib.h"
#include "Framework/Core/DataFlow/CoreTypes.h"
#include <cmath>

//#include <map>
using namespace std;

// extern bool DBG_CORE_ESTIMATION; // set in Lib/Misc/Debug.cpp
// extern bool DBG_CORE; // set in Lib/Misc/Debug.cpp

namespace Core {
namespace Neighborhood {

/* uint16_t NeighborTable::GetNbrTableSize() {
    if( nbrs.empty()) return 0;
    else return (uint16_t) nbrs.size();
  }
 */

uint16_t NeighborTable::SizeOnWF(WaveformId_t wid){
    uint16_t goodNbr=0;
    if( nbrs.Size() == 0) return goodNbr;
    else {
        for(Neighbors_t::Iterator it = nbrs.Begin(); it!=nbrs.End(); ++it) {
            Link *link = it->Second();
            if(link->metrics.quality > 0 && link->linkId.waveformId == wid) {
                goodNbr++;
            }
        }
    }
    return goodNbr;
}

uint16_t NeighborTable::Size(){
    uint16_t goodNbr=0;
    if( nbrs.Size() == 0) return goodNbr;
    else {
        for(Neighbors_t::Iterator it = nbrs.Begin(); it!=nbrs.End(); ++it) {
            Link *link = it->Second();
            if(link->metrics.quality > 0) {
                goodNbr++;
            }
        }
    }
    return goodNbr;
}

uint16_t NeighborTable::GetNeighborsIds(NodeId_t *nodeIds){
    uint16_t goodNbr=0;
    if( nbrs.Size() == 0) return goodNbr;
    else {
        for(Neighbors_t::Iterator it = nbrs.Begin(); it!=nbrs.End(); ++it) {
            Link *link = it->Second();
            if(link->metrics.quality > 0) {
                nodeIds[goodNbr]=link->linkId.nodeId;
                goodNbr++;;
            }
        }
    }
    return goodNbr;
}

uint16_t NeighborTable::GetNeighborLinkSet(Link **_links){
    uint16_t goodNbr=0;
    if( nbrs.Size() == 0) return goodNbr;
    else {
        for(Neighbors_t::Iterator it = nbrs.Begin(); it!=nbrs.End(); ++it) {
            Link *link = it->Second();
            if(link->metrics.quality > 0) {
                _links[goodNbr] = link;
                goodNbr++;;
            }
        }
    }
    return goodNbr;
}


uint16_t NeighborTable::GetNeighborLinkSet(Link **_links, WaveformId_t wid){
    uint16_t goodNbr=0;
    if( nbrs.Size() == 0) return goodNbr;
    else {
        for(Neighbors_t::Iterator it = nbrs.Begin(); it!=nbrs.End(); ++it) {
            Link *link = it->Second();
            if(link->metrics.quality > 0 && link->linkId.waveformId == wid) {
                _links[goodNbr] = link;
                goodNbr++;;
            }
        }
    }
    return goodNbr;
}


NodeId_t NeighborTable::GetNeighborId(uint16_t table_index) {
    int x = 0;
    if(table_index > nbrs.Size()-1){
        Debug_Printf(DBG_CORE_ESTIMATION,"Neighbor:: GetNeighborID:: bad index %d\n", table_index);
        return 0;
    }
    for(Neighbors_t::Iterator it = nbrs.Begin(); it!=nbrs.End(); ++it) {
        if(x == table_index) {
            return (it->First().nodeId);
        }
        x++;
    }
    return 0;
}

/*Link* NeighborTable::GetBestQualityLink(NodeId_t nodeId)
{
    Link* links[MAX_WAVEFORMS];
    uint16_t found=0;
    uint16_t best_index=0;
    for (WaveformId_t i=1; i <= MAX_WAVEFORMS; i++){
        links[i]= GetNeighborLink(nodeId, i);
        if(links[i]){
            found++;
            if(found==1){
                best_index=i;
            }
            else {
                if(links[best_index]->metrics.quality < links[i]->metrics.quality){
                    best_index=i;
                }
            }
        }
    }

    if(found){
        return links[best_index];
    }
    return NULL;
}*/

Link* NeighborTable::GetLinkwComp(NodeId_t nodeId, LinkComparatorI &comparator)
{
    Link* links[MAX_WAVEFORMS];
    uint16_t found=0;
    uint16_t best_index=0;
    for (WaveformId_t i=1; i <= MAX_WAVEFORMS; i++){
        links[i]= GetNeighborLink(nodeId, i);
        if(links[i]){
            found++;
            if(found==1){
                best_index=i;
            }
            else {
                if(comparator.BetterThan(links[i]->metrics,links[best_index]->metrics)){
                    best_index=i;
                }
            }
        }
    }
    if(found){
        return links[best_index];
    }
    return NULL;
}

Link* NeighborTable::GetNeighborLink(NodeId_t nodeId) {
    return GetNeighborLink(nodeId, NULL_WF_ID);
}

Link* NeighborTable::GetNeighborLink(NodeId_t nodeId, WaveformId_t wid)
{
    LinkId linkId(nodeId, wid);
    Neighbors_t::Iterator it = nbrs.Find(linkId);
    if(it!=nbrs.End()) return (it->Second());
    else return 0;
}

bool NeighborTable::LinkExists(NodeId_t id) {
    LinkId linkId(id);
    return nbrs[linkId]->metrics.quality > 0;
}

Link* NeighborTable::AddNeighbor(NodeId_t id,  WaveformId_t wid, LinkMetrics _metrics) {
    LinkId linkId(id,wid);
    nbrs[linkId]= new Link();
    nbrs[linkId]->type = TwoWay;
    nbrs[linkId]->linkId.nodeId=id;
    nbrs[linkId]->linkId.waveformId=wid;
    nbrs[linkId]->metrics = _metrics;
    Debug_Printf(DBG_CORE,"Neighbor Table:: Adding new neighbor::  %d on waveform %d with quality %f, %f\n", id, wid,_metrics.quality, nbrs[linkId]->metrics.quality);
    return nbrs[linkId];
}

bool NeighborTable::UpdateNeighbor(NodeId_t id, WaveformId_t wid, LinkMetrics _metrics)
{
    LinkId linkId(id,wid);
    //if(nbrs[linkId]){
    if(nbrs.Find(linkId) != nbrs.End()){
        Debug_Printf(DBG_CORE,"Neighbor Table:: Updating neighbor::  %d on waveform %d with quality %f\n", id, wid, nbrs[linkId]->metrics.quality);fflush(stdout);
    }else{
        Debug_Printf(DBG_CORE,"Neighbor Table:: Adding new neighbor::  %d on waveform %d\n", id, wid);fflush(stdout);
        nbrs[linkId]= new Link();
        nbrs[linkId]->type = TwoWay;
        nbrs[linkId]->linkId.nodeId=id;
        nbrs[linkId]->linkId.waveformId=wid;
    }
    nbrs[linkId]->metrics = _metrics;
    return true;
}

//Updates the neighbor in some simple way. right now just increments the quality by 0.1
bool NeighborTable::TouchNeighbor(NodeId_t id,  WaveformId_t wid){
    LinkId linkId(id,wid);
    Neighbors_t::Iterator ii= nbrs.Find(linkId);

    if(ii != nbrs.End()){
        Link *lnk = ii->Second();
        lnk->metrics.quality += 0.1;
        Debug_Printf(DBG_CORE_ESTIMATION,"Neighbor Table:: TouchNeighbor::  %d on waveform %d with quality %f\n", id, wid, lnk->metrics.quality);
        if(lnk->metrics.quality > 1.0) {lnk->metrics.quality=1.0;}
    }else{
        Debug_Printf(DBG_CORE,"Neighbor Table:: TouchNeighbor:: Received a pkt from node not in table. Adding new neighbor::  %d on waveform %d\n", id, wid);
        LinkMetrics metrics;
        metrics.quality=0.1;
        UpdateNeighbor(id, wid, metrics);
    }

    return true;
}


void NeighborTable::RemoveNeighbor(LinkId id) {
    //LinkId linkId(id.nodeId, id.waveformId);
	 Neighbors_t::Iterator it = nbrs.Find(id);
	if(it != nbrs.End()){
		delete(it->Second());
		nbrs.Erase(it);
		Debug_Printf(DBG_CORE,"Neighbor Table:: Removed nbr link,  nodeid: %d, waveform %d \n", id.nodeId, id.waveformId);
	}
	else {
		Debug_Printf(DBG_ERROR,"Neighbor Table:: Framework issued a delete link, but didnot find neighbor,  nodeid: %d, on  waveform %d \n", id.nodeId, id.waveformId);
	}
}
   

uint16_t NeighborTable::GetPendingPackets(uint16_t neighbor){
    return 0;
}


bool NeighborTable::UpdateTransmissionDelay(WaveformId_t wid, NodeId_t nid, double newDelayMicro)
{
	Link *link = GetNeighborLink(nid, wid);
	if(link ==0){
		return false;
	}
	link->metrics.avgLatency=CalculateAvgDelay(link->metrics.avgLatency, newDelayMicro);
	return true;
}


bool NeighborTable::UpdateTransmissionDelay(WaveformId_t wid, double newDelayMicro)
{
	uint16_t nbrSize = SizeOnWF(wid);
	Link* linkset[nbrSize];
	GetNeighborLinkSet(linkset,wid);
	for (int i=0; i< nbrSize; i++){
		Link *link = linkset[i];
		if(link ==0){
			continue;
		}
		link->metrics.avgLatency=CalculateAvgDelay(link->metrics.avgLatency, newDelayMicro);
	}
	return true;
}


double NeighborTable::CalculateAvgDelay (double oldDelayMicroLog, double updateDelayMicro)
{
	double rval=0;
	UFixed_7_8_t weightage = 0.75;
	if(oldDelayMicroLog == 0){
		Debug_Printf(DBG_CORE, "NeighborTable:: CalculateAvgDelay: Current dealy is %f, new delay is %f\n", oldDelayMicroLog, updateDelayMicro);
		rval = updateDelayMicro;
	}
	else {
		rval = pow(2, oldDelayMicroLog);
		rval = rval * weightage + updateDelayMicro * (1-weightage); //Exponentially weighted moving average
	}
	if(rval!=0) {rval = log2(rval);}
	return rval;
}

}//End of namespace
}
