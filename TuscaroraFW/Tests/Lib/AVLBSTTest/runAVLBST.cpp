////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#include "runAVLBST.h"
#include <Sys/Run.h>

#include <string>

#define NUMITEMS 1000

// extern bool DBG_MAIN; // set in Lib/Misc/Debug.cpp
// extern bool DBG_TEST; // set in Lib/Misc/Debug.cpp


AVLBSTTest::AVLBSTTest() {
//   DBG_MAIN=true; // set in Lib/Misc/Debug.cpp
//   DBG_TEST=true; // set in Lib/Misc/Debug.cpp
  shortAVLBST = new AVLBST_T<uint16_t >() ;
  rnd = new UniformRandomInt(35000,45000);
}

void AVLBSTTest:: Execute(int argc, char *argv[]){
  //Sequential test
	Debug_Printf(DBG_TEST,"************Sequential test************* \n");
 Level_0A();
  
  //Generate random numbers
	Debug_Printf(DBG_TEST,"************Random Number test************* \n");

  Level_0B();
}

void AVLBSTTest::PrintElemnentsofTreeStructured(AVLBSTElement<uint16_t> *curElement, AVLBSTHeight_t offsetheight=0 ){
	if(curElement){
		if(curElement->left) {
			PrintElemnentsofTreeStructured(curElement->left,curElement->DepthL() < curElement->DepthR() ? offsetheight + (curElement->DepthR() - curElement->DepthL())  : offsetheight);
			//DebugNoTimeNoMod_Printf(DBG_TEST,"            ");
		}
		else {
		//	DebugNoTimeNoMod_Printf(DBG_TEST,"\n");
		}
		for(int i=0; i < ((curElement->CalculateDepth()-1)+ offsetheight); i++){
			DebugNoTimeNoMod_Printf(DBG_TEST,"            ");
		}
		if (curElement->parent) {
			DebugNoTimeNoMod_Printf(DBG_TEST,"%d(%d_%d_%d)",curElement->data, curElement->parent->data,curElement->DepthL(),curElement->DepthR());
		}
		else{
			DebugNoTimeNoMod_Printf(DBG_TEST,"%d(--_%d_%d)",curElement->data,curElement->DepthL(),curElement->DepthR());
		}
		DebugNoTimeNoMod_Printf(DBG_TEST,"\n");
		if(curElement->right) {
			PrintElemnentsofTreeStructured(curElement->right,curElement->DepthR() < curElement->DepthL() ? offsetheight + (curElement->DepthL() - curElement->DepthR())  : offsetheight);
		}
		else{
			//DebugNoTimeNoMod_Printf(DBG_TEST,"\n");
		}
	}
}


void AVLBSTTest::PrintElemnentsofTreeDepthFirst(AVLBSTElement<uint16_t> *curElement ){
	Debug_Printf(DBG_TEST,"CurElement %d \n", curElement->data);
	if(curElement->left) {
		PrintElemnentsofTreeDepthFirst(curElement->left);
	}else{
		Debug_Printf(DBG_TEST,"%d No Left\n", curElement->data);
	}
	if(curElement->right) {
		PrintElemnentsofTreeDepthFirst(curElement->right);
	}else{
		Debug_Printf(DBG_TEST,"%d No Right\n", curElement->data);
	}
	if(curElement->parent) {
		Debug_Printf(DBG_TEST,"%d 's parent %d \n", curElement->data, curElement->parent->data);
	}else{
		Debug_Printf(DBG_TEST,"%d has no Parent\n", curElement->data);
	}
	Debug_Printf(DBG_TEST,"%d DepthL() = %d DepthR() = %d \n",curElement->data, curElement->DepthL(), curElement->DepthR());
	fflush(stdout);
}


