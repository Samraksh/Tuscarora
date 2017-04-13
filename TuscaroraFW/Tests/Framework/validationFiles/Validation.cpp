////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

//  node 1 should get 72 messages
//  node 2 should get 130 messages
//  node 3 should get 109 messages
// rest of them do not get any messages.



#include <iostream>
#include <fstream>
#include <stdint.h>
#include <cstdlib>
#include <sstream>
#include <string>

#include "Lib/Waveform/AlwaysOn/WFTestStats.h"
#include "Lib/Framework/FrameworkStat.h"

using namespace std;

// change size of array to number of nodes.
void Parse_waveform_bro(uint16_t nodeid);
void Parse_waveform_uni(uint16_t nodeid);
void Parse_framework();
void Parse_neighbor();
void Parse_frameworkBroadcast();
AckUnicastSummary* u_summary;
AckBroadcastSummary* b_summary;
FrameworkStat f_summary;
NeighborStat* n_summary;
FrameworkStatBroadcast fb_summary;
string filepath;

uint16_t SIMTIME;
uint16_t NODES;
uint16_t WF_NUM;

bool unicast=false;
bool broadcast=false;

int main(int argc, char** argv)
{
	//Get environment variable
	char * path = getenv("DCE_DIR");
	string mode="Unicast";
	filepath= std::string(path) +  "/source/ns-3-dce/files-";
	//char* input_str;
	if(argc == 6){
		NODES = atoi(argv[1]);
		SIMTIME = atoi(argv[2]);
		WF_NUM = atoi(argv[3]);
		mode= string(argv[4]);
		filepath=string(argv[5]);
		cout << "NODES: " << NODES << " SIMTIME: " << SIMTIME << " WF_NUM:" << WF_NUM << " Unicast/Broadcast: " << mode << endl; 

	}else{
		cout << "Number of arguments is not 5: Node size, RunTime, No. of Waveforms, Unicast/Broadcast, output dir" << endl;
		exit(1);
	}

	if(mode.compare("Broadcast")==0 || mode.compare("broadcast")==0 ){
		broadcast=true;
		cout << "Running validation for BROADCAST" << endl;
	}else {
		unicast=true;
		cout << "Running validation for UNICAST" << endl;
	}
	
	cout << "DCE folder is " << path << endl;
	filepath = std::string(path) +  "/source/ns-3-dce/files-";
	cout << filepath << endl;

	///Allocate memory
	u_summary = new AckUnicastSummary[NODES]; 
	b_summary = new AckBroadcastSummary[NODES];
	//FrameworkStat f_summary;
	n_summary = new NeighborStat[NODES] ;
	//FrameworkStatBroadcast fb_summary;
	
	
	//for each node, parse all waveform and find total for each node.
	for(uint16_t index =0; index < NODES; index++){
		//cout<<"Parsing unicast files for node: "<< index<< endl;
		Parse_waveform_uni(index);
		//cout<<"Parsing broadcast files for node: "<< index<< endl;
		Parse_waveform_bro(index);
	}
	
	Parse_framework();
	Parse_neighbor();
	Parse_frameworkBroadcast();
	/*
	uint16_t positive_frameworkAck = 0;
	uint16_t negative_frameworkAck = 0;
	uint16_t total_valid_sent = 0;  //total number of unicast which was on the neighbor table
	uint16_t total_sent = 0;  //total number of unicast requested by pattern
	uint16_t total_invalid_destination = 0;
	**/
	uint16_t total_positive_wf_recv = 0;
	uint16_t total_negative_wf_recv = 0;
	uint16_t total_positive_wf_sent = 0;
	uint16_t total_negative_wf_sent = 0;
	uint16_t total_positive_dst_recv = 0;
	uint16_t total_negative_dst_recv = 0;
	uint16_t total_unicast_sent = 0;
	uint16_t total_unicast_recv = 0;
	uint16_t total_ack = 0;

	uint16_t total_positive_wf_recv_b = 0;
	uint16_t total_negative_wf_recv_b = 0;
	uint16_t total_positive_wf_sent_b = 0;
	uint16_t total_negative_wf_sent_b = 0;
	//uint16_t total_broadcast_sent = 0;
	uint16_t total_le_broadcast_sent = 0;
	uint16_t total_data_broadcast_sent = 0;
	uint16_t total_discovery_recv = 0;
	uint16_t total_data_recv=0;

	uint16_t total_neighbor_size = 0;

	uint16_t positive_fw_ack = 0;
	uint16_t negative_fw_ack = 0;
	uint16_t data_broadcast_sent = 0;
	uint16_t le_broadcast_sent = 0;
	//find total of all nodes 
	for(uint16_t node_count = 0; node_count < NODES;  node_count++){
		total_positive_wf_recv  += u_summary[node_count].positive_wf_recv;
		total_negative_wf_recv  += u_summary[node_count].negative_wf_recv;
		total_positive_wf_sent  += u_summary[node_count].positive_wf_sent;
		total_negative_wf_sent  += u_summary[node_count].negative_wf_recv;
		total_positive_dst_recv += u_summary[node_count].positive_dst_recv;
		total_negative_dst_recv += u_summary[node_count].negative_dst_recv;
		total_unicast_sent += u_summary[node_count].total_unicast_sent;     
		total_unicast_recv += u_summary[node_count].total_unicast_recv;     
		total_ack += u_summary[node_count].total_ack;     
		//cout << "Positive ack: " << total_positive_wf_recv << " " << total_positive_wf_sent << " " << total_positive_dst_recv << endl;
		//cout << "Negative ack: " << total_negative_wf_recv << " " << total_negative_wf_sent << " " << total_negative_dst_recv << endl;
		//cout << "total_unicast_sent: " << total_unicast_sent << endl;
		//cout << "total_unicast_Recv: " << total_unicast_recv << endl;
		//cout << "total_ack: " << total_ack << endl;

	}
	cout << endl << endl;

	for(uint16_t node_count = 0; node_count < NODES;  node_count++){
		total_positive_wf_recv_b  += b_summary[node_count].positive_wf_recv_b;
		total_negative_wf_recv_b  += b_summary[node_count].negative_wf_recv_b;
		total_positive_wf_sent_b  += b_summary[node_count].positive_wf_sent_b;
		total_negative_wf_sent_b  += b_summary[node_count].negative_wf_recv_b;
		//total_broadcast_sent += b_summary[node_count].total_broadcast_sent; 
		total_le_broadcast_sent   += b_summary[node_count].total_le_broadcast_sent;
		total_data_broadcast_sent += b_summary[node_count].total_data_broadcast_sent;    
		total_discovery_recv += b_summary[node_count].total_discovery_recv;     
		total_data_recv += b_summary[node_count].total_data_recv;     
		/*cout << "Node: " << node_count << ", Positive ack: " << total_positive_wf_recv_b << " " << total_positive_wf_sent_b;
		//cout << "Negative ack: " << total_negative_wf_recv_b << " " << total_negative_wf_sent_b << endl;
		cout << ", Total_le_broadcast_sent: " << total_le_broadcast_sent ;
		cout << ", Data broadcast sent: " << b_summary[node_count].total_data_broadcast_sent;
		//cout << "total_discovery_recv: " << total_discovery_recv << endl;
		cout << "Data broadcast recved: " << b_summary[node_count].total_data_recv << endl;
		*/
	}

/*	cout << "unicast positive fwack: " << f_summary.positive_frameworkAck 
		<< " , negative fwack: " << f_summary.negative_frameworkAck
		<< " , total_valid_sent: " << f_summary.total_valid_sent
		<< " , total_invalid_destination: " << f_summary.total_invalid_destination << endl;

	cout << "broadcast positive_fw_ack : " << fb_summary.positive_fw_ack 
		<< " , negative_fw_ack: " << fb_summary.negative_fw_ack
		<< " , data_broadcast_sent: " << fb_summary.data_broadcast_sent
		<< " , le_broadcast_sent: " << fb_summary.le_broadcast_sent << endl;
*/

	uint16_t total_average_neighbor = 0;
	for(uint16_t node_count =0 ; node_count < NODES; node_count++){
		uint16_t total_count = n_summary[node_count].neighbor_size;
		uint16_t average = total_count / (SIMTIME -1);
		//cout << "neighbor_size: " << n_summary[node_count].neighbor_size << " denominator: " << SIMTIME-1 << " average: " << (n_summary[node_count].neighbor_size / SIMTIME-1) << endl;
		//cout << "Node: " << node_count<< ", neighbor_size: " << total_count << ", denominator: " << SIMTIME-1 << ", average: " << average << endl;
		total_average_neighbor += average;
	}
	
	float average_neighbor =  (float)total_average_neighbor / NODES; //find per node
	cout << "Average Neighbor Size: " << average_neighbor << endl;
	float leRcvRate = total_discovery_recv/(total_le_broadcast_sent*average_neighbor/WF_NUM);
	cout << "LE sent: "<< total_le_broadcast_sent<<", LE received: "<< total_discovery_recv << ", LE recv rate: "<< leRcvRate<< endl;
	
	if(unicast){
		double total_reception_rate = (double) ((double) total_unicast_recv / (double)(f_summary.total_valid_sent));
		cout << "Total Unicast Recvd: " <<total_unicast_recv << ", total unicast sent: " << f_summary.total_valid_sent << endl;
		cout << total_reception_rate << endl;
		if(total_reception_rate > 0.7){
			cout << "Unicast Passed: Total_reception_rate is: " << total_reception_rate << endl;
			cout << "UNITPASS" <<endl;
		}else{
			cout << "Unicast Failed: Total reception rate is: " << total_reception_rate << endl;
			cout << "UNITFAIL" <<endl;
		} 
	}else{
		cout << "Total broadcast sent: " << total_data_broadcast_sent << endl;
		float expected_broadcast_recv = average_neighbor * (SIMTIME-1) * NODES; // total  broadcast packet expected. (average neighbor size) * broadcast freq * simulation duration.
		cout << "average neighbor size:" << average_neighbor << ", Broadcast per wf: " << SIMTIME-1 << ", Network size: " << NODES <<endl;
		cout << "expected broadcast recv: " << expected_broadcast_recv << endl; 
		cout << "total_broadcast_recved: " << total_data_recv << endl;
		float recv_rate = total_data_recv / expected_broadcast_recv; 
		if(recv_rate > 0.7){
			cout << "Broadcast Passed: Total_reception_rate is " << recv_rate << endl;
			cout << "UNITPASS" <<endl;
		}else{
			cout << "Broadcast Failed: Total_reception_rate is " << recv_rate << endl;
			cout << "UNITFAIL" <<endl;
		}
	}
	
	return 1;
}

