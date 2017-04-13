////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef SIMULATION_OPTS_H
#define SIMULATION_OPTS_H

#include "commandline.h"

// Use macros to define string constants so that mistakes generate
// compile-time errors.

#define RandomWalk2dMobilityModelInt 1
#define ConstantPositionMobilityModelInt 4
#define GaussMarkovMobilityModelInt 2
#define RandomWaypointMobilityModelInt 3
#define TracefileMobilityModelInt 5

// prepend an underscore to avoid name clash with class.
static const string _RandomWalk2dMobilityModel = "RandomWalk2dMobilityModel";
static const string _ConstantPositionMobilityModel = "ConstantPositionMobilityModel";
static const string _GaussMarkovMobilityModel = "GaussMarkovMobilityModel";
static const string _RandomWaypointMobilityModel = "RandomWaypointMobilityModel";
static const string _TracefileMobilityModel = "TracefileMobilityModel";

static const string FriisPropagationLossModelstr = "FriisPropagationLossModel";
static const string RangePropagationLossModelstr = "RangePropagationLossModel";

static const string RandomAlwaysOnLinkEstimation = "RandomAlwaysOn";
static const string ScheduledLinkEstimation = "Scheduled";
//static const string ScheduledPeriodicDCLinkEstimation = "scheduledPeriodicDC";
static const string ConflictAwareLinkEstimation = "ConflictAware";
//static const string ScheduleAwarePeriodicNoDiscoLinkEstimation = "scheduleAwarePeriodicNoDisco";
//static const string ScheduleAwarePeriodicDCLinkEstimation = "scheduleAwarePeriodicDC";

static const string GlobalDiscoveryType = "global";
static const string Oracle2HDiscoveryType = "oracle2hop";
static const string NullDiscoveryType = "nulldiscovery";
static const string Longlink2HDiscoveryType = "longlink2hop";

static const int DataRate1Mbps = 1000000; //in bps, 11Mbps is default

static const string DefaultTracefile = "$TUS/TraceFiles/Simple-Size10-10secs.tr";


/// Subclass Parameter to reflect our particular parameters.
struct SimulationOpts {
  // These are our parameters listed out, so that they can be accessed
  // directly.  Parameters can also be accessed with the operator[]
  // but must be dynamically cast to access the value.
  int rngSeed, rngRun;
  unsigned my_node_id;
  unsigned nodes;
  unsigned deadNeighborPeriod;
  double density;
  double runTime;
  double linkEstimationPeriod;
  double range;
  double speedMin, speedMax;
  string mobilityModel;
  string tracefile;
  string linkEstimationType;
  string interferenceModel;
  string discoveryType;
  bool verbose;
  string pattern;
  string waveformConfig;

  bool writeDescription;
  string patternArgs;
  int dataRate;
  bool displayTextOutput;
  bool logText; // log text as well as binary

  
  Opts _opts; 

  std::string intToString(int i)
  {
    std::stringstream ss;
    std::string s;
    ss << i;
    s = ss.str();
    
    return s;
  }

  /// Create a constuctor which declares the options we're interested in.
  SimulationOpts () {
    this->_init();
  };

  SimulationOpts ( int argc, char **argv ) {
    this->_init();
    if ( ! this->_parse ( argc, argv )) exit ( 1 );
  };

  string __str; 
  string _str() {
    this->__str = this->_opts.str();
    return this->__str;
  };
  
  string __usage; 
  string _usage() {
    this->__usage = this->_opts.usage();
    return this->__usage;
  };
  
  uint8_t mobilityModelInt () {
    string s = this->mobilityModel;
  
    if ( s == _RandomWalk2dMobilityModel )
      return RandomWalk2dMobilityModelInt;

    if ( s == _ConstantPositionMobilityModel )
      return ConstantPositionMobilityModelInt;

    if ( s == _GaussMarkovMobilityModel )
      return GaussMarkovMobilityModelInt;

    if ( s == _RandomWaypointMobilityModel )
      return RandomWaypointMobilityModelInt;
    
    if ( s == _TracefileMobilityModel )
      return TracefileMobilityModelInt;

    printf ( "Unknown mobility model %s\n", s.c_str() );
    exit ( 1 );
  };

