////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef COP_OPTS_H
#define COP_OPTS_H

// #include "commandline.h"
#include <Base/BasicTypes.h>
#include "Lib/Logs/log.h"
#include <Sys/RuntimeOpts.h>

// row selection methods: (defined as string constants to get the
// compiler's help for spelling mistakes.
//static const string VinodMethod = "Vinod";



struct LocalBroadcastOpts: RuntimeOpts {
  double broadcastPeriod;
  
  /// Create a constuctor which declares the options we're interested in.
  LocalBroadcastOpts() {
    this->_init();
  };

  LocalBroadcastOpts( int argc, char* argv[] ) {
    this->_init();
    if ( ! this->_parse ( argc, argv )) {
      printf ( "error parsing\n%s\n\n", this->_usage().c_str() );
      exit ( 1 );
    }
    this->_writeDescription();
  };

  void _init() {
    // printf ( "runLocalBroadcastOpts init\n%s\n\n", this->_usage().c_str() );
    this->broadcastPeriod = 1;
    this->_opts
      .addValue ( "BroadcastPeriod",
		  "Average Period for broadcasting in seconds.",
		  &(this->broadcastPeriod) );
  };

};



struct CopOpts: LocalBroadcastOpts {
	
	const string SeenMethod = "Seen";
	const string StalenessMethod = "Staleness";
	const string PropotionalStalenessMethod = "PropotionalStaleness";

	double distanceSensitivity;
	string rowSelectionMethod;
	unsigned maxBytesInPayload;
	unsigned maxPacketsPerCycle;
	double publishPeriod;
	bool reliable;

	/// Create a constuctor which declares the options we're interested in.
	CopOpts() {
	this->_init(); // _init does the work.
	};

	CopOpts( int argc, char* argv[] ) {
	this->_init(); // _init does the work.
	if ( ! this->_parse ( argc, argv )) {
		printf ( "error parsing\n%s\n\n", this->_usage().c_str() );
		exit ( 1 );
	}
	this->_writeDescription();
	};

	void _init() {
	// printf ( "runCopOpts init\n%s\n\n", this->_usage().c_str() );
	this->distanceSensitivity = 1;
	this->rowSelectionMethod = SeenMethod;
	this->maxBytesInPayload = 800;
	this->maxPacketsPerCycle = 1;
	this->publishPeriod = this->broadcastPeriod;
	this->reliable=true;

	this->_opts
		.addValue ( "RowSelectionMethod"
			, "Method to select rows from the cop table. "
			"Can be: \n"
			"  Seen:      If the row has already been seen more \n"
			"             than two times, don't select. If selecting, \n"
			"             select proportional to hops.\n"
			"  Staleness: Choose proportional to expected staleness. \n"
			"  PropotionalStaleness:     Select when the delay is greater than distance."
			, &(this->rowSelectionMethod) )
		.addChoice ( SeenMethod )
		.addChoice ( StalenessMethod )
		.addChoice ( PropotionalStalenessMethod )
		.addValue ( "DistanceSensitivity"
			, "Sensitivity to distance: 0=not sensitive, 1=default.\n"
			"Distance sensitivity (DS) scales the distance in the row \n"
			"selection algorithm. So DS=1 makes all distances look like 0. \n"
			"DS=1 does not change the distance while DS>1 amplifies the \n"
			"effect of distance."
			, &(this->distanceSensitivity) )
		.addValue ( "MaxBytesInPayload"
			, "Max bytes in a packet payload."
			, &(this->maxBytesInPayload) )
		.addValue ( "MaxPacketsPerCycle"
			, "The maximum number of packets a node can send out per cycle."
			, &(this->maxPacketsPerCycle) )
		.addValue ( "PublishPeriod"
			, "Average time between node info updates in seconds.\n"
			"Default is BroadcastPeriod."
			, &(this->publishPeriod))
		.addValue ( "Broadcast"
			, "Use Reliable Cop or unreliable cop.\n"
			"Default is Reliable."
			, &(this->reliable))
		;
			
		
	};

};

#endif // COP_OPTS_H