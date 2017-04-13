////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

/*
#include <Interfaces/Core/Link.h>

using namespace Core;

namespace Core {

template <class WF_AddressType>
WF_LinkId<WF_AddressType>::WF_LinkId(){}

template <class WF_AddressType>
WF_LinkId<WF_AddressType>::WF_LinkId(WF_AddressType _id){ nodeId = _id; waveformId = UNINT_WF_ID;}

template <class WF_AddressType>
WF_LinkId<WF_AddressType>::WF_LinkId(WF_AddressType _id, WaveformId_t _wid){ nodeId = _id; waveformId = _wid;}
 
template <class WF_AddressType>
bool WF_LinkId<WF_AddressType>::operator == (WF_LinkId<WF_AddressType> rhs) const {
if (rhs.nodeId == nodeId ){
	if(rhs.waveformId == waveformId || rhs.waveformId == UNINT_WF_ID || waveformId == UNINT_WF_ID){return true;}
	else {return false;}
}else {
	return false;
}
}

template <class WF_AddressType>
bool WF_LinkId<WF_AddressType>::operator < (const WF_LinkId<WF_AddressType> rhs) const {
	if(rhs.nodeId != nodeId) return nodeId < rhs.nodeId;
	else if(rhs.waveformId == UNINT_WF_ID || waveformId == UNINT_WF_ID){ return false;}
	else return waveformId < rhs.waveformId;
}

} //end namespace

*/
  
  
/*
bool operator < (const LinkId &a) const {
return (a.nodeId < nodeId && (a.waveformId < waveformId || a.waveformId== UNINT_WF_ID));
}
bool operator > (LinkId a) const {
return (!this->operator<(a) && ! this->operator==(a));
}

bool operator < (const LinkId &rhs) const {
if(rhs.waveformId == UNINT_WF_ID){
	return nodeId < rhs.nodeId;
}
else { 
	return (nodeId < rhs.nodeId && waveformId < rhs.waveformId);
}
}*/
