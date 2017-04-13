////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef WF_TEST_STATS_H_
#define WF_TEST_STATS_H_

struct AckUnicastSummary{
	uint16_t positive_wf_recv; //received by waveform from framework
	uint16_t negative_wf_recv; //not received by waveform from framework
	uint16_t positive_wf_sent; //sent by waveform on radio
	uint16_t negative_wf_sent; //failed to send 
	uint16_t positive_dst_recv;
	uint16_t negative_dst_recv;
	uint16_t total_unicast_sent;
	uint16_t total_unicast_recv;
	uint16_t total_ack;
};

struct AckBroadcastSummary{
	uint16_t positive_wf_recv_b; //received by waveform from framework
	uint16_t negative_wf_recv_b; //not received by waveform from framework
	uint16_t positive_wf_sent_b; //sent by waveform on radio
	uint16_t negative_wf_sent_b; //failed to send 
	uint16_t total_le_broadcast_sent;
	uint16_t total_data_broadcast_sent;
	uint32_t total_discovery_recv;
	uint16_t total_data_recv;
};

#endif //WF_TEST_STATS_H_