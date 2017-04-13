////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include "runMap.h"

#include <sstream>

#define NUMITEMS 100000

// extern bool DBG_MAIN; // set in Lib/Misc/Debug.cpp
// extern bool DBG_TEST; // set in Lib/Misc/Debug.cpp

using namespace std;


MapTest::MapTest() {
// 	DBG_MAIN=true; // set in Lib/Misc/Debug.cpp
// 	DBG_TEST=true; // set in Lib/Misc/Debug.cpp
#ifdef BSTMAP
	// map = new BSTMapT<NodeId_t, string> ();
#else 
	// map = new ListMapT<NodeId_t, string>() ;
#endif

	rnd = new UniformRandomInt(35000,45000);
}

void MapTest:: Execute(int argc, char *argv[]){
	//Sequential test
	Level_0A();
	Level_0C();
	//Generate random numbers
	//Level_0B();
}

void MapTest::Level_0A(){
	int l1=10;
	int l2=NUMITEMS/l1;
	stringstream ss;

	Debug_Printf(DBG_TEST,"inserting uint16_t... \n");fflush(stdout);
	for(uint16_t i = l1; i >= 1; i--) {
		for(uint16_t j = (i * l2) - l2+1 ; j <= i * l2; j++) {
			ss << j;
			string s= "string of " +  ss.str();
			Debug_Printf(DBG_TEST,"inserting %d, %s \n", j, s.c_str());fflush(stdout);
			map[j]=s;
			//Debug_Printf(DBG_TEST,"Done. \n");fflush(stdout);
			string s1 = map[j];
			Debug_Printf(DBG_TEST, "string read back is %s \n", s1.c_str());
		}
	}

	map.Insert(16, map[17]);

	Debug_Printf(DBG_TEST, "\n Reading back ... \n");fflush(stdout);
	for(uint16_t i = l1*l2; i >=1 ; i--) {

		Debug_Printf(DBG_TEST, "Looking for item %d....\n", i);
		string s = map[i];
		Debug_Printf(DBG_TEST, "Item at location %d  is %s\n", i, s.c_str()); fflush(stdout);
	}

	//Debug_Printf(DBG_TEST, "\n Iterator Test back ... \n");fflush(stdout);
	//	BSTMapT<NodeId_t, string>::Iterator it = map.Begin();
	//	Debug_Printf(DBG_TEST, "it->First()=%d it->Second()=%s....\n", it->First(), (it->Second()).c_str());fflush(stdout);
	//	it = it+1;
	//	Debug_Printf(DBG_TEST, "it->First()=%d it->Second()=%s....\n", it->First(), (it->Second()).c_str());fflush(stdout);

       Debug_Printf(DBG_TEST, "\n Iterator Test ... \n");fflush(stdout);
	for( BSTMapT<NodeId_t, string>::Iterator i = map.Begin(); i!=map.End() ; ++i) {
		Debug_Printf(DBG_TEST, "i->First()=%d i->Second()=%s....\n", i->First(), (i->Second()).c_str());fflush(stdout);
	}
	Debug_Printf(DBG_TEST, "\n END Iterator Test  ... \n");fflush(stdout);

	Debug_Printf(DBG_TEST, "\n Iterator Test ... \n");fflush(stdout);
	for( BSTMapT<NodeId_t, string>::Iterator i = map.Begin(); i!=map.End() ; ++i) {

		Debug_Printf(DBG_TEST, "i->First()=%d i->Second()=%s....\n", i->First(), (i->Second()).c_str());fflush(stdout);
		i = i+2;
	}
	Debug_Printf(DBG_TEST, "\n END Iterator Test  ... \n");fflush(stdout);

	Debug_Printf(DBG_TEST, "\n Iterator Test back and delete... \n");fflush(stdout);
	for( BSTMapT<NodeId_t, string>::Reverse_Iterator i = map.RBegin(); i!=map.REnd() ; ++i) {
		++i;
		Debug_Printf(DBG_TEST, "Erasing i->First()=%d i->Second()=%s....\n", i->First(), (i->Second()).c_str());fflush(stdout);
		map.Erase(i);
		if( i != map.REnd() ) {
			Debug_Printf(DBG_TEST, "I AFTER ERASE i->First()=%d i->Second()=%s....\n", i->First(), (i->Second()).c_str());fflush(stdout);
		}
		else {
			Debug_Printf(DBG_TEST, "I after erase is REnd \n");fflush(stdout);
		}

		Debug_Printf(DBG_TEST, "+++++MAP CONTENTS SIZE=%d \n", map.Size()); fflush(stdout);
				for( BSTMapT<NodeId_t, string>::Reverse_Iterator i2 = map.RBegin(); i2!=map.End() ; ++i2) {
					Debug_Printf(DBG_TEST, "i2->First()=%d i->Second()=%s....\n", i2->First(), (i2->Second()).c_str());fflush(stdout);
				}
		Debug_Printf(DBG_TEST, "-----END OF MAP CONTENTS SIZE=%d \n", map.Size()); fflush(stdout);
	}
	Debug_Printf(DBG_TEST, "\n Iterator Test back ... \n");fflush(stdout);

	Debug_Printf(DBG_TEST, "\n Deleting ... \n");fflush(stdout);
	Debug_Printf(DBG_TEST, "Elements left in Map: %d \n", map.Size());
	for(uint16_t i = l1*l2; i >= 1; i--) {
		string s = map[i];
		map.Erase(i);
		Debug_Printf(DBG_TEST, "Erased item %d, which was '%s'. Elements left in Map: %d \n",i, s.c_str(), map.Size()); fflush(stdout);
	}
	Debug_Printf(DBG_TEST, "END OF Level_0A \n"); fflush(stdout);
}

