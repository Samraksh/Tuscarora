////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

/***************************************************************
open emulation files and check the given row has -1 or not

argument 1: number of node
argumetn 2: fixed destination id
argument 3: line number
****************************************************************/




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
                  

    //collects all information at time x
    vector<Node> location;
    //collects all time information and allocate actual memory
    vector<vector<Node> > oracle(10000, vector<Node>(1000));
void next_hop(int source_node, int timestamp, int& dest, int total_node, int range, int fixed_destination);
int main(int argc, char* argv[])
{

    //cout << "Go though  the node" << endl;

    //generate file path
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
    string post_path = "/emulation" + ss1.str() + ".txt";
    string complete_path;
    vector<string> path;

    //get number of nodes
    unsigned int total_node = atoi(argv[1]);

    //generate path
    for(unsigned int i=0; i< total_node ; i++)
    {
        ostringstream convert;
        convert << i;
        complete_path=pre_path+convert.str();
        complete_path += post_path;
        path.push_back(complete_path);
	//show path for debuggin
       // cout << path[i] << endl;
    }


    //go though all nodes (all individual Token.bin file) to collect all stat data
    //cout << "Go though all the node ^^" << endl;
    int timestamp =0;
    //Now I need to find out at which moment everyone got idea where fixed destination could be.
    int startline_count = 0;
    for(unsigned int node=0; node<total_node; node++)
    {

        const char* file_path = path[node].c_str();
        ifstream fin(file_path,ios::in | ios::binary);
        //Now open files and read every statistics for each time stamp
        //path[node];
        //printf("File path is %s\n",file_path);
        if(!fin)
        {
            cout << "This node is not terminating node" << node  << endl;
        }
        else
        {
      
	    //read from emulation file
	    double fixed_destination_x, fixed_destination_y, x,y;
            int temp_startline_count = 0;
            timestamp = 0;
	    while (fin >> fixed_destination_x >> fixed_destination_y >> x >> y)
            {              
                //cout << "fixed_destination: (" << fixed_destination_x << "," << fixed_destination_y << ")" << endl;
     	        Node n;
                n.destination_x = fixed_destination_x;
                n.destination_y = fixed_destination_y;
                n.current_x = x;
                n.current_y = y;
                //location.push_back(n);
                oracle[timestamp][node].destination_x  = fixed_destination_x;
                oracle[timestamp][node].destination_y = fixed_destination_y;
                oracle[timestamp][node].current_x = x;
                oracle[timestamp][node].current_y = y;
                timestamp++;
                if(fixed_destination_x == -1)
                    temp_startline_count++;
 	    }
            //cout << "temp_startline_count" << temp_startline_count << endl;
            if(temp_startline_count > startline_count)
                startline_count = temp_startline_count;
    	}
    }
    //Now check if given line number has valid fixed destination coordinate or not
    int offset = atoi(argv[3]);
    //for all node at given time instance
    for(int index =0; index < total_node; index++){
        if(oracle[offset][index].destination_x == -1){
            cout << "FAIL! Node: " << index << " does NOT know location of Fixed Destination: " << fixed_destination << endl;
            return 90;
        }
    }     
}
