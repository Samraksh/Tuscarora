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
    //cout << "startline_count" << startline_count << endl;

    //store startline_count in file for later use.
    string startline_path = "./startline.txt";
    FILE *file = fopen ( startline_path.c_str(), "w" );
    if ( file == NULL ){
        cout << "could not open startline.txt" << endl;
        exit (1);
    }       
    stringstream ss;
    ss << startline_count;  //x coordinate of fixed location
    fprintf ( file, "%s\n", ss.str().c_str() );
    fclose ( file );





/**************************************************************************************************
  Create file for each time stamp. Contains oracle information for each moment
***************************************************************************************************/
  /*  for(unsigned int snapshot = 0 ; snapshot < timestamp ; snapshot++)
    {
        stringstream out1;   
        out1 << snapshot;
        string path = "/home/wsn/FixedWireless/Tuscarora/Tests/Patterns/Frisson/snapshot/snapshot" + out1.str() + ".txt";
        FILE *file = fopen ( path.c_str(), "w" );
        if ( file == NULL ){
            cout << "This node is not terminating node" << endl;
            exit (1);
        }       
        for(unsigned int node=0; node < total_node; node++)
        {
            stringstream ss;
            ss << oracle[snapshot][node].destination_x  //x coordinate of fixed location
            << " " << oracle[snapshot][node].destination_y
            << " " << oracle[snapshot][node].current_x
            << " " << oracle[snapshot][node].current_y;
	    fprintf ( file, "%s\n", ss.str().c_str() );
            cout << "Writing: " << snapshot <<"   " << ss.str() << endl;
            if((oracle[snapshot][node].destination_x < 0) || ( oracle[snapshot][node].destination_y < 0)){
                if(snapshot >= atoi(argv[5])){
                    cout << "Read fixed destination error.Why I get -1" << endl;
                    abort();
                }
            }

        }
        fclose ( file );
    }
*/
/**************************************************************************************************
  calcualte file path for given sorce node to fixed_destination.
  this program only stores information with ONE fixed destination
***************************************************************************************************/

    int data_source = atoi(argv[3]);
    int range = atoi(argv[4]);
    int start_line = atoi(argv[5]);
    int source_node = data_source;

    //For each time instance
    for(int instance = start_line ; instance < timestamp; instance++)
    {
        //create files stores path count for each instance
        stringstream inst_num;   
        inst_num << instance;
        string instance_path = "result/time_stamp" + inst_num.str() + "_" + fixed_destination_id + ".txt";
        FILE *file = fopen ( instance_path.c_str(), "w" );
        if ( file == NULL ){
            cout << "Cannot open file to store emulated path count" << endl;
            exit (1);
        }         
       // cout << "Find path for timestamp " << instance << endl;
        //Assign each node to be an initiator and emulate
        vector<int> complete_path;
        for(int initiator = 0; initiator < total_node; initiator++){
            //cout << "For initiator " << initiator << endl; 
            if(initiator != fixed_destination){
                int dest = -1;
                source_node = initiator;    
                //Start path caliculation
                int debug_count = 100;
                uint32_t path_count = 1;
                complete_path.push_back(initiator);   
                //cout <<  "complet_path is " << complete_path[0] << endl;
                if(complete_path.size() != 1){
                    //Something went wrong. The size should be one.
                    cout << "complete_path length should be 1" << endl;
                    abort();
                }    
                do
                {    
                    path_count++;
                    //get next hop
                    next_hop(source_node, instance, dest, total_node, range,fixed_destination);
                    //Check if the dest is already in compilete_path
                    if(find(complete_path.begin(), complete_path.end(), dest) !=complete_path.end()){
                        //cout << "I am comming back to my intermidiate node " << dest << "This generates loop" << endl;
                        stringstream ss;
                        ss << initiator << " ";
                        fprintf ( file, "%s\n", "-100");
                        break;
                    }                  
                    // add next hop to complete_path.
                    complete_path.push_back(dest);   
                    //cout << " complete_path is" << " ";
                    stringstream travel;
                     for(unsigned int it = 0; it < complete_path.size(); it++){
                        travel << complete_path[it] << " ";
                    }      
                    //cout << travel.str() << endl;
                    //invalid return
                    if(dest == -1){
                        fprintf ( file, "%s\n", "-5");
                        //cout << "Could not find valid next hop... Terminating" << endl;
                        break;
                    }
                    else if(dest == fixed_destination){  //reached destination
                        stringstream ss;
                        ss << path_count;// << " ";
                        string s = ss.str(); 
                        s = s + " path:" + travel.str();      
                        fprintf (file, "%s\n",  ss.str().c_str() );
                        //printf("Node %d is the destination path length is %s\n",dest,ss.str().c_str());
                        break;
                    }
                    else{
                        source_node = dest;
                        stringstream ss;
                        ss << dest << " ";
                        //cout << "Next hop is " << dest << endl;
                    }
                    debug_count--;
                    if(debug_count < 0){ 
                        abort();
                        break;
                    }
                }while(dest != fixed_destination);
                complete_path.clear(); //clear information.
            }      
            else{
                stringstream ss;
                fprintf ( file, "%s\n", "-10");
            }
                 
        }
        fclose(file);
    }
}




