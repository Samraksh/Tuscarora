////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef RUNTIME_OPTS_H_
#define RUNTIME_OPTS_H_

#include <cstdio> // fopen, fclose, fprintf
// #include "commandline.h"
#include "Base/BasicTypes.h"
#include "SimulationOpts.h"
//extern NodeId_t MY_NODE_ID;

/// Subclass Parameter to reflect *our* particular parameters.
struct RuntimeOpts: SimulationOpts {
  // These are our parameters listed out, so that they can be accessed
  // directly.  Parameters can also be accessed with the operator[]
  // but must be dynamically cast to access the value.

  // Opts _opts; // underscore to avoid clashes with members. 

  /// Create a constuctor which declares the options we're interested in.
  RuntimeOpts() { this->_init(); };
  RuntimeOpts( int argc, char* argv[] ) {
    this->_init();
    char helpstr[] = "--help";
		if(argc > 0){
			printf("User passed atleast one argument: Parsing it \n");
			if ( !strcmp(helpstr,*argv) ) {
				printf ( "\n%s\n\n", this->_usage().c_str() );
				exit ( 1 );
			}
			else{
				if ( ! this->_parse ( argc, argv )) {
					printf ( "error parsing\n%s\n\n", this->_usage().c_str() );
					exit ( 1 );
				}
			}
		}
    this->_writeDescription();
  };

  void _writeDescription() {
    if ( this->writeDescription ) {
      // In case this run is to identify the simdir, write uid to it and quit.
      printf( "writing simdir %s to simulation_description\n",
    	      this->simulationDirectory().c_str() );
      FILE *file = fopen ( "simulation_description", "w" );
      if ( file == NULL ) {
	printf ( "Error opening description file \"simulation_description\"\n" );
	exit (1);
      }
      fputs ( this->simulationDirectory().c_str(), file ); 
      /* fprintf ( file, "description%s\n", this->simulationDirectory().c_str() ); */
      fclose ( file );
      exit ( 0 );
    }
  };
  
  void _init() {
    // printf ( "runCopOpts init\n%s\n\n", this->_usage().c_str() );

    // printf ( "runCopOpts init done\n%s\n\n", this->_usage().c_str() );
  };

};

#endif // RUNTIME_OPTS_H_
