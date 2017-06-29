////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef PLATFORM_H_
#define PLATFORM_H_


#include <Sys/RuntimeOpts.h>
#include <Base/FrameworkTypes.h>
#include <Lib/PAL/PAL_Lib.h>
#include <unistd.h>
#include <stdio.h>


NodeId_t CONTROL_BASE_ID;	//A basestation or a controller node id
NodeId_t MY_NODE_ID;
RuntimeOpts* RUNTIME_OPTS;
uint32_t NETWORK_SIZE;

uint32_t runTime;
uint64_t StartTime, CurTime;

uint32_t sleepCount;

FILE *simProgress;

uint64_t ExpiredTime(){ 
  CurTime = PAL::SysTime::GetEpochTimeInMicroSec();
  return (CurTime - StartTime);
}

bool IsRunDone(double runTimeInSecs){
	uint64_t expiredTime = ExpiredTime();
	//printf("Expired Time: %lu, Total RunTime: %f \n", expiredTime, runTimeInSecs*1000000);
  if(expiredTime < runTimeInSecs * 1000000) return false;
  else return true; 
}

double  PercentToRun(double runTimeInSecs){
	uint64_t expiredTime = ExpiredTime();
	//printf("Expired Time: %lu, Total RunTime: %f \n", expiredTime, runTimeInSecs*1000000);
	uint64_t totaltime = runTimeInSecs * 1000000;
  if(expiredTime < totaltime) {
	  return double(expiredTime*100)/double(totaltime);
  }
  else return -1; 
}


void InitPlatform(RuntimeOpts *opts){
	RUNTIME_OPTS = opts;
	runTime = opts->runTime;
	StartTime = PAL::SysTime::GetEpochTimeInMicroSec();

	simProgress = fopen ("Progress", "w");
	if(!simProgress) {
		printf("Error opening progress file in Run.h ... quiting.\n");
		exit(1);
	}
	
#if defined(PLATFORM_LINUX)
	printf("\n---- Initializing for a Standard Linux (POSIX) Platform ----\n\n");
	MY_NODE_ID = opts->my_node_id;
	NETWORK_SIZE = opts->nodes;
#elif defined(PLATFORM_DCE) 
	printf("\n---- Initializing for a NS3-DCE Platform ----\n\n");
	MY_NODE_ID = atoi(getenv("NODEID"));
	NETWORK_SIZE=atoi(getenv("NETWORK_SIZE"));
	CONTROL_BASE_ID = opts->nodes;
#endif
	RngSeedManager::SetBaseStreamIndex(MY_NODE_ID);
	printf("---- Initialized system for Node: %d ----\n\n", MY_NODE_ID);	
		
}

void RunMasterLoopNeverExit(){
	while(1){
		//printf("Wake count: %d \n",sleepCount); fflush(stdout);
		
		usleep(500000); //sleep for 500milli seconds
		sleepCount++;
	}
}

void RunTimedMasterLoop(){

	if(RUNTIME_OPTS->runTime == 0) {
		printf("Deployment will not quit\n");fflush(stdout);
		RunMasterLoopNeverExit();
	}
	else{
		printf("Deployment will run for %f seconds. \n", RUNTIME_OPTS->runTime);fflush(stdout);
		while(1){
			//printf("Wake count: %d \n",sleepCount); fflush(stdout);
			double pleft=PercentToRun(RUNTIME_OPTS->runTime);
			if(pleft < 0){
				printf("Deployment ran for %f seconds. Quiting..\n", RUNTIME_OPTS->runTime);fflush(stdout);
				fprintf(simProgress,"%2.2f",100.00);
				fclose(simProgress);
				exit(0);
			}
			else {
				rewind(simProgress); //return to the beging of the file.
				//simProgress has only a single value that shows how much of simulation is done.
				fprintf(simProgress,"%2.2f",pleft);
			}
			usleep(500000); //sleep for 500milli seconds
			sleepCount++;
		}
	}
}

#endif //PLATFORM_H
