/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/* Copyright: The Samraksh Company
 * SLIM-WF:Synchronous Lightweight Interfernce-aware Model-driven Wavefrm
 * The usage of this module is governed by the associated license file.
 * Authors: Bora Karaoglu, Mukundan Sridharan
 */

#ifndef TUSCARORA_SLIM_PHY_H
#define TUSCARORA_SLIM_PHY_H

#include <stdint.h>
#include "slim-mac-header.h"
#include "ns3/callback.h"
#include "ns3/event-id.h"
#include "ns3/packet.h"
#include "ns3/object.h"
#include "ns3/traced-callback.h"
#include "ns3/nstime.h"
#include "ns3/ptr.h"
#include "ns3/random-variable-stream.h"
#include "slim-phy.h"
#include "slim-mode.h"
#include "ns3/isotropic-antenna-model.h"


#include "slim-tx-vector.h"
#include "slim-interference-helper.h"

#include "tuscarora-slim-channel.h"


namespace ns3 {

#define HT_PHY 127

//class TuscaroraSlimChannel;
class SlimPhyStateHelper;


class TuscaroraSlimPhy : public SlimPhy
{
public:
  typedef std::vector <SlimTxVector> ListofSlimTxVectors;

  static TypeId GetTypeId (void);

  TuscaroraSlimPhy ();
  virtual ~TuscaroraSlimPhy ();
;
  /**
   * Set the SlimChannel this TuscaroraSlimPhy is to be connected to.
   *
   * \param channel the SlimChannel this TuscaroraSlimPhy is to be connected to
   */
  void SetChannel (Ptr<TuscaroraSlimChannel> channel);

  /**
   * Set the current channel number.
   *
   * \param id the channel number
   */
  virtual void SetChannelNumber (uint16_t id);
  /**
   * Return the current channel number.
   *
   * \return the current channel number
   */
  virtual uint16_t GetChannelNumber (void) const;
  /**
   * \return the required time for channel switch operation of this SlimPhy
   */
  virtual Time GetChannelSwitchDelay (void) const;

  /**
   * Starting receiving the packet (i.e. the first bit of the preamble has arrived).
   *
   * \param packet the arriving packet
   * \param rxPowerDbm the receive power in dBm
   * \param txVector the TXVECTOR of the arriving packet
   * \param packetType The type of the received packet (values: 0 not an A-MPDU, 1 corresponds to any packets in an A-MPDU except the last one, 2 is the last packet in an A-MPDU) 
   * \param rxDuration the duration needed for the reception of the arriving packet
   */
  void StartReceivePacket (Ptr<Packet> packet,
                           double rxPowerDbm,
                           SlimTxVector txVector,
                           uint8_t packetType,
                           Time rxDuration);

  /**
   * Sets the RX loss (dB) in the Signal-to-Noise-Ratio due to non-idealities in the receiver.
   *
   * \param noiseFigureDb noise figure in dB
   */
  void SetRxNoiseFigure (double noiseFigureDb);
  /**
   * Sets the minimum available transmission power level (dBm).
   *
   * \param start the minimum transmission power level (dBm)
   */
  void SetTxPowerStart (double start);
  /**
   * Sets the maximum available transmission power level (dBm).
   *
   * \param end the maximum transmission power level (dBm)
   */
  void SetTxPowerEnd (double end);
  /**
   * Sets the number of transmission power levels available between the
   * minimum level and the maximum level.  Transmission power levels are
   * equally separated (in dBm) with the minimum and the maximum included.
   *
   * \param n the number of available levels
   */
  void SetNTxPower (uint32_t n);
  /**
   * Sets the transmission gain (dB).
   *
   * \param gain the transmission gain in dB
   */
  void SetTxGain (double gain);
  /**
   * Sets the reception gain (dB).
   *
   * \param gain the reception gain in dB
   */
  void SetRxGain (double gain);
  /**
   * Sets the energy detection threshold (dBm).
   * The energy of a received signal should be higher than
   * this threshold (dbm) to allow the PHY layer to detect the signal.
   *
   * \param threshold the energy detction threshold in dBm
   */
  void SetEdThreshold (double threshold);

