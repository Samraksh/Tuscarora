////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

/**************************

  based on shortest_current. Now path is changed for use on wsn03
  current 0.6 change adj matrix to vector 
  ver 0.5 Cope with the change in frisson. The emulation log is not synced among nodes. Now I need to  skip lines till everyone hear about fixed destination.
  ver 0.4 Added shortest path finder. 
  ver 0.3 if loop found. evict it as possible destination and continue 
  ver 0.2 fixed error in timesapm specification. timestamp was passed instaed of instance. 
  ver 0.1 initial release

  This program first build adj-matrix from emulationXX.txt
  Then this information is passed to dijkstra to build Shortest pass tree.
  Finally, shortest path is calculated and it is showed backwords
Syntax
first argument: number of node
second argument: fixed_destination id
Third argument: communication range;
Forth argument: Start Line (added after ver0.5)

Note: Need to manualy set V value for now.
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

#include <stdio.h>
#include <limits.h>
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
// Number of vertices in the graph
#define X 50
vector<vector<double> > vector_graph;
int prev[1000];
// A utility function to find the vertex with minimum distance value, from
// the set of vertices not yet included in shortest path tree
//int minDistance(double dist[], bool sptSet[])
int minDistance(double dist[], bool sptSet[], int V)
{
   // Initialize min value
   double min = 999999999.0;
   int min_index;
 
   for (int v = 0; v < V; v++)
     if (sptSet[v] == false && dist[v] <= min)
         min = dist[v], min_index = v;
 
   return min_index;
}
 
// A utility function to print the constructed distance array
int printSolution(double dist[], int n)
{
//   printf("Vertex   Distance from Source\n");
//   for (int i = 0; i < V; i++)
//     printf("%d \t\t %f\n", i, dist[i]);
}
 
// Funtion that implements Dijkstra's single source shortest path algorithm
// for a graph represented using adjacency matrix representation
void dijkstra(vector<vector<double> > vector_graph,double graph[X][X], int src, int V)
//void dijkstra(vector<vector<double> > graph, int src, int V)
{
     double dist[V];     // The output array.  dist[i] will hold the shortest
                      // distance from src to i
 
     bool sptSet[V]; // sptSet[i] will true if vertex i is included in shortest
                     // path tree or shortest distance from src to i is finalized

     // Initialize all distances as INFINITE and stpSet[] as false
     for (int i = 0; i < V; i++)
        dist[i] = 999999999.0, sptSet[i] = false;
 
     // Distance of source vertex from itself is always 0
     dist[src] = 0;
 
     // Find shortest path for all vertices
     for (int count = 0; count < V-1; count++)
     {
         // Pick the minimum distance vertex from the set of vertices not
         // yet processed. u is always equal to src in first iteration.
         int u = minDistance(dist, sptSet,V);
            
         // Mark the picked vertex as processed
         sptSet[u] = true;
 
         // Update dist value of the adjacent vertices of the picked vertex.
         for (int v = 0; v < V; v++){
 
             // Update dist[v] only if is not in sptSet, there is an edge from
             // u to v, and total weight of path from src to  v through u is
             // smaller than current value of dist[v]
            /* 
             if (!sptSet[v] && graph[u][v] && dist[u] != 999999999.0 && dist[u]+graph[u][v] < dist[v]){
                 dist[v] = dist[u] + graph[u][v];
                 prev[v] = u;
             }*/
        
             if (!sptSet[v] && vector_graph[u][v] && dist[u] != 999999999.0 && dist[u]+vector_graph[u][v] < dist[v]){
                 dist[v] = dist[u] + vector_graph[u][v];
                 prev[v] = u;
             }
         }   
     } 
     // print the constructed distance array
     printSolution(dist, V);
}
 




