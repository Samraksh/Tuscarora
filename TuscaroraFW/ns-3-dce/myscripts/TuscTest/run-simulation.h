////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

  //Start simulation
  //Simulator::Stop (Seconds(120.0));
  Simulator::Stop (Seconds(runTime));
  Simulator::Run ();
//Simulator::Destroy ();

if(verbose) printf("Simulator done\n");
