#ifndef _SERIALIZATION_H_
#define _SERIALIZATION_H_


#include <Interfaces/Core/MessageT.h>
#include <Lib/DS/GenericMessages.h>

using namespace Core;


namespace Shim {
namespace Serialization {

	void* Serialize (FMessage_t *fmsg,  GenericMsgPayloadSize_t& serial_msg_size);
	void DeSerialize (FMessage_t *fmsg, const GenericMsgPayloadSize_t& serial_msg_size,  void * serial_msg);

}
}



#endif //_SERIALIZATION_H_
