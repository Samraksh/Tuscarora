////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include "runBiMap.h"
#include <Sys/Run.h>

#include <assert.h>
#include "Lib/Misc/Debug.h"
#define NUMITEMS 20

// extern bool DBG_MAIN; // set in Lib/Misc/Debug.cpp
// extern bool DBG_TEST; // set in Lib/Misc/Debug.cpp

using namespace std;


BiMapTest::BiMapTest() {
	// 	DBG_MAIN=true; // set in Lib/Misc/Debug.cpp
	// 	DBG_TEST=true; // set in Lib/Misc/Debug.cpp
#ifdef BSTMAP
	// map = new BiBSTMapT<NodeId_t, string> ();
#else 
	// map = new ListMapT<NodeId_t, string>() ;
#endif

	rnd = new UniformRandomInt(35000,45000);
}

void BiMapTest:: Execute(int argc, char *argv[]){
	//Sequential test
	//Level_0A();
	//Level_0C();
    Level_0C();
	Level_0A();
	Level_0A();
	Level_0C();
	//Generate random numbers
	//Level_0B();
}

void BiMapTest::Level_0A(){ //NON sequential but deterministic write read test
	Debug_Printf(DBG_TEST, "--------- START OF Level_0A --------- \n"); fflush(stdout);
	int l2=2;
	int l1=NUMITEMS/l2;

	BiBSTMapT<NodeId_t, string>::Iterator1 it1;
	BiBSTMapT<NodeId_t, string>::Iterator2 it2;

    Debug_Printf(DBG_TEST, "\n Iterator1 Test ... \n");fflush(stdout);
    for( BiBSTMapT<NodeId_t, string>::Iterator1 i = map.Begin1(); i != map.End1() ; ++i) {
        Debug_Printf(DBG_TEST, "i->First()=%d i->Second()=%s....\n", i->First(), (i->Second()).c_str());fflush(stdout);
    }
    Debug_Printf(DBG_TEST, "\n END Iterator1 Test  ... \n");fflush(stdout);

    Debug_Printf(DBG_TEST, "\n Iterator2 Test ... \n");fflush(stdout);
    for( BiBSTMapT<NodeId_t, string>::Iterator2 i = map.Begin2(); i != map.End2() ; ++i) {
        Debug_Printf(DBG_TEST, "i->First()=%d i->Second()=%s....\n", i->First(), (i->Second()).c_str());fflush(stdout);
    }
    Debug_Printf(DBG_TEST, "\n END Iterator2 Test  ... \n");fflush(stdout);

    string s = "string of ";
	Debug_Printf(DBG_TEST,"inserting uint16_t... \n");fflush(stdout);
	for(uint16_t i = l1; i >= 1; i--) {
		for(uint16_t j = (i * l2) - l2+1 ; j <= i * l2; j++) {
			s = s + to_string(j);
			Debug_Printf(DBG_TEST,"inserting (%d, %s) \n", j, s.c_str());fflush(stdout);

			//s = j;
			map.Insert(j,s);
			Debug_Printf(DBG_TEST,"Inserting Done. \n");fflush(stdout);
			//it1 = map.Find1(j);
			//it1_2 = map.End1();
			if(map.Find1(j) == map.End1()) {
				Debug_Printf(DBG_TEST, "(%d, %s) is not found in the map using Find1 \n", j, s.c_str());
			}
			else {
			    string s1 = map.Find1(j)->Second();
			    Debug_Printf(DBG_TEST, "(%d, %s) is found in the map using Find1 and read back as (%d,%s)\n", j, s.c_str(), map.Find1(j)->First(), s1.c_str());
			}
			assert(j == map.Find1(j)->First());
			assert(s == map.Find1(j)->Second());
			it2 = map.Find2(s);
			if(map.Find2(s) == map.End2()) {
				Debug_Printf(DBG_TEST, "(%d, %s) is not found in the map using Find2 \n", j, s.c_str());
			}
			else {
			    string s1 = map.Find2(s)->Second();
			    Debug_Printf(DBG_TEST, "(%d, %s) is found in the map using Find2 and read back as (%d,%s)\n", j, s.c_str(), map.Find2(s)->First(), s1.c_str());
			}
			assert(j == map.Find2(s)->First());
			assert(s == map.Find2(s)->Second());
		}
	}
	Debug_Printf(DBG_TEST, "\n INSERTING LOOP DONE ... \n");fflush(stdout);



	Debug_Printf(DBG_TEST, "\n Iterator1 Test ... \n");fflush(stdout);
	for( BiBSTMapT<NodeId_t, string>::Iterator1 i = map.Begin1(); i != map.End1() ; ++i) {
		Debug_Printf(DBG_TEST, "i->First()=%d i->Second()=%s....\n", i->First(), (i->Second()).c_str());fflush(stdout);
	}
	Debug_Printf(DBG_TEST, "\n END Iterator1 Test  ... \n");fflush(stdout);



	Debug_Printf(DBG_TEST, "\n Iterator2 Test ... \n");fflush(stdout);
	for( BiBSTMapT<NodeId_t, string>::Iterator2 i = map.Begin2(); i != map.End2() ; ++i) {
		Debug_Printf(DBG_TEST, "i->First()=%d i->Second()=%s....\n", i->First(), (i->Second()).c_str());fflush(stdout);
	}
	Debug_Printf(DBG_TEST, "\n END Iterator2 Test  ... \n");fflush(stdout);


	Debug_Printf(DBG_TEST, "\n Attempting to insert (%d,%s) \n", 16, map.Find1(17)->Second().c_str());fflush(stdout);
	if(map.Insert(16, map.Find1(17)->Second())){
		Debug_Printf(DBG_TEST, " Successfully inserted (%d,%s) \n", 16, map.Find1(17)->Second().c_str());fflush(stdout);
	}
	else{
		Debug_Printf(DBG_TEST, " Failed to insert (%d,%s) \n", 16, map.Find1(17)->Second().c_str());fflush(stdout);
	}



	Debug_Printf(DBG_TEST, "\n Reading back Entire Map Using Find2 ... \n");fflush(stdout);
	for(uint16_t i = l1*l2; i >=1 ; i--) {

		Debug_Printf(DBG_TEST, "Looking for item %d....\n", i);
		string s = map.Find1(i)->Second();
		Debug_Printf(DBG_TEST, "Item at location %d  is %s\n", i, s.c_str()); fflush(stdout);
	}

	//Debug_Printf(DBG_TEST, "\n Iterator1 Test back ... \n");fflush(stdout);
	//	BiBSTMapT<NodeId_t, string>::Iterator1 it = map.Begin1();
	//	Debug_Printf(DBG_TEST, "it->First()=%d it->Second()=%s....\n", it->First(), (it->Second()).c_str());fflush(stdout);
	//	it = it+1;
	//	Debug_Printf(DBG_TEST, "it->First()=%d it->Second()=%s....\n", it->First(), (it->Second()).c_str());fflush(stdout);

	Debug_Printf(DBG_TEST, "\n Iterator1 Test ... \n");fflush(stdout);
	for( BiBSTMapT<NodeId_t, string>::Iterator1 i = map.Begin1(); i != map.End1() ; ++i) {
		Debug_Printf(DBG_TEST, "i->First()=%d i->Second()=%s....\n", i->First(), (i->Second()).c_str());fflush(stdout);
	}
	Debug_Printf(DBG_TEST, "\n END Iterator1 Test  ... \n");fflush(stdout);

	Debug_Printf(DBG_TEST, "\n Iterator1 Test Increments of 3 ... \n");fflush(stdout);
	for( BiBSTMapT<NodeId_t, string>::Iterator1 i = map.Begin1(); i!=map.End1() ; ++i) {
		Debug_Printf(DBG_TEST, "i->First()=%d i->Second()=%s....\n", i->First(), (i->Second()).c_str());fflush(stdout);
		i = i+2;
	}
	Debug_Printf(DBG_TEST, "\n END Iterator1 Test Increments of 3  ... \n");fflush(stdout);

	Debug_Printf(DBG_TEST, "\n Iterator2 Test Increments of 3... \n");fflush(stdout);
	for( BiBSTMapT<NodeId_t, string>::Iterator2 i = map.Begin2(); i != map.End2() ; ++i) {
		Debug_Printf(DBG_TEST, "i->First()=%d i->Second()=%s....\n", i->First(), (i->Second()).c_str());fflush(stdout);
		i = i+2;
	}
	Debug_Printf(DBG_TEST, "\n END Iterator2 Test Increments of 3 ... \n");fflush(stdout);

	Debug_Printf(DBG_TEST, "\n Reverse_Iterator1 Test and delete Increments of 3  ... \n");fflush(stdout);
	for( BiBSTMapT<NodeId_t, string>::Reverse_Iterator1 i = map.RBegin1(); i!=map.REnd1() ; ++i) {
		++i;
		Debug_Printf(DBG_TEST, "Erasing i->First()=%d i->Second()=%s....\n", i->First(), (i->Second()).c_str());fflush(stdout);
		map.Erase1(i);
		if( i != map.REnd1() ) {
			Debug_Printf(DBG_TEST, "I AFTER ERASE i->First()=%d i->Second()=%s....\n", i->First(), (i->Second()).c_str());fflush(stdout);
		}
		else {
			Debug_Printf(DBG_TEST, "I after erase is REnd1 \n");fflush(stdout);
		}

		Debug_Printf(DBG_TEST, "+++++MAP CONTENTS SIZE=%d \n", map.Size()); fflush(stdout);
		for( BiBSTMapT<NodeId_t, string>::Reverse_Iterator1 i2 = map.RBegin1(); i2!=map.End1() ; ++i2) {
			Debug_Printf(DBG_TEST, "i2->First()=%d i->Second()=%s....\n", i2->First(), (i2->Second()).c_str());fflush(stdout);
		}
		Debug_Printf(DBG_TEST, "-----END OF MAP CONTENTS SIZE=%d \n", map.Size()); fflush(stdout);
	}
	Debug_Printf(DBG_TEST, "\n END Reverse_Iterator1 Test and delete Increments of 3 .... \n");fflush(stdout);

	Debug_Printf(DBG_TEST, "\n Deleting ... \n");fflush(stdout);
	Debug_Printf(DBG_TEST, "Elements left in Map: %d \n", map.Size());
	for(uint16_t i = l1*l2; i >= 1; i--) {
		Debug_Printf(DBG_TEST, "Attempting to erase with KEY = %d\n", i);
		if(map.Find1(i) != map.End1()) {
			string s = map.Find1(i)->Second();
			map.Erase1(i);
			Debug_Printf(DBG_TEST, "Erased item %d, which was '%s'. Elements left in Map: %d \n",i, s.c_str(), map.Size()); fflush(stdout);
		}
		else{
			map.Erase1(i);
			Debug_Printf(DBG_TEST, "Could not find an item with KEY = %d to erase \n", i);
			map.Erase1(i);
		}
	}
	Debug_Printf(DBG_TEST, "------------- END OF Level_0A ---------------\n"); fflush(stdout);
	/**/
}

