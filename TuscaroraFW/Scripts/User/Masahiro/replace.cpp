////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#include <iostream>
#include <fstream>
#include <stdint.h>
#include <cstdlib>
#include <sstream>

using namespace std;

//declare structure holding result
struct ReplaceDataStat{
	uint16_t total_replaced_sent;
 	uint16_t total_replaced_recv;
};

void Parse_MemMap();
ReplaceDataStat data[10];
string file_path;

int main(){
  //Get environment variable
  char * path = getenv("DCE_DIR");
  //cout << "DCE folder is " << path << endl;
  file_path = std::string(path) +  "/source/ns-3-dce/files-";
  //cout << file_path << endl;
  uint16_t total_recv = 0;
  uint16_t total_sent = 0;

  Parse_MemMap();
 // cout << "Show final statas" << endl;
  for(uint16_t index =0; index < 10 ; index++){
 //     cout << "Node" << index <<  " sent: " << data[index].total_replaced_sent << " recv: " << data[index].total_replaced_recv << endl;
  total_recv += data[index].total_replaced_recv;
  total_sent += data[index].total_replaced_sent;
}
 // cout << "total_sent - total_canceled = " << total_sent-total_canceled << endl;
//  cout << "total_recv = " << total_recv << endl;
//  cout << "total_sent: " << total_sent << endl;
  if(total_recv >= total_sent*0.9){
     cout << "Replace test... OK" << endl;
  }else{
     cout << "Replace test... NG" << endl;
  }

//  cout << "total_cancled: " << total_canceled << endl;




return 1;

}

void Parse_MemMap(){
//   cout << "Parse replace.bin" << endl;
   ifstream fin;

   for(uint16_t node_count =0; node_count < 10; node_count++){
         stringstream ss;
         ss << node_count;
         string path = file_path + ss.str() + "/replace.bin"; 
  	 fin.open(path.c_str());
  	 if(fin.fail()){
	 //cout << "Error::Failed to open " << path << endl;
   	 continue;
	}
  //      cout << " Successfully opened file " << node_count << endl;
 	  //file exists and opened successfully!
 	  //Now parse it.
   	 uint16_t a =0;
  	 uint16_t count;
  	 uint16_t recv;
  	 ReplaceDataStat obj;
	 data[node_count].total_replaced_recv = 0;
         data[node_count].total_replaced_sent = 0;
  	 do{	
      		 fin.read((char*) &obj, sizeof(ReplaceDataStat));
      		 data[node_count].total_replaced_recv = obj.total_replaced_recv;
      		 data[node_count].total_replaced_sent = obj.total_replaced_sent;
    //             cout << " sent: " << data[node_count].total_replaced_sent << " recv: " << data[node_count].total_replaced_recv << endl;
      		 a++;
 	  }while(fin.gcount () == sizeof(ReplaceDataStat));
  	 fin.close();
   }
  // cout << "Finish parsing file "  << endl;
}    

