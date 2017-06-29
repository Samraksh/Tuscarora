#include "runFAMap.h"

#include <sstream>

#define NUMITEMS 20

// extern bool DBG_MAIN; // set in Lib/Misc/Debug.cpp
// extern bool DBG_TEST; // set in Lib/Misc/Debug.cpp

using namespace std;


FAMapTest::FAMapTest() {

}

void FAMapTest:: Execute(int argc, char *argv[]){
	//Sequential test
	Level_0A();
//
	Level_ADDBEYONDSIZE();
//
	Level_0C();
	//Generate random numbers
	//Level_0B();
}

void FAMapTest::Level_ADDBEYONDSIZE(){
	int l1=4;
	int l2=NUMITEMS/l1;
	stringstream ss;


	Debug_Printf(DBG_TEST,"\n***TESTING INITIALIZATION****\n");fflush(stdout);
	auto map_ptr = new FABSTMapT<NUMITEMS, NodeId_t, string> ();
	if(map_ptr->Size() != 0){
		 Debug_Printf(DBG_TEST,"TEST FAIL: Initial size not 0\n");fflush(stdout);
	}
	uint32_t cur_num_items = map_ptr->Size();


	Debug_Printf(DBG_TEST,"inserting uint16_t... \n");fflush(stdout);
	for(uint16_t i = l1; i >= 1; i--) {
		for(uint16_t j = (i * l2) - l2+1 ; j <= i * l2; j++) {
			ss << j;
			string s= "string of " +  ss.str();
			Debug_Printf(DBG_TEST,"inserting %d, %s \n", j, s.c_str());fflush(stdout);
			(*map_ptr)[j]=s;

			if(++cur_num_items != map_ptr->Size()){
				Debug_Printf(DBG_TEST,"TEST FAIL : Size did not increase after inserting %d, %s \n", j, s.c_str());fflush(stdout);

			}

			//Debug_Printf(DBG_TEST,"Done. \n");fflush(stdout);
			string s1 = (*map_ptr)[j];
			Debug_Printf(DBG_TEST, "string read back is %s \n", s1.c_str());
		}
	}

    Debug_Printf(DBG_TEST,"\n***TESTING: Inserting an element beyon the capacity *** \n");fflush(stdout);
    uint16_t j = NUMITEMS+1;
    ss << j;
	string s= "string of " +  ss.str();
	Debug_Printf(DBG_TEST,"inserting %d, %s \n", j, s.c_str());fflush(stdout);
    if(map_ptr->Insert(j, s) ) {
        	 Debug_Printf(DBG_TEST,"TEST FAIL: Insertion beyond capacity \n");fflush(stdout);
    }
  	else if(map_ptr->Size() != NUMITEMS){
  		Debug_Printf(DBG_TEST,"TEST FAIL : NUMITEMS beyond capacity \n");fflush(stdout);
  	}


	delete(map_ptr);

}

void FAMapTest::PrintElemnentsofTreeStructured(FAAVLBSTElement<FAMapElement<NodeId_t, string>> *curElement, AVLBSTHeight_t offsetheight ){
	if(curElement){
		if(curElement->left) {
			PrintElemnentsofTreeStructured((FAAVLBSTElement<FAMapElement<NodeId_t, string>>*) curElement->left,curElement->DepthL() < curElement->DepthR() ? offsetheight + (curElement->DepthR() - curElement->DepthL())  : offsetheight);
			//DebugNoTimeNoMod_Printf(DBG_TEST,"            ");
		}
		else {
		//	DebugNoTimeNoMod_Printf(DBG_TEST,"\n");
		}
		for(int i=0; i < ((curElement->CalculateDepth()-1)+ offsetheight); i++){
			DebugNoTimeNoMod_Printf(DBG_TEST,"            ");
		}
		if (curElement->parent) {
			DebugNoTimeNoMod_Printf(DBG_TEST,"%d(%d_%d_%d_%d)",curElement->data.GetKey(), curElement->parent->data.GetKey(),curElement->DepthL(),curElement->DepthR(),curElement->depth);
		}
		else{
			DebugNoTimeNoMod_Printf(DBG_TEST,"%d(--_%d_%d)",curElement->data.GetKey(),curElement->DepthL(),curElement->DepthR());
		}
		DebugNoTimeNoMod_Printf(DBG_TEST,"\n");
		if(curElement->right) {
			PrintElemnentsofTreeStructured((FAAVLBSTElement<FAMapElement<NodeId_t, string>>*)curElement->right,curElement->DepthR() < curElement->DepthL() ? offsetheight + (curElement->DepthL() - curElement->DepthR())  : offsetheight);
		}
		else{
			//DebugNoTimeNoMod_Printf(DBG_TEST,"\n");
		}
	}
}

