////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include "standard-includes.h"
#include "coursechangecallback.h"
#include "injectMobility.h"
#include "physical-world-hybrid.h"

int main (int argc, char *argv[])
{
  SimulationOpts args ( argc - 1, argv+1 );

  physicalWorld_hybrid_t physicalWorld ( &args );
  physicalWorld.setMobility();
  // physicalWorld.setDce();

  /////////////////////////////////////////////////////////////////////////// 
  //                                                                       //
  // Tracing configuration                                                 //
  //                                                                       //
  ///////////////////////////////////////////////////////////////////////////

  std::ifstream infile ( "CourseChangeData.txt" );
  if ( infile.good() ) {
	  std::remove ( "CourseChangeData.txt" );
  }
  Config::Connect ( "/NodeList/*/$ns3::MobilityModel/CourseChange",
		    MakeCallback ( &CourseChangeCallback ));

    //Schedule a event to change mobility of node 0 at time 2 seconds
  Simulator::Schedule (Seconds (2.0), &InjectMobility, physicalWorld.nodes, 0, 1.0,1.0, 1.0);
  Simulator::Schedule (Seconds (2.0), &InjectMobility, physicalWorld.nodes, 1, 100.0,100.0, 100.0);
  Simulator::Schedule (Seconds (2.0), &InjectMobility, physicalWorld.nodes, 2, 200.0,200.0, 200.0);

  
  //Start simulation
  Simulator::Stop ( Seconds ( args.runTime ));
  Simulator::Run ();

  if ( args.verbose ) printf ( "Simulator done\n" );

  Simulator::Destroy ();
}
