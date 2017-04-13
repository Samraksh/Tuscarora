////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef _TIMESTAMPING_H_
#define _TIMESTAMPING_H_

#include <Framework/Core/DataFlow/PiggyBacker.h>
#include <Interfaces/Waveform/EventTimeStamp.h>
#include <Interfaces/ExternalServices/TimeStampingI.h>


using namespace Core;
//using namespace ExternalServices;

 
namespace ExternalServices {

template <class WFAddressType>
class TimeStamping : public TimeStampingI <WFAddressType> {
	Core::PiggyBackerI<WFAddressType> *pbf;
	WaveformId_t wid;
	
public:
	TimeStamping(Core::PiggyBackerI<WFAddressType>& piggyBacker, WaveformId_t _wid){
		bool registration = piggyBacker.RegisterPiggyBackee(Core::PB_TIMESYNC, Waveform::WFA_UINT64, sizeof(EventTimeStamp) , "PBS_TEST_Piggy_10B", *((PiggyBackeeI<uint64_t>*)this), _wid);
	
		if (!registration) {
			Debug_Printf(DBG_TEST, "PB_TEST: No valid piggybacker registered. Exiting\n");
			exit(1);
		}
		pbf=&piggyBacker;
		wid = _wid;
	}
	

	uint16_t CreateData (WFAddressType dest, bool unaddressed,  uint8_t*& data, uint8_t dataSize){
		if (dataSize < sizeof( EventTimeStamp)) {
			Debug_Printf(DBG_TIMESYNC, "Exepected data size of the framework is less than event timestamp size\n"); 
			return 0;
		}
			
		EventTimeStamp *ets = new EventTimeStamp();
		data = (uint8_t *) ets;
		Debug_Printf(DBG_TIMESYNC, "Created EventTimeStamp with time: %lu structure of size %lu\n", ets->senderEventTime.GetTime(), sizeof(EventTimeStamp)); 
		
		return sizeof(EventTimeStamp);
	}

	void ReceiveData (WFAddressType src, uint8_t *data, uint8_t dataSize, U64NanoTime _receiveTimestamp){
		
		if(data==0 and dataSize==0){ //Implicity Sync Timestamping
			Debug_Printf(DBG_TIMESYNC, "TimeStamping: Implict Mode: Received a packet at time : %lu\n", _receiveTimestamp.GetTime()); 
		}
		else if (dataSize >0 && dataSize < sizeof(EventTimeStamp)) { //Error
			Debug_Printf(DBG_TIMESYNC, "Received data size of the framework is less than event timestamp size, something wrong\n"); 
			return ;
		}
		else { //Explicit sender based timestamping
			EventTimeStamp *ets = (EventTimeStamp *)data;
			uint64_t senderTimeAtReceiver = _receiveTimestamp - ets->senderDelay;
		
			Debug_Printf(DBG_TIMESYNC, "Received data: Sender Time %lu, sender delay %lu, sender time at Receiver: %lu\n", ets->senderEventTime.GetTime(), ets->senderDelay, senderTimeAtReceiver);; 
		}
	}
};

/*template<>
TimeStamping<uint64_t>::TimeStamping(Core::PiggyBackerI<uint64_t>& piggyBacker, WaveformId_t _wid)
{
	bool registration = piggyBacker.RegisterPiggyBackee(Core::PB_TIMESYNC, Waveform::WFA_UINT64, sizeof(EventTimeStamp) , "PBS_TEST_Piggy_10B", *((PiggyBackeeI*)this), _wid);

	
	if (!registration) {
		Debug_Printf(DBG_TEST, "PB_TEST: No valid piggybacker registered. Exiting\n");
		exit(1);
	}
	pbf=&piggyBacker;
	wid = _wid;
}
*/


} // End of namespace

#endif // _TIMESTAMPING_H_