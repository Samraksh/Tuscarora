////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

  


#include "Lib/PAL/PAL_Lib.h"
#include "Tests/FW_Init.h"
#include <Sys/Run.h>

#include "runCop.h"
#include <cassert> 
#include "Lib/Logs/logString.h" //logString 
#include <cstdio> // sscanf 

#define DBG_WAVEFORM_ 0
#define DBG_CORE_API_ 0


using namespace Patterns;
using namespace PAL;
// extern bool DBG_WAVEFORM; // set in Lib/Misc/Debug.cpp
// extern bool DBG_TEST; // set in Lib/Misc/Debug.cpp


CopTest::CopTest ( CopOpts &opts ){
	Debug_Printf(DBG_TEST, "CopTest::CopTest\n");  
	processMessageDelegate = new Delegate <void, SizedBuffer> (
		this, &CopTest::ProcessMessage );

	if ( !opts.reliable ) {
		cop = new BCop;
	}else {
		//cop = new RCop;
		cop = new BCop;
	}
	assert(cop);
  
	cop->SetNotifyDelegate ( this->processMessageDelegate );
	cop->SetCopPeriod ( opts.broadcastPeriod );
	cop->SetDistanceSensitivity ( opts.distanceSensitivity );
	
	cop->SetMaxBytesInPayload ( opts.maxBytesInPayload );
	cop->SetMaxPacketsPerCycle ( opts.maxPacketsPerCycle );

	cop->SetRowSelectionMethod ( "Staleness" );
	
	publishPeriod = opts.publishPeriod;

	simulationTimeInSeconds = opts.runTime;
	
	timerDel = new TimerDelegate (this, &CopTest::HandleTimer);
	testTimer = new Timer(200, PAL::PERIODIC, *timerDel);
	// Debug_Printf(DBG_TEST, "CopTest::CopTest: done\n");
	assert(testTimer);
	firstTimeInTimerHandler = true;
	Debug_Printf(DBG_TEST, "CopTest::Constructor Done.\n");  
};

CopTest::~CopTest(){
	Debug_Printf(DBG_TEST, "CopTest::~CopTest: deleting testTimer\n");  
	delete this->testTimer;
	this->testTimer = 0;

	Debug_Printf(DBG_TEST, "CopTest::~CopTest: deleting timerDel\n"); 
	delete this->timerDel;
	this->timerDel = 0;

	Debug_Printf(DBG_TEST, "CopTest::~CopTest: deleting processMessageDelegate\n");  
	delete this->processMessageDelegate;
	this->processMessageDelegate = 0;

	delete this->cop;
};

void CopTest::Publish () {
	//stringstream ss;
	//ss << "hello from " << MY_NODE_ID << ": x=" << this->x()
	//	<< " y=" << this->y()
	//	<< " ts=" << this->TestAliveTimeInSecs();

	char buf[64];
	memset(buf,0,64);
	sprintf(buf, "Hello from %d: x=%d, y=%d, ts=%2.3f \0",  MY_NODE_ID, this->x(), this->y(), this->TestAliveTimeInSecs());

	Debug_Printf(DBG_TEST,  "CopTest::Publish: publishing: %s\n", buf );
	this->cop->Publish ( string(buf) );
};

double CopTest::Distance( double x1, double y1, double x2, double y2 ) {
	double dx = x1 - x2, dy = y1 - y2;
	return sqrt ( dx * dx + dy * dy );
};

void CopTest::ProcessMessage ( SizedBuffer b ) {
	string s((char*) b.data);
	Debug_Printf(DBG_TEST,  "CopTest::ProcessMessage: Received message: %s\n\n", s.c_str() );
	unsigned nodeId;
	double x, y, ts;
	sscanf ( s.c_str(), "hello from %u: x=%lf y=%lf ts=%lf", &nodeId, &x, &y, &ts );
	stringstream ss;
	ss << this->TestAliveTimeInSecs() << " "
		<< this->TestAliveTimeInSecs() - ts << " "
		<< this->Distance ( this->x(), this->y(), x, y ) << " "
		<< x << " " << y << " " << nodeId;
	logString ( "runCop.log", "seconds staleness distance x y from", ss.str() );
};

void CopTest::HandleTimer(uint32_t event){
	//Debug_Printf(DBG_TEST, "CopTest::HandleTimer\n");  
	if ( this->firstTimeInTimerHandler ) {
		cop->Start();
		this->firstTimeInTimerHandler = false;
		// timer takes microseconds
		this->testTimer->Change ( (uint32_t)(this->publishPeriod * 1000000), PERIODIC );
	}
	//Debug_Printf(DBG_TEST,  "CopTest::HandleTimer:: Going to call  publish...\n" ); 


	// check if we should continue
	Debug_Printf(DBG_TEST,  "CopTest::HandleTimer:: Alive time: %f, Total run time: %u \n",  PTN_Utils::PatternAliveTimeInSecs(), simulationTimeInSeconds ); 
	
	if ( PTN_Utils::PatternAliveTimeInSecs() < simulationTimeInSeconds - (publishPeriod * 1.1) )
	{
		Debug_Printf(DBG_TEST,  "CopTest::HandleTimer:: Going to call  publish...\n" ); 
		this->Publish();
	}
	else { // time to stop
		this->cop->Stop();
		this->testTimer->Suspend();
		Debug_Printf(DBG_TEST,  "Stopping the copTest\n" ); 
	}
};

void CopTest::Execute(CopOpts &opts ) {
	Debug_Printf(DBG_TEST, "CopTest::Execute\n");  
	testTimer->Start();
	Debug_Printf(DBG_TEST, "CopTest::Execute done\n");  
}

int main (int argc, char* argv[]) {
	RuntimeOpts runopts ( argc-1, argv+1 );
	CopOpts opts ( argc-1, argv+1 ); 
	InitPlatform(&opts);	//This does a platform specific initialization

	if ( MY_NODE_ID == 0 ) {
		Debug_Printf(DBG_TEST,  "Cop args are: \n" );
		for ( int i = 0; i < argc; i++ ) {
		Debug_Printf(DBG_TEST,  "[%d] %s\n", i, argv[i] );
		}
		Debug_Printf(DBG_TEST,  "\n");
	}

	FW_Init fwInit;
	fwInit.Execute(&runopts);
	
	// GossipTest *gossipTest = new GossipTest();
	// gossipTest->Execute(&opts);

	// Debug_Printf(DBG_TEST, "testCop begin\n");
	Debug::SetTextOutput ( opts.displayTextOutput );
	
	
	//Run a broadcast cop test.
	opts.reliable = false;
	
	CopTest copTest( opts );
	copTest.Execute( opts );

	//RunMasterLoopNeverExit(); //never exists on dce
	RunTimedMasterLoop();
	return 0;
}


