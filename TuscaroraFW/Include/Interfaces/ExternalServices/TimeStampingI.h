////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef _TIMESTAMPINGI_H_
#define _TIMESTAMPINGI_H_


#include <Interfaces/Core/PiggyBackingI.h>

using namespace Core;

namespace ExternalServices {

template <class WFAddressType>	
class TimeStampingI : public PiggyBackeeI<WFAddressType> {	

	
public:	
	TimeStampingI(){}
	TimeStampingI(PiggyBackerI<WFAddressType> &piggyBacker){}
	
	virtual uint16_t CreateData (WFAddressType dest, bool unaddressed,  uint8_t*& data, uint8_t dataSize) = 0;

	virtual void ReceiveData (WFAddressType src, uint8_t *data, uint8_t dataSize, U64NanoTime _receiveTimestamp) = 0;
	
	
	virtual ~TimeStampingI() {}
};


} //End of namespace




#endif //_TIMESTAMPINGI_H_


