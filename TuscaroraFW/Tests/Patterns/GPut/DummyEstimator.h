////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef DUMMY_ESTIMATOR_H_
#define DUMMY_ESTIMATOR_H_

#include "Src/Framework/Core/Neighborhood/NeighborTable.h"
#include <Interfaces/PWI/Framework_I.h>
#include "Lib/PAL/PAL_Lib.h"

extern bool DBG_TEST;

namespace Core {
namespace Estimation {

class DummyEstimator : public LinkEstimatorI{
 private:
  RandomSchedule<UniformRandomInt, UniformRNGState,uint64_t,  uint64_t> *beaconSchedule;
  //Framework_I *fi;
  NodeId_t IdGenerator;
  
  uint32_t leSeqno;
  Core::NeighborDelegate *fworkNbrDel;
  
  
  NeighborTable *coreNbrTable;
 
  NeighborDelegate * leDel;
 public:
  WaveformId_t wid; 
  void PotentialNeighborUpdate (Discovery::PotentialNeighbor &pnbr, Discovery::PNbrUpdateTypeE type){
  }
  LinkMetrics* OnPacketReceive(Waveform::WF_MessageBase *rcvMsg){
    LinkMetrics *lm = new LinkMetrics();
    lm->quality = 0.7;
    return lm;
  }
  
  DummyEstimator(){
    IdGenerator=100;
    IdGenerator++;
    wid=10;
  }

  bool RegisterDelegate (Core::NeighborDelegate& del){
    
    Debug_Printf(DBG_TEST, "DummyEsitmator: Framework has registered a callback for link estimation  with ptr %p\n", &del);
    fworkNbrDel = &del;
    return true;
  }
  
  void SetParam(uint32_t beaconPeriod, uint32_t inactivePeriod){}
  
  void AddNeighbor(){
    Link *link = new Link();
    //LinkMetrics* metrics = new LinkMetrics(); //Get the current metrics of the sending node and update it.
    link->linkId.nodeId = IdGenerator++;
    link->linkId.waveformId = wid;
    link->metrics.quality = (0.5)+ (float)link->linkId.nodeId/1000;
    
    //Notify pattern interface about new link
    Debug_Printf(DBG_TEST,"NeighborTest:: Adding a new neighbor %d on waveform %d, with quality %f\n", link->linkId.nodeId, wid, link->metrics.quality);
    NeighborUpdateParam _param;
    _param.changeType = NBR_NEW;
    _param.nodeId = link->linkId.nodeId;
    _param.link = *link;
    fworkNbrDel->operator ()(_param);    
  }
  
  void RemoveNeighbor(){
    //Notify pattern interface about new link
    
    NeighborUpdateParam _param;
    _param.changeType = NBR_DEAD;
    _param.nodeId = --IdGenerator;
    _param.link.linkId.nodeId=IdGenerator;
    _param.link.linkId.waveformId=wid;
    Debug_Printf(DBG_TEST,"NeighborTest:: Removing neighbor %d on waveform %d\n", _param.nodeId, _param.link.linkId.waveformId);
    fworkNbrDel->operator ()(_param);  
  }
  
  void PerturbNeighbor();
  
};

}
}


#endif //DUMMY_ESTIMATOR_H_