void AVLBSTTest::Level_0A(){
int l1=5;
int l2=NUMITEMS/l1;

rnd = new UniformRandomInt(0,NUMITEMS);

 Debug_Printf(DBG_TEST,"Inserting uint16_t ... \n");fflush(stdout);
  for(uint16_t i = l1; i >= 1; i--)
  {
    for(uint16_t j = (i * l2) - l2+1 ; j <= i * l2; j++)
    {
     Debug_Printf(DBG_TEST,"Inserting %d NumElemnents = %d \n", j, shortAVLBST->Size());fflush(stdout);
      //MyData *x=new MyData(j);
      shortAVLBST->Insert(j);
     //Debug_Printf(DBG_TEST,"Done. \n");fflush(stdout);
      //Debug_Printf(DBG_TEST, "top number is %d \n", shortAVLBST->PeekRoot());

      DebugNoTimeNoMod_Printf(DBG_TEST, "\n runAVLBST ********BEGIN Tree*********NumElemnents = %d \n", shortAVLBST->Size());fflush(stdout);
      //rintElemnentsofTreeDepthFirst(shortAVLBST->GetRootPtr());
      PrintElemnentsofTreeStructured(shortAVLBST->GetRootPtr(),0);
      //shortAVLBST->PrintTreeStructured();
      DebugNoTimeNoMod_Printf(DBG_TEST, "\n runAVLBST --------END  Tree---------\n");fflush(stdout);
    }
  }
 Debug_Printf(DBG_TEST,"Inserting %d NumElemnents = %d \n", 42, shortAVLBST->Size());fflush(stdout);
  shortAVLBST->Insert(42);
 Debug_Printf(DBG_TEST,"Inserting %d NumElemnents = %d \n", 42, shortAVLBST->Size());fflush(stdout);
  shortAVLBST->Insert(42);
 Debug_Printf(DBG_TEST,"Inserting %d NumElemnents = %d \n", 42, shortAVLBST->Size());fflush(stdout);
  shortAVLBST->Insert(42);
  DebugNoTimeNoMod_Printf(DBG_TEST, "\n runAVLBST ********BEGIN Tree*********NumElemnents = %d \n", shortAVLBST->Size());fflush(stdout);
  //rintElemnentsofTreeDepthFirst(shortAVLBST->GetRootPtr());
  PrintElemnentsofTreeStructured(shortAVLBST->GetRootPtr(),0);
  //shortAVLBST->PrintTreeStructured();
  DebugNoTimeNoMod_Printf(DBG_TEST, "\n runAVLBST --------END  Tree---------\n");fflush(stdout);

  DebugNoTimeNoMod_Printf(DBG_TEST, "Traversing from begin to end\n ")
  for(AVLBSTElement<uint16_t>* i = shortAVLBST->Begin(); i != NULL; i = shortAVLBST->Next(i) ){
		DebugNoTimeNoMod_Printf(DBG_TEST, "CurElement = %d ", i->data);fflush(stdout);
  }
  DebugNoTimeNoMod_Printf(DBG_TEST, "\n ")

  DebugNoTimeNoMod_Printf(DBG_TEST, "Traversing from end to begin\n ")
  for(AVLBSTElement<uint16_t>* i = shortAVLBST->End(); i != NULL; i = shortAVLBST->Previous(i) ){
		DebugNoTimeNoMod_Printf(DBG_TEST, "CurElement = %d ", i->data);fflush(stdout);
  }
  DebugNoTimeNoMod_Printf(DBG_TEST, "\n ")


  uint16_t value;
  while(shortAVLBST->Size() > 0){
	  value = rnd->GetNext();
	  if(shortAVLBST->Search(value)){
		 Debug_Printf(DBG_TEST, "\n Extracting %d NumElemnents = %d ... \n", value, shortAVLBST->Size());fflush(stdout);
		  if(!shortAVLBST->Delete(value)) {
			 Debug_Printf(DBG_TEST, "\n Delete failed for %d NumElemnents = %d ... \n", value, shortAVLBST->Size());fflush(stdout);
		      DebugNoTimeNoMod_Printf(DBG_TEST, "\n runAVLBST ********BEGIN Tree*********\n");
		      //rintElemnentsofTreeDepthFirst(shortAVLBST->GetRootPtr());
		      PrintElemnentsofTreeStructured(shortAVLBST->GetRootPtr(),0);
		      //shortAVLBST->PrintTreeStructured();
		      DebugNoTimeNoMod_Printf(DBG_TEST, "\n runAVLBST --------END  Tree---------\n");
		  }
		  else{
			 Debug_Printf(DBG_TEST, "\n Delete successful for %d NumElemnents = %d ... \n", value, shortAVLBST->Size());fflush(stdout);
		      DebugNoTimeNoMod_Printf(DBG_TEST, "\n runAVLBST ********BEGIN Tree*********\n");
		      //rintElemnentsofTreeDepthFirst(shortAVLBST->GetRootPtr());
		      PrintElemnentsofTreeStructured(shortAVLBST->GetRootPtr(),0);
		      //shortAVLBST->PrintTreeStructured();
		      DebugNoTimeNoMod_Printf(DBG_TEST, "\n runAVLBST --------END  Tree---------\n");
		  }
	  }
  }
}


void AVLBSTTest::Level_0B(){
  int l1=NUMITEMS; 
  
  rnd = new UniformRandomInt(750,1250);
  uint16_t value;
  
  for(uint16_t i = l1; i >= 1; i--)
  {
    value = rnd->GetNext();
   Debug_Printf(DBG_TEST,"Inserting %d \n", value);fflush(stdout);
    //MyData *value=new MyData(key);  
    shortAVLBST->Insert(value);
    DebugNoTimeNoMod_Printf(DBG_TEST, "\n runAVLBST ********BEGIN Tree*********\n");
    //rintElemnentsofTreeDepthFirst(shortAVLBST->GetRootPtr());
    PrintElemnentsofTreeStructured(shortAVLBST->GetRootPtr(),0);
    //shortAVLBST->PrintTreeStructured();
    DebugNoTimeNoMod_Printf(DBG_TEST, "\n runAVLBST --------END  Tree---------\n");
   Debug_Printf(DBG_TEST, "top number is %d \n", shortAVLBST->PeekRoot());
  }
 // IterateElements(shortAVLBST);
 Debug_Printf(DBG_TEST, "\n Extracting ... \n");fflush(stdout);
  uint16_t newMax =0;
  for(uint16_t i = l1; i >= 1; i--)
  {
    newMax = shortAVLBST->ExtractRoot();
   Debug_Printf(DBG_TEST, "Extracted num is %d top number is %d, elements left in heap %d \n", newMax, shortAVLBST->PeekRoot(),  shortAVLBST->Size());
  }
}


//int testBST(int argc, char* argv[]){
int main(int argc, char* argv[]){
  //Make sure RuntimeOpts construction is the very first line of main
  RuntimeOpts opts ( argc-1, argv+1 );
  InitPlatform(&opts);    //This does a platform specific initialization

  AVLBSTTest hTest;
  hTest.Execute(argc,argv);

  RunTimedMasterLoop();
  return 1;
}
