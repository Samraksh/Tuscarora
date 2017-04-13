////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#include "runBST.h"

#define NUMITEMS 100000

// extern bool DBG_MAIN; // set in Lib/Misc/Debug.cpp
// extern bool DBG_TEST; // set in Lib/Misc/Debug.cpp


BSTTest::BSTTest() {
//   DBG_MAIN=true; // set in Lib/Misc/Debug.cpp
//   DBG_TEST=true; // set in Lib/Misc/Debug.cpp
  shortBST = new BST_T<uint16_t >() ;
  rnd = new UniformRandomInt(35000,45000);
}

void BSTTest:: Execute(int argc, char *argv[]){
  //Sequential test
  Level_0A();
  
  //Generate random numbers
  Level_0B();
}


void BSTTest::Level_0A(){
int l1=10;
int l2=NUMITEMS/l1;
  
 Debug_Printf(DBG_TEST,"Inserting uint16_t... \n");fflush(stdout);
  for(uint16_t i = l1; i >= 1; i--)
  {
    for(uint16_t j = (i * l2) - l2+1 ; j <= i * l2; j++)
    {
     Debug_Printf(DBG_TEST,"Inserting %d \n", j);fflush(stdout);
      //MyData *x=new MyData(j);
      shortBST->Insert(j);
     //Debug_Printf(DBG_TEST,"Done. \n");fflush(stdout);
     Debug_Printf(DBG_TEST, "top number is %d \n", shortBST->PeekRoot());
    }
  }

 Debug_Printf(DBG_TEST, "\n Extracting ... \n");fflush(stdout);
  uint16_t newTop =0;
  for(uint16_t i = l1*l2; i >= 1; i--)
  {
      newTop = shortBST->ExtractRoot();
     Debug_Printf(DBG_TEST, "Extracted num is %d top number is %d, elements left in heap %d \n", newTop, shortBST->PeekRoot(),  shortBST->NoOfElements());
  }
}


void BSTTest::Level_0B(){
  int l1=NUMITEMS; 
  rnd = new UniformRandomInt(750,1250);
  
  for(uint16_t i = l1; i >= 1; i--)
  {
    uint16_t value = rnd->GetNext();  
   Debug_Printf(DBG_TEST,"Inserting %d \n", value);fflush(stdout);
    //MyData *value=new MyData(key);  
    shortBST->Insert(value);
     //Debug_Printf(DBG_TEST,"Done. \n");fflush(stdout);
   Debug_Printf(DBG_TEST, "top number is %d \n", shortBST->PeekRoot()); 
  }

 Debug_Printf(DBG_TEST, "\n Extracting ... \n");fflush(stdout);
  uint16_t newMax =0;
  for(uint16_t i = l1; i >= 1; i--)
  {
    newMax = shortBST->ExtractRoot();
   Debug_Printf(DBG_TEST, "Extracted num is %d top number is %d, elements left in heap %d \n", newMax, shortBST->PeekRoot(),  shortBST->NoOfElements());
  }
}


//int testBST(int argc, char* argv[]){
int main(int argc, char* argv[]){
  //Make sure RuntimeOpts construction is the very first line of main
  RuntimeOpts opts ( argc-1, argv+1 );
  InitPlatform(&opts);	//This does a platform specific initialization

  BSTTest hTest;
  hTest.Execute(argc,argv);

  while(1){sleep(1);}
  return 1;
}