void BiMapTest::Level_0C(){ //RANDOM WRITE READ
	Debug_Printf(DBG_TEST, "--------- START OF Level_0C --------- \n"); fflush(stdout);
	int l2=2;
	int l1=NUMITEMS/l2;
	string s = "string of ";
	Debug_Printf(DBG_TEST,"inserting uint16_t... \n");fflush(stdout);
	for(uint16_t i = l1; i >= 1; i--) {
		for(uint16_t j = (i * l2) - l2+1 ; j <= i * l2; j++) {
			s = s +  to_string(j);
			Debug_Printf(DBG_TEST,"inserting %d, %s \n", j, s.c_str());fflush(stdout);
			//map[j]=s;
			map.Insert(j,s);
			//Debug_Printf(DBG_TEST,"Done. \n");fflush(stdout);
			//string s1 = map[j];
			string s1 = map.Find1(j)->Second();
			Debug_Printf(DBG_TEST, "string read back is %s \n", s1.c_str());
		}
	}

	rnd = new UniformRandomInt(0,(l1*l2)+1);
	int value;
	BiBSTMapT<NodeId_t, string>::Iterator1 it;
	int k=20;
	Debug_Printf(DBG_TEST, "\n Deleting ... \n");fflush(stdout);
	while(map.Size() > 0){
		--k;
		value = rnd->GetNext();
		it = map.Find1(value);
		if (it == map.End1())  {
			Debug_Printf(DBG_TEST, "Elements not found: %d \n", value);
		}
		else {
			Debug_Printf(DBG_TEST, "Element found: %d \n ", value);
			int curkey = it->First();
			string curstr = it->Second();
			if(map.Erase1(it)){

				Debug_Printf(DBG_TEST, "Erased item %d, which was '%s'. NumElements left in Map: %d \n",curkey, curstr.c_str(), map.Size()); fflush(stdout);
			}
			else{
				Debug_Printf(DBG_TEST, "could not Erase1 item %d, which was '%s'. NumElements left in Map: %d \n",curkey, curstr.c_str(), map.Size()); fflush(stdout);
			}

			Debug_Printf(DBG_TEST, "+++++MAP CONTENTS SIZE=%d \n", map.Size()); fflush(stdout);

			for( BiBSTMapT<NodeId_t, string>::Iterator1 i = map.Begin1(); i!=map.End1() ; ++i) {
				Debug_Printf(DBG_TEST, "i->First()=%d i->Second()=%s....\n", i->First(), (i->Second()).c_str());fflush(stdout);
			}
			Debug_Printf(DBG_TEST, "-----END OF MAP CONTENTS SIZE=%d \n", map.Size()); fflush(stdout);
		}
	}
	Debug_Printf(DBG_TEST, "----------- END OF Level_0C --------------\n"); fflush(stdout);

}

