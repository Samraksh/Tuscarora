////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////////////// 
  //                                                                       //
  // Tracing configuration                                                 //
  //                                                                       //
  ///////////////////////////////////////////////////////////////////////////

  std::ifstream infile("CourseChangeData.txt");
  if(infile.good()) {
	  std::remove("CourseChangeData.txt");
	}
Config::Connect ("/NodeList/*/$ns3::MobilityModel/CourseChange",MakeCallback (&CourseChangeCallback));
  
