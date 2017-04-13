////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

//assume FW does not reject any packet from pattern.If it does , I need to come up the way to count number of PDN_FW_RECV without double counting it.
 


#include <iostream>
#include <fstream>
#include <stdint.h>
#include <cstdlib>
#include <sstream>

using namespace std;

//declare structure holding result
struct CancelDataStat{
	uint16_t recv;
 	uint16_t sent;
	uint16_t canceled;
        uint16_t wf_nack;
        uint16_t wf_sent_out;
        uint16_t dst_nack;
};

void Parse_MemMap();
//SendDataStat data[10];
CancelDataStat data[10];
string file_path;

int main(){
  //Get environment variable
  char * path = getenv("DCE_DIR");
 // cout << "DCE folder is " << path << endl;
  file_path = std::string(path) +  "/source/ns-3-dce/files-";
  //cout << file_path << endl;
  uint16_t total_canceled = 0;
  uint16_t total_recv = 0;
  uint16_t total_sent = 0;
  uint16_t total_wf_nack = 0;
  uint16_t total_wf_sent_out = 0;
  uint16_t total_dst_nack = 0;
  Parse_MemMap();
 // cout << "Show final statas" << endl;


  for(uint16_t index =0; index < 10 ; index++){
  //    cout << "Node" << index << " canceled: " << data[index].canceled << " sent: " << data[index].sent << " recv: " << data[index].recv << " wf_nack: " << data[index].wf_nack << " dst_nack: " << data[index].dst_nack << endl;

  total_canceled += data[index].canceled;
  total_recv += data[index].recv;
  total_sent += data[index].sent;
  total_wf_nack += data[index].wf_nack;
  total_wf_sent_out += data[index].wf_sent_out;
  total_dst_nack += data[index].dst_nack;
}
if((total_sent - total_canceled- total_wf_nack - total_dst_nack )*0.9 < total_recv)
{
    cout << "Cancel_Datarequest..... OK" << endl;
}else{
    cout << "Cancle_DataRequest..... NG" << endl;
}



return 1;

}

void Parse_MemMap(){
  // cout << "Parse cancel.bin" << endl;
   ifstream fin;

   for(uint16_t node_count =0; node_count < 10; node_count++){
         stringstream ss;
         ss << node_count;
         string path = file_path + ss.str() + "/cancel.bin"; 
  	 fin.open(path.c_str());
  	 if(fin.fail()){
	 //cout << "Error::Failed to open " << path << endl;
   	 continue;
	}
       // cout << " Successfully opened file " << node_count << endl;
 	  //file exists and opened successfully!
 	  //Now parse it.
   	uint16_t a =0;
  	 uint16_t cancel_count = 0;
         uint16_t cancel_decrement = 0;
         uint16_t sent = 0;
  	 uint16_t recv = 0;
         uint16_t wf_sent_out = 0;
         uint16_t dst_nack = 0;
  	 CancelDataStat obj;
	 data[node_count].recv = 0;
         data[node_count].sent = 0;
         data[node_count].canceled = 0;
         data[node_count].wf_nack = 0;
         data[node_count].wf_sent_out = 0;
         data[node_count].dst_nack = 0;
  	 do{	
      		 fin.read((char*) &obj, sizeof(CancelDataStat));
                 if(recv < obj.recv){ data[node_count].recv = obj.recv; recv = obj.recv; }
		 if(sent < obj.sent) { data[node_count].sent = obj.sent; sent = obj.sent; }
      		 if(cancel_count < obj.canceled) {data[node_count].canceled = obj.canceled; cancel_count = obj.canceled;}
                 if(cancel_decrement < obj.wf_nack) {data[node_count].wf_nack = obj.wf_nack; cancel_decrement = obj.wf_nack;}
                 if(wf_sent_out < obj.wf_sent_out) {data[node_count].wf_sent_out = obj.wf_sent_out; wf_sent_out = obj.wf_sent_out;}
                 if(dst_nack < obj.dst_nack) {data[node_count].dst_nack = obj.dst_nack; dst_nack = obj.dst_nack; }
    //             cout << "canceled: " << data[node_count].canceled << " sent: " << data[node_count].sent << " recv: " << data[node_count].recv << " wf_nack: " << data[node_count].wf_nack <<  " wf_sent_out: " << data[node_count].wf_sent_out  << " dst_ack: " << data[node_count].dst_nack<<  endl; 
		a++;
 	  }while(fin.gcount () == sizeof(CancelDataStat));
  	 fin.close();
   }
   //cout << "Finish parsing file "  << endl;
}    

