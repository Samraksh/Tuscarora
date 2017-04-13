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

Ptr<TuscaroraSlimPhy> phy;


//This creates a tx vector closely resembling 802.11 DSSS 1Mbps rate tx with long preamble
SlimTxVector CreateSlimTxVector(Time totalduration){
	Ptr<SlimTxVector> txvector_ptr = CreateObject<SlimTxVector> ();
	txvector_ptr->SetUniqueName("Tuscarora1MbpsMode");

	Ptr<SlimMode> slimpaylodptr = CreateObject<SlimMode> ();
	slimpaylodptr->SetUniqueName("TuscaroraPayloadMode");
	slimpaylodptr->SetDuration(totalduration);
	txvector_ptr->GetListofModes().push_back(*slimpaylodptr);

	txvector_ptr->SetPayloadModePosition(0);


	return *txvector_ptr;

}


NS_LOG_COMPONENT_DEFINE ("SlimWaveformExample");


double CalculateSnr (double signal, double noiseInterference, uint32_t BW)
{
	// thermal noise at 290K in J/s = W
			static const double BOLTZMANN = 1.3803e-23;
	// Nt is the power of thermal noise in W
	double Nt = BOLTZMANN * 290.0 * BW;
	// receiver noise Floor (W) which accounts for thermal noise and non-idealities of the receiver

	double m_noiseFigure = std::pow (10.0, phy->GetRxNoiseFigure() / 10.0);

	double noiseFloor = m_noiseFigure * Nt;
	double noise = noiseFloor + noiseInterference;
	double snr = signal / noise;
	return snr;
}


void SchedulePacketReception (double rxPowerDbm, SlimTxVector txvector, bool ispacket = 0)
{
//	SlimTxVector txvector = CreateSlimTxVector();

	if (ispacket) ++n_packets_created;
	phy -> StartReceivePacket (Create<Packet> (),
									 rxPowerDbm,
									 txvector,
	                                 0, phy->CalculateTxDuration(txvector));

}


