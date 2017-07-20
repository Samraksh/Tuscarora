////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef DEFAULTPOLICIES_H_
#define DEFAULTPOLICIES_H_

#include <Interfaces/Core/PolicyManagerI.h>
#include <string>

namespace Core {
namespace Policy {

  PacketSchedulingPolicyE packetPolicy = FCFS;
  //for debugging purpose made it 4. Used to be 1
  uint16_t packetsBufferedPerPattern =4;

  bool IsFragmentationEnabled = true;
  uint16_t fwMaxPacketSizePolicy = 10000; //Policy driven max packet payload size that the framework accepts from the patterns. Any non-zero value overrides max framework packet size. A zero value means an auto calculated maximum framework packet size which is set equal to the minimum of the maxpayload size supported by the available waveforms.

  LinkSelectionPolicyE nbrLinkSelectionPolicy = PATTERN_DEFINED;
  WaveformId_t preferredWaveform;
  
  bool PiggyBackingEnabled=true;
  std::string ApprovedPiggyBackeesStrings[]={"PBS_TEST_Piggy_10B", "PBS_TIMESyNC_8B", "PBS_LInkEst_4B"};
  uint8_t NoOfApprovedPiggyBackees = 3;

}//End of namespace
}



#endif //DEFAULTPOLICIES_H_