  void _init () {
    // printf ( "SimulationOpts init \n%s\n\n", this->_usage().c_str() );
    
    // set class members to the default values
    this->rngSeed = 1;
    this->rngRun = 1;
    this->nodes = 3;
    this->deadNeighborPeriod = 5;
    this->density = 10;
    this->runTime=6;
    this->linkEstimationPeriod = 1000000;
	
    this->range = 2000;//default range
    this->speedMin = 2.0;
    this->speedMax = 4.0;
    this->my_node_id = 333;
    this->mobilityModel = _RandomWalk2dMobilityModel;
    this->tracefile = DefaultTracefile;
    this->linkEstimationType = RandomAlwaysOnLinkEstimation;
    this->interferenceModel = "FriisPropagationLossModel";//RangePropagationLossModel;
    //this->discoveryType = GlobalDiscoveryType;
		this->discoveryType = NullDiscoveryType;
    this->verbose = false;
    this->pattern = "";
    this->writeDescription = false;
    this->patternArgs = ""; // "type=0;num=1";
    this->dataRate = DataRate1Mbps;
    this->displayTextOutput = true;
    this->logText = true;
    // END set class members to the default values
    
    // Bind options to the class members.  After parsing the
    // commandline, the commandline options will be accessible via the
    // class members.
    this->_opts
      .addValue ( "Test",
		  "Pattern or test to run.",
		  &(this->pattern) )

      .addValue( "Size", 
		 "Number of nodes in simulation.", 
		 &(this->nodes) )

      .addValue( "WFConfig", 
		 "Waveform Configuration File", 
		 &(this->waveformConfig) )

      .addValue ( "Density",
		  "Average number of nodes in radio range.",
		  &(this->density) )

      .addValue ( "RunTime",
		  "Running time in seconds.",
		  &(this->runTime) )
      
      .addValue ( "NodeID",
		  "ID of the node (unsigned) integer 16-bit.",
		  &(this->my_node_id) )

      .addValue ( "Mobility",
		  "Mobility model to use.",
		  &(this->mobilityModel) )
      .addChoice( _RandomWalk2dMobilityModel )
      .addChoice( _ConstantPositionMobilityModel )
      .addChoice( _GaussMarkovMobilityModel )
      .addChoice( _RandomWaypointMobilityModel )

      .addValue( "Tracefile", 
		 "Tracefile to use if tracefile mobility model is specified.",
		 &(this->tracefile) )
      
      .addValue( "DeadNeighborPeriod", 
		 "Number of missed link estimation messages for neighbor "
		 "to be considered dead.", 
		 &(this->deadNeighborPeriod) )

      .addValue ( "LinkEstimationPeriod",
		  "Beaconing rate in microseconds.",
		  &(this->linkEstimationPeriod) )

      .addValue ( "Range",
		  "Radio range of the nodes in meters. \n"
		  "Should be 560 to match the assumed density of "
		  "friss interference model.",
		  &(this->range) )

      .addValue ( "SpeedMin",
		  "Minimum speed of a node in meters per second.",
		  &(this->speedMin) )

      .addValue ( "SpeedMax",
		  "Maximum speed of a node in meters per second.",
		  &(this->speedMax) )

      .addValue ( "LinkEstimationType",
		  "Type of link estimation.",
		  &(this->linkEstimationType) )
      .addChoice ( RandomAlwaysOnLinkEstimation )
      .addChoice ( ScheduledLinkEstimation )
      //.addChoice ( ScheduledPeriodicDCLinkEstimation )
      .addChoice ( ConflictAwareLinkEstimation )
      //.addChoice ( ScheduleAwarePeriodicDCLinkEstimation )
      //.addChoice ( ScheduleAwarePeriodicNoDiscoLinkEstimation )

      .addValue ( "InterferenceModel",
		  "Propagation loss model to use.",
		  &(this->interferenceModel) )
      .addChoice( FriisPropagationLossModelstr )
      .addChoice( FriisPropagationLossModelstr )

      .addValue ( "DiscoveryType",
		  "Network discovery type.",
		  &(this->discoveryType) )
      .addChoice( GlobalDiscoveryType )
      .addChoice( Oracle2HDiscoveryType )
      .addChoice( NullDiscoveryType )
      .addChoice( Longlink2HDiscoveryType )

      .addValue ( "Verbose",
		  "Enable wifi logging via ns3.",
		  &(this->verbose) )

      .addValue ( "WriteDescription",
		  "Write description of the simulation and exit.",
		  &(this->writeDescription) )

      .addValue ( "PatternArgs",
		  "Arguments to pass to the pattern.",
		  &(this->patternArgs) )

      .addValue ( "DataRate",
		  "Data Rate.",
		  &(this->dataRate) )
      .addChoice( DataRate1Mbps )

      .addValue( "RngSeed", 
		 "Seed for random number generator",
		 &(this->rngSeed) )
      
      .addValue( "RngRun", 
		 "Run number that with the seed determines the random number generator",
		 &(this->rngRun) )

      .addValue ( "DisplayTextOutput",
		  "Send text to stdout rather than log in binary.",
		  &(this->displayTextOutput) )

      .addValue( "LogText",
		 "Log text as well as binary.", 
		 &(this->logText) );

    // printf ( "SimulationOpts init done\n%s\n\n", this->_usage().c_str() );
  };

  double averageSpeed() {
    return ( this->speedMax + this->speedMin ) / 2;
  };
  
  bool _parse ( int argc, char **argv ) {
    if ( ! this->_opts.parse ( argc, argv )) {
      printf ( "Parsing Error.\n" ); 
      return false;
    }
    else {
      //printf ( "Argument parsed ok.\n" );
      return true;
    }
  };

  string _dump_string;
  string _dump() {
    this->_dump_string = this->_opts.dump();
    return _dump_string;
  };

  string __simulationDirectory_string;
  string simulationDirectory() {
    stringstream s;
    bool first = true;
    for ( unsigned i = 0; i < this->_opts.opts.Size(); i++ ) {
      Opts::Opt *opt = this->_opts.opts[i];
      if ( opt->name == "WriteDescription" ) continue;
      if ( first ) first = false;
      else s << "/";
      s << opt->name << "_" << opt->str();
    }
    this->__simulationDirectory_string = s.str();
    return this->__simulationDirectory_string;
  };
  string __simulationDirectoryWithNode_string;
  string simulationDirectory( unsigned nodeId ) {
    stringstream s;
    s << this->simulationDirectory();
    s << (s.str().size()? "/": "") << nodeId;
    this->__simulationDirectoryWithNode_string = s.str();
    return this->__simulationDirectoryWithNode_string;
  };
  
};


#endif // SIMULATION_OPTS_H
