////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#include "runMemMap.h"
#include "Lib/Misc/MemMap.h"
#include <Sys/Run.h>


int main(int argc, char* argv[]) {
	//Make sure RuntimeOpts construction is the very first line of main
	RuntimeOpts opts ( argc-1, argv+1 );
	InitPlatform(&opts);	//This does a platform specific initialization

  
	MemMap<int> map("MemMapTest");
	map.Truncate();
	map.addRecord(1);
	map.addRecord(2);
	map.addRecord(3);
	
	map.openForRead();

	printf("%d-%d--", *map.getNext(), map.hasNext());
	printf("%d-%d--", *map.getNext(), map.hasNext());
	printf("%d-%d--", *map.getNext(), map.hasNext());
	
	map.closeForRead();
}
