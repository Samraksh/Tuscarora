////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef MEMMAP_H_
#define MEMMAP_H_


#include <fstream>
#include <stdint.h>
#include <unistd.h>
#include "sys/mman.h"
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <fcntl.h>
#include <cstdlib>

#ifdef NO_USE_TYPES_DIR
#include "FrameworkTypes.h"
#else
#include <Base/FrameworkTypes.h>
#endif

#define BITS 32
#define MAXNBR 3000 //can not be a extern

using namespace Types;
extern const uint16_t MAX_NBR;
namespace PAL {
///The Store and the Header structures need to be of the same size to do mem-mapping
///Figure out which is smaller pad it
struct Store {
  uint32_t x; //Divide by 1000
  uint32_t y; //Divide by 1000
  uint32_t neighbors[(MAXNBR / BITS) + 1];
};


 enum EstimationLogE {
   LINK_ADDED = 1,
   LINK_REMOVED = 2,//
   LINK_UPDATED = 3,
   LINK_SKIPPED = 4,//
   LINK_SENT = 5,
   LINK_POTENTIAL_ADD = 6,
   LINK_POTENTIAL_DEL = 7
 };
 
 struct EstimationLogElement {
   double x;
   double y;
   NodeId_t id;
   NodeId_t nbr;
   uint64_t time;
   EstimationLogE type;
 };
 

class Header {
 public:
  uint32_t nodeid;
  uint32_t beaconPeriod;
  uint8_t inactivityPeriod;
  uint16_t range;
  uint8_t linkEstimationType;
  uint8_t discoveryType;
  uint16_t numNodes;
  uint16_t runTime;
  uint8_t density;
  uint8_t mobilityModel;
  uint16_t speed;
};

union FWTData {
  Store s;
  Header h;
};


template <class Obj> class MemMap {
 private:
  int fd;
  std::string fname;
  std::ofstream file;
  Obj* data;
  Obj* ptr;
  long size;
 public:
  MemMap(std::string filename) { fname = filename; data = 0; ptr = 0; }
  void Truncate() { remove(fname.c_str()); }
  void openForRead(bool createBlankFile = false) {
    struct stat stat_buf;
    if( createBlankFile ) {
      if ( 0 != stat(fname.c_str(), &stat_buf) ) {
	file.open(fname.c_str(), std::ios::out | std::ios::binary | std::ios::app);
	file.close();
      }
    }
    if ( 0 != stat(fname.c_str(), &stat_buf) ) {
      perror ( "openForRead.stat" );
      printf("Cannot open %s\n", fname.c_str());
      exit ( EXIT_FAILURE );
    }
    size = stat_buf.st_size;
    if ( 0 > (fd = open(fname.c_str(), O_RDWR, (mode_t) 0600 ))) {
      perror ( "open" );
      exit ( EXIT_FAILURE );
    }
    data = (Obj*)mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    ptr = data;
    //for(uint x = 0; x <  stat_buf.st_size / sizeof(Type); x++) {
  }
  long getNum() { return size / sizeof(Obj); }
  bool hasNext() {
    if(size == 0) return false;
    return (uint64_t)(ptr - data) < (uint64_t)((size / sizeof(Obj)) - 1);
  }
  Obj* getNext() { return ptr++;}
  Obj* get(int num) { printf("data is %p and get %d is %p\n", data, num, data + num); return data + num; }
  void closeForRead() {
    munmap(data, size);
    close(fd);
  }
  void addRecord(const Obj& obj) {
    file.open(fname.c_str(), std::ios::out | std::ios::binary | std::ios::app);
    file.write(reinterpret_cast<const char*>(&obj), sizeof(Obj));
    file.close();
  }
};
}
#endif /* MEMMAP_H_ */
