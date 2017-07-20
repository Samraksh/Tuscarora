////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef POLICYMANAGER_H_
#define POLICYMANAGER_H_

#include <Interfaces/Core/PolicyManagerI.h>

namespace Core {
namespace Policy {

class PolicyManager: public PolicyManagerI {

public:
    static bool SetPacketPolicy(PacketSchedulingPolicyE _policy);
    static uint16_t GetAsnpPriority(uint16_t asnpInstanceId);
    static bool SetAsnpPriority(uint16_t patternInstanceId, uint8_t priority);
    static uint16_t GetPacketsBufferedPerPattern();
    
    static bool SetLinkSelectionPolicy(LinkSelectionPolicyE _linkPolicy, WaveformId_t _wid);
    static LinkSelectionPolicyE GetLinkSelectionPolicy();
    static WaveformId_t GetPrefferedWaveform();
	static inline bool IsPiggyBackingEnabled();
	static uint16_t GetMaxFWPacketPayloadSize();
    static bool IsValidPiggyBackee (PB_TypeE type,  Waveform::WF_AddressTypeE addressType, uint8_t dataSize, const std::string &authString);
};

  
} //end of namepsace.
}

#endif /* POLICYMANAGER_H_ */