void FAMapTest::Level_0A(){
	int l1=4;
	int l2=NUMITEMS/l1;
	stringstream ss;


	Debug_Printf(DBG_TEST,"\n***TESTING INITIALIZATION****\n");fflush(stdout);
	auto map_ptr = new FABSTMapT<NUMITEMS, NodeId_t, string> ();
	if(map_ptr->Size() != 0){
		 Debug_Printf(DBG_TEST,"TEST FAIL: Initial size not 0\n");fflush(stdout);
	}
	uint32_t cur_num_items = map_ptr->Size();


	Debug_Printf(DBG_TEST,"inserting uint16_t... \n");fflush(stdout);
	for(uint16_t i = l1; i >= 1; i--) {
		for(uint16_t j = (i * l2) - l2+1 ; j <= i * l2; j++) {
			ss << j;
			string s= "string of " +  ss.str();
			Debug_Printf(DBG_TEST,"inserting %d, %s \n", j, s.c_str());fflush(stdout);
			(*map_ptr)[j]=s;

			if(++cur_num_items != map_ptr->Size()){
				Debug_Printf(DBG_TEST,"TEST FAIL : NUM ITEMS NOT INCREMENTED  FAILED \n");fflush(stdout);
			}

			//Debug_Printf(DBG_TEST,"Done. \n");fflush(stdout);
			string s1 = (*map_ptr)[j];
			Debug_Printf(DBG_TEST, "string read back is %s \n", s1.c_str());
		}
	}

	cur_num_items = map_ptr->Size();

	Debug_Printf(DBG_TEST, "\n Reading back ... \n");fflush(stdout);
	for(uint16_t i = l1*l2; i >=1 ; i--) {

		Debug_Printf(DBG_TEST, "Looking for item %d....\n", i);
		string s = (*map_ptr)[i];
		Debug_Printf(DBG_TEST, "Item at location %d  is %s\n", i, s.c_str()); fflush(stdout);
	}

	Debug_Printf(DBG_TEST, "\n Printing Structured ... \n");fflush(stdout);

	PrintElemnentsofTreeStructured((FAAVLBSTElement<FAMapElement<NodeId_t, string>>*)map_ptr->myBST.GetRootPtr(),0);

	//Debug_Printf(DBG_TEST, "\n Iterator Test back ... \n");fflush(stdout);
	//	FABSTMapT<NodeId_t, string>::Iterator it = map_ptr->Begin();
	//	Debug_Printf(DBG_TEST, "it->First()=%d it->Second()=%s....\n", it->First(), (it->Second()).c_str());fflush(stdout);
	//	it = it+1;
	//	Debug_Printf(DBG_TEST, "it->First()=%d it->Second()=%s....\n", it->First(), (it->Second()).c_str());fflush(stdout);

    Debug_Printf(DBG_TEST, "\n Iterator Test ... \n");fflush(stdout);
	for( auto i = map_ptr->Begin(); i!=map_ptr->End() ; ++i) {
		Debug_Printf(DBG_TEST, "i->First()=%d i->Second()=%s....\n", i->First(), (i->Second()).c_str());fflush(stdout);
	}
	Debug_Printf(DBG_TEST, "\n END Iterator Test  ... \n");fflush(stdout);

	Debug_Printf(DBG_TEST, "\n Iterator Test non linear reading ... \n");fflush(stdout);
	for( auto i = map_ptr->Begin(); i!=map_ptr->End() ; ++i) {

		Debug_Printf(DBG_TEST, "i->First()=%d i->Second()=%s....\n", i->First(), (i->Second()).c_str());fflush(stdout);
		i = i+2;
	}
	Debug_Printf(DBG_TEST, "\n END Iterator Test  ... \n");fflush(stdout);


	Debug_Printf(DBG_TEST, "\n Printing Structured ... \n");fflush(stdout);

	PrintElemnentsofTreeStructured((FAAVLBSTElement<FAMapElement<NodeId_t, string>>*)map_ptr->myBST.GetRootPtr(),0);
	Debug_Printf(DBG_TEST, "\n Iterator Test back and delete... \n");fflush(stdout);
	for( auto i = map_ptr->RBegin(); i!=map_ptr->REnd() ; ++i) {
		++i;
		Debug_Printf(DBG_TEST, "Erasing i->First()=%d i->Second()=%s....\n", i->First(), (i->Second()).c_str());fflush(stdout);
		map_ptr->Erase(i);
		if( i != map_ptr->REnd() ) {
			Debug_Printf(DBG_TEST, "I AFTER ERASE i->First()=%d i->Second()=%s....\n", i->First(), (i->Second()).c_str());fflush(stdout);
		}
		else {
			Debug_Printf(DBG_TEST, "I after erase is REnd \n");fflush(stdout);
		}

		Debug_Printf(DBG_TEST, "+++++MAP CONTENTS SIZE=%d \n", map_ptr->Size()); fflush(stdout);
				for( auto i2 = map_ptr->RBegin(); i2!=map_ptr->End() ; ++i2) {
					Debug_Printf(DBG_TEST, "i2->First()=%d i->Second()=%s....\n", i2->First(), (i2->Second()).c_str());fflush(stdout);
				}
		Debug_Printf(DBG_TEST, "-----END OF MAP CONTENTS SIZE=%d \n", map_ptr->Size()); fflush(stdout);
	}
	Debug_Printf(DBG_TEST, "\n Iterator Test back ... \n");fflush(stdout);


	Debug_Printf(DBG_TEST, "\n Deleting remaining elements... \n");fflush(stdout);
	Debug_Printf(DBG_TEST, "Elements left in FAMap: %d \n", map_ptr->Size());
	for(uint16_t i = l1*l2; i >= 1; i--) {
		PrintElemnentsofTreeStructured((FAAVLBSTElement<FAMapElement<NodeId_t, string>>*)map_ptr->myBST.GetRootPtr(),0);
		for( auto i = map_ptr->Begin(); i!=map_ptr->End() ; ++i) {
			Debug_Printf(DBG_TEST, "i->First()=%d i->Second()=%s....\n", i->First(), (i->Second()).c_str());fflush(stdout);
		}
		string s = (*map_ptr)[i];
		map_ptr->Erase(i);
		PrintElemnentsofTreeStructured((FAAVLBSTElement<FAMapElement<NodeId_t, string>>*)map_ptr->myBST.GetRootPtr(),0);
		for( auto i = map_ptr->Begin(); i!=map_ptr->End() ; ++i) {
			Debug_Printf(DBG_TEST, "i->First()=%d i->Second()=%s....\n", i->First(), (i->Second()).c_str());fflush(stdout);
		}
		Debug_Printf(DBG_TEST, "Erased item %d, which was '%s'. Elements left in FAMap: %d \n",i, s.c_str(), map_ptr->Size()); fflush(stdout);
	}
	Debug_Printf(DBG_TEST, "END OF Level_0A \n"); fflush(stdout);

	delete(map_ptr);
}

