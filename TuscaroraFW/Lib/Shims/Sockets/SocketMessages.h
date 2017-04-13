////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef LIB_SHIMS_SOCKETS_SOCKETMESSAGES_H_
#define LIB_SHIMS_SOCKETS_SOCKETMESSAGES_H_
#include <Lib/DS/GenericMessages.h>

namespace Lib {
namespace Shims {

class GenericSocketMessages :public Generic_VarSized_Msg{
public:
	GenericSocketMessages(GenericMsgPayloadSize_t _payload_size)
		:Generic_VarSized_Msg(_payload_size) {
	};
	GenericSocketMessages(GenericMsgPayloadSize_t  _payload_size, char* _payload)
	    :Generic_VarSized_Msg(_payload_size, (uint8_t*)_payload){
	};
    int GetType(){
        int calltype;
        memcpy( &calltype, (void*)(this->GetPayload()), sizeof(int));
        return(calltype);
    }
};
}
};


#endif /* LIB_SHIMS_SOCKETS_SOCKETMESSAGES_H_ */