void Parse_waveform_uni(uint16_t node_id)
{
	//cout << "Parse waveform unicast output for node"<< node_id << endl;
	ifstream fin;

	//for each waveform, open output file
	for(uint16_t wf_index =2; wf_index < WF_NUM+2; wf_index++){
		stringstream node, file;
		node << node_id;
		file << wf_index;
		string path = filepath + node.str() + "/" +file.str() + "u.bin";
		fin.open(path.c_str());
		//cout<<"opening file: " << path<< endl;
		if(fin.fail()){
			cout << "Error::Failed to open " << path << endl;
			continue;
		}
		//file exists and opened successfully!
		//Now parse it.
		/*uint16_t positve_wf_recv =0;
		uint16_t negative_wf_recv = 0;
		uint16_t positive_wf_sent = 0;
		uint16_t negative_wf_sent = 0;
		uint16_t positive_dst_recv = 0;
		uint16_t negative_dst_recv = 0;
		uint16_t total_unicast_sent = 0;*/
		
		AckUnicastSummary obj; int recCount=1;
		do{	
			fin.read((char*) &obj, sizeof(AckUnicastSummary));
			//cout << "Read Record: " << recCount << endl<<flush;
			u_summary[node_id].positive_wf_recv += obj.positive_wf_recv;
			u_summary[node_id].negative_wf_recv += obj.negative_wf_recv;
			u_summary[node_id].positive_wf_sent += obj.positive_wf_sent;
			u_summary[node_id].negative_wf_sent += obj.negative_wf_sent;
			u_summary[node_id].positive_dst_recv += obj.positive_dst_recv;
			u_summary[node_id].negative_dst_recv += obj.negative_dst_recv;
			u_summary[node_id].total_unicast_sent += obj.total_unicast_sent;
			u_summary[node_id].total_unicast_recv += obj.total_unicast_recv;
			u_summary[node_id].total_ack += obj.total_ack;
			fin.read((char*) &obj, sizeof(AckUnicastSummary));
		}while(fin.gcount () == sizeof(AckUnicastSummary));
		fin.close();
		//cout << "Finish parsing file: "<< path  << endl;
	}
	//cout << "Finish parsing all files" << endl;
}    

