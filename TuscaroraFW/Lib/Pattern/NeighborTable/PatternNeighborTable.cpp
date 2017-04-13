////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include "PatternNeighborTable.h"


Patterns::PatternNeighborTable::PatternNeighborTable(LinkComparatorTypeE  lcType)
: PatternNeighborTableI(lcType)
{
	switch(lcType){
	case QUALITY_LC:
		nbrhood = new PtnNbrHood<QualityComparator>();
		break;
	case COST_LC:
		nbrhood = new PtnNbrHood<CostComparator>();
		break;
	case QUAL_XMIT_DELAY_LC:
		nbrhood = new PtnNbrHood<QualAndXmitDelayComparator>();
		break;
	default:
		nbrhood = new PtnNbrHood<QualityComparator>();
		break;
	}
}

PatternNeighborIterator Patterns::PatternNeighborTable::Begin()
{
	PatternNeighborIterator *rtn;
	//PopulateNeighbors();
	if(nbrhood->Size() > 0) {
		rtn = new PatternNeighborIterator(this, 0);
	}else {
		rtn = new PatternNeighborIterator();
	}
	return *rtn;

}

PatternNeighborIterator Patterns::PatternNeighborTable::End()
{
	PatternNeighborIterator *rtn;
	//PopulateNeighbors();
	uint16_t size = nbrhood->Size();
	if(size > 0) {
		//printf("Returning an iterator pointing to node %d\n", size-1);
		rtn = new PatternNeighborIterator(this,size-1);
	}else {
		rtn = new PatternNeighborIterator();
	}
	return *rtn;
}


NodeId_t Patterns::PatternNeighborTable::GetNeighborID(uint16_t table_index)
{
	return 0;
}


Link* Patterns::PatternNeighborTable::GetNeighborLink(NodeId_t nodeId)
{
	LinkId id(nodeId);
	return nbrhood->GetLink(id);
}

uint16_t Patterns::PatternNeighborTable::GetNumberOfNeighbors()
{
	return nbrhood->Size();
}

Link* PatternNeighborTable::GetBestLink(WaveformId_t wid)
{
	return nbrhood->GetBestLink(wid);
}

void Patterns::PatternNeighborTable::UpdateTable(NeighborUpdateParam _param)
{
	//bool signalPattern=false;
	LinkMap *newLink;Link *ptnLink;
	switch(_param.changeType)
	{
	case NBR_NEW:
		newLink = new LinkMap(_param.link.linkId, _param.link);
		nbrhood->Insert(newLink);
		//signalPattern =true;
		break;
	case NBR_DEAD:
		if(nbrhood->DeleteLink(_param.link.linkId)){
			//signalPattern =true;
		}
		break;
	case NBR_UPDATE:
		//Debug_Printf(DBG_PATTERN, "PatternNeighborTable:: updating neighbor %d on waveform %d\n", _param.nodeId, _param.link.linkId.waveformId);fflush (stdout);
		ptnLink = nbrhood->GetLink(_param.link.linkId);
		if(ptnLink){
			if(ptnLink->linkId.waveformId){
				ptnLink->metrics = _param.link.metrics;
			}
		}else {
			printf("Mismatch in neighbor table status, I (Pattern %d) dont have link (%d, %d) in table, but received an update event \n Created new link\n",
					this->patternId,_param.nodeId, _param.link.linkId.waveformId);
			newLink = new LinkMap(_param.link.linkId, _param.link);
			nbrhood->Insert(newLink);
		}

		break;
	default:
		printf("PatternNeighborTable:: Error: wrong neighbor update signal\n");
		break;
	}

}


void Patterns::PatternNeighborTable::PrintTable(){
	Debug_Printf(DBG_PATTERN,"--PatternNeighborTable--\n" );
	uint16_t nbrCount = GetNumberOfNeighbors();
	PatternNeighborIterator it = Begin();
	for(uint16_t ii=0; ii< nbrCount; ++ii) {
		if(ii != 0) it=it.GetNext();
		Debug_Printf(DBG_PATTERN,"\t\t (%d)  neighbor Id is %d, on waveform %d, quality = %0.5f, energy =  %0.5f cost =  %0.5f\n"
				,ii, it->linkId.nodeId, it->linkId.waveformId, it->metrics.quality, it->metrics.energy, it->metrics.cost);fflush (stdout);

	}
	Debug_Printf(DBG_PATTERN,"--End of PatternNeighborTable--\n");fflush (stdout);

}
