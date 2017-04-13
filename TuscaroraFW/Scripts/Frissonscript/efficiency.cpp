////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

/**************************

This version is used on masahiro3 to evaluate simulation result.
Path is hardcoded so need to be changed every time.

  current 0.7 changed how I calculate valid path percentage. Now denominator is total valid path not 100
  ver 0.6 fixed path cont reading. As they are not hop count. They are count of nodes in the path.(shortest: initiator -> 1 hop before destination, emulation initiator->fixed_destination) 
  ver 0.5 comparison loop condition is same but sanpshot index is changed to instance_num
  ver 0.4 add parameter to specify fixed node id
  ver 0.3 add argument to specify starting line.
  ver 0.2 now program distinguish looping and no path
  ver 0.1 this program is used to find the ratio (Frisson pass/ Best pass)
  
Syntax
first argument: number of node
second argument: start line
third argument: end line;
forth argument: fixed node id
fifth argument: file name specifier
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


int main(int argc, char* argv[])
{

    vector<vector<int> > shortest;
    vector<vector<int> > frisson;
    vector<int> line;

    //get number of nodes
    unsigned int total_node = atoi(argv[1]);

    //get number of start line
    //unsigned int start_line = atoi(argv[2]);
    //get start line number from file.
    string startline_path = "./startline.txt";
    int start_line = 0;                                 
    const char* file_path = startline_path.c_str();
    ifstream fin(file_path,ios::in | ios::binary);
    if(!fin)
    {
        cout << "Cannot open shorest path file" << startline_path << endl;
    }
    else
    {
        //read startline path
        
        fin >> start_line;
        if(start_line ==0){
            cout << "Failed to read start line number" << endl; 
            exit(1);
        }
    }
    start_line++; //It points to last line with -1, so increment it by one
    //get number of timestamp
    unsigned int timestamp = atoi(argv[2]);
    
    //get fixed_destination_id
    unsigned int fixed_destination = atoi(argv[3]);
    ostringstream ss;
    ss << fixed_destination;

    //generate file path to files contains locaton information
    string frisson_pre_path = "./result/time_stamp";
    
    string shortest_pre_path = "./result/shortest_path_count";
    

    string post_path = ".txt";
    string complete_path;
    vector<string> shortest_path;
    vector<string> frisson_path;
    
    //generate path to all Frisson files
    for(unsigned int i=start_line; i < timestamp ; i++)
    {
        ostringstream convert;
        convert << i;
        complete_path= frisson_pre_path+convert.str() + "_" + ss.str();
        complete_path += post_path;
        frisson_path.push_back(complete_path);
	//show path for debuggin
        //cout << complete_path << endl;
    }

    //generate path to all shortestpath files
    for(unsigned int i=start_line; i< timestamp ; i++)
    {
        ostringstream convert;
        convert << i;
        complete_path=shortest_pre_path+convert.str() + "_" + ss.str();
        complete_path += post_path;
        shortest_path.push_back(complete_path);
	//show path for debuggin
        //cout << complete_path << endl;
    }
    //Now read shortest path information
    for(int snapshot = start_line ; snapshot < timestamp ; snapshot++){
        for(int node =0; node < total_node; node++){
            const char* file_path = shortest_path[node].c_str();
            ifstream fin(file_path,ios::in | ios::binary);
            if(!fin)
            {
                cout << "Cannot open shorest path file" << shortest_path[node]  << endl;
            }
            else
            {
                //read shortest path
                int path_count =0;
                while (fin >> path_count){
                    line.push_back(path_count);                
                }
                shortest.push_back(line);      
                line.clear();  
            }
        }
    }

    //Now read Frisson path information
    for(int snapshot = start_line ; snapshot < timestamp ; snapshot++){
        for(int node =0; node < total_node; node++){
            const char* file_path = frisson_path[node].c_str();
            ifstream fin(file_path,ios::in | ios::binary);
            if(!fin)
            {
                cout << "Cannot open frisson path file" << frisson_path[node]  << endl;
            }
            else
            {
                //read frisson path
                int path_count;
                while (fin >> path_count){
                    line.push_back(path_count);                
                }
                frisson.push_back(line);        
                line.clear();
            }
        }
    }

   // cout << "timestamp count: " << timestamp << endl;
    int looping_count;
    int no_path;
    double average_success = 0;
    double average_performance = 0;
    int specifier = atoi(argv[4]);
    stringstream specifierout;
    specifierout << specifier;
    int instance_num = 0;
    int denominator_count = 0;
    //For each timestamp instance
    //for(int snapshot=0; snapshot < timestamp; snapshot++)
    //for(unsigned int snapshot=0; snapshot < shortest.size(); snapshot++){
    for(int snapshot=start_line; snapshot < timestamp; snapshot++){
       //for each timestamp, reset total_valid_path to total_node size
       double total_valid_path = (double)total_node;
       looping_count = 0;
       no_path = 0;
       string ratio_path = "./ratio/";
       stringstream ss;
       ss << snapshot;
       ratio_path = ratio_path + ss.str() + "_" + specifierout.str();
       FILE *file = fopen(ratio_path.c_str(), "w");
       if(file==NULL){
           cout << "Failed to open log file" << endl;
       }
       else{   
           for(int index = 0 ; index < total_node; index++){
               int frisson_path_count  = frisson[instance_num][index];//only counting initiator not destination.(This is actually number of node in the path including initiator but not destination
               int shortest_path_count = shortest[instance_num][index]-1; //counting initiator and destination.(This is actually number of node in the path including initiator and fixed destination) 
               if(shortest_path_count == 0){
                   //cout << "No shortest path exist" << endl;
                   shortest_path_count = 1;
                   fprintf(file,"-80"); //no shortest path
                   total_valid_path = total_valid_path - 1.0;   
               }                
               //if looping
               else if(frisson_path_count == -100){
                   looping_count++;
                   fprintf(file,"-100\n");
                    //cout << instance_num << " "<< index << " Looping " << frisson_path_count << endl;
               }
               else if(frisson_path_count == -5){
                  no_path++;
                  //cout << instance_num << " "<< index << " No path fond " << frisson_path_count <<endl;
                  fprintf(file,"-5\n");
               }
               else{    
                   double ratio = (double)frisson_path_count / (double)shortest_path_count;
                   fprintf(file,"%f\n",ratio);
                   //cout << instance_num << " "<< index << " ratio between " << (double)frisson_path_count << " and " << (double)shortest_path_count << " is " << ratio << endl;
                   average_performance += ratio;
                   denominator_count++;
               }
           }
       }
      // cout << "At time instance " << snapshot <<" " << looping_count << "loops detected. No path: "<< no_path <<endl;
       double success_ratio = (total_valid_path -(double)(no_path + looping_count)) /total_valid_path;
       //create file and store successfull ratio
       //cout << "Success ratio is " << success_ratio  << endl;
      // cout << "Total valid path is  " << total_valid_path  << endl;
      // cout << "average performance is " << average_performance << endl;
      // cout << "denominator_count is " << denominator_count << endl;
       string successful_ratio_path = "./ratio/sccessful";
       successful_ratio_path = successful_ratio_path +  "_" + specifierout.str();
       FILE *file1 = fopen(successful_ratio_path.c_str(), "a");
       if(file1==NULL){
           cout << "Failed to open log file" << endl;
       }
       else{
           fprintf(file1,"%f %d %d\n",success_ratio,no_path, looping_count);
        //   cout << "Success ratio is " << success_ratio  << endl;
           fclose(file1);
       }
       average_success += success_ratio;
       instance_num++;
    }
   // cout << "sum of efficiency is " << average_performance << endl;
    //cout << "denominator_count is " << denominator_count << endl;
    double path_reachability = (average_success) / instance_num;   //path reachability
    double path_length_ratio = (average_performance) / (double)denominator_count; //path length ratio
    //store result in to file
    string finalresult_ratio_path = "./final_result";
    FILE *file2 = fopen(finalresult_ratio_path.c_str(), "a");
    if(file2==NULL){
        cout << "Failed to open log file" << endl;
        exit(1);
    }
    else{
        fprintf(file2,"%f %f\n",path_reachability, path_length_ratio); //write final statistics into file. if looping count, no_path count is needed, add them here
        fclose(file2);
    }
}
