////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include <fstream>

//
// This function will be used below as ac trace sink, if the command-line
// argument or default value "useCourseChangeCallback" is set to true
//

#define CourseChangeFile "CourseChangeData.txt"
static void
RemoveOldCourseChangeData () {
	std::ifstream infile ( CourseChangeFile );
	if ( infile.good() ) {
		std::remove ( CourseChangeFile );
	}
	std::ofstream outputf2;
	outputf2.open( CourseChangeFile, std::ios::out | std::ios::app ); 
	outputf2.close();
};

static void
CourseChangeCallback (std::string path, Ptr<const MobilityModel> model)
{

std::ofstream outputf2;
outputf2.open( CourseChangeFile, std::ios::out | std::ios::app ); 

Vector position = model->GetPosition ();
Vector velocity = model->GetVelocity ();

outputf2 << "CourseChange at t = " << Simulator::Now ().GetSeconds ()
	 << " " << path
	 << " Vx=" << velocity.x << ", Vy=" << velocity.y
	 << " x=" << position.x
	 << ", y=" << position.y
	 << ", z=" << position.z
	 << std::endl;

outputf2.close();

};
