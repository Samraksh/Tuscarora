#include "runBitMap.h"
#include <Sys/Run.h>

#include <assert.h>
#include "Lib/Misc/Debug.h"


using namespace std;


BitMapTest::BitMapTest() {
	// 	DBG_MAIN=true; // set in Lib/Misc/Debug.cpp
	// 	DBG_TEST=true; // set in Lib/Misc/Debug.cpp
#ifdef BSTMAP
	// map = new BiBSTMapT<NodeId_t, string> ();
#else 
	// map = new ListMapT<NodeId_t, string>() ;
#endif

	rnd = new UniformRandomInt(35000,45000);
	
	Debug_Printf(DBG_TEST, "Setting up bitMap with array size %d for %d bits  \n", FindArraySize(bmap.CountTotal()) ,bmap.CountTotal()); fflush(stdout);
}

void BitMapTest:: Execute(int argc, char *argv[]){
	Level_0A();
}

void BitMapTest::Level_0A(){ //NON sequential but deterministic write read test
	Debug_Printf(DBG_TEST, "--------- START OF Level_0A --------- \n"); fflush(stdout);
	int l1=5;
	int l2=NUMITEMS/l1;

	for(int i = l1-1; i >=0 ; i--) {
		for(int j = l2-1 ; j >= 0; j--) {
			uint index= i*l2 + j;
			//Debug_Printf(DBG_TEST, "I: %u, J: %u, L2: %u, index %d \n ",i,j,l2, index);
			Debug_Printf(DBG_TEST, "Before set: index %d: %d ,,,, ", index,bmap.Read(index));
			bmap.Set(index);
			Debug_Printf_Cont (DBG_TEST, "After set: index %d: %d \n",index,bmap.Read(index));
		}
	}
	
	Debug_Printf(DBG_TEST, "\n INSERTING LOOP DONE, numner set: %d \n", bmap.CountSet());fflush(stdout);
	for(int i = 0; i <l1 ; i++) {
		for(int j = l2-1 ; j >= 0; j--) {
			uint index= i*l2 + j;
			Debug_Printf(DBG_TEST, "Before Unset: index %d: %d ,,,, ", index,bmap.Read(index));
			bmap.UnSet(index);
			Debug_Printf_Cont (DBG_TEST, "After Unset: index %d: %d \n", index,bmap.Read(index));
		}
	}
	Debug_Printf(DBG_TEST, "\n INSERTING LOOP DONE, numner set: %d \n", bmap.CountSet());fflush(stdout);

	Debug_Printf(DBG_TEST, "------------- END OF Level_0A ---------------\n"); fflush(stdout);
}


/*
void BitMapTest::Level_0B(){ //RANDOM WRITE READ
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
*/

//int testMap(int argc, char* argv[]){
int main(int argc, char* argv[]){
	//Make sure RuntimeOpts construction is the very first line of main
	RuntimeOpts opts ( argc-1, argv+1 );
	stringstream ss;
	InitPlatform(&opts);    //This does a platform specific initialization

	BitMapTest hTest;
	hTest.Execute(argc,argv);

	RunTimedMasterLoop();
	return 0;
}
