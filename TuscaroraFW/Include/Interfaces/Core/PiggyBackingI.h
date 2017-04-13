////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef PIGGYBACKING_I_H_
#define PIGGYBACKING_I_H_

#include <Base/FrameworkTypes.h>
#include <Framework/Core/DataFlow/CoreTypes.h>
#include <Base/Delegate.h>
#include <Interfaces/Waveform/WF_Types.h>
#include <string>

namespace Core {

enum PB_TypeE {
	PB_TEST,
	PB_ASNP_CONTROL,
	PB_TIMESYNC,
	PB_LINK_ESTIMATION
};

template <class WFAddressType>
class PiggyBackerI;

class PiggyBackeeBase {};

/**
 * Interface that must be implemented by modules that want to send piggybacked data on outgoing packets. 
 * Please note that the module needs to be registered successfully with the framework before piggybacking can happen.
 * The interface is meant to be used only by the TimeStamping and TimeSync services. No other service
 * is supported at this point. Any other module trying to use this service will result in correct system.
 */
template <class WFAddressType>
class PiggyBackeeI : public PiggyBackeeBase {
public:	
	
	PiggyBackeeI(){}
	PiggyBackeeI(PiggyBackerI<WFAddressType> &sendDelegate){}
	
	/**
	 * @brief Creates the piggybacked data to be sent out. This method gets called by the framework on the sending side, whenever their is enough space for piggybacking in an outgoing packet.
	 * 
	 * @param dest The destination address the packet is meant for
	 * @param unaddressed Boolen. If set to true, the packet is not addressed to anyone in particular and will be sent out on the channel for anyone who can receive it.
	 * @param data Pointer to the piggybacked data. This method should create the data array and set the address to this pointer
	 * @param dataSize Max size of the piggybacked data that can be accomdated
	 * @return uint16_t Actucal size of the piggybacked data created.
	 */
	
	virtual uint16_t CreateData (WFAddressType dest, bool unaddressed,  uint8_t*& data, uint8_t dataSize) = 0;

	/**
	 * @brief Receives the piggybacked data and handles it, whenever such data is received by framework. This method gets called by the framework on the receiving side.
	 * 
	 * @param src Node Id of the sending node.
	 * @param data Pointer to the piggybacked data
	 * @param dataSize Size of the piggybacked data. If IMPLICIT_SYNC_TIMESTAMPING is used, the datasize will be set to 0 and only the 
	 * receiveTimestamp field will have valid value. 
	 * @param receiveTimestamp Time at which the packet was received at the waveform layer
	 * @return void
	 */
	virtual void ReceiveData (WFAddressType src, uint8_t *data, uint8_t dataSize, U64NanoTime receiveTimestamp) = 0;
	
	virtual ~PiggyBackeeI() {}
};



///This class will be implemented by the framework to provide piggybacking service
template <class WFAddressType>
class PiggyBackerI {
public:	
	
	PiggyBackerI(){}
	PiggyBackerI(Dataflow::U64AddressSendDataDelegate_t &sendDelegate);
	
	
	virtual bool RegisterPiggyBackee (PB_TypeE type, Waveform::WF_AddressTypeE addressType, uint8_t dataSize, const std::string &authString, PiggyBackeeI<WFAddressType> &receivingModule, WaveformId_t wId)=0;

	
	virtual uint8_t GetNumberOfRegisteredPiggyBackees(WaveformId_t wid)=0;
	
	/**
	 * @brief Allows a Piggebackee to send a message on demand. This is a best effort service. The framework can choose to ignore this message.
	 * 
	 * @param wid Waveform Id on which the message should be sent out.
	 * @param authString Authentication string of the piggebackee to verify its allowed to send
	 * @param dataSize size of the data
	 * @param data The pointer to data
	 * @return Return type is bool. Returns true if data was sent to waveform successfully. Note this does not mean
	 */
	
	virtual bool SendDataNow(WaveformId_t wid,  const std::string &authString, MessageTypeE msgType, uint8_t dataSize, uint8_t *data, WFAddressType dest, bool unaddressed)=0;
	
	virtual ~PiggyBackerI() {}
};





} //End namespace

#endif //PIGGYBACK_I_H_
