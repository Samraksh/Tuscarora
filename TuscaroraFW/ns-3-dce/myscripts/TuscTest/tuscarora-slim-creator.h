////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include "ns3/slim-module.h"

//This creates a tx vector closely resembling 802.11 DSSS 1Mbps rate tx with long preamble
SlimTxVector CreateSlimTxVector(){
	SlimTxVector txvector, txvector2;
	txvector.SetUniqueName("Tuscarora1MbpsMode");


	SlimMode slimheader, slimpaylod;
	slimheader.SetUniqueName("TuscaroraHeaderMode");
	slimheader.SetBandwidth(22000000);
	//slimheader.SetDataRate(MicroSeconds(1)); // Set time to transmit one bit of information
	//slimheader.SetPhyRate(1000000);
	slimheader.SetCodeRate(0.01);
	//slimheader.SetSlimModeError(new TuscaroraDsssDbpskModeError()); //BK: Potentially dangereous. Carefull //ToDO: Think about a safer method if time permits.
	slimheader.SetDuration(MicroSeconds(144));
	txvector.GetListofModes().push_back(slimheader);

	slimpaylod = slimheader;
	slimpaylod.SetDuration(MicroSeconds(1));//This is Irrelevant since the packet payload size will be determined at the time of transmission
	slimheader.SetUniqueName("TuscaroraPayloadMode");
	txvector.GetListofModes().push_back(slimpaylod);

	txvector.SetPayloadModePosition(1);

	txvector2 = txvector;

	return txvector;

}

Ptr<TuscaroraSlimPhy> CreateTuscaroraSlimPhy(
		Ptr<TuscaroraSlimChannel> channel
		, Ptr<MobilityModel> mobilitymodel
		, Ptr<AntennaModel>  antenna
		, uint16_t channelnumber
	){
	Ptr<TuscaroraSlimPhy> phy = CreateObject<TuscaroraSlimPhy> ();

	NS_ASSERT(mobilitymodel);
	phy->SetMobility(mobilitymodel);
	phy->SetAntenna(antenna);

	phy->SetChannel (channel);
	phy->SetChannelNumber(channelnumber);

	phy->GetTxVectorList().push_back(CreateSlimTxVector());

	return phy;
}

Ptr<TuscaroraSlimChannel> CreateTuscaroraSlimChannel(){
	Ptr<TuscaroraSlimChannel> channel = CreateObject<TuscaroraSlimChannel> ();
	Ptr<PropagationLossModel> prop_loss =  CreateObject<FriisPropagationLossModel> ();
	Ptr<PropagationDelayModel>  prop_delay = CreateObject<ConstantSpeedPropagationDelayModel> ();
	channel->SetPropagationLossModel(prop_loss);
	channel->SetPropagationDelayModel (prop_delay);
	return channel;
}