void FAMapTest::Level_0C(){
	int l1=10;
	int l2=NUMITEMS/l1;
	stringstream ss;

	Debug_Printf(DBG_TEST,"\n***TESTING INITIALIZATION****\n");fflush(stdout);
	auto map_ptr = new FABSTMapT<NUMITEMS, NodeId_t, string> ();
	if(map_ptr->Size() != 0){
		 Debug_Printf(DBG_TEST,"TEST FAIL: Initial size not 0\n");fflush(stdout);
	}
	uint32_t cur_num_items = map_ptr->Size();



	Debug_Printf(DBG_TEST,"inserting uint16_t... \n");fflush(stdout);
	for(uint16_t i = l1; i >= 1; i--) {
		for(uint16_t j = (i * l2) - l2+1 ; j <= i * l2; j++) {
			ss << j;
			string s= "string of " +  ss.str();
			Debug_Printf(DBG_TEST,"inserting %d, %s \n", j, s.c_str());fflush(stdout);
			(*map_ptr)[j]=s;
			if(++cur_num_items != map_ptr->Size()){
				Debug_Printf(DBG_TEST,"TEST FAIL : NUM ITEMS NOT INCREMENTED  FAILED \n");fflush(stdout);
			}
			//Debug_Printf(DBG_TEST,"Done. \n");fflush(stdout);
			string s1 = (*map_ptr)[j];
			Debug_Printf(DBG_TEST, "string read back is %s \n", s1.c_str());
		}
	}

	rnd = new UniformRandomInt(0, (l1*l2)+1);
	int value;
	auto it = map_ptr->Begin();
	int k=20;
	Debug_Printf(DBG_TEST, "\n Deleting ... \n");fflush(stdout);
	while(map_ptr->Size() > 0){
		--k;
		value = rnd->GetNext();
		it = map_ptr->Find(value);
		if (it == map_ptr->End())  {
			Debug_Printf(DBG_TEST, "Elements not found: %d \n", value);
		}
		else {
			Debug_Printf(DBG_TEST, "Element found: %d \n ", value);
			int curkey = it->First();
			string curstr = it->Second();
			if(map_ptr->Erase(it)){

				Debug_Printf(DBG_TEST, "Erased item %d, which was '%s'. NumElements left in FAMap: %d \n",curkey, curstr.c_str(), map_ptr->Size()); fflush(stdout);
			}
			else{
				Debug_Printf(DBG_TEST, "could not Erase item %d, which was '%s'. NumElements left in FAMap: %d \n",curkey, curstr.c_str(), map_ptr->Size()); fflush(stdout);
			}

			Debug_Printf(DBG_TEST, "+++++MAP CONTENTS SIZE=%d \n", map_ptr->Size()); fflush(stdout);
                        
			for( auto i = map_ptr->Begin(); i!=map_ptr->End() ; ++i) {
				Debug_Printf(DBG_TEST, "i->First()=%d i->Second()=%s....\n", i->First(), (i->Second()).c_str());fflush(stdout);
			}
			Debug_Printf(DBG_TEST, "-----END OF MAP CONTENTS SIZE=%d \n", map_ptr->Size()); fflush(stdout);
		}
	}
	Debug_Printf(DBG_TEST, "END OF Level_0C \n"); fflush(stdout);
	delete(map_ptr);
}