void Parse_waveform_bro(uint16_t node_id)
{
	//cout << "Parse waveform broadcast output" << endl;
	ifstream fin;

	//for each waveform, open output file
	for(uint16_t wf_index =2; wf_index < WF_NUM+2; wf_index++){
		stringstream node, file;
		node << node_id;
		file << wf_index;
		string path = filepath + node.str() + "/" +file.str() + "b.bin";
		fin.open(path.c_str());
		if(fin.fail()){
			cout << "Error::Failed to open " << path << endl;
			continue;
		}
		//file exists and opened successfully!
		//Now parse it.
		/*uint16_t	b_summary.negative_wf_recv_b = 0;
		uint16_t b_summary.negative_wf_sent_b = 0;
		uint16_t b_summary.positive_wf_sent_b = 0;
		uint16_t b_summary.postive_wf_recv_b = 0;
		uint16_t b_summary.total_broadcast_sent = 0;*/
		AckBroadcastSummary obj;
		do{	
			fin.read((char*) &obj, sizeof(AckBroadcastSummary));
			b_summary[node_id].positive_wf_recv_b += obj.positive_wf_recv_b;
			b_summary[node_id].negative_wf_recv_b += obj.negative_wf_recv_b;
			b_summary[node_id].positive_wf_sent_b += obj.positive_wf_sent_b;
			b_summary[node_id].negative_wf_sent_b += obj.negative_wf_sent_b;
			b_summary[node_id].total_le_broadcast_sent += obj.total_le_broadcast_sent;  
			b_summary[node_id].total_data_broadcast_sent += obj.total_data_broadcast_sent;  
			//b_summary[node_id].total_broadcast_sent += obj.total_broadcast_sent;     
			b_summary[node_id].total_discovery_recv += obj.total_discovery_recv;
			b_summary[node_id].total_data_recv += obj.total_data_recv; 
			fin.read((char*) &obj, sizeof(AckBroadcastSummary));
		}while(fin.gcount () == sizeof(AckBroadcastSummary));
		fin.close();
	}
	//cout << "Finish parsing file "  << endl;
}    

