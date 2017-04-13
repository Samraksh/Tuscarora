////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#include "runList.h"
#include <Sys/Run.h>

#define NUMITEMS 100

// extern bool DBG_MAIN; // set in Lib/Misc/Debug.cpp
// extern bool DBG_TEST; // set in Lib/Misc/Debug.cpp




ListTest::ListTest() {
//   DBG_MAIN=true; // set in Lib/Misc/Debug.cpp
//   DBG_TEST=true; // set in Lib/Misc/Debug.cpp

#ifdef DUPS
  shortList = new SortedListT<uint16_t, true, StdLess<uint16_t> >() ;
#else
  shortList = new SortedListT<uint16_t,false, StdLess<uint16_t> >() ;
#endif
  
  rnd = new UniformRandomInt(35000,45000);
}

void ListTest:: Execute(int argc, char *argv[]){
  //Sequential test
  //Level_0A();
  
  //Generate random numbers
  Level_0B();
}


void ListTest::Level_0A(){
int l1=10;
int l2=2;
  
 Debug_Printf(DBG_TEST,"Inserting uint16_t... \n");fflush(stdout);
  for(uint16_t i = l1; i >= 1; i--)
  {
    for(uint16_t j = (i * l2) - l2+1 ; j <= i * l2; j++)
    {
     Debug_Printf(DBG_TEST,"Inserting %d \n", j);fflush(stdout);
      shortList->Insert(j);
      //Debug_Printf(DBG_TEST,"Done. \n");fflush(stdout);
     Debug_Printf(DBG_TEST, "First number is %d \n", shortList->GetItem(0));
    }
  }

 Debug_Printf(DBG_TEST, "\n Reading back ... \n");fflush(stdout);
  uint16_t item =0;
  for(uint16_t i = l1*l2; i >= 1; i--)
  {
   Debug_Printf(DBG_TEST, "Looking for item %d....\n", i-1);
    item = shortList->GetItem(i-1);
   Debug_Printf(DBG_TEST, "Item at location %d  is %d\n", i-1, item);
  }
  
 Debug_Printf(DBG_TEST, "\n Deleting ... \n");fflush(stdout);
  uint16_t newMax =0;
  newMax = shortList->GetItem(shortList->Size() -1);
 Debug_Printf(DBG_TEST, "Max num: %d , elements left in list: %d \n", newMax, shortList->Size());
  for(uint16_t i = l1*l2; i >= 1; i--)
  {
    shortList->Delete(shortList->Size() -1);
    newMax = shortList->GetItem(shortList->Size() -1);
   Debug_Printf(DBG_TEST, "Max num: %d.  elements left in list: %d \n", newMax, shortList->Size());
  }
}


void ListTest::Level_0B(){
  int l1=100; 
  
  rnd = new UniformRandomInt(750,1250);
  
  for(uint16_t i = l1; i >= 1; i--)
  {
    uint16_t value = rnd->GetNext();  
   Debug_Printf(DBG_TEST,"Inserting %d \n", value);fflush(stdout);
      
    shortList->Insert(value);
     //Debug_Printf(DBG_TEST,"Done. \n");fflush(stdout);
    //Debug_Printf(DBG_TEST, "Min number is %d \n", shortList[0]); 
  }

 Debug_Printf(DBG_TEST, "\n Reading back ... \n");fflush(stdout);
  uint16_t item =0;
  for(uint16_t i = l1; i >= 1; i--)
  {
    
    item = shortList->GetItem(shortList->Size() -i);
   Debug_Printf(DBG_TEST, "Item at location  %d  is %d \n", shortList->Size() -i, item);
  }
  
 Debug_Printf(DBG_TEST, "\n Deleting ... \n");fflush(stdout);
  uint16_t newMax =0;
  newMax = shortList->GetItem(shortList->Size() -1);
 Debug_Printf(DBG_TEST, "Max num: %d , elements left in list: %d \n", newMax, shortList->Size());
  for(uint16_t i = l1; i >= 1; i--)
  {
    shortList->Delete(shortList->Size() -1);
    newMax = shortList->GetItem(shortList->Size() -1);
   Debug_Printf(DBG_TEST, "Max num: %d.  elements left in list: %d \n", newMax, shortList->Size());
  }
}


//int testList(int argc, char* argv[]){
int main(int argc, char* argv[]){
  //Make sure RuntimeOpts construction is the very first line of main
  RuntimeOpts opts ( argc-1, argv+1 );
  InitPlatform(&opts);    //This does a platform specific initialization

  ListTest hTest;
  hTest.Execute(argc,argv);

  RunTimedMasterLoop();
  return 0;
}
