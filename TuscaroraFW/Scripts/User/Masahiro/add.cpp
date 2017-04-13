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
struct AddDataStat{
	uint16_t total_recv;  //number of positive dst_recv
	uint16_t total_node_added; //number of successfully added node
	uint16_t total_nack; //number of negative dst_recv
	uint16_t total_destSize; //original destination count
};
void Parse_MemMap();
//SendDataStat data[10];
AddDataStat data[10];
string file_path;

int main(){
  //Get environment variableu
  char * path = getenv("DCE_DIR");
  // cout << "DCE folder is " << path << endl;
  //set path to the output file.
  file_path = std::string(path) +  "/source/ns-3-dce/files-";
  //cout << file_path << endl;
     uint16_t total_node_added = 0;
  	 uint16_t recv = 0;
     uint16_t total_nack = 0;
     uint16_t total_destSize = 0;
  Parse_MemMap();
 // cout << "Show final statas" << endl;


  for(uint16_t index =0; index < 10 ; index++){
      //cout << "Node" << index << " total_added: " << data[index].total_node_added << " recv: " << data[index].total_recv << " total_nack: " << data[index].total_nack << " total_destSize: " << data[index].total_destSize << endl;
      if(index == 1){
         continue; //skip. Node 1 cannot send data to Node 1.
      }
  total_node_added += data[index].total_node_added;
  recv += data[index].total_recv;
  total_nack += data[index].total_nack;
  total_destSize += data[index].total_destSize;
}
if(total_node_added == ((total_nack + recv) - total_destSize) )
{
    cout << "Add_Datarequest..... OK" << endl;
}else{
    cout << "Add_..... NG" << endl;
}



return 1;

}

void Parse_MemMap(){
  // cout << "Parse cancel.bin" << endl;
   ifstream fin;

   for(uint16_t node_count =0; node_count < 10; node_count++){
         stringstream ss;
         ss << node_count;
         string path = file_path + ss.str() + "/add.bin"; 
  	 fin.open(path.c_str());
  	 if(fin.fail()){
	 //cout << "Error::Failed to open " << path << endl;
   	 continue;
	}
       // cout << " Successfully opened file " << node_count << endl;
 	  //file exists and opened successfully!
 	  //Now parse it.
     uint16_t a =0;
     uint16_t total_node_added = 0;
  	 uint16_t recv = 0;
     uint16_t total_nack = 0;
     uint16_t total_destSize = 0;
  	 AddDataStat obj;
	 data[node_count].total_recv = 0;
     data[node_count].total_node_added = 0;
     data[node_count].total_nack =0;
     data[node_count].total_destSize=0;
  	 do{	
      		 fin.read((char*) &obj, sizeof(AddDataStat));
             if(recv < obj.total_recv){ data[node_count].total_recv = obj.total_recv; recv = obj.total_recv; }
		     if(total_node_added < obj.total_node_added) { data[node_count].total_node_added = obj.total_node_added; total_node_added = obj.total_node_added; }
      		 if(total_nack < obj.total_nack) {data[node_count].total_nack = obj.total_nack; total_nack = obj.total_nack;}
             if(total_destSize < obj.total_destSize) {data[node_count].total_destSize = obj.total_destSize; total_destSize = obj.total_destSize;}
             //cout << "Node" << node_count << " total_added: " << data[node_count].total_node_added << " recv: " << data[node_count].total_recv << " total_nack: " << data[node_count].total_nack << " total_destSize: " << data[node_count].total_destSize << endl;
                 //cout << "canceled: " << data[node_count].canceled << " sent: " << data[node_count].sent << " recv: " << data[node_count].recv << " wf_nack: " << data[node_count].wf_nack <<  " wf_sent_out: " << data[node_count].wf_sent_out  << " dst_ack: " << data[node_count].dst_nack<<  endl; 
		a++;
 	  }while(fin.gcount () == sizeof(AddDataStat));
  	 fin.close();
   }
   //cout << "Finish parsing file "  << endl;
}    

