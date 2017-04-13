////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include "SimulationOpts.h"
/*** Utility program  ***/

struct mainOpts_t : parameters_t {
  bool Parse, Help, Usage;
  mainOpts_t (){
    options.push_back (
      &((*(new parameter_t < bool >))
	.addPtr( &(this->Parse) )
	.addName( "parse" )
	.addLong( "parse" )
	.addDefaultValue( false )
	.addHelp( "Parse arguments." )));
    
    options.push_back (
      &((*(new parameter_t < bool >))
	.addPtr( &(this->Help) )
	.addName( "help" )
	.addShort( 'h' )
	.addLong( "help" )
	.addDefaultValue( false )
	.addHelp( "Show help." )));
    
    options.push_back (
      &((*(new parameter_t < bool >))
	.addPtr( &(this->Usage) )
	.addName( "usage" )
	.addShort( 'u' )
	.addLong( "usage" )
	.addDefaultValue( false )
	.addHelp( "Show usage." )));
  };
};
    
int main ( int argc, const char** argv ) {
  bool did_something = false;
  mainOpts_t mainOpts;
  SimulationOpts args;
  if (! mainOpts.parse ( (unsigned) argc-1, argv+1 )) {
    // error
    printf("Error parsing.\n\nUsage:\n%s\n", mainOpts.usage().c_str());
    return 1;
  }
  if ( mainOpts.Parse ) {
    bool ok = true;

    // printf ( "Parsing argv: \n" );
    // for ( unsigned i = 0; i < mainOpts.argv.size(); i++ ) {
    //   printf ( "  %s\n", mainOpts.argv[i] );
    // }
    // printf("\n");

    
    if (! args.parse( mainOpts.argv.size(),
		      (const char**) mainOpts.argv.data() )) {
      ok = false;
      printf( "Error parsing.\nArgs so far are:\n%s\n\nUsage:\n%s\n",
	      args.dump().c_str(), args.usage().c_str() );
    }
    if (args.unknowns.size()) {
      ok = false;
      printf( "Found unknown args:\n" );
      for ( unsigned i = 0; i < args.unknowns.size(); i++ ) {
	printf ( "  %s\n", args.unknowns[i].c_str() );
      }
    }
    // if (args.positionals.size()) {
    //   printf( "Found positional arguments: \n" );
    //   for ( unsigned i = 0; i < args.positionals.size(); i++ ) {
    // 	printf ( "  %s\n", args.positionals[i].c_str() );
    //   }
    // }
    if ( ok ) {
      printf("Parse results: \n%s\n", args.dump().c_str() );
      printf ( "OK\n" );
    }
    did_something = true;
  }
    
  if ( mainOpts.Usage ) {
    printf( "%s\n", args.usage().c_str());
    did_something = true;
  }

  if ( mainOpts.Help || ! did_something ) {
    printf( "%s\n", mainOpts.usage().c_str());
    did_something = true;
  }
	
    
}