void ComparePacketReceptionwithSINR(
		double rxPowerDbm,
		double intfPowerDbm,
		SlimTxVector txvector,
		Time interference_duration,
		Time inter_interference_duration){
	double rxPowerW, intfPowerW, snr1, ber1, snr2, ber2, psr;
	printf("Starting comparing simulation results with the theoretical values. \n");

	PointerValue tmp;
	txvector.GetMode().GetAttribute("ErrorModPtr",tmp);
	Ptr<Object> error3 =  tmp.GetObject();
	Ptr<SlimModeError> error2 = error3->GetObject<SlimModeError>();



	double expected_n_packet_success;
	double theoretical_6_sigma;

	if(rxPowerDbm >= phy->GetEdThreshold()) {
		rxPowerW = std::pow (10.0, rxPowerDbm / 10.0)/1000;
		intfPowerW = std::pow (10.0, intfPowerDbm / 10.0)/1000;
		snr1 = CalculateSnr(rxPowerW,0,txvector.GetMode().GetBandwidth());
		ber1 = error2->GetBER(snr1);
		snr2 = CalculateSnr(rxPowerW,intfPowerW,txvector.GetMode().GetBandwidth());
		ber2 = error2->GetBER(snr2);



		uint32_t numbits = (txvector.CalculateTxDuration().GetNanoSeconds()/txvector.GetMode().GetDataRate().GetNanoSeconds());
		uint32_t Intfnumbits = 0;
		uint32_t nonIntfnumbits = 0;
		for (Time int_start_time = inter_interference_duration; int_start_time < txvector.CalculateTxDuration(); int_start_time += interference_duration+inter_interference_duration){
			Intfnumbits += (interference_duration.GetNanoSeconds()/txvector.GetMode().GetDataRate().GetNanoSeconds());
		}
		nonIntfnumbits = numbits - Intfnumbits;

		uint32_t num_error_tolerated = txvector.GetMode().GetCodeRate() * numbits;


		printf("Fraction of Correctable bits = %f \n", txvector.GetMode().GetCodeRate());

		psr = 0;

		for(uint64_t i = 0; i <= num_error_tolerated; ++i ){
			for(uint64_t j = 0; j <= num_error_tolerated - i; ++j ){
				psr += Binomial(i, ber1, nonIntfnumbits) * Binomial(j, ber2, Intfnumbits);
				assert(psr < 1);
			}
		}
		expected_n_packet_success = double(n_packets_created) * psr;
		theoretical_6_sigma = double(6) * double(std::sqrt(double(n_packets_created)*psr*(1-psr)));
	}
	else{
		expected_n_packet_success = 0;
		theoretical_6_sigma = 0;
	}



	double absolute_diff = (expected_n_packet_success > n_packets_success ? expected_n_packet_success - n_packets_success : n_packets_success - expected_n_packet_success );

	printf("rxPowerDbm = %2.2f, n_packets_created = %jd, n_packets_success = %jd, expected_n_packet_success = %f, absolute_diff %f, theoretical_6_sigma = %f, Percent Error = %.6f \n"
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
	n_packets_created = 0;
	n_packets_success = 0;
	n_packets_error = 0;
	Config::SetDefault ("ns3::TuscaroraSlimPhy::EnergyDetectionThreshold", DoubleValue (-500.0));

	uint32_t numPackets = 1000;
	double rxPowerDbm = -104;
	Time packetDuration = MicroSeconds(2000);

	double intfPowerDbm = -110;
	Time interference_duration = MicroSeconds(2);
	Time inter_interference_duration = MicroSeconds(2);

	CommandLine cmd;
	cmd.AddValue ("numPackets", "Number of packet to be simulated", numPackets);
	cmd.AddValue ("rxPowerDbm", "The received power to be simulated", rxPowerDbm);
	cmd.AddValue ("packetDuration", "Length of packet in microseconds", packetDuration);

	cmd.AddValue ("intfPowerDbm", "The received power to be simulated", intfPowerDbm);
	cmd.AddValue ("interference_duration", "The duration of each interfering section", interference_duration);
	cmd.AddValue ("inter_interference_duration", "The duration of each interfering section", inter_interference_duration);
	cmd.Parse (argc,argv);


	SlimTxVector txvector = CreateSlimTxVector(packetDuration);
	SlimTxVector txvector2 = CreateSlimTxVector(interference_duration);


	//-- Create PHY
	printf("Setting up simulation with numPackets = %d\n", numPackets);

	phy = CreateObject<TuscaroraSlimPhy> ();
	phy->SetAntenna(CreateObject<IsotropicAntennaModel> ());
	phy->GetTxVectorList().push_back(txvector);
	phy->GetTxVectorList().push_back(txvector2);


	phy->SetReceiveOkCallback (MakeCallback (&RxFromPhyOK));
	phy->SetReceiveErrorCallback (MakeCallback (&RxFromPhyError));



	  //------------------------------------------------------------
	  //-- Applications
	  //--------------------------------------------
	Time packetStartTime,packetEndTime,inter;
	for (uint32_t i=0; i<numPackets; ++i){
		//Schedule main packet
		packetStartTime = Seconds(1)+ (packetDuration + Seconds(1)) * i;
		packetEndTime = packetStartTime + packetDuration;
		Simulator::Schedule (packetStartTime, &SchedulePacketReception,rxPowerDbm, txvector, 1 ) ;
		//Schedule interferers
		for (Time int_start_time = packetStartTime + inter_interference_duration; int_start_time < packetEndTime; int_start_time = int_start_time + interference_duration+inter_interference_duration){
			Simulator::Schedule (int_start_time , &SchedulePacketReception,intfPowerDbm, txvector2, 0 ) ;
		}
	}

	Simulator::Schedule (Seconds(2)+ (packetDuration + Seconds(1)) * numPackets , &ComparePacketReceptionwithSINR,
			rxPowerDbm, intfPowerDbm, txvector, interference_duration, inter_interference_duration  ) ;
	  //------------------------------------------------------------
	  //-- Simulation
	  //--------------------------------------------
	Simulator::Stop (Seconds(3)+ (packetDuration + Seconds(1)) * numPackets);
	printf("Starting simulation\n");
	Simulator::Run ();
	Simulator::Destroy ();
	return 0;
}




