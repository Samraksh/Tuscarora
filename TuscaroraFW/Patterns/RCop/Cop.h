////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef COP_H_
#define COP_H_
/* Cop.h - Base for all Cop-like patterns.
 * Includes application interface (Publish), parameter-setting and logging.
 * Lower-level interface is pure virtual (abstract).
 */

#include <PAL/PAL.h>
#include <Interfaces/Pattern/PatternBase.h>
#include "Lib/Misc/datastructs.h" // map_t, vector_t
#include "CopTypes.h"
//#include "q.h" // FIFO Q
#include "Lib/Buffers/FifoList.h"

#include <sstream>
using std::stringstream;

#include <cassert> // assert

using namespace PWI;
using namespace PAL;
using namespace Patterns;
using namespace Patterns::Cop; // for CopNodeInfo and CopMessage

namespace Patterns {
namespace Cop {

struct SizedBuffer {
	uint16_t size; uint8_t* data;
	SizedBuffer():size(0), data(0) {};
	SizedBuffer ( uint16_t size, uint8_t* data ):
	size(size), data(data) {};
};
  
  // row selection methods: (defined as string constants to get the
  // compiler's help for spelling mistakes.
  static const string SeenMethod = "Seen";
  static const string StalenessMethod = "Staleness";
  static const string PropotionalStalenessMethod = "PropotionalStaleness";
  static const uint16_t COP_INITIAL_NONCE = 1;


class Cop : public PatternBase{
	  
public:
	const int COP_DEFAULT_PAYLOAD = 500;
	const int COP_DEFAULT_PPC = 1;
	const double COP_DEFAULT_PERIOD = 1.0;
	const double COP_DEFAULT_SENSITIVITY = 1.0;
	const double COP_DEFAULT_QUALITY_THRESHOLD = 0.1;
	const uint16_t COP_TIME_GRANULARITY_USEC = 1000; // now() units: msec

	Cop(char* name = (char*)"Cop_1");
    virtual ~Cop ();

    /*** parameter-setting methods. ***/
    void SetCopPeriod ( double seconds ) {
      this->copPeriod = seconds;
    };

    void SetDistanceSensitivity ( double distanceScaler ) {
      this->distanceSensitivity = distanceScaler;
    };

    void SetMaxBytesInPayload ( unsigned bytes ) {
		if (bytes > 0) this->maxBytesInPayload = bytes;
    };

    void SetMaxPacketsPerCycle ( unsigned count ) {
		if (count > 0) this->maxPacketsPerCycle = count;
    };

    // The Notify delegate is a callback from the application. Cop
    // calls it when it gets new information published by other nodes.
    void SetNotifyDelegate ( Delegate <void, SizedBuffer> *del ) {
      this->Notify = del;
    };

    /// Return a broadcast delay based on the current broadcast period.
    /// This is common to *Cop (for now).
    
  
    /***** Application interface methods *****/
    bool Start();
    bool Stop();

    /// Publishes and takes ownership of a new[]-allocated buffer.
    /// data must be freed with delete[] when we are done with it.
    bool Publish (uint16_t size, uint8_t* buff );
    bool Publish (string s);

    /*** Common vars - used/set by the environment in a mysterious way. ****/
    PatternId_t myPatternID; /// identifier used to mark packets
    NodeId_t myNodeId;

    /**** Framework interface methods (upcalls) *******/
    // each of these does common processing, then
    // invokes a virtual private custom...() version.
    // In this (base) class implementation, the custom method does nothing.
    void ActivityTimerHandler (uint32_t event );
    void NeighborUpdateEvent (NeighborUpdateParam nbrUpdate);  // nh chg
    void ControlResponseEvent (ControlResponseParam response); // ctl rtn
    void DataStatusEvent (DataStatusParam notification);
    void ReceiveMessageEvent (FMessage_t& msg);  // incoming message handler

    //static CopTimestamp_t TimeStamp();
	//CopTimestamp_t PatternAliveTimeStamp();  
    //double PatternAliveTimeInSecs(); // time since start of simulation

	virtual void SetRowSelectionMethod ( string method ) {}
protected:

	string patternName;  
	/*** Info to be shared - same in all derived classes. ***/
    CopNodeInfo myInfo;  // my information - always sent.

    /* "assign this to your delegate if you want to be notified of
     * messages from other nodes..."  ????
     * I do n't know what this is.
     */
    Delegate <void, SizedBuffer> *Notify;
	PacketBufferHelper pktHelper;

	PatternNeighborTableI *myNbrTable;
	
	///Main cop table that stores the data.
	CopTable_t copTable;
	
    /* simulation parameters should really be static.
     * However, that's problematic since I don't know how nodes are init'd in
     * the simulation - is the class loaded once for all nodes, or...?
     * Shouldn't matter, as long as everybody usess the same thing.
     * But I'm leaving them as non-static members for now.
     */
    double copPeriod;
    double distanceSensitivity;
    unsigned maxBytesInPayload;
    unsigned maxPacketsPerCycle;
    UniformRandomInt *randPublishDelay;
    uint64_t startTime;
    TimerDelegate *activityTimerDel;
    Timer *activityTimer;
    FrameworkAttributes fwAttrs;
    std::set<ControlResponseTypeE> pendingControlResponses;

    bool stopped;    // for graceful shutdown ... just in case
    uint16_t nonce;  // for handshaking with Framework
    //bool OKToSend;   // ditto
    
    uint32_t GetRandomPublishDelay () {
		// broadcast delay averages 1 second. Scale it by multiplying.
		unsigned r = randPublishDelay->GetNext();
		return r;
    };

private:
	virtual const char* GetPatternName() {
		return (patternName.c_str());
	};

	/*** utility ***/
	void dumpBytes(uint16_t len, uint8_t* data);

	void StartTimers();
	void FinishStartup();

	virtual void CustomActivityTimerHandler(uint32_t event) {
		/* do nothing */
	}

	virtual void CustomControlRspHandler(ControlResponseParam response) {
		/* do nothing... */
	};

	virtual void CustomNeighborUpdateHandler(NeighborUpdateParam nbrUpdate) {
		/* do nothing... */
	}

	virtual void CustomDataStatusHandler(DataStatusParam notification) {
		/* do nothing... */
	}

	virtual void CustomStartTimers() {
		/* do nothing... */
	}

	virtual void ProcessCopMessage(CopMessage& input) {
		Debug_Printf(DBG_PATTERN,"Cop: received CopMessage: %s\n", input.ToString().c_str());
		/* do nothing else... */
	}

	void PrintCopTable();	
	
#if 0
    // XXX these will need to be tweaked to match metrics and have a
    // strategy-independent interface.
    virtual void LogStaleness ( ExportRow &item, int improved, int hops);
    virtual void LogBroadcastSize ( unsigned bytes );
    virtual void LogSentPacket ( PacketId pid, unsigned bytes, unsigned items );
    //number of stale items, number of items seen already,
    // number of new items.
    virtual void LogReceivedPacket ( PacketId pid, unsigned stale,
				     unsigned current, unsigned fresh );
    PacketLogger packetLogger;  // XXX not used anywhere!
#endif

}; // Cop base class
  
}; // Cop namespace
}; // pattern namespace

#endif // COP_H_