void MapTest::Level_0C(){
	int l1=10;
	int l2=NUMITEMS/l1;
	stringstream ss;

	Debug_Printf(DBG_TEST,"inserting uint16_t... \n");fflush(stdout);
	for(uint16_t i = l1; i >= 1; i--) {
		for(uint16_t j = (i * l2) - l2+1 ; j <= i * l2; j++) {
			ss << j;
			string s= "string of " +  ss.str();
			Debug_Printf(DBG_TEST,"inserting %d, %s \n", j, s.c_str());fflush(stdout);
			map[j]=s;
			//Debug_Printf(DBG_TEST,"Done. \n");fflush(stdout);
			string s1 = map[j];
			Debug_Printf(DBG_TEST, "string read back is %s \n", s1.c_str());
		}
	}

	rnd = new UniformRandomInt(0, (l1*l2)+1);
	int value;
	BSTMapT<NodeId_t, string>::Iterator it;
	int k=20;
	Debug_Printf(DBG_TEST, "\n Deleting ... \n");fflush(stdout);
	while(map.Size() > 0){
		--k;
		value = rnd->GetNext();
		it = map.Find(value);
		if (it == map.End())  {
			Debug_Printf(DBG_TEST, "Elements not found: %d \n", value);
		}
		else {
			Debug_Printf(DBG_TEST, "Element found: %d \n ", value);
			int curkey = it->First();
			string curstr = it->Second();
			if(map.Erase(it)){

				Debug_Printf(DBG_TEST, "Erased item %d, which was '%s'. NumElements left in Map: %d \n",curkey, curstr.c_str(), map.Size()); fflush(stdout);
			}
			else{
				Debug_Printf(DBG_TEST, "could not Erase item %d, which was '%s'. NumElements left in Map: %d \n",curkey, curstr.c_str(), map.Size()); fflush(stdout);
			}

			Debug_Printf(DBG_TEST, "+++++MAP CONTENTS SIZE=%d \n", map.Size()); fflush(stdout);
                        
			for( BSTMapT<NodeId_t, string>::Iterator i = map.Begin(); i!=map.End() ; ++i) {
				Debug_Printf(DBG_TEST, "i->First()=%d i->Second()=%s....\n", i->First(), (i->Second()).c_str());fflush(stdout);
			}
			Debug_Printf(DBG_TEST, "-----END OF MAP CONTENTS SIZE=%d \n", map.Size()); fflush(stdout);
		}
	}
	Debug_Printf(DBG_TEST, "END OF Level_0C \n"); fflush(stdout);
}

void MapTest::Level_0B(){
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
    shortMap->Erase(shortMap->size() -1);
    newMax = shortMap->GetItem(shortMap->size() -1);
   Debug_Printf(DBG_TEST, "Max num: %d.  elements left in list: %d \n", newMax, shortMap->size());
  }
	 */


}

//int testMap(int argc, char* argv[]){
int main(int argc, char* argv[]){
  //Make sure RuntimeOpts construction is the very first line of main
  RuntimeOpts opts ( argc-1, argv+1 );
  InitPlatform(&opts);    //This does a platform specific initialization


  MapTest hTest;
  hTest.Execute(argc,argv);

  RunTimedMasterLoop();
  return 0;
}
