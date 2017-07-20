////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include "sys/mman.h"
#include <unistd.h>
#include <math.h>
#include <string.h>

#include "../../../../Platform/linux/PAL/Logs/MemMap.h"

const uint16_t MAX_NBR=100;

enum Status {
  UnknownNeighbor,
  Neighbor,
  FalseNeighbor,
  NonNeighbor
};

int main(int argc, char *argv[]) {
  printf("This no longer functions as expected. If this is used, let Bryan know, and he will fix it\n");
  return 0;
  int f = 0;
  int i = 0;
  f = open(argv[1], O_RDONLY);
  i = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);
  Header* header = (Header*)mmap(0, sizeof(Header), PROT_READ, MAP_SHARED, f, 0);
  lseek(i, header->sizeOfStruct * header->numRecords * header->numNodes - 1, SEEK_SET);
  int tmp = write(i, "", 1);
  //Load initial map
  FWTData *data = (FWTData*)mmap(0, sizeof(FWTData) * (header->numRecords + 1) * header->numNodes, PROT_READ, MAP_SHARED, f, 0);
  Store *inter = (Store*)mmap(0, sizeof(Store) * (header->numRecords * header->numNodes), PROT_READ | PROT_WRITE, MAP_SHARED, i, 0);
  //Graphing it to 2d array
  Store **n = new Store*[header->numNodes];
  for(int x = 0; x < header->numNodes; x++) {
    n[x] = (Store*)&data[x * (header->numRecords + 1) + 1];
  }

  printf("Records:\t%d\nPeriod:\t\t%d\nInactive:\t%d\nRange:\t\t%d\nType:\t\t%d\nSize:\t\t%d\nDensity:\t%d\nSpeed:\t\t%d\nStruct:\t\t%d\nSizeof:\t\t%d\n", header->numRecords, header->beaconPeriod, header->inactivityPeriod, header->range, header->linkEstimationType, header->numNodes, header->density, header->speed, header->sizeOfStruct, sizeof(Store));

  for(uint32_t t = 0; t < header->numRecords; t++) {
    for(uint32_t i = 0; i < header->numNodes; i++) {
      printf("%d %d\n", i, t);
      Store* src = &n[i][t];
      Store* des = &inter[(t * header->numNodes) + i];
      printf("%p %p\n", src, des);
      memcpy(des, src, header->sizeOfStruct);
      //inter[t * header->numRecords + n] = i[n][t];
    }
    //return 0;
  }

  munmap(data, sizeof(FWTData) * (header->numRecords + 1) * header->numNodes);
  munmap(inter, sizeof(Store) * header->numRecords * header->numNodes);
  close(f);
  return tmp;
}
