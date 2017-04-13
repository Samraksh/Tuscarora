////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef PIGGYBACKER_H_
#define PIGGYBACKER_H_

#include <Interfaces/Core/PiggyBackingI.h>
#include "Lib/DS/BSTMapT.h"
#include "Framework/PolicyManager/PolicyManager.h"

namespace Core {

struct PiggyBackeeId {
	std::string name;
	WaveformId_t wid;
public:
	PiggyBackeeId () {}
	PiggyBackeeId (std::string _name,	WaveformId_t _wid) {
		name = _name;
		wid = _wid;
	}
	bool operator == (const PiggyBackeeId B) const{
		if (name.compare(B.name)==0 && wid == B.wid) return true;
		else return false;
	}
	bool operator < (const PiggyBackeeId B) const{
		if (wid < B.wid) return true;
		else return false;
	}
};	

struct PiggyBackeeInfo {
	std::string name;
	WaveformId_t wid;
	uint8_t paylaodSize;
	PiggyBackeeBase *ptr;
public:
	PiggyBackeeInfo (){}
	PiggyBackeeInfo (std::string _name, WaveformId_t _wid, uint16_t _paylaodSize, PiggyBackeeBase *_ptr) {
		name = _name;
		wid = _wid;
		paylaodSize = _paylaodSize;
		ptr = _ptr;
	}
};	

template <class WFAddressType>
class PiggyBacker: public PiggyBackerI<WFAddressType> {
	BSTMapT<PiggyBackeeId, PiggyBackeeInfo> PiggyBackeeInfoMap;
	BSTMapT<WaveformId_t, std::string> PiggyBackeeStringMap;
	BSTMapT<WaveformId_t, uint8_t> PiggyBackeeCountMap;
	Dataflow::U64AddressSendDataDelegate_t *sendDataDelegate;
protected:
	
	bool CheckRegisteredPiggyBackee(WaveformId_t wid, const std::string &authString){
		PiggyBackeeId id(authString, wid);
		if(PiggyBackeeInfoMap.Find(id)== PiggyBackeeInfoMap.End()){
			return false;
		}
		return true;
	}


public:

	PiggyBacker(){}
	
	PiggyBacker(Dataflow::U64AddressSendDataDelegate_t &sendDelegate){
		sendDataDelegate = &sendDelegate;
	}
	
	
	bool RegisterPiggyBackee (PB_TypeE type, Waveform::WF_AddressTypeE addressType, uint8_t dataSize, const std::string &authString, PiggyBackeeI<WFAddressType> &receivingModule, WaveformId_t wid);

	
	bool SendDataNow(WaveformId_t wid,  const std::string &authString, MessageTypeE msgType, uint8_t dataSize, uint8_t *data, WFAddressType dest, bool unaddressed);
	
	uint8_t GetNumberOfRegisteredPiggyBackees(WaveformId_t wid)
	{
		if(PiggyBackeeCountMap.Find(wid)== PiggyBackeeCountMap.End()){
			Debug_Printf(DBG_CORE_API, "PiggyBacker:: No registered piggybackees for wid %d yet\n", wid);
			return 0;
		}else {
			Debug_Printf(DBG_CORE_API, "PiggyBacker:: No. of registered piggybackees for wid %d is : %d \n", wid, PiggyBackeeCountMap[wid]);
			return PiggyBackeeCountMap[wid];
		}
	}
	
	uint8_t GetPiggyPaylaodSize(WaveformId_t wid){
		if(PiggyBackeeCountMap.Find(wid)== PiggyBackeeCountMap.End()){
			return 0;
		}
		if(PiggyBackeeCountMap[wid] == 0) {return 0;}
		
		string name= PiggyBackeeStringMap[wid];
		PiggyBackeeId id(name, wid);
		uint8_t  ret = PiggyBackeeInfoMap[id].paylaodSize;
		return ret;
	}

	PiggyBackeeBase* GetPiggyBackeePtr(WaveformId_t wid){
		if(PiggyBackeeCountMap.Find(wid)== PiggyBackeeCountMap.End()){
			return 0;
		}
		if(PiggyBackeeCountMap[wid] == 0) {return 0;}
		
		string name= PiggyBackeeStringMap[wid];
		PiggyBackeeId id(name, wid);
		PiggyBackeeBase* ret = PiggyBackeeInfoMap[id].ptr;
		return ret;
	}
	
	
};

template <class WFAddressType>
bool PiggyBacker<WFAddressType>::SendDataNow(WaveformId_t wid,  const std::string &authString, MessageTypeE msgType, uint8_t dataSize, uint8_t *data, WFAddressType dest, bool unaddressed){
	if(CheckRegisteredPiggyBackee(wid, authString)){
		FMessage_t msg(dataSize);
		memcpy(msg.GetPayload(), data, dataSize);
		return sendDataDelegate->operator()(wid,  dest ,msgType, msg ,unaddressed);
	}
	return false;	
}


template <class WFAddressType>
bool PiggyBacker<WFAddressType>::RegisterPiggyBackee(PB_TypeE type, Waveform::WF_AddressTypeE addressType, uint8_t dataSize, const std::string &authString, PiggyBackeeI<WFAddressType> &receivingModule, WaveformId_t wid){
	Debug_Printf(DBG_CORE_API, "PiggyBacker:: Somebody is trying to register as piggybackees for wid %d\n", wid);
	bool ret =	Policy::PolicyManager::IsValidPiggyBackee(type, addressType,dataSize,authString);
	if(ret){
		PiggyBackeeId id(authString, wid);
		Debug_Printf(DBG_CORE_API, "PiggyBacker:: Inserting new piggy backee with payload size %d for wid %d \n", dataSize, wid);
		PiggyBackeeInfo info(authString, wid, dataSize, (PiggyBackeeBase*)&receivingModule);
		PiggyBackeeInfoMap.Insert(id, info);
		PiggyBackeeStringMap.Insert(wid,authString);
		if(PiggyBackeeCountMap.Find(wid)== PiggyBackeeCountMap.End()){
			//Debug_Printf(DBG_CORE_API, "PiggyBacker:: No registered piggybackees for wid %d, instering...\n", wid);
			PiggyBackeeCountMap.Insert(wid,1);
			Debug_Printf(DBG_CORE_API, "PiggyBacker:: No. of registered piggybackees for wid %d is : %d \n", wid, PiggyBackeeCountMap[wid]);
		}else {
			PiggyBackeeCountMap[wid]++;
		}
	}
	return ret;
}



} //End namespace

#endif // PIGGYBACKER_H_
