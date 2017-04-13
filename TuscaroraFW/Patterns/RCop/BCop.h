////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef BCOP_H_
#define BCOP_H_

#include <PAL/PAL.h>
#include <Interfaces/Pattern/PatternBase.h>

#include "Lib/Misc/datastructs.h" // map_t, vector_t
#include "Cop.h"  // namespaces Patterns and Patterns:CopAux

#include <sstream>
#include <cassert> // assert

#include "Lib/Logs/logPacket.h" // PacketLogger

using namespace PWI;
using namespace PAL;

namespace Patterns {
namespace Cop {

enum BCopRowSelectionMethod { Seen, Staleness, PropotionalStaleness, Invalid };

class BCop : public Cop {
public:
	BCop(char* name = (char*)"BCop_1");
	~BCop();

	void SetRowSelectionMethod ( BCopRowSelectionMethod method );
	void SetRowSelectionMethod ( string method ) {
		Debug_Printf(DBG_PATTERN, "BCop::SetRowSelectionMethod: Setting the row selection method to : %s \n", method.c_str());
		
		BCopRowSelectionMethod m =
		method == SeenMethod? Seen:
		method == StalenessMethod? Staleness:
		method == PropotionalStalenessMethod? PropotionalStaleness:
		Invalid;
		return SetRowSelectionMethod ( m );
	};

private:
	virtual char *getPatternName() {
		return (char *) "BCop";
	};

	UniformRandomValue rng;
	//std::map<uint16_t,PendingMessageRecord *> bufferedMsgs;



	/* If not commented out, these override the virtual functions
		* in the base Cop class.  The ones commented out are not needed
		* in this class.
		*/
	void CustomActivityTimerHandler(uint32_t event);
	//    void customControlRspHandler(ControlResponseParam response);
	void CustomNeighborUpdateHandler(NeighborUpdateParam nbrUpdate){}
	void CustomDataStatusHandler(DataStatusParam notification);
	//    void customStartTimers();
	void CustomReceiveMessageHandler(FMessage_t& msg);

	// invariant: all Row entries in the map are new'd.
	// Why the wrapped type?  And why the typedef here?
	
	BCopRowSelectionMethod rowSelectionMethod;

	bool SelectRow ( Row *r ); // set dynamically to one of the following
	bool SelectBySeen ( Row *r );
	bool SelectByStaleness ( Row *r );
	bool SelectByPropotionalStaleness ( Row *r );

	bool BroadcastMessage( CopMessage* msg, uint16_t nonce);
	void BroadcastFromCopTable ();

	void ProcessCopMessage (CopMessage& received);
	//PendingMessageRecord* findQueuedMsg();
}; // BCop class

	
}; // Cop namespace	
}; // pattern namespace
#endif // BCOP_H_
