////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include "standard-includes.h"
#include "physical-world-hybrid.h"
#include "coursechangecallback.h"

int main (int argc, char *argv[]) {
  SimulationOpts args ( argc - 1, argv+1 );

  physicalWorld_hybrid_t physicalWorld ( &args );
  //physicalWorld.setMobility();
  physicalWorld.setDce();
  
  /////////////////////////////////////////////////////////////////////////// 
  //                                                                       //
  // Tracing configuration                                                 //
  //                                                                       //
  ///////////////////////////////////////////////////////////////////////////

  // std::ifstream infile ( "CourseChangeData.txt" );
  // if ( infile.good() ) {
  // 	  std::remove ( "CourseChangeData.txt" );
  // }
  RemoveOldCourseChangeData();
  Config::Connect ( "/NodeList/*/$ns3::MobilityModel/CourseChange",
		    MakeCallback ( &CourseChangeCallback ));
  
  //Start simulation
  Simulator::Stop ( Seconds ( args.runTime ));
  
  /*bool netAnim = true;
  if (netAnim == 1) {
    physicalWorld.anim();
  }*/
  
  Simulator::Run();


  if ( args.verbose ) printf ( "Simulator done\n" );



  //#include "combine-memory-map.h"
  //#include "destroy-simulation.h"
 Simulator::Destroy ();

}
