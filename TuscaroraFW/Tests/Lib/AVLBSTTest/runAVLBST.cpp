////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#include "runAVLBST.h"
#include <Sys/Run.h>

#include <string>

#define NUMITEMS 50

// extern bool DBG_MAIN; // set in Lib/Misc/Debug.cpp
// extern bool DBG_TEST; // set in Lib/Misc/Debug.cpp


AVLBSTTest::AVLBSTTest() {
}

void AVLBSTTest:: Execute(int argc, char *argv[]){
  //Sequential test
	Debug_Printf(DBG_TEST,"\n\n************Sequential test************* \n");
 Level_0A();

 //Generate random numbers
//	Debug_Printf(DBG_TEST,"\n\n************Duplicate number insertion test************* \n");
//
// Level_0B();
  
  //Generate random numbers
	Debug_Printf(DBG_TEST,"\n\n************Random Number test************* \n");

  Level_0C();

  //Multiple times initialize and destruct trees
	Debug_Printf(DBG_TEST,"\n\n************Destruction with remaining elements test************* \n");

  Level_0D();

  Debug_Printf(DBG_TEST,"\n\n************ALL TESTS COMPLETED ************* \n");

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

/*
 * Level_0A: Sequential test
 * 		1-	Insert non-random non-continuous NUMITEMS elements one by one
 * 		2- 	Insert some number multiple times
 * 		3- 	Traverse the tree from front to end
 * 		4- 	Traverse the tree from end to front
 * 		5-	Draw random elements from [0,NUMITEMS] and delete them
 */
void AVLBSTTest::Level_0A(){
int l1=5;
int l2=NUMITEMS/l1;


rnd = new UniformRandomInt(0,NUMITEMS);

Debug_Printf(DBG_TEST,"\n***TESTING INITIALIZATION****\n");fflush(stdout);
shortAVLBST = new AVLBST_T<uint16_t >() ;
if(shortAVLBST->Size() != 0){
	 Debug_Printf(DBG_TEST,"TEST FAIL: Initial size not 0\n");fflush(stdout);
}
uint32_t cur_num_items = 0;


 Debug_Printf(DBG_TEST,"\n***TESTING: Inserting deterministic but non-continuous elements *** \n");fflush(stdout);
 Debug_Printf(DBG_TEST,"Inserting uint16_t ... \n");fflush(stdout);
  for(uint16_t i = l1; i >= 1; i--)
  {
    for(uint16_t j = (i * l2) - l2+1 ; j <= i * l2; j++)
    {
     Debug_Printf(DBG_TEST,"Inserting %d NumElemnents = %d \n", j, shortAVLBST->Size());fflush(stdout);
      if(! shortAVLBST->Insert(j) ) {
     	 Debug_Printf(DBG_TEST,"TEST FAIL: Insertion FAILED \n");fflush(stdout);
      }
      else if(shortAVLBST->Size() != ++cur_num_items){
     	 Debug_Printf(DBG_TEST,"TEST FAIL : NUM ITEMS NOT INCREMENTED  FAILED \n");fflush(stdout);
      }

      DebugNoTimeNoMod_Printf(DBG_TEST, "\n runAVLBST ********BEGIN Tree*********NumElemnents = %d \n", shortAVLBST->Size());fflush(stdout);
      //rintElemnentsofTreeDepthFirst(shortAVLBST->GetRootPtr());
      PrintElemnentsofTreeStructured(shortAVLBST->GetRootPtr(),0);
      //shortAVLBST->PrintTreeStructured();
      DebugNoTimeNoMod_Printf(DBG_TEST, "\n runAVLBST --------END  Tree---------\n");fflush(stdout);
    }
  }


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


  Debug_Printf(DBG_TEST,"\n***TESTING: Random deletion *** \n");fflush(stdout);
  uint16_t value;
  while(shortAVLBST->Size() > 0){
	  value = rnd->GetNext();
	  if(shortAVLBST->Search(value)){
		 Debug_Printf(DBG_TEST, "\n Extracting %d NumElemnents = %d ... \n", value, shortAVLBST->Size());fflush(stdout);
		  if(!shortAVLBST->Delete(value)) {
			 Debug_Printf(DBG_TEST, "\n TEST FAIL: Delete failed for a found element =  %d NumElemnents = %d ... \n", value, shortAVLBST->Size());fflush(stdout);
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

  Debug_Printf(DBG_TEST,"\n***TESTING: Tree destruction *** \n");fflush(stdout);
  delete(shortAVLBST);
}


/*
 * Level_0B: Duplicate number insertion test
 * 		1-	Insert non-random non-continuous NUMITEMS elements one by one
 * 		2- 	Insert some number multiple times
 * 		3- 	Traverse the tree from front to end
 * 		4- 	Traverse the tree from end to front
 * 		5-	Draw random elements from [0,NUMITEMS] and delete them
 */
/* BK: Cancelling test since we allow duplicate numbers as long as comparator works
void AVLBSTTest::Level_0B(){
uint16_t test_element = 42;
uint32_t cur_num_items;
Debug_Printf(DBG_TEST,"\n***TESTING INITIALIZATION****\n");fflush(stdout);
shortAVLBST = new AVLBST_T<uint16_t >() ;
if(shortAVLBST->Size() != 0){
	 Debug_Printf(DBG_TEST,"TEST FAIL: Initial size not 0\n");fflush(stdout);
}

 Debug_Printf(DBG_TEST,"\n***TESTING Element insertion ****\n");fflush(stdout);
 Debug_Printf(DBG_TEST,"Inserting %d NumElemnents = %d \n", test_element, shortAVLBST->Size());fflush(stdout);
 cur_num_items = shortAVLBST->Size();
 if(! shortAVLBST->Insert(test_element) ) {
	 Debug_Printf(DBG_TEST,"TEST FAIL: Insertion FAILED \n");fflush(stdout);
 }
 else if(shortAVLBST->Size() != ++cur_num_items){
	 Debug_Printf(DBG_TEST,"TEST FAIL : NUM ITEMS NOT INCREMENTED  FAILED \n");fflush(stdout);
 }

  DebugNoTimeNoMod_Printf(DBG_TEST, "\n runAVLBST ********BEGIN Tree*********NumElemnents = %d \n", shortAVLBST->Size());fflush(stdout);
  //rintElemnentsofTreeDepthFirst(shortAVLBST->GetRootPtr());
  PrintElemnentsofTreeStructured(shortAVLBST->GetRootPtr(),0);
  //shortAVLBST->PrintTreeStructured();
  DebugNoTimeNoMod_Printf(DBG_TEST, "\n runAVLBST --------END  Tree---------\n");fflush(stdout);


  cur_num_items = shortAVLBST->Size();
  Debug_Printf(DBG_TEST,"\n***TESTING  Duplicate Element insertion ****\n");fflush(stdout);
  Debug_Printf(DBG_TEST,"Inserting %d NumElemnents = %d \n", test_element, shortAVLBST->Size());fflush(stdout);
  if( shortAVLBST->Insert(test_element) ) {
 	 Debug_Printf(DBG_TEST,"TEST FAIL: Insertion succeeded on duplicate element \n");fflush(stdout);
  }
  else if(shortAVLBST->Size() != cur_num_items){
 	 Debug_Printf(DBG_TEST,"TEST FAIL : size incremented after inserting duplicate element \n");fflush(stdout);
  }


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


  Debug_Printf(DBG_TEST,"\n***TESTING: Tree destruction *** \n");fflush(stdout);
  delete(shortAVLBST);
}
 */


void AVLBSTTest::Level_0C(){
  int l1=0;
  rnd = new UniformRandomInt(0,NUMITEMS);


  Debug_Printf(DBG_TEST,"\n***TESTING INITIALIZATION****\n");fflush(stdout);
  shortAVLBST = new AVLBST_T<uint16_t >() ;
  if(shortAVLBST->Size() != 0){
  	 Debug_Printf(DBG_TEST,"TEST FAIL: Initial size not 0\n");fflush(stdout);
  }


  Debug_Printf(DBG_TEST,"\n***TESTING RANDOM INSERTION ****\n");fflush(stdout);
  uint16_t value;
  for(uint16_t i = NUMITEMS; i >= 1; i--)
  {
    value = rnd->GetNext();
   Debug_Printf(DBG_TEST,"Inserting %d \n", value);fflush(stdout);
    //MyData *value=new MyData(key);  
    if(shortAVLBST->Insert(value)){
    	++l1;
    }
    DebugNoTimeNoMod_Printf(DBG_TEST, "\n runAVLBST ********BEGIN Tree*********\n");
    PrintElemnentsofTreeStructured(shortAVLBST->GetRootPtr(),0);
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

  if(shortAVLBST->Size() != 0){
	  Debug_Printf(DBG_TEST,"TEST FAIL: Remianing elements after test \n");fflush(stdout);
  }
  delete(shortAVLBST);
}


void AVLBSTTest::Level_0D(){

	int l1=5;
	int l2=NUMITEMS/l1;
	uint16_t cur_num_items;

	for(uint16_t a = 0; a < NUMITEMS; ++a){
	  Debug_Printf(DBG_TEST,"\n***TESTING INITIALIZATION****\n");fflush(stdout);
	  shortAVLBST = new AVLBST_T<uint16_t >() ;
	  if(shortAVLBST->Size() != 0){
	  	 Debug_Printf(DBG_TEST,"TEST FAIL: Initial size not 0\n");fflush(stdout);
	  }
	  cur_num_items = 0;

	  Debug_Printf(DBG_TEST,"\n***TESTING: Inserting deterministic but non-continuous elements *** \n");fflush(stdout);
	  Debug_Printf(DBG_TEST,"Inserting uint16_t ... \n");fflush(stdout);
	   for(uint16_t i = l1; i >= 1; i--)
	   {
	     for(uint16_t j = (i * l2) - l2+1 ; j <= i * l2; j++)
	     {
	      Debug_Printf(DBG_TEST,"Inserting %d NumElemnents = %d \n", j, shortAVLBST->Size());fflush(stdout);
	       if(! shortAVLBST->Insert(j) ) {
	      	 Debug_Printf(DBG_TEST,"TEST FAIL: Insertion FAILED \n");fflush(stdout);
	       }
	       else if(shortAVLBST->Size() != ++cur_num_items){
	      	 Debug_Printf(DBG_TEST,"TEST FAIL : NUM ITEMS NOT INCREMENTED  FAILED \n");fflush(stdout);
	       }

	       DebugNoTimeNoMod_Printf(DBG_TEST, "\n runAVLBST ********BEGIN Tree*********NumElemnents = %d \n", shortAVLBST->Size());fflush(stdout);
	       //rintElemnentsofTreeDepthFirst(shortAVLBST->GetRootPtr());
	       PrintElemnentsofTreeStructured(shortAVLBST->GetRootPtr(),0);
	       //shortAVLBST->PrintTreeStructured();
	       DebugNoTimeNoMod_Printf(DBG_TEST, "\n runAVLBST --------END  Tree---------\n");fflush(stdout);
	     }
	   }
	   Debug_Printf(DBG_TEST,"\n***TESTING: Tree destruction *** \n");fflush(stdout);
	   delete(shortAVLBST);
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