void FAMapTest::Level_0B(){
	/*int l1=100;

  rnd = new UniformRandomInt(750,1250);

  for(uint16_t i = l1; i >= 1; i--)
  {
    uint16_t value = rnd->GetNext();
   Debug_Printf(DBG_TEST,"inserting %d \n", value);fflush(stdout);

    shortFAMap->Insert(value);
     //Debug_Printf(DBG_TEST,"Done. \n");fflush(stdout);
    //Debug_Printf(DBG_TEST, "Min number is %d \n", shortFAMap[0]);
  }

 Debug_Printf(DBG_TEST, "\n Reading back ... \n");fflush(stdout);
  uint16_t item =0;
  for(uint16_t i = l1; i >= 1; i--)
  {

    item = shortFAMap->GetItem(shortFAMap->size() -i);
   Debug_Printf(DBG_TEST, "Item at location  %d  is %d \n", shortFAMap->size() -i, item);
  }

 Debug_Printf(DBG_TEST, "\n Deleting ... \n");fflush(stdout);
  uint16_t newMax =0;
  newMax = shortFAMap->GetItem(shortFAMap->size() -1);
 Debug_Printf(DBG_TEST, "Max num: %d , elements left in list: %d \n", newMax, shortFAMap->size());
  for(uint16_t i = l1; i >= 1; i--)
  {
    shortFAMap->Erase(shortFAMap->size() -1);
    newMax = shortFAMap->GetItem(shortFAMap->size() -1);
   Debug_Printf(DBG_TEST, "Max num: %d.  elements left in list: %d \n", newMax, shortFAMap->size());
  }
	 */


}

//int testFAMap(int argc, char* argv[]){
int main(int argc, char* argv[]){
  //Make sure RuntimeOpts construction is the very first line of main
  RuntimeOpts opts ( argc-1, argv+1 );
  InitPlatform(&opts);    //This does a platform specific initialization


  FAMapTest hTest;
  hTest.Execute(argc,argv);

  RunTimedMasterLoop();
  return 0;
}