  /**
   * Sets the device this PHY is associated with.
   *
   * \param device the device this PHY is associated with
   */
  void SetDevice (Ptr<Object> device);
  /**
   * Sets the mobility model.
   *
   * \param mobility the mobility model this PHY is associated with
   */
  void SetMobility (Ptr<Object> mobility);
  void SetAntenna (Ptr<AntennaModel> antenna);
  TuscaroraSlimPhy::ListofSlimTxVectors& GetTxVectorList();
  /**
   * Return the RX noise figure (dBm).
   *
   * \return the RX noise figure in dBm
   */
  double GetRxNoiseFigure (void) const;
  /**
   * Return the transmission gain (dB).
   *
   * \return the transmission gain in dB
   */
  double GetTxGain (void) const;
  /**
   * Return the reception gain (dB).
   *
   * \return the reception gain in dB
   */
  double GetRxGain (void) const;
  /**
   * Return the energy detection threshold (dBm).
   *
   * \return the energy detection threshold in dBm
   */
  double GetEdThreshold (void) const;

  /**
   * Return the device this PHY is associated with
   *
   * \return the device this PHY is associated with
   */
  Ptr<Object> GetDevice (void) const;
  /**
   * Return the mobility model this PHY is associated with.
   *
   * \return the mobility model this PHY is associated with
   */
  Ptr<Object> GetMobility (void);
  Ptr<AntennaModel> GetAntenna (void);

  /**
   * Sets the list of tx modes supported by the PHY
   *
   */
  //void SetDeviceRateSet(const SlimModeList& list);
  /**
   * Returns the list of tx modes supported by the PHY
   *
   */
  //SlimModeList& GetDeviceRateSet();

  /**
   * Return the minimum available transmission power level (dBm).
   * \return the minimum available transmission power level (dBm)
   */
  virtual double GetTxPowerStart (void) const;
  /**
   * Return the maximum available transmission power level (dBm).
   * \return the maximum available transmission power level (dBm)
   */
  virtual double GetTxPowerEnd (void) const;
  /**
   * Return the number of available transmission power levels.
   *
   * \return the number of available transmission power levels
   */
  virtual uint32_t GetNTxPower (void) const;
  virtual void SetReceiveOkCallback (SlimPhy::RxOkCallback callback);
  virtual void SetReceiveErrorCallback (SlimPhy::RxErrorCallback callback);
  virtual void SendPacket (Ptr<const Packet> packet, SlimTxVector txvector, uint8_t packetType);
  virtual void RegisterListener (SlimPhyListener *listener);
  virtual void UnregisterListener (SlimPhyListener *listener);
  virtual void SetSleepMode (void);
  virtual void ResumeFromSleep (void);
  virtual bool IsStateCcaBusy (void);
  virtual bool IsStateIdle (void);
  virtual bool IsStateBusy (void);
  virtual bool IsStateRx (void);
  virtual bool IsStateTx (void);
  virtual bool IsStateSwitching (void);
  virtual bool IsStateSleep (void);
  virtual Time GetStateDuration (void);
  virtual Time GetDelayUntilIdle (void);
  virtual Time GetLastRxStartTime (void) const;
  //virtual uint32_t GetNModes (void) const;
  //virtual SlimMode GetMode (uint32_t mode) const;
  SlimTxVector GetTxVector(uint32_t txmodenum);


  virtual bool IsModeSupported (SlimTxVector txvector);
  /*virtual bool IsMcsSupported (SlimMode mode);*/
  virtual double CalculateSnr (SlimMode txMode, double ber) const;
  virtual Ptr<SlimChannel> GetChannel (void) const;
  
 /**
  * Assign a fixed random variable stream number to the random variables
  * used by this model.  Return the number of streams (possibly zero) that
  * have been assigned.
  *
  * \param stream first stream index to use
  * \return the number of stream indices assigned by this model
  */
  virtual int64_t AssignStreams (int64_t stream);

  /**
   * \param freq the operating frequency on this node (2.4 GHz or 5GHz).
   */
  virtual void SetFrequency (uint32_t freq);
  /**
   * \return the operating frequency on this node
   */
  virtual uint32_t GetFrequency (void) const;