int main(int argc, char* argv[])
{

    //generate file path to files contains locaton information
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

    //generate path to all emulation files
    for(unsigned int i=0; i< total_node ; i++)
    {
        ostringstream convert;
        convert << i;
        complete_path=pre_path+convert.str();
        complete_path += post_path;
        path.push_back(complete_path);
	//show path for debuggin
        //cout << path[i] << endl;
    }


    //go though all nodes to collect location of fixed dest and each node
   // cout << "Go though all the node" << endl;
    int timestamp =0;
    for(unsigned int node=0; node<total_node; node++)
    {

        const char* file_path = path[node].c_str();
        ifstream fin(file_path,ios::in | ios::binary);
        //Now open files and read every statistics for each time stamp
        path[node];
        //printf("File path is %s\n",file_path);
        if(!fin)
        {
            cout << "Could not open emulation file" << path[node]  << endl;
        }
        else
        {
      
	    //read from emulation file
	    double fixed_destination_x, fixed_destination_y, x,y;
            timestamp = 0;
	    while (fin >> fixed_destination_x >> fixed_destination_y >> x >> y)
            {
                
    	        Node n;
                n.destination_x = fixed_destination_x;
                n.destination_y = fixed_destination_y;
                n.current_x = x;
                n.current_y = y;
//                cout << fixed_destination_x << " " << fixed_destination_y << " " << x << " " << y << " " <<  endl;
                oracle[timestamp][node].destination_x  = fixed_destination_x;
                oracle[timestamp][node].destination_y = fixed_destination_y;
                oracle[timestamp][node].current_x = x;
                oracle[timestamp][node].current_y = y;
               // printf("timestamp is %d\n",timestamp);
                timestamp++;
 	    }
    	}
    }

/**************************************************************************************************
  calcualte neighbor table and its distance
***************************************************************************************************/

    int range = atoi(argv[3]);
    int source_node = 0;
    int start_line = atoi(argv[4]);
    //For each time instance. Timestamp contains how many lines in each emulation .txt
//    for(int instance = 0; instance < total_node; instance++)
//    for(int instance = 0; instance < timestamp ; instance++)    
    //skip lines till all node get location of fixed destination
    for(int instance = start_line; instance < timestamp ; instance++)    
    {
        //cout << "timestamp is " << instance << endl;
        vector<double> row;
        vector<vector<double> > vector_graph;
        double graph[X][X];       
	//Go through all the node to build ajd matrix
        for(unsigned int node=0; node<total_node; node++)
        {
            //retreave node in question
            double current_x = oracle[instance][node].current_x;
            double current_y = oracle[instance][node].current_y;
            //just in case
            if((current_x < 0) || (current_y <0))
                abort();
            //cout << node << " location (" <<  current_x << "," <<  current_y <<")" << endl;
            for(unsigned int neighbor =0; neighbor < total_node; neighbor++){
                double potential_neighbor_x = oracle[instance][neighbor].current_x;
                double potential_neighbor_y = oracle[instance][neighbor].current_y;
                //cout << neighbor << " location (" << potential_neighbor_x << "," << potential_neighbor_y <<")" << endl;
                //check if this node is my neighbor or not
                double neighbor_dist = sqrt( ((current_x - potential_neighbor_x) * (current_x - potential_neighbor_x))  + ((current_y - potential_neighbor_y) * (current_y - potential_neighbor_y)));
                //cout << node << " Distance between (" << potential_neighbor_x << "," << potential_neighbor_y <<") and (" << current_x << "," << current_y <<")" << endl;
                if(neighbor_dist < range){
                    //this node has path to node. 
                    row.push_back(neighbor_dist);    
                    graph[node][neighbor] = neighbor_dist;
                }
                else{
                    row.push_back(0.0);
                    graph[node][neighbor] = 0;
                }
            }       
            vector_graph.push_back(row);
            row.clear();
        }  

        //Now, I can use dijkstra to find shortest path from all node to fixed destination 
        //cout << "Now open file to wrtie" << endl;
        //open file to store path count at instance
        stringstream inst_num;   
        inst_num << instance;
        string instance_path = "result/shortest_path_count" + inst_num.str() + "_" + fixed_destination_id + ".txt";
        FILE *file = fopen ( instance_path.c_str(), "w" );
        if ( file == NULL ){
            cout << "Could not open filefor shortest count: " << instance_path << endl;
            exit (1);
        }

        //Now find the shortest pass from node i to node j
        //for(int starter = 0; starter < total_node; starter++){
            //set previous node for starter(fixed_destination) to -1
            //prev[starter] = -1;
            prev[fixed_destination] = -1;
            //build shortest pass tree
            int size = total_node;
            dijkstra(vector_graph,graph, fixed_destination,size);
            //now I build Shortest Path Tree. Clear adj matrix for next iteration.
            vector_graph.clear();
            //dijkstra(graph, fixed_destination,size);
            
         
          //  cout << "Finish building SPT" << endl;
            //Now calculate shortest path from all node to fixed destination
            for(int k = 0; k <total_node; k++){
                int i = k;
                vector <int> complete_path;        
            //    cout << "From node " << i <<" to Node " << fixed_destination ;
                while(i>=0) {
                    //cout << " " << i << " ";
                    complete_path.push_back(i);
                    i = prev[i]; 
                }
              //  cout << " Total hop count is " << complete_path.size() << endl;
	        //store hop count to file
                stringstream path_count;
                path_count << complete_path.size();           
                fprintf ( file, "%s\n", path_count.str().c_str() );                   
                //clear complete_path for next iteration
                complete_path.clear();
            }
        //}
        fclose(file);
    }
    //cout << "Finished everything" << endl;

}

            

   

