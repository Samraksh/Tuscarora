////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include "CustomPatternNeighborTable.h"
//#include "PatternNeighborIterator.h"


namespace PWI {
namespace Neighborhood {

//QualityComparator CustomPatternNeighborTable::defualtLinkComparator;

CustomPatternNeighborTable::CustomPatternNeighborTable(NeighborTable* _coreTable, PatternId_t _patternId, PatternEventDispatch_I *_eventDispatcher)
{
	coreNbrTable = _coreTable;
	eventDispatch = _eventDispatcher;
	patternId = _patternId ;
	nbrhood= new PNbrHood();

	//Set default threshold for links
	threshold.energy = 1000000;
	threshold.quality =0;
	threshold.dataRate =0;
	threshold.cost = 1000000;
	threshold.avgLatency = 1000000;
	//threshold.expLatencyToXmit = 1000000;

	currentLCType = QUALITY_LC;
	comparator = new QualityComparator();
	//comparator = &defualtLinkComparator;



	PopulateNeighbors();
}

uint16_t CustomPatternNeighborTable::GetNumberOfNeighbors(){
	return nbrhood->Size();
}

uint8_t CustomPatternNeighborTable::GetNumberOfWaveform()
{

	return noOfWfs;
}

uint8_t CustomPatternNeighborTable::GetWaveformIds(WaveformId_t* wIdArray)
{

	return noOfWfs;
}

void CustomPatternNeighborTable::SetNeighborThreshold(LinkMetrics _threshold)
{
	threshold = _threshold;
}

void CustomPatternNeighborTable::SetLinkComparatorType(LinkComparatorTypeE lcType)
{
	//This is a static object!!! Cannot delete Just overwrite//free the current comparator
	//if(comparator != NULL && comparator != &defualtLinkComparator ){
	if(comparator != NULL){
		switch(currentLCType){
		case QUALITY_LC:
			delete (static_cast <QualityComparator*>(comparator));
			break;
		case COST_LC:
			delete (static_cast <CostComparator*>(comparator));
			break;
		case QUAL_XMIT_DELAY_LC:
			delete (static_cast <QualAndXmitDelayComparator*>(comparator));
			break;
		default:
			printf("CustomPatternNeighborTable::SetLinkComparatorType(): currentLCType is not known!!\n");
			assert(comparator==NULL);
			//delete (static_cast <QualityComparator*>(comparator)); //BK:
			break;
		}
	}

	//create a new one.
	LinkMapComparatorI *_comp;

	switch(lcType){
	case QUALITY_LC:
		_comp= new QualityComparator();
		break;
	case COST_LC:
		_comp= new CostComparator();
		break;
	case QUAL_XMIT_DELAY_LC:
		_comp= new QualAndXmitDelayComparator();
		break;
	default:
		_comp= new QualityComparator();
		break;
	}
	comparator = _comp;

}

LinkComparatorTypeE CustomPatternNeighborTable::GetLinkComparatorType(){
	return currentLCType;
}
/*
void CustomPatternNeighborTable::SetLinkComparator(LinkComparatorI& _comparator)
{
  comparator = &_comparator;
}
 */

Link* CustomPatternNeighborTable::GetNeighborLink(NodeId_t nodeId){
	//return 0;
	return coreNbrTable->GetNeighborLink(nodeId);
}
NodeId_t CustomPatternNeighborTable::GetNeighborID(uint16_t table_index){
	return coreNbrTable->GetNeighborId(table_index);
}


uint16_t CustomPatternNeighborTable::PopulateNeighbors()
{
	uint16_t nbrCount = coreNbrTable->Size();
	printf("CustomPatternNeighborTable:: Populating the patterns table: %d neighbors\n",nbrCount);
	if(nbrCount ==0 ) {return 0;}

	printf("Core neighbor table has %d neighbors\n",nbrCount);

	Link *nbrs[nbrCount];
	coreNbrTable->GetNeighborLinkSet(nbrs);
	for (int i =0; i < nbrCount ; i++) {
		if(comparator->BetterThan (nbrs[i]->metrics, threshold)){
			LinkMap *newLink = new LinkMap(nbrs[i]->linkId, *(nbrs[i]));
			nbrhood->Insert(newLink);
		}
	}
	return 0;
}

void CustomPatternNeighborTable::NeighborUpdate(NeighborUpdateParam _param)
{
	bool signalPattern=false;
	switch(_param.changeType){
	case NBR_NEW:
		if(comparator->BetterThan (_param.link.metrics, threshold)){
			LinkMap *newLink = new LinkMap(_param.link.linkId, _param.link);
			nbrhood->Insert(newLink);
			signalPattern =true;
		}
		break;
	case NBR_DEAD:
		if(nbrhood->DeleteLink(_param.link.linkId)){
			signalPattern =true;
		}
		break;
	case NBR_UPDATE:
		if(comparator->BetterThan (_param.link.metrics, threshold)){
			//If this guy just crossed the threshold we need to insert him in PTN.

			Link ptnLink = nbrhood->GetLink(_param.link.linkId);
			if(ptnLink.linkId.waveformId != NULL_WF_ID){
				//printf("Link in PNT is %p, link got from core is %p \n",ptnLink, _param.link);
				//He is already in PTN. but we might have to resort the table, because this link has changed.
				//TODO:: Implement resort on the list
				//nbrhood->Sort();
			}
			else {
				//Looks like this guy just qualified.
				LinkMap *newLink = new LinkMap(_param.link.linkId, _param.link);
				nbrhood->Insert(newLink);
				_param.changeType = NBR_NEW;
			}
			signalPattern =true;
		}
		else {
			//check if this guy is there in ptn neighborhood already
			Link ptnLink = nbrhood->GetLink(_param.link.linkId);
			if (ptnLink.linkId.nodeId == NULL_NODE_ID){

			}
			else{
				_param.changeType = NBR_DEAD;
				signalPattern =true;
				nbrhood->DeleteLink(ptnLink.linkId);
			}

		}
		break;
	default:
		printf("CustomPatternNeighborTable:: Error: wrong neighbor update signal\n");
		break;
	}

	if(signalPattern) {
		//printf("CustomPatternNeighborTable:: Invoking the pattern neighbor event for pattern %d\n", patternId);
		eventDispatch->InvokeNeighborUpdateEvent(patternId, _param);
		//pattern->nbrDelegate->operator()(_param);
	}

	//PopulateNeighbors();
}



}//End of namespace
}

