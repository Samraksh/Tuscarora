////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include "ns3/core-module.h"
#include "ns3/slim-module.h"
#include <cmath>
#include <cassert>

//#include "slim-validation-constant-SINR.h"

using namespace ns3;

uint64_t n_ss_packets_created;
uint64_t n_ms_packets_created;
uint64_t n_ss_packets_success;
uint64_t n_ms_packets_success;
uint64_t n_packets_error;

//This creates a tx vector closely resembling 802.11 DSSS 1Mbps rate tx with long preamble
SlimTxVector CreateSlimTxVector(int numsections, Time totalduration){
	Ptr<SlimTxVector> txvector_ptr = CreateObject<SlimTxVector> ();
	txvector_ptr->SetUniqueName("Tuscarora1MbpsMode");

	SlimMode slimpaylod  = *(CreateObject<SlimMode> ());
	slimpaylod.SetUniqueName("TuscaroraPayloadMode");
	slimpaylod.SetDuration(totalduration/(int64_t)numsections);


	for(int i = 0; i < numsections; ++i){
		txvector_ptr->GetListofModes().push_back(slimpaylod);
	}

	txvector_ptr->SetPayloadModePosition(numsections-1);


	return *txvector_ptr;

}


NS_LOG_COMPONENT_DEFINE ("SlimWaveformExample");




void ScheduleMSPacketReception (Ptr<TuscaroraSlimPhy> phy, double rxPowerDbm, int64_t numsections, Time totalduration)
{
	SlimTxVector txvector = CreateSlimTxVector(numsections, totalduration);

	if(numsections == 1) ++n_ss_packets_created;
	else ++n_ms_packets_created;

	phy -> StartReceivePacket (Create<Packet> (),
									 rxPowerDbm,
									 txvector,
	                                 0, phy->CalculateTxDuration(txvector));

}




void ComparePacketReceptionwithSINR(Ptr<TuscaroraSlimPhy> _phy, double rxPowerDbm){
	double ss_successratio = ((double)n_ss_packets_success)/((double)n_ss_packets_created); //Use ss_successratio as unbiased estimator of psr
	//double ms_successratio = ((double)n_ms_packets_success)/((double)n_ms_packets_created);

	double expected_ms_packet_success = double(n_ms_packets_created) * ss_successratio;
	double theoretical_6_sigma = double(6) * double(std::sqrt(double(n_ms_packets_created)*ss_successratio*(1-ss_successratio)));
	double absolute_diff =
			(n_ms_packets_success > expected_ms_packet_success ? n_ms_packets_success - expected_ms_packet_success : expected_ms_packet_success - n_ms_packets_success );


	printf("n_ss_packets_created = %jd, n_ss_packets_success = %jd, n_ms_packets_created = %jd, n_ms_packets_success %jd, absolute_diff = %.2f theoretical_6_sigma = %.2f\n"
			,n_ss_packets_created, n_ss_packets_success, n_ms_packets_created, n_ms_packets_success, absolute_diff, theoretical_6_sigma);
	assert( absolute_diff <= theoretical_6_sigma);
}

void RxFromPhyOK (Ptr<Packet> aggregatedPacket, double rxSnr, SlimTxVector txVector){
	if(txVector.GetListofModes().size() == 1) ++n_ss_packets_success;
	else ++n_ms_packets_success;
	return;
}

void RxFromPhyError (Ptr<const Packet> packet, double rxSnr){
	++n_packets_error;
	return;
}

int main (int argc, char *argv[])
{

	n_ss_packets_created = n_ms_packets_created = n_ss_packets_success =  n_ms_packets_success = n_packets_error =  0;


	uint32_t numPackets = 10000;
	double rxPowerDbm = -100;
	int64_t numsections = 10;
	int64_t totalmicroseconds = 2000;


	CommandLine cmd;
	cmd.AddValue ("numPackets", "Number of packet to be simulated", numPackets);
	cmd.AddValue ("rxPowerDbm", "The received power to be simulated", rxPowerDbm);
	cmd.AddValue ("numsections", "The number of sections to be simulated", numsections);
	cmd.AddValue ("totalmicroseconds", "Length of packet in microseconds", totalmicroseconds);
	cmd.Parse (argc,argv);


	Time totalduration = MicroSeconds(totalmicroseconds);

	//-- Create PHY
	Config::SetDefault ("ns3::TuscaroraSlimPhy::EnergyDetectionThreshold", DoubleValue (-500.0));
	Ptr<TuscaroraSlimPhy> phy = CreateObject<TuscaroraSlimPhy> ();
	phy->SetAntenna(CreateObject<IsotropicAntennaModel> ());
	phy->GetTxVectorList().push_back(CreateSlimTxVector(1,totalduration));
	phy->GetTxVectorList().push_back(CreateSlimTxVector(numsections,totalduration));


	phy->SetReceiveOkCallback (MakeCallback (&RxFromPhyOK));
	phy->SetReceiveErrorCallback (MakeCallback (&RxFromPhyError));



	  //------------------------------------------------------------
	  //-- Applications
	  //--------------------------------------------

	for (uint32_t i=0; i<numPackets/2 ; ++i){
		Simulator::Schedule (Seconds(i+1), &ScheduleMSPacketReception,phy,rxPowerDbm,1,totalduration ) ;
	}
	for (uint32_t i=numPackets/2+1; i<numPackets ; ++i){
		Simulator::Schedule (Seconds(i+1), &ScheduleMSPacketReception,phy,rxPowerDbm,numsections,totalduration ) ;
	}

	Simulator::Schedule (Seconds(numPackets+2), &ComparePacketReceptionwithSINR,phy,rxPowerDbm) ;

	  //------------------------------------------------------------
	  //-- Simulation
	  //--------------------------------------------
	Simulator::Stop (Seconds (numPackets+3));
	Simulator::Run ();
	Simulator::Destroy ();
	return 0;
}