void Parse_framework()
{
	//cout << "Parse framework output" << endl;
	ifstream fin;

	//for each waveform, open output file
	for(uint16_t node_index =0; node_index < NODES; node_index++){
		stringstream node;
		node << node_index;  
		string path = filepath + node.str() + "/framework.bin";
		//cout << "Path:: " << path << endl;
		fin.open(path.c_str());
		if(fin.fail()){
			cout << "Error::Failed to open " << path << endl;
			continue;
		}
		FrameworkStat obj;
		int count = 0;
		do{	
			count++;
			fin.read((char*) &obj, sizeof(FrameworkStat));
			f_summary.positive_frameworkAck += obj.positive_frameworkAck;
			f_summary.negative_frameworkAck += obj.negative_frameworkAck;
			f_summary.total_valid_sent += obj.total_valid_sent;  
			f_summary.total_sent += obj.total_sent;  
			f_summary.total_invalid_destination += obj.total_invalid_destination;  
			/*cout << "Node: " << node_index << " positive fwack: " << obj.positive_frameworkAck 
											<< " negative fwack: " << obj.negative_frameworkAck
											<< " total_valid_sent: " << obj.total_valid_sent
											<< " total_invalid_destination: " << obj.total_invalid_destination << endl;
											*/
			fin.read((char*) &obj, sizeof(FrameworkStat));
		}while(fin.gcount () == sizeof(FrameworkStat));
		
		//cout << "Finish parsing file " << count   << endl;
		fin.close();
	}
	//cout << "Finish parsing file "<< endl;
}    

