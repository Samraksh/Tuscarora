////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#include "PI_NeighborManager.h"

namespace PWI {
namespace Neighborhood {


void PI_NeighborManager::InitializeNeighborhood(PatternId_t pid)
{
  CustomPatternNeighborTable *table = new CustomPatternNeighborTable(corNbrTable, pid, eventDispatcher);
  patternTables[pid]= table;
}

void PI_NeighborManager::SetNeighborThreshold(PatternId_t pid, LinkMetrics* threshold)
{
	PatternToPNT_Map_t::Iterator it= patternTables.Find(pid);
	if(it != patternTables.End()){
		patternTables[pid]->SetNeighborThreshold(*threshold);
	}else {
		printf("PI_NeighborManager::SetNeighborThreshold: No custom pattern neighbor table found for pattern id %d\n", pid);
	}
}


void PI_NeighborManager::SetLinkComparator(PatternId_t pid, LinkComparatorTypeE compType)
{
	PatternToPNT_Map_t::Iterator it= patternTables.Find(pid);
	if(it != patternTables.End()){
		patternTables[pid]->SetLinkComparatorType(compType); 
	}else {
		printf("PI_NeighborManager::SetLinkComparator: No custom pattern neighbor table found for pattern id %d\n", pid);
	}
}

LinkComparatorTypeE PI_NeighborManager::GetLinkComparator(PatternId_t pid)
{
	PatternToPNT_Map_t::Iterator it= patternTables.Find(pid);
	if(it != patternTables.End()){
		return (patternTables[pid]->GetLinkComparatorType() ); 
	}else {
		printf("PI_NeighborManager::GetLinkComparator: No custom pattern neighbor table found for pattern id %d\n", pid);
	}
  return QUALITY_LC;
}

void PI_NeighborManager::SetCoreNbrTable(NeighborTable &table){
  corNbrTable = &table;
}


void PI_NeighborManager::SetPatternNbrTable(uint16_t patternId, CustomPatternNeighborTable &table){
  patternTables[patternId]= &table;
}


CustomPatternNeighborTable& PI_NeighborManager::GetNeighborTable(uint16_t patternId){
  return *patternTables[patternId];
}


void PI_NeighborManager::UpdatePatternTables(NeighborUpdateParam _param)
{
  //uint8_t pcount = patternTables.count();

  for (PatternToPNT_Map_t::Iterator it = patternTables.Begin(); it != patternTables.End(); ++it){
    //printf("NeighborManager:: Updating the neighbor table of %d pattern\n", it->First());
    it->Second()->NeighborUpdate(_param);
  } 
}

} //end namespace
}