//For each node, find euclidian distance
void next_hop(int source_node, int timestamp, int& dest, int total_node, int range, int fixed_destination)
{
    //cout << "Checking next hop for node " << source_node << " at timestamp " << timestamp << endl;
     //Location of fixed_destination
    double fixed_x = 0;  //x coordinate of fixed destination
    double fixed_y = 0;  //y coordinate of fixed destination
    double current_x = 0;
    double current_y = 0;
    // shortest distance
    double shortest_distance = 0;
    double communication_range = (double)range;
    dest = -1;     
    //Retreave fixed destination at source_node
    fixed_x = oracle[timestamp][source_node].destination_x;
    fixed_y = oracle[timestamp][source_node].destination_y;
    current_x = oracle[timestamp][source_node].current_x;
    current_y = oracle[timestamp][source_node].current_y;
    //cout << "(" << fixed_x << "," << fixed_y<< ")" << endl;
    //flag used to store first distance
    bool flag = true;
    //for each node in network, find euclidian distance between fixed_destination
    for(unsigned int node=0; node<total_node; node++)
    {
        if(1){//node != source_node){
            //retreave coordinate of node
            double temp_x = oracle[timestamp][node].current_x;
            double temp_y = oracle[timestamp][node].current_y;
            //check if this node is my neighbor or not
            double neighbor_dist = sqrt( ((current_x - temp_x) * (current_x - temp_x))  + ((current_y - temp_y) * (current_y - temp_y)));
            //cout << node << " Distance between (" << temp_x << "," << temp_y <<") and (" << current_x << "," << current_y <<")" << endl;
            if(neighbor_dist < communication_range){
                //this node is within range. But make sure this is not myself.The only situation where I need to find distance between myself is 
                //this neighbor node is fixed distination 
                if((node == source_node) && (node != fixed_destination)){
                     //This node at(temp_x, temp_y) is not valid neighbor
                     //skip this node
                     //cout << "Skip node " << node << endl;
                }
                else{
                    //This is my neighbor. So find distance to fixed destination
                    //cout << "This is my neighbor. Now find euclidian distance between fixed destination(" << fixed_x << "," << fixed_y << ")" << endl;
//                  double dist = sqrt((fixed_x - temp_x)^2 + (fixed_y - temp_y)^2));
                    double dist = sqrt( ((fixed_x - temp_x) * (fixed_x - temp_x))  + ((fixed_y - temp_y) * (fixed_y - temp_y)));
                    //cout << "Node ID: " << node << "temp_x " << temp_x << " temp_y " << temp_y << " current_x " << current_x << " current_y " << current_y << " dist " << dist << endl; 
                    //is this very first distance calculated?
                    if(flag){
                       // cout << "initial shortest distance is " << dist << endl;
                        shortest_distance = dist;
                        dest = node;
                        flag = false;
                    }
                    else{
                        //Is this shortest?
                        if(dist < shortest_distance){
                            shortest_distance = dist;
                            dest = node;
                            //cout << "shortest_distance is " << dist << endl;
                        }
                    }                 
                }
            }
        }
        else{
             //No point finding distance between myself
             //cout << "No point finding distance between myself. "<< endl;
        }
    }   
    //I exaused all nodes. Now check if distance between myself and fixed_destination node is not shortest
    double dist_me_fixed = sqrt( ((fixed_x - current_x) * (fixed_x - current_x))  + ((fixed_y - current_y) * (fixed_y - current_y)));
    //cout << "The distance between me and fixed destination is " << dist_me_fixed;
    if(shortest_distance > dist_me_fixed){
        //I am the closest node to the distination.
        dest = -1;
    }
}


            

   

