/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/* Copyright: The Samraksh Company
 * SLIM-WF:Synchronous Lightweight Interfernce-aware Model-driven Wavefrm
 * The usage of this module is governed by the associated license file.
 * Authors: Bora Karaoglu, Mukundan Sridharan
 */

#include "tuscarora-slim-phy.h"
#include "slim-channel.h"
#include "slim-mode.h"
#include "slim-phy-state-helper.h"
#include "ns3/simulator.h"
#include "ns3/packet.h"
#include "ns3/assert.h"
#include "ns3/log.h"
#include "ns3/double.h"
#include "ns3/uinteger.h"
#include "ns3/enum.h"
#include "ns3/pointer.h"
#include "ns3/net-device.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/boolean.h"
#include "ns3/ampdu-tag.h"

#include <cmath>


namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("TuscaroraSlimPhy");


TypeId
TuscaroraSlimPhy::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::TuscaroraSlimPhy")
    .SetParent<SlimPhy> ()
    .AddConstructor<TuscaroraSlimPhy> ()
    .AddAttribute ("EnergyDetectionThreshold",
                   "The energy of a received signal should be higher than "
                   "this threshold (dbm) to allow the PHY layer to detect the signal and synchronize with the packet.",
                   DoubleValue (-96.0),
                   MakeDoubleAccessor (&TuscaroraSlimPhy::SetEdThreshold,
                                       &TuscaroraSlimPhy::GetEdThreshold),
                   MakeDoubleChecker<double> ())
