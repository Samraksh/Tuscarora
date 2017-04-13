////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef FITESTSTATS_H_
#define FITESTSTATS_H_


struct AckDataStat{
	uint16_t total_sent;
	uint16_t total_sent_out;
	uint16_t total_dst_ack;
	uint16_t total_recv;
};


struct SendDataStat{
	uint16_t total_sent;
	uint16_t total_recv;
};

struct ApiDataStat{
	uint16_t total_canceled_node;
	uint16_t total_node_added;
	uint16_t total_replaced;
	uint16_t total_sent;
};

struct CancelDataStat{
	uint16_t total_recv;
	uint16_t total_sent;
	uint16_t total_cancled;
	uint16_t total_wf_nack;
	uint16_t total_sent_out;
	uint16_t total_dst_ack;
};

struct AddDataStat{
	uint16_t total_recv;
	uint16_t total_node_added;
	uint16_t total_nack;
	uint16_t total_destSize;
};

struct ReplaceDataStat{
	//uint16_t total_sent;
	uint16_t total_replaced_send;
	uint16_t total_replaced_recv;
	//uint16_t total_replaced_wfnack;
	//uint16_t total_replaced_dstnack;
};

#endif //FITESTSTATS_H_