  //virtual Time CalculateTxDuration (uint32_t size, SlimTxVector txvector, double frequency, uint8_t packetType, uint8_t incFlag);


private:
  double powerdbm;
  //TuscaroraSlimPhy (const TuscaroraSlimPhy &o);
  virtual void DoDispose (void);
  /**
   * Return the energy detection threshold.
   *
   * \return the energy detection threshold.
   */
  double GetEdThresholdW (void) const;
  /**
   * Convert from dBm to Watts.
   *
   * \param dbm the power in dBm
   * \return the equivalent Watts for the given dBm
   */
  double DbmToW (double dbm) const;
  /**
   * Convert from dB to ratio.
   *
   * \param db
   * \return ratio
   */
  double DbToRatio (double db) const;
  /**
   * Convert from Watts to dBm.
   *
   * \param w the power in Watts
   * \return the equivalent dBm for the given Watts
   */
  double WToDbm (double w) const;
  /**
   * Convert from ratio to dB.
   *
   * \param ratio
   * \return dB
   */
  double RatioToDb (double ratio) const;
  /**
   * Get the power of the given power level in dBm.
   * In TuscaroraSlimPhy implementation, the power levels are equally spaced (in dBm).
   *
   * \param power the power level
   * \return the transmission power in dBm at the given power level
   */
  double GetPowerDbm (uint8_t power) const;
  /**
   * The last bit of the packet has arrived.
   *
   * \param packet the packet that the last bit has arrived
   * \param event the corresponding event of the first time the packet arrives
   */
  void EndReceive (Ptr<Packet> packet, Ptr<SlimInterferenceHelper::Event> event);

private:
  virtual void DoInitialize (void);

  bool     m_initialized;         //!< Flag for runtime initialization
  double   m_edThresholdW;        //!< Energy detection threshold in watts
  double   m_ccaMode1ThresholdW;  //!< Clear channel assessment (CCA) threshold in watts
  double   m_txGainDb;            //!< Transmission gain (dB)
  double   m_rxGainDb;            //!< Reception gain (dB)
  double   m_txPowerBaseDbm;      //!< Minimum transmission power (dBm)
  double   m_txPowerEndDbm;       //!< Maximum transmission power (dBm)
  uint32_t m_nTxPower;            //!< Number of available transmission power levels

  Ptr<TuscaroraSlimChannel> m_channel;        //!< SlimChannel that this TuscaroraSlimPhy is connected to
  uint16_t             m_channelNumber;  //!< Operating channel number
  Ptr<Object>          m_device;         //!< Pointer to the device
  Ptr<Object>          m_mobility;       //!< Pointer to the mobility model
  Ptr<AntennaModel>          m_antenna;       //!< Pointer to the antenna model


  /**
   * This vector holds the set of transmission modes that this
   * TuscaroraSlimPhy(-derived class) can support. In conversation we call this
   * the DeviceRateSet (not a term you'll find in the standard), and
   * it is a superset of standard-defined parameters such as the
   * OperationalRateSet, and the BSSBasicRateSet (which, themselves,
   * have a superset/subset relationship).
   *
   * Mandatory rates relevant to this TuscaroraSlimPhy can be found by
   * iterating over this vector looking for SlimMode objects for which
   * SlimMode::IsMandatory() is true.
   *
   * A quick note is appropriate here (well, here is as good a place
   * as any I can find)...
   *
   * In the standard there is no text that explicitly precludes
   * production of a device that does not support some rates that are
   * mandatory (according to the standard) for PHYs that the device
   * happens to fully or partially support.
   *
   * This approach is taken by some devices which choose to only support,
   * for example, 6 and 9 Mbps ERP-OFDM rates for cost and power
   * consumption reasons (i.e., these devices don't need to be designed
   * for and waste current on the increased linearity requirement of
   * higher-order constellations when 6 and 9 Mbps more than meet their
   * data requirements). The wording of the standard allows such devices
   * to have an OperationalRateSet which includes 6 and 9 Mbps ERP-OFDM
   * rates, despite 12 and 24 Mbps being "mandatory" rates for the
   * ERP-OFDM PHY.
   *
   * Now this doesn't actually have any impact on code, yet. It is,
   * however, something that we need to keep in mind for the
   * future. Basically, the key point is that we can't be making
   * assumptions like "the Operational Rate Set will contain all the
   * mandatory rates".
   */
  //SlimModeList m_deviceRateSet;
  ListofSlimTxVectors m_listofSlimTxVectors;

  std::vector<uint32_t> m_bssMembershipSelectorSet;
  //std::vector<uint8_t> m_deviceMcsSet;
  EventId m_endRxEvent;

  Ptr<UniformRandomVariable> m_random;  //!< Provides uniform random variables.
  double m_channelStartingFrequency;    //!< Standard-dependent center frequency of 0-th channel in MHz
  Ptr<SlimPhyStateHelper> m_state;      //!< Pointer to SlimPhyStateHelper
  SlimInterferenceHelper m_interference;    //!< Pointer to SlimInterferenceHelper
  Time m_channelSwitchDelay;            //!< Time required to switch between channel
  uint16_t m_mpdusNum;                  //!< carries the number of expected mpdus that are part of an A-MPDU
  //Ptr <SlimRateDefinitions> m_ratedefs;
};

} // namespace ns3


#endif /* YANS_Slim_PHY_H */