void BiMapTest::Level_0B(){
	/*int l1=100;
  rnd = new UniformRandomInt(750,1250);

  for(uint16_t i = l1; i >= 1; i--)
  {
    uint16_t value = rnd->GetNext();
   Debug_Printf(DBG_TEST,"inserting %d \n", value);fflush(stdout);

    shortMap->Insert(value);
     //Debug_Printf(DBG_TEST,"Done. \n");fflush(stdout);
    //Debug_Printf(DBG_TEST, "Min number is %d \n", shortMap[0]);
  }

 Debug_Printf(DBG_TEST, "\n Reading back ... \n");fflush(stdout);
  uint16_t item =0;
  for(uint16_t i = l1; i >= 1; i--)
  {

    item = shortMap->GetItem(shortMap->size() -i);
   Debug_Printf(DBG_TEST, "Item at location  %d  is %d \n", shortMap->size() -i, item);
  }

 Debug_Printf(DBG_TEST, "\n Deleting ... \n");fflush(stdout);
  uint16_t newMax =0;
  newMax = shortMap->GetItem(shortMap->size() -1);
 Debug_Printf(DBG_TEST, "Max num: %d , elements left in list: %d \n", newMax, shortMap->size());
  for(uint16_t i = l1; i >= 1; i--)
  {
    shortMap->Erase1(shortMap->size() -1);
    newMax = shortMap->GetItem(shortMap->size() -1);
   Debug_Printf(DBG_TEST, "Max num: %d.  elements left in list: %d \n", newMax, shortMap->size());
  }
	 */


}

//int testMap(int argc, char* argv[]){
int main(int argc, char* argv[]){
	//Make sure RuntimeOpts construction is the very first line of main
	RuntimeOpts opts ( argc-1, argv+1 );
	stringstream ss;
	InitPlatform(&opts);    //This does a platform specific initialization

	BiMapTest hTest;
	hTest.Execute(argc,argv);

	RunTimedMasterLoop();
	return 0;
}
