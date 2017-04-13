////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include "ns3/core-module.h"
#include "ns3/slim-module.h"
#include <cmath>
#include <cassert>
#include "binomial.h"

//#include "slim-validation-constant-SINR.h"

using namespace ns3;

uint64_t n_packets_created;
uint64_t n_packets_success;
uint64_t n_packets_error;




//This creates a tx vector closely resembling 802.11 DSSS 1Mbps rate tx with long preamble
SlimTxVector CreateSlimTxVector(){
	Ptr<SlimTxVector> txvector_ptr = CreateObject<SlimTxVector> ();
	txvector_ptr->SetUniqueName("Tuscarora1MbpsMode");

	Ptr<SlimMode> slimpaylodptr = CreateObject<SlimMode> ();
	slimpaylodptr->SetUniqueName("TuscaroraPayloadMode");
	txvector_ptr->GetListofModes().push_back(*slimpaylodptr);

	txvector_ptr->SetPayloadModePosition(0);


	return *txvector_ptr;

}


NS_LOG_COMPONENT_DEFINE ("SlimWaveformExample");


double CalculateSnr (double signal, double noiseInterference, uint32_t BW, Ptr<TuscaroraSlimPhy> _phy)
{
	// thermal noise at 290K in J/s = W
			static const double BOLTZMANN = 1.3803e-23;
	// Nt is the power of thermal noise in W
	double Nt = BOLTZMANN * 290.0 * BW;
	// receiver noise Floor (W) which accounts for thermal noise and non-idealities of the receiver

	double m_noiseFigure = std::pow (10.0, _phy->GetRxNoiseFigure() / 10.0);

	double noiseFloor = m_noiseFigure * Nt;
	double noise = noiseFloor + noiseInterference;
	double snr = signal / noise;
	return snr;
}


void SchedulePacketReception (Ptr<TuscaroraSlimPhy> phy, double rxPowerDbm, SlimTxVector txvector)
{
//	SlimTxVector txvector = CreateSlimTxVector();

	++n_packets_created;
	phy -> StartReceivePacket (Create<Packet> (),
									 rxPowerDbm,
									 txvector,
	                                 0, phy->CalculateTxDuration(txvector));

}

void ComparePacketReceptionwithSINR(Ptr<TuscaroraSlimPhy> _phy, double rxPowerDbm, SlimTxVector txvector){
	printf("Starting comparing simulation results with the theoretical values. \n");

	PointerValue tmp;
	txvector.GetMode().GetAttribute("ErrorModPtr",tmp);
	Ptr<Object> error3 =  tmp.GetObject();
	Ptr<SlimModeError> error2 = error3->GetObject<SlimModeError>();



	uint64_t expected_n_packet_success;
	uint64_t theoretical_6_sigma;

	if(rxPowerDbm >= _phy->GetEdThreshold()) {
		double rxPowerW = std::pow (10.0, rxPowerDbm / 10.0)/1000;
		double snr = CalculateSnr(rxPowerW,0,txvector.GetMode().GetBandwidth(),_phy);
		double ber = error2->GetBER(snr);




		uint32_t numbits = (txvector.CalculateTxDuration().GetNanoSeconds()/txvector.GetMode().GetDataRate().GetNanoSeconds());
		uint32_t num_error_tolerated = txvector.GetMode().GetCodeRate() * numbits;


		double psr = 0;

		for(uint64_t i = 0; i <= num_error_tolerated; ++i ){
			psr += Binomial(i, ber, numbits);
		}
		expected_n_packet_success = n_packets_created * psr;
		theoretical_6_sigma = ceil(double(6) * double(std::sqrt(double(n_packets_created)*psr*(1-psr))));
	}
	else{
		expected_n_packet_success = 0;
		theoretical_6_sigma = 0;
	}



	uint64_t absolute_diff = (expected_n_packet_success > n_packets_success ? expected_n_packet_success - n_packets_success : n_packets_success - expected_n_packet_success );

	printf("rxPowerDbm = %2.2f, n_packets_created = %jd, n_packets_success = %jd, expected_n_packet_success = %jd, absolute_diff %jd, theoretical_6_sigma = %jd, Percent Error = %.6f \n"
			, rxPowerDbm, n_packets_created, n_packets_success, expected_n_packet_success, absolute_diff, theoretical_6_sigma, double(absolute_diff)/double(n_packets_created)  );

	assert( absolute_diff <= theoretical_6_sigma + 1);
}

void RxFromPhyOK (Ptr<Packet> aggregatedPacket, double rxSnr, SlimTxVector txVector){
	++n_packets_success;
	return;
}

void RxFromPhyError (Ptr<const Packet> packet, double rxSnr){
	++n_packets_error;
	return;
}

int main (int argc, char *argv[])
{
	uint32_t numPackets = 1000;
	double rxPowerDbm = -100;
	Config::SetDefault ("ns3::TuscaroraSlimPhy::EnergyDetectionThreshold", DoubleValue (-500.0));
	Config::SetDefault ("ns3::SlimMode::Duration",  TimeValue (MicroSeconds (12000)));

	CommandLine cmd;
	cmd.AddValue ("numPackets", "Number of packet to be simulated", numPackets);
	cmd.AddValue ("rxPowerDbm", "The received power to be simulated", rxPowerDbm);
	cmd.Parse (argc,argv);

	printf("Setting up simulation with numPackets = %d\n", numPackets);

	n_packets_created = 0;
	n_packets_success = 0;
	n_packets_error = 0;
	SlimTxVector txvector = CreateSlimTxVector();


	//-- Create PHY


	Ptr<TuscaroraSlimPhy> phy = CreateObject<TuscaroraSlimPhy> ();
	phy->SetAntenna(CreateObject<IsotropicAntennaModel> ());
	phy->GetTxVectorList().push_back(txvector);


	phy->SetReceiveOkCallback (MakeCallback (&RxFromPhyOK));
	phy->SetReceiveErrorCallback (MakeCallback (&RxFromPhyError));



	  //------------------------------------------------------------
	  //-- Applications
	  //--------------------------------------------

	for (uint32_t i=0; i<numPackets ; ++i){
		Simulator::Schedule (Seconds(i+1), &SchedulePacketReception,phy,rxPowerDbm, txvector) ;
	}

	Simulator::Schedule (Seconds(numPackets+2), &ComparePacketReceptionwithSINR,phy,rxPowerDbm, txvector) ;

	  //------------------------------------------------------------
	  //-- Simulation
	  //--------------------------------------------
	Simulator::Stop (Seconds (numPackets+3));
	printf("Starting simulation\n");
	Simulator::Run ();
	Simulator::Destroy ();
	return 0;
}




