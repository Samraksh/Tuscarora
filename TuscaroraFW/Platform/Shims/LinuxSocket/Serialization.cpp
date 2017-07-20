#include "Serialization.h"

namespace Shim {
namespace Serialization {

void* Serialize (FMessage_t *fmsg,  GenericMsgPayloadSize_t& serial_msg_size){
	GenericSerializer<MessageTypeE, WaveformId_t, NodeId_t, GenericMsgPayloadSize_t, void* >
	gs(fmsg->GetType(), fmsg->GetWaveform(), fmsg->GetSource(), (GenericMsgPayloadSize_t)(fmsg->GetPayloadSize() *sizeof(uint8_t)), (void*)fmsg->GetPayload());

	//Copy serialized message since it will be destructed by GenericSerializer
	serial_msg_size = gs.Get_Generic_VarSized_Msg_Ptr()->GetPayloadSize();
	void* serial_msg =  malloc(serial_msg_size);
	memcpy( (void*)serial_msg, gs.Get_Generic_VarSized_Msg_Ptr()->GetPayload(), serial_msg_size);

	return static_cast<void*>(serial_msg);
}
void DeSerialize(FMessage_t *fmsg, const GenericMsgPayloadSize_t& serial_msg_size,  void * serial_msg){
	Generic_VarSized_Msg gen_msg(serial_msg_size, static_cast<uint8_t*>(serial_msg)); //This consumes serial_msg, the destructor will destruct serial_msg

	GenericMsgPayloadSize_t v_payload_size;
	void* v_payload = NULL;
	MessageTypeE type = fmsg->GetType();
	WaveformId_t wid =fmsg->GetWaveform();
	NodeId_t nid = fmsg->GetSource();
	GenericDeSerializer<MessageTypeE, WaveformId_t, NodeId_t, GenericMsgPayloadSize_t, void* >
	gs(&gen_msg, type, wid, nid, v_payload_size, v_payload); //This copies serial_msg contents to the variables. For the case of pointers, it copies contents to the location pointed by the pointer. If the pointer is NULL, then malloc is called to allocate memory,

	fmsg->SetPayloadSize(v_payload_size);
	fmsg->SetPayload(static_cast<uint8_t*>(v_payload));

}

}
}