//    .AddAttribute ("CcaMode1Threshold",
//                   "The energy of a received signal should be higher than "
//                   "this threshold (dbm) to allow the PHY layer to declare CCA BUSY state",
//                   DoubleValue (-99.0),
//                   MakeDoubleAccessor (&TuscaroraSlimPhy::SetCcaMode1Threshold,
//                                       &TuscaroraSlimPhy::GetCcaMode1Threshold),
//                   MakeDoubleChecker<double> ())
    .AddAttribute ("TxGain",
                   "Transmission gain (dB).",
                   DoubleValue (0.0),
                   MakeDoubleAccessor (&TuscaroraSlimPhy::SetTxGain,
                                       &TuscaroraSlimPhy::GetTxGain),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("RxGain",
                   "Reception gain (dB).",
                   DoubleValue (0.0),
                   MakeDoubleAccessor (&TuscaroraSlimPhy::SetRxGain,
                                       &TuscaroraSlimPhy::GetRxGain),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("TxPowerLevels",
                   "Number of transmission power levels available between "
                   "TxPowerStart and TxPowerEnd included.",
                   UintegerValue (1),
                   MakeUintegerAccessor (&TuscaroraSlimPhy::m_nTxPower),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("TxPowerEnd",
                   "Maximum available transmission level (dbm).",
                   DoubleValue (16.0206),
                   MakeDoubleAccessor (&TuscaroraSlimPhy::SetTxPowerEnd,
                                       &TuscaroraSlimPhy::GetTxPowerEnd),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("TxPowerStart",
                   "Minimum available transmission level (dbm).",
                   DoubleValue (16.0206),
                   MakeDoubleAccessor (&TuscaroraSlimPhy::SetTxPowerStart,
                                       &TuscaroraSlimPhy::GetTxPowerStart),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("RxNoiseFigure",
                   "Loss (dB) in the Signal-to-Noise-Ratio due to non-idealities in the receiver."
                   " According to Wikipedia (http://en.wikipedia.org/wiki/Noise_figure), this is "
                   "\"the difference in decibels (dB) between"
                   " the noise output of the actual receiver to the noise output of an "
                   " ideal receiver with the same overall gain and bandwidth when the receivers "
                   " are connected to sources at the standard noise temperature T0 (usually 290 K)\".",
                   DoubleValue (0),
                   MakeDoubleAccessor (&TuscaroraSlimPhy::SetRxNoiseFigure,
                                       &TuscaroraSlimPhy::GetRxNoiseFigure),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("StateHelperPtr", "The pointer to the helper class object that keeps the state of the phy layer",
                   PointerValue (CreateObject<SlimPhyStateHelper> ()),
                   MakePointerAccessor (&TuscaroraSlimPhy::m_state),
                   MakePointerChecker<SlimPhyStateHelper> ())
    .AddAttribute ("ChannelSwitchDelay",
                   "Delay between two short frames transmitted on different frequencies.",
                   TimeValue (MicroSeconds (250)),
                   MakeTimeAccessor (&TuscaroraSlimPhy::m_channelSwitchDelay),
                   MakeTimeChecker ())
    .AddAttribute ("ChannelNumber",
                   "Channel center frequency = Channel starting frequency + 5 MHz * nch",
                   UintegerValue (1),
                   MakeUintegerAccessor (&TuscaroraSlimPhy::SetChannelNumber,
                                         &TuscaroraSlimPhy::GetChannelNumber),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("Frequency", "The central operating frequency.",
                   UintegerValue (2407),
                   MakeUintegerAccessor (&TuscaroraSlimPhy::GetFrequency,
                                        &TuscaroraSlimPhy::SetFrequency),
                   MakeUintegerChecker<uint32_t> ())
	.AddAttribute ("AntennaPtr",
				   "The pointer pointing to the associated antenna object",
				   PointerValue (CreateObject<IsotropicAntennaModel> ()),
				   MakePointerAccessor (&TuscaroraSlimPhy::m_antenna),
				   MakePointerChecker<AntennaModel> ())
	//.AddAttribute ("RateDefinitions",
	//			   "The objects that hold rates supported",
	//			   PointerValue (),
	//			   MakePointerAccessor (&TuscaroraSlimPhy::m_ratedefs),
	//			   MakePointerChecker<SlimRateDefinitions> ())


  ;
  return tid;
}

NS_OBJECT_ENSURE_REGISTERED (TuscaroraSlimPhy);

TuscaroraSlimPhy::TuscaroraSlimPhy ()
  :  m_initialized (false),
    m_channelNumber (1),
    m_endRxEvent (),
    m_mpdusNum(0)
{
  NS_LOG_FUNCTION (this);
  m_random = CreateObject<UniformRandomVariable> ();
 // m_ratedefs = CreateObject<SlimRateDefinitions> ();


}

TuscaroraSlimPhy::~TuscaroraSlimPhy ()
{
  NS_LOG_FUNCTION (this);
}

void
TuscaroraSlimPhy::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  m_channel = 0;
  //m_deviceRateSet.clear ();
 // m_deviceMcsSet.clear();
  m_device = 0;
  m_mobility = 0;
  m_state = 0;
}

void
TuscaroraSlimPhy::DoInitialize ()
{
  NS_LOG_FUNCTION (this);
  m_initialized = true;
}



void
TuscaroraSlimPhy::SetRxNoiseFigure (double noiseFigureDb)
{
  NS_LOG_FUNCTION (this << noiseFigureDb);
  m_interference.SetNoiseFigure (DbToRatio (noiseFigureDb));
}
void
TuscaroraSlimPhy::SetTxPowerStart (double start)
{
  NS_LOG_FUNCTION (this << start);
  m_txPowerBaseDbm = start;
}
void
TuscaroraSlimPhy::SetTxPowerEnd (double end)
{
  NS_LOG_FUNCTION (this << end);
  m_txPowerEndDbm = end;
}
void
TuscaroraSlimPhy::SetNTxPower (uint32_t n)
{
  NS_LOG_FUNCTION (this << n);
  m_nTxPower = n;
}
void
TuscaroraSlimPhy::SetTxGain (double gain)
{
  NS_LOG_FUNCTION (this << gain);
  m_txGainDb = gain;
}
void
TuscaroraSlimPhy::SetRxGain (double gain)
{
  NS_LOG_FUNCTION (this << gain);
  m_rxGainDb = gain;
}
void
TuscaroraSlimPhy::SetEdThreshold (double threshold)
{
  NS_LOG_FUNCTION (this << threshold);
  m_edThresholdW = DbmToW (threshold);
}

//void
//TuscaroraSlimPhy::SetErrorRateModel (Ptr<SlimErrorRateModel> rate)
//{
//  m_interference.SetErrorRateModel (rate);
//}
void
TuscaroraSlimPhy::SetDevice (Ptr<Object> device)
{
  m_device = device;
}
void
TuscaroraSlimPhy::SetMobility (Ptr<Object> mobility)
{
  m_mobility = mobility;
}
void
TuscaroraSlimPhy::SetAntenna (Ptr<AntennaModel> antenna)
{
  m_antenna = antenna;
}

double
TuscaroraSlimPhy::GetRxNoiseFigure (void) const
{
  return RatioToDb (m_interference.GetNoiseFigure ());
}
double
TuscaroraSlimPhy::GetTxPowerStart (void) const
{
  return m_txPowerBaseDbm;
}
double
TuscaroraSlimPhy::GetTxPowerEnd (void) const
{
  return m_txPowerEndDbm;
}
double
TuscaroraSlimPhy::GetTxGain (void) const
{
  return m_txGainDb;
}
double
TuscaroraSlimPhy::GetRxGain (void) const
{
  return m_rxGainDb;
}

double
TuscaroraSlimPhy::GetEdThreshold (void) const
{
  return WToDbm (m_edThresholdW);
}

//Ptr<SlimErrorRateModel>
//TuscaroraSlimPhy::GetErrorRateModel (void) const
//{
//  return m_interference.GetErrorRateModel ();
//}
Ptr<Object>
TuscaroraSlimPhy::GetDevice (void) const
{
  return m_device;
}
Ptr<Object>
TuscaroraSlimPhy::GetMobility (void)
{
  return m_mobility;
}
Ptr<AntennaModel>
TuscaroraSlimPhy::GetAntenna (void)
{
  return m_antenna;
}

double
TuscaroraSlimPhy::CalculateSnr (SlimMode txMode, double ber) const
{
  return 0.0;
}

Ptr<SlimChannel>
TuscaroraSlimPhy::GetChannel (void) const
{
  return m_channel;
}
void
TuscaroraSlimPhy::SetChannel (Ptr<TuscaroraSlimChannel> channel)
{
  m_channel = channel;
  m_channel->Add (this);
}

void
TuscaroraSlimPhy::SetChannelNumber (uint16_t nch)
{
  if (!m_initialized)
    {
      // this is not channel switch, this is initialization
      NS_LOG_DEBUG ("start at channel " << nch);
      m_channelNumber = nch;
      return;
    }

  NS_ASSERT (!IsStateSwitching ());
  switch (m_state->GetState ())
    {
    case TuscaroraSlimPhy::RX:
      NS_LOG_DEBUG ("drop packet because of channel switching while reception");
      m_endRxEvent.Cancel ();
      goto switchChannel;
      break;
    case TuscaroraSlimPhy::TX:
      NS_LOG_DEBUG ("channel switching postponed until end of current transmission");
      Simulator::Schedule (GetDelayUntilIdle (), &TuscaroraSlimPhy::SetChannelNumber, this, nch);
      break;
    case TuscaroraSlimPhy::CCA_BUSY:
    case TuscaroraSlimPhy::IDLE:
      goto switchChannel;
      break;
    case TuscaroraSlimPhy::SLEEP:
      NS_LOG_DEBUG ("channel switching ignored in sleep mode");
      break;
    default:
      NS_ASSERT (false);
      break;
    }

  return;

switchChannel:

  NS_LOG_DEBUG ("switching channel " << m_channelNumber << " -> " << nch);
  m_state->SwitchToChannelSwitching (m_channelSwitchDelay);
  m_interference.EraseEvents ();
  /*
   * Needed here to be able to correctly sensed the medium for the first
   * time after the switching. The actual switching is not performed until
   * after m_channelSwitchDelay. Packets received during the switching
   * state are added to the event list and are employed later to figure
   * out the state of the medium after the switching.
   */
  m_channelNumber = nch;
}

uint16_t
TuscaroraSlimPhy::GetChannelNumber (void) const
{
  return m_channelNumber;
}

Time
TuscaroraSlimPhy::GetChannelSwitchDelay (void) const
{
  return m_channelSwitchDelay;
}


void
TuscaroraSlimPhy::SetSleepMode (void)
{
  NS_LOG_FUNCTION (this);
  switch (m_state->GetState ())
    {
    case TuscaroraSlimPhy::TX:
      NS_LOG_DEBUG ("setting sleep mode postponed until end of current transmission");
      Simulator::Schedule (GetDelayUntilIdle (), &TuscaroraSlimPhy::SetSleepMode, this);
      break;
    case TuscaroraSlimPhy::RX:
      NS_LOG_DEBUG ("setting sleep mode postponed until end of current reception");
      Simulator::Schedule (GetDelayUntilIdle (), &TuscaroraSlimPhy::SetSleepMode, this);
      break;
    case TuscaroraSlimPhy::SWITCHING:
      NS_LOG_DEBUG ("setting sleep mode postponed until end of channel switching");
      Simulator::Schedule (GetDelayUntilIdle (), &TuscaroraSlimPhy::SetSleepMode, this);
      break;
    case TuscaroraSlimPhy::CCA_BUSY:
    case TuscaroraSlimPhy::IDLE:
      NS_LOG_DEBUG ("setting sleep mode");
      m_state->SwitchToSleep ();
      break;
    case TuscaroraSlimPhy::SLEEP:
      NS_LOG_DEBUG ("already in sleep mode");
      break;
    default:
      NS_ASSERT (false);
      break;
    }
}

void
TuscaroraSlimPhy::ResumeFromSleep (void)
{
  NS_LOG_FUNCTION (this);
  switch (m_state->GetState ())
    {
    case TuscaroraSlimPhy::TX:
    case TuscaroraSlimPhy::RX:
    case TuscaroraSlimPhy::IDLE:
    case TuscaroraSlimPhy::CCA_BUSY:
    case TuscaroraSlimPhy::SWITCHING:
      NS_LOG_DEBUG ("not in sleep mode, there is nothing to resume");
      break;
    case TuscaroraSlimPhy::SLEEP:
      NS_LOG_DEBUG ("resuming from sleep mode");
      //Time delayUntilCcaEnd = m_interference.GetEnergyDuration (m_ccaMode1ThresholdW); //
      Time delayUntilCcaEnd = MicroSeconds(0);
      m_state->SwitchFromSleep (delayUntilCcaEnd);
      break;
    }
}

void
TuscaroraSlimPhy::SetReceiveOkCallback (RxOkCallback callback)
{
  m_state->SetReceiveOkCallback (callback);
}
void
TuscaroraSlimPhy::SetReceiveErrorCallback (RxErrorCallback callback)
{
  m_state->SetReceiveErrorCallback (callback);
}
void
TuscaroraSlimPhy::StartReceivePacket (Ptr<Packet> packet,
                                 double rxPowerDbm,
                                 SlimTxVector txVector,
                                 uint8_t packetType, Time rxDuration)
{
  NS_LOG_FUNCTION (this << packet << rxPowerDbm << txVector.GetMode() << (uint32_t)packetType);
  AmpduTag ampduTag;
  rxPowerDbm += m_rxGainDb;
  powerdbm = rxPowerDbm;
  double rxPowerW = DbmToW (rxPowerDbm);
  SlimMode txMode = txVector.GetMode();
  Time endRx = Simulator::Now () + rxDuration;

  Ptr<SlimInterferenceHelper::Event> event;
  event = m_interference.Add (packet->GetSize (),
                              &txMode,
                              rxDuration,
                              rxPowerW,
		          txVector);  // we need it to calculate duration of HT training symbols

  switch (m_state->GetState ())
    {
    case TuscaroraSlimPhy::SWITCHING:
      NS_LOG_DEBUG ("drop packet because of channel switching");
      NotifyRxDrop (packet);
      /*
       * Packets received on the upcoming channel are added to the event list
       * during the switching state. This way the medium can be correctly sensed
       * when the device listens to the channel for the first time after the
       * switching e.g. after channel switching, the channel may be sensed as
       * busy due to other devices' tramissions started before the end of
       * the switching.
       */
      if (endRx > Simulator::Now () + m_state->GetDelayUntilIdle ())
        {
          // that packet will be noise _after_ the completion of the
          // channel switching.
          goto maybeCcaBusy;
        }
      break;
    case TuscaroraSlimPhy::RX:
      NS_LOG_DEBUG ("drop packet because already in Rx (power=" <<
                    rxPowerW << "W)");
      NotifyRxDrop (packet);
      if (endRx > Simulator::Now () + m_state->GetDelayUntilIdle ())
        {
          // that packet will be noise _after_ the reception of the
          // currently-received packet.
          goto maybeCcaBusy;
        }
      break;
    case TuscaroraSlimPhy::TX:
      NS_LOG_DEBUG ("drop packet because already in Tx (power=" <<
                    rxPowerW << "W)");
      NotifyRxDrop (packet);
      if (endRx > Simulator::Now () + m_state->GetDelayUntilIdle ())
        {
          // that packet will be noise _after_ the transmission of the
          // currently-transmitted packet.
          goto maybeCcaBusy;
        }
      break;
    case TuscaroraSlimPhy::CCA_BUSY:
    case TuscaroraSlimPhy::IDLE:
      if (rxPowerW > m_edThresholdW)
        {
          //if (IsModeSupported (txMode) || IsMcsSupported(txMode))
    	  if (IsModeSupported (txVector) ) {
              if (packet->PeekPacketTag (ampduTag) && m_mpdusNum == 0)
                {
                  //received the first MPDU in an MPDU
                  m_mpdusNum = ampduTag.GetNoOfMpdus()-1;
                }
              else if (packet->PeekPacketTag (ampduTag) && m_mpdusNum > 0)
                {
                  //received the other MPDUs that are part of the A-MPDU
                  if (ampduTag.GetNoOfMpdus() < m_mpdusNum)
                    {
                      NS_LOG_DEBUG ("Missing MPDU from the A-MPDU " << m_mpdusNum - ampduTag.GetNoOfMpdus());
                      m_mpdusNum = ampduTag.GetNoOfMpdus();
                    }
                  else
                      m_mpdusNum--;
                }
              else if (m_mpdusNum > 0 )
                {
                  NS_LOG_DEBUG ("Didn't receive the last MPDUs from an A-MPDU " << m_mpdusNum);
                  m_mpdusNum = 0;
                }
              NS_LOG_DEBUG ("sync to signal (power=" << rxPowerW << "W)");
              // sync to signal
              m_state->SwitchToRx (rxDuration);
              NS_ASSERT (m_endRxEvent.IsExpired ());
              NotifyRxBegin (packet);
              m_interference.NotifyRxStart ();
              m_endRxEvent = Simulator::Schedule (rxDuration, &TuscaroraSlimPhy::EndReceive, this,
                                                  packet,
                                                  event);
            }
          else {
              NS_LOG_DEBUG ("drop packet because it was sent using an unsupported mode (" << txMode << ")");
              NotifyRxDrop (packet);
              goto maybeCcaBusy;
            }
        }
      else
        {
          NS_LOG_DEBUG ("drop packet because signal power too Small (" <<
                        rxPowerW << "<" << m_edThresholdW << ")");
          NotifyRxDrop (packet);
          goto maybeCcaBusy;
        }
      break;
    case TuscaroraSlimPhy::SLEEP:
      NS_LOG_DEBUG ("drop packet because in sleep mode");
      NotifyRxDrop (packet);
      break;
    }

  return;

maybeCcaBusy:
  // We are here because we have received the first bit of a packet and we are
  // not going to be able to synchronize on it
  // In this model, CCA becomes busy when the aggregation of all signals as
  // tracked by the SlimInterferenceHelper class is higher than the CcaBusyThreshold

  Time delayUntilCcaEnd = MicroSeconds(0); //BK: Removed CCA mode   // = m_interference.GetEnergyDuration (m_ccaMode1ThresholdW);
  if (!delayUntilCcaEnd.IsZero ())
    {
      m_state->SwitchMaybeToCcaBusy (delayUntilCcaEnd);
    }
}

void
TuscaroraSlimPhy::SendPacket (Ptr<const Packet> packet, SlimTxVector txVector, uint8_t packetType)
{
  NS_LOG_FUNCTION (this << packet << txVector.GetMode()  << (uint32_t)txVector.GetTxPowerLevel() << (uint32_t)packetType);
  /* Transmission can happen if:
   *  - we are syncing on a packet. It is the responsability of the
   *    MAC layer to avoid doing this but the PHY does nothing to
   *    prevent it.
   *  - we are idle
   */
  NS_ASSERT (!m_state->IsStateTx () && !m_state->IsStateSwitching ());

  if (m_state->IsStateSleep ())
    {
      NS_LOG_DEBUG ("Dropping packet because in sleep mode");
      NotifyTxDrop (packet);
      return;
    }

  Time txDuration = CalculateTxDuration (txVector);
  if (m_state->IsStateRx ())
    {
      m_endRxEvent.Cancel ();
      m_interference.NotifyRxEnd ();
    }
  NotifyTxBegin (packet);
  //uint32_t dataRate500KbpsUnits;
  /*if (txVector.GetMode().GetModulationClass () == SLIM_MOD_CLASS_HT)
    {
      dataRate500KbpsUnits = 128 + m_ratedefs->SlimModeToMcs (txVector.GetMode());
    }
  else
    {
      dataRate500KbpsUnits = txVector.GetMode().GetDataRate () * txVector.GetNss() / 500000;
    }*/
  //dataRate500KbpsUnits = txVector.GetMode().GetDataRate () / 500000;
  //bool isShortPreamble = (SLIM_PREAMBLE_SHORT == preamble);
  NotifyMonitorSniffTx (packet, txVector, GetChannelNumber (), txVector.GetTxPowerLevel());
 // NotifyMonitorSniffTx (packet, (uint16_t)GetChannelFrequencyMhz (), GetChannelNumber (), dataRate500KbpsUnits, isShortPreamble, txVector.GetTxPowerLevel());
  m_state->SwitchToTx (txDuration, packet, GetPowerDbm (txVector.GetTxPowerLevel()), txVector);
  m_channel->Send (this, packet, GetPowerDbm (txVector.GetTxPowerLevel()) + m_txGainDb, txVector, packetType, txDuration);
}


SlimTxVector TuscaroraSlimPhy::GetTxVector(uint32_t txmodenum){
	return m_listofSlimTxVectors[txmodenum];
}

TuscaroraSlimPhy::ListofSlimTxVectors& TuscaroraSlimPhy::GetTxVectorList(){
	return m_listofSlimTxVectors;
}
//uint32_t
//TuscaroraSlimPhy::GetNModes (void) const
//{
//	return 0;
//  return m_deviceRateSet.size ();
//}
//SlimMode
//TuscaroraSlimPhy::GetMode (uint32_t mode) const
//{
//  return m_deviceRateSet[mode];
//}
bool
TuscaroraSlimPhy::IsModeSupported (SlimTxVector txvector)
{
//	for (SlimTxVector::ListofSlimModes::iterator it =  txvector.GetListofModes()->begin(); it != txvector.GetListofModes()->end(); ++it){
//		for (uint32_t i = 0; i < GetNModes (); ++i) {
//		  if ((*i) != GetMode (i)) {
//			  return false;
//			}
//		}
//	}
//  return true;
	for (ListofSlimTxVectors::iterator it =  m_listofSlimTxVectors.begin(); it != m_listofSlimTxVectors.end(); ++it){
		if(it->GetUniqueName() == txvector.GetUniqueName() ) {
			return true;
		}
	}
	return false;
}

/*bool
TuscaroraSlimPhy::IsMcsSupported (SlimMode mode)
{
  for (uint32_t i = 0; i < m_ratedefs->GetNMcs (); i++)
    {
      if (mode == m_ratedefs->McsToSlimMode(m_ratedefs->GetMcs (i)))
        {
          return true;
        }
    }
  return false;
}*/
uint32_t
TuscaroraSlimPhy::GetNTxPower (void) const
{
  return m_nTxPower;
}

//void TuscaroraSlimPhy::SetDeviceRateSet(const SlimModeList& list){
//	while(!m_deviceRateSet.empty()){
//		m_deviceRateSet.pop_back();
//	}
//	for(SlimModeList::const_iterator it = list.begin(); it != list.end(); ++it){
//		m_deviceRateSet.push_back(*it);
//	}
//}
//
//SlimModeList& TuscaroraSlimPhy::GetDeviceRateSet() {
//	return(m_deviceRateSet);
//}

void
TuscaroraSlimPhy::RegisterListener (SlimPhyListener *listener)
{
  m_state->RegisterListener (listener);
}

void
TuscaroraSlimPhy::UnregisterListener (SlimPhyListener *listener)
{
  m_state->UnregisterListener (listener);
}

bool
TuscaroraSlimPhy::IsStateCcaBusy (void)
{
  return m_state->IsStateCcaBusy ();
}

bool
TuscaroraSlimPhy::IsStateIdle (void)
{
  return m_state->IsStateIdle ();
}
bool
TuscaroraSlimPhy::IsStateBusy (void)
{
  return m_state->IsStateBusy ();
}
bool
TuscaroraSlimPhy::IsStateRx (void)
{
  return m_state->IsStateRx ();
}
bool
TuscaroraSlimPhy::IsStateTx (void)
{
  return m_state->IsStateTx ();
}
bool
TuscaroraSlimPhy::IsStateSwitching (void)
{
  return m_state->IsStateSwitching ();
}
bool
TuscaroraSlimPhy::IsStateSleep (void)
{
  return m_state->IsStateSleep ();
}

Time
TuscaroraSlimPhy::GetStateDuration (void)
{
  return m_state->GetStateDuration ();
}
Time
TuscaroraSlimPhy::GetDelayUntilIdle (void)
{
  return m_state->GetDelayUntilIdle ();
}

Time
TuscaroraSlimPhy::GetLastRxStartTime (void) const
{
  return m_state->GetLastRxStartTime ();
}

double
TuscaroraSlimPhy::DbToRatio (double dB) const
{
  double ratio = std::pow (10.0, dB / 10.0);
  return ratio;
}

double
TuscaroraSlimPhy::DbmToW (double dBm) const
{
  double mW = std::pow (10.0, dBm / 10.0);
  return mW / 1000.0;
}

double
TuscaroraSlimPhy::WToDbm (double w) const
{
  return 10.0 * std::log10 (w * 1000.0);
}

double
TuscaroraSlimPhy::RatioToDb (double ratio) const
{
  return 10.0 * std::log10 (ratio);
}

double
TuscaroraSlimPhy::GetEdThresholdW (void) const
{
  return m_edThresholdW;
}

double
TuscaroraSlimPhy::GetPowerDbm (uint8_t power) const
{
  NS_ASSERT (m_txPowerBaseDbm <= m_txPowerEndDbm);
  NS_ASSERT (m_nTxPower > 0);
  double dbm;
  if (m_nTxPower > 1)
    {
      dbm = m_txPowerBaseDbm + power * (m_txPowerEndDbm - m_txPowerBaseDbm) / (m_nTxPower - 1);
    }
  else
    {
      NS_ASSERT_MSG (m_txPowerBaseDbm == m_txPowerEndDbm, "cannot have TxPowerEnd != TxPowerStart with TxPowerLevels == 1");
      dbm = m_txPowerBaseDbm;
    }
  return dbm;
}

void
TuscaroraSlimPhy::EndReceive (Ptr<Packet> packet, Ptr<SlimInterferenceHelper::Event> event)
{
  NS_LOG_FUNCTION (this << packet << event);
  NS_ASSERT (IsStateRx ());
  NS_ASSERT (event->GetEndTime () == Simulator::Now ());

  struct SlimInterferenceHelper::SnrPer snrPer;
  snrPer = m_interference.CalculateSnrPer (event);
  m_interference.NotifyRxEnd ();

  NS_LOG_DEBUG ("mode=" << (event->GetPayloadMode ()->GetDataRate ()) <<
                ", snr=" << snrPer.snr << ", per=" << snrPer.per << ", size=" << packet->GetSize ());
  if (m_random->GetValue () > snrPer.per)
    {
      NotifyRxEnd (packet);
      //uint32_t dataRate500KbpsUnits;
      //if ((event->GetPayloadMode ().GetModulationClass () == SLIM_MOD_CLASS_HT))
      //  {
      //    dataRate500KbpsUnits = 128 + m_ratedefs->SlimModeToMcs (event->GetPayloadMode ());
      //  }
      //else
       // {
//          dataRate500KbpsUnits = event->GetPayloadMode ()->GetDataRate () / 500000;
       // }
     // bool isShortPreamble = (SLIM_PREAMBLE_SHORT == event->GetPreambleType ());
      double signalDbm = RatioToDb (event->GetRxPowerW ()) + 30;
      double noiseDbm = RatioToDb (event->GetRxPowerW () / snrPer.snr) - GetRxNoiseFigure () + 30;
      NotifyMonitorSniffRx (packet, event->GetTxVector(), GetChannelNumber (), signalDbm, noiseDbm);
      SlimDbmHeader hdr;
      hdr.SetData(powerdbm);
      packet->AddHeader(hdr);
      m_state->SwitchFromRxEndOk (packet, snrPer.snr, event->GetTxVector ());
    }
  else
    {
      /* failure. */
      NotifyRxDrop (packet);
      m_state->SwitchFromRxEndError (packet, snrPer.snr);
    }
}

int64_t
TuscaroraSlimPhy::AssignStreams (int64_t stream)
{
  NS_LOG_FUNCTION (this << stream);
  m_random->SetStream (stream);
  return 1;
}

void
TuscaroraSlimPhy::SetFrequency (uint32_t freq)
{
  m_channelStartingFrequency = freq;
}



uint32_t
TuscaroraSlimPhy::GetFrequency (void) const
{
  return m_channelStartingFrequency;
}









} // namespace ns3
