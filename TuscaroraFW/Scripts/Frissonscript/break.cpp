////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

/**************************
  current 0.61 change output file name. now it contains fixed id number
  0.6 Changed format of emulation file to cope with mistake I made in frisson file.(the timestamp among files are not synced). Now it has "-1" if node have not heard about fixed destination yet.
  ver 0.5 next hop calculation. (I need to also compare distance between myself and fixed distination as I could be the closest one) 
  ver 0.4 Bug fix. As I am allocating memory for vector in the begining, I need to manulay change this value if node size changed.
  ver 0.3 Adding complete path to hold entire trip. This is used to detect looping
  ver 0.2 fixed error in specifing instance. timestamp was passed instead of instance
  ver 0.1 initial release
  This program read all node emulation output and mimic singleton.
  It creates array with [timestamp][nodeid]

Syntax
first argument: number of node
second argument: fixed_destination id
third argument: source node: From this node, Frisson data send is initiated
fourth argument: communication range;
fifth argument: timestamp


Note: I am allocating memory for vector in the begining so this value needs to be adjusted manually.
***************************/
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdlib.h>
#include <vector>
#include <list>
#include <stdint.h>
#include <algorithm>
#include <string>
#include <math.h>   //sqrt
using namespace std;
   struct Node {
              double destination_x; //x coordinate of fixed destination
              double destination_y; //x coordinate of fixed destination
              double current_x; //x coordinate 
              double current_y; //x coordinate
              Node():destination_x(0),destination_y(0),current_x(0),current_y(0){ }
    };
                  
       struct emulation_out {
              uint16_t fixed_destination;
              uint32_t destination_x;
              uint32_t destination_y;
              uint32_t my_x;
              uint32_t my_y;            
       };
    //collects all information at time x
    vector<Node> location;
    //collects all time information and allocate actual memory
    vector<vector<Node> > oracle;
    void next_hop(int source_node, int timestamp, int& dest, int total_node, int range, int fixed_destination);
int main(int argc, char* argv[])
{
    //cout << "Brake emulation.bin into each fixed destination" << endl;

    //generate file path to emulation.bin
    char* ppath;
    ppath = getenv("NS3_DCE");
    
    //Generate path to the output file for each node
    string pre_path(ppath);
    //Get fixed_destination id
    unsigned int fixed_destination = atoi(argv[2]);
    stringstream ss1;   
    ss1 << fixed_destination;
    string fixed_destination_id = ss1.str();
    pre_path = pre_path + "/files-";
    string post_path = "/emulation.bin";
    string complete_path;
    vector<string> path;

    //get number of nodes
    unsigned int total_node = atoi(argv[1]);
    //cout <<"Get total node number" << total_node << endl;
    //generate path
    for(unsigned int i=0; i< total_node ; i++)
    {
        ostringstream convert;
        convert << i;
        complete_path=pre_path + convert.str();
        complete_path += post_path;
        path.push_back(complete_path);
	//show path for debuggin
        //cout << path[i] << endl;
    }


    //Now I have path to all emulation.bin files. The number of emulation.bin is same as total node.
    // This emulation.bin has <fixed_destination_id, fixed_destination_x, fixed_destination_y,my_x,my_y>
    //My current approch requires one emulation output for each fixed_destination_id, I need to break them.

    //cout << "Go though all emulation bin and break them" << endl;
    int timestamp =0;
    //Now I need to find out at which moment everyone got idea where fixed destination could be.
    int startline_count = 0;
    for(unsigned int node=0; node<total_node; node++)
    {

        const char* file_path = path[node].c_str();
        ifstream fin(file_path,ios::in | ios::binary);
        if(!fin)
        {
            cout << "Could not open emulation.bin at node: " << node  << endl;
        }
        else
        {
      
	    //read from emulation file
            int temp_startline_count = 0;
            timestamp = 0;
            //data_read stores data temporarily
            emulation_out data_read;
            //read emulation.bin file  
            do{
                fin.read((char *) &data_read,sizeof(emulation_out));
                //check if end of file flag is true or not.  
                if(fin.eof())
                    break;          
                
                //cout << "At node " << node << " fixed_destination:" << data_read.fixed_destination <<  " (" << (int)data_read.destination_x << "," << (int)data_read.destination_y << ")" << endl;
                //Now write it to file
                stringstream index;
                index<< node;
                string write_path = pre_path + index.str();
                index.str(""); //clear
                index << data_read.fixed_destination;
                write_path = write_path+"/emulation" + index.str() + ".txt";
                //cout << write_path << endl;

                //open file to write
                FILE *file = fopen ( write_path.c_str(), "a" );
                if ( file == NULL ){
                    cout << "Could not open file to write: " << write_path << endl;
                    exit (1);
                }       
                stringstream ss;
                ss << (int32_t)data_read.destination_x  //x coordinate of fixed location
                << " " << (int32_t)data_read.destination_y
                << " " << data_read.my_x
                << " " << data_read.my_y;
                fprintf ( file, "%s\n", ss.str().c_str() );
                //cout << "Writing: " <<   ss.str() << endl;
                fclose ( file );       

      
                timestamp++;
                if(data_read.destination_x == -1)
                    temp_startline_count++;
 	    }while(1);
            //cout << "temp_startline_count" << temp_startline_count << endl;
            if(temp_startline_count > startline_count)
                startline_count = temp_startline_count;
    	}
    }
    //cout << "startline_count" << startline_count << endl;

    //Now I break emulation.bin. Now load them into memory.
}


            

   

