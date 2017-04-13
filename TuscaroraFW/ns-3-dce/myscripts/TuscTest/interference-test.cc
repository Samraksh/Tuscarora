////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include "standard-includes.h"
#include "physical-world-hybrid.h"

int main (int argc, char *argv[])
{
  SimulationOpts args ( argc - 1, argv+1 );
  physicalWorld_hybrid_t physicalWorld ( &args );
  physicalWorld.setMobility();
  physicalWorld.setDce();
  
  //Start simulation
  Simulator::Stop ( Seconds ( args.runTime ));
  Simulator::Run ();

  if ( args.verbose ) printf ( "Simulator done\n" );

  Simulator::Destroy ();

}