void Parse_neighbor()
{
	//cout << "Parse framework output" << endl;
	ifstream fin;

	//for each waveform, open output file
	for(uint16_t node_index =0; node_index < NODES; node_index++){
		stringstream node;
		node << node_index;  
		string path = filepath + node.str() + "/neighbor.bin";
		//cout << "Path:: " << path << endl;
		fin.open(path.c_str());
		if(fin.fail()){
			cout << "Error::Failed to open " << path << endl;
			continue;
		}
		NeighborStat obj;
		int count = 0;
		n_summary[node_index].neighbor_size=0;
		fin.read((char*) &obj, sizeof(NeighborStat));
		while(fin.gcount() == sizeof(NeighborStat)){	
			count++;
			//fin.read((char*) &obj, sizeof(NeighborStat));
			n_summary[node_index].neighbor_size += obj.neighbor_size;
			//cout << "Node: " << node_index << " neighbor_size " << obj.neighbor_size << endl; 
			fin.read((char*) &obj, sizeof(NeighborStat));    
		}//while(fin.gcount () == sizeof(NeighborStat));
		//cout << "Finish parsing file " << count   << endl;
		fin.close();
	}
	//cout << "Finish parsing file "<< endl;
}    

void Parse_frameworkBroadcast()
{
	//cout << "Parse framework output" << endl;
	ifstream fin;

	//for each waveform, open output file
	for(uint16_t node_index =0; node_index < NODES; node_index++){
		stringstream node;
		node << node_index;  
		string path = filepath + node.str() + "/broadcast.bin";
		//cout << "Path:: " << path << endl;
		fin.open(path.c_str());
		if(fin.fail()){
			cout << "Error::Failed to open " << path << endl;
			continue;
		}
		FrameworkStatBroadcast obj;
		int count = 0;
		do{	
			count++;
			fin.read((char*) &obj, sizeof(FrameworkStatBroadcast));
			fb_summary.positive_fw_ack += obj.positive_fw_ack;
			fb_summary.negative_fw_ack += obj.negative_fw_ack;
			fb_summary.data_broadcast_sent += obj.data_broadcast_sent;  
			fb_summary.le_broadcast_sent += obj.le_broadcast_sent;  
			
			/*cout << "Node: " << node_index << " positive fwack: " << obj.positive_fw_ack
											<< " negative fwack: " << obj.negative_fw_ack
											<< " data_broadcast_sent: " << obj.data_broadcast_sent
											<< " le_broadcast_sent: " << obj.le_broadcast_sent << endl;
											*/
			fin.read((char*) &obj, sizeof(FrameworkStatBroadcast));
		}while(fin.gcount () == sizeof(FrameworkStatBroadcast));
		
		//cout << "Finish parsing file " << count   << endl;
		fin.close();
	}
	//cout << "Finish parsing file "<< endl;
}    
