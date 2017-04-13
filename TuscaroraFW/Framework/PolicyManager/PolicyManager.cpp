////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include "PolicyManager.h"
#include "DefaultPolicies.h"
#include <string>

using namespace std;


namespace Core {
namespace Policy {
  
  
bool PolicyManager::SetPacketPolicy(PacketSchedulingPolicyE _policy){
  if(_policy == PRIORITY_FCFS){
    packetPolicy = _policy; 
  }
  else {
    packetPolicy = FCFS;
  }
  return true;
}
  
uint16_t PolicyManager::GetAsnpPriority (uint16_t asnpInstanceId){
  if(packetPolicy==FCFS){
    return 1;
  }
  return asnpInstanceId;
  
}

bool PolicyManager::SetAsnpPriority (uint16_t patternInstanceId, uint8_t priority){
  return true;
}

uint16_t PolicyManager::GetPacketsBufferedPerPattern()
{
  return packetsBufferedPerPattern;
}

LinkSelectionPolicyE PolicyManager::GetLinkSelectionPolicy()
{
  return nbrLinkSelectionPolicy;
}

bool PolicyManager::SetLinkSelectionPolicy(LinkSelectionPolicyE linkPolicy, WaveformId_t wid)
{
  nbrLinkSelectionPolicy = linkPolicy;
  if(linkPolicy == PREFFERED_WAVEFORM){
    preferredWaveform = wid;
  }
  return true;
}

WaveformId_t PolicyManager::GetPrefferedWaveform()
{
  return preferredWaveform;
}

bool PolicyManager::IsPiggyBackingEnabled()
{
	return PiggyBackingEnabled;
}


bool PolicyManager::IsValidPiggyBackee(PB_TypeE type, Waveform::WF_AddressTypeE addressType, uint8_t dataSize, const string &authString){

	for (int i=0; i < NoOfApprovedPiggyBackees; i++ ){
		if(authString.compare(ApprovedPiggyBackeesStrings[i])==0){
			Debug_Printf(DBG_PIGGYBACK, "PolicyManager::IsValidPiggyBackee:: Found Auth String: %s \n",authString.c_str());
			return true;
		}
	}
	Debug_Printf(DBG_PIGGYBACK, "PolicyManager::IsValidPiggyBackee::Did not find Auth String: %s \n",authString.c_str());
	return false;
}

  
} //End namespace
}
