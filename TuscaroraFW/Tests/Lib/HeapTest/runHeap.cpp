////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#include "runHeap.h"

#define NUMITEMS 100

// extern bool DBG_MAIN; // set in Lib/Misc/Debug.cpp
// extern bool DBG_TEST; // set in Lib/Misc/Debug.cpp



HeapTest::HeapTest() {
//   DBG_MAIN=true; // set in Lib/Misc/Debug.cpp
//   DBG_TEST=true; // set in Lib/Misc/Debug.cpp
#ifdef MINHEAP
  shortHeap = new HeapT<uint16_t,MyLess<uint16_t> >() ;
#else
  shortHeap = new HeapT<uint16_t,MyGreater<uint16_t> >() ;
#endif
  
  rnd = new UniformRandomInt(35000,45000);
}

void HeapTest:: Execute(int argc, char *argv[]){
  //Sequential test
  Level_0A();
  
  //Generate random numbers
  //Level_0B();
}


void HeapTest::Level_0A(){
int l1=1;
int l2=5;
  
 Debug_Printf(DBG_TEST,"Inserting uint16_t... \n");fflush(stdout);
  for(uint16_t i = l1; i >= 1; i--)
  {
    for(uint16_t j = (i * l2) - l2+1 ; j <= i * l2; j++)
    {
     Debug_Printf(DBG_TEST,"Inserting %d \n", j);fflush(stdout);
      shortHeap->Insert(j);
     //Debug_Printf(DBG_TEST,"Done. \n");fflush(stdout);
     Debug_Printf(DBG_TEST, "top number is %d , elements left in heap %d\n", shortHeap->PeekTop(), shortHeap->Size());
    }
  }

 Debug_Printf(DBG_TEST, "\n Extracting ... \n");fflush(stdout);
  uint16_t newTop =0;
  for(uint16_t i = l1*l2; i >= 1; i--)
  {
      newTop = shortHeap->ExtractTop();
     Debug_Printf(DBG_TEST, "Extracted num is %d top number is %d, elements left in heap %d \n", newTop, shortHeap->PeekTop(),  shortHeap->Size());
  }
}


void HeapTest::Level_0B(){
  int l1=100; 
  rnd = new UniformRandomInt(750,1250);
  
  for(uint16_t i = l1; i >= 1; i--)
  {
    uint16_t value = rnd->GetNext();  
   Debug_Printf(DBG_TEST,"Inserting %d \n", value);fflush(stdout);
      
    shortHeap->Insert(value);
     //Debug_Printf(DBG_TEST,"Done. \n");fflush(stdout);
   Debug_Printf(DBG_TEST, "top number is %d \n", shortHeap->PeekTop()); 
  }

 Debug_Printf(DBG_TEST, "\n Extracting ... \n");fflush(stdout);
  uint16_t newMax =0;
  for(uint16_t i = l1; i >= 1; i--)
  {
    newMax = shortHeap->ExtractTop();
   Debug_Printf(DBG_TEST, "Extracted num is %d top number is %d, elements left in heap %d \n", newMax, shortHeap->PeekTop(),  shortHeap->Size());
  }
}


//int testHeap(int argc, char* argv[]){
int main(int argc, char* argv[]){
  //Make sure RuntimeOpts construction is the very first line of main
  RuntimeOpts opts ( argc-1, argv+1 );
  InitPlatform(&opts);	//This does a platform specific initialization

  HeapTest hTest;
  hTest.Execute(argc,argv);

  //RunMasterLoopNeverExit(); //never exists on dce
  RunTimedMasterLoop();
	
  return 0;
}
