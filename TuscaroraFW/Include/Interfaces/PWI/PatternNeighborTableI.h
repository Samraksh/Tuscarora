////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef PATTERNNEIGHBORTABLEI_H_
#define PATTERNNEIGHBORTABLEI_H_

#include <Base/BasicTypes.h>
#include <Base/FrameworkTypes.h>
#include <Interfaces/Core/Node.h>
#include <Interfaces/Core/Link.h>
#include <Interfaces/PWI/LinkCompartorI.h>

using namespace PAL;
using namespace PWI::Neighborhood;

///Patern Writers Interface definition and types
namespace Patterns {
   
  /*struct PatternNeighborInfo : public Core::NodeInfo{ 
  public:
    uint64_t bytesPending;
    uint64_t pktsPending;
  };*/

  
  class PatternNeighborIterator;
  
  ///Interface definition for the Neighbor service, that maintains a sorted list of neighbors based on their metric
  class PatternNeighborTableI{
    
  public:   
    PatternNeighborTableI (LinkComparatorTypeE  lcType){}
    ///Sets the comparator used to sort the neighbors
    //virtual void SetLinkComparator (LinkComparatorI &comparator) = 0;
    
    ///Returns the number of waveforms as the return valude and  their Ids in the parameter
    //virtual uint8_t GetWaveformIds(WaveformId_t *wIdArray) = 0;
    
    ///Returns the number of nodes currently reachable that satisfies the Patterns neighbor criteria.
    virtual uint16_t GetNumberOfNeighbors ()=0;
    
    ///Returns the top of list of neighbor sorted by quality in an iterator.
    virtual PatternNeighborIterator Begin ()=0;

    ///Returns the end of list of neighbor sorted by quality in an iterator.
    virtual PatternNeighborIterator End ()=0;
    
    ///Returns the information about the neighbor specified by the nodeId.
    virtual Core::Link* GetNeighborLink (NodeId_t nodeId)=0;
    
    ///Returns the best link on a given waveform specified by wid.
    virtual Link* GetBestLink(WaveformId_t wid)=0 ;
    
    //Returns the node ID of the neighbor corresponding to the index specified.
    //virtual NodeId_t GetNeighborID (uint16_t idx)=0;
    
    //Returns the number of packets pending for a specified neighbor.
    //virtual uint16_t GetPendingPackets (uint16_t neighbor)=0;
    
    ///Updates the neighor table with received neighbor update parameter
    virtual void UpdateTable(NeighborUpdateParam _param)=0;
    
    ///Virtual empty destructor. Needed for abstract virtual class in C++.
    virtual ~PatternNeighborTableI(){};

    virtual void PrintTable()=0;
  };

}
#endif /* PATTERNNEIGHBORTABLEI_H_ */
