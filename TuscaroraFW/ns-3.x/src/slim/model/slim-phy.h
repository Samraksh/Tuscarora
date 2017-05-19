/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2005,2006 INRIA
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 */

#ifndef SLIM_PHY_H
#define SLIM_PHY_H

#include <stdint.h>
#include "ns3/callback.h"
#include "ns3/packet.h"
#include "ns3/object.h"
#include "ns3/nstime.h"
#include "ns3/ptr.h"
#include "slim-mode.h"
#include "ns3/traced-callback.h"
#include "slim-tx-vector.h"

namespace ns3 {

class SlimChannel;
class NetDevice;

/**
 * \brief receive notifications about phy events.
 */
class SlimPhyListener
{
public:
  virtual ~SlimPhyListener ();

  /**
   * \param duration the expected duration of the packet reception.
   *
   * We have received the first bit of a packet. We decided
   * that we could synchronize on this packet. It does not mean
   * we will be able to successfully receive completely the
   * whole packet. It means that we will report a BUSY status until
   * one of the following happens:
   *   - NotifyRxEndOk
   *   - NotifyRxEndError
   *   - NotifyTxStart
   */
  virtual void NotifyRxStart (Time duration) = 0;
  /**
   * We have received the last bit of a packet for which
   * NotifyRxStart was invoked first and, the packet has
   * been successfully received.
   */
  virtual void NotifyRxEndOk (void) = 0;
  /**
   * We have received the last bit of a packet for which
   * NotifyRxStart was invoked first and, the packet has
   * _not_ been successfully received.
   */
  virtual void NotifyRxEndError (void) = 0;
  /**
   * \param duration the expected transmission duration.
   * \param txPowerDbm the nominal tx power in dBm
   *
   * We are about to send the first bit of the packet.
   * We do not send any event to notify the end of
   * transmission. Listeners should assume that the
   * channel implicitely reverts to the idle state
   * unless they have received a cca busy report.
   */
  virtual void NotifyTxStart (Time duration, double txPowerDbm) = 0;

  /**
   * \param duration the expected busy duration.
   *
   * This method does not really report a real state
   * change as opposed to the other methods in this class.
   * It merely reports that, unless the medium is reported
   * busy through NotifyTxStart or NotifyRxStart/End,
   * it will be busy as defined by the currently selected
   * CCA mode.
   *
   * Typical client code which wants to have a clear picture
   * of the CCA state will need to keep track of the time at
   * which the last NotifyCcaBusyStart method is called and
   * what duration it reported.
   */
  virtual void NotifyMaybeCcaBusyStart (Time duration) = 0;
  /**
   * \param duration the expected channel switching duration.
   *
   * We do not send any event to notify the end of
   * channel switching. Listeners should assume that the
   * channel implicitely reverts to the idle or busy states.
   */
  virtual void NotifySwitchingStart (Time duration) = 0;
  /**
   * Notify listeners that we went to sleep
   */
  virtual void NotifySleep (void) = 0;
  /**
   * Notify listeners that we woke up
   */
  virtual void NotifyWakeup (void) = 0;
};


/**
 * \brief 802.11 PHY layer model
 * \ingroup slim
 *
 */
class SlimPhy : public Object
{
public:
  /**
   * The state of the PHY layer.
   */
  enum State
  {
    /**
     * The PHY layer is IDLE.
     */
    IDLE,
    /**
     * The PHY layer has sense the medium busy through the CCA mechanism
     */
    CCA_BUSY,
    /**
     * The PHY layer is sending a packet.
     */
    TX,
    /**
     * The PHY layer is receiving a packet.
     */
    RX,
    /**
     * The PHY layer is switching to other channel.
     */
    SWITCHING,
    /**
     * The PHY layer is sleeping.
     */
    SLEEP
  };

  /**
   * arg1: packet received successfully
   * arg2: snr of packet
   * arg3: mode of packet
   * arg4: type of preamble used for packet.
   */
  typedef Callback<void,Ptr<Packet>, double, SlimTxVector> RxOkCallback;
  /**
   * arg1: packet received unsuccessfully
   * arg2: snr of packet
   */
  typedef Callback<void,Ptr<const Packet>, double> RxErrorCallback;

  static TypeId GetTypeId (void);

  SlimPhy ();
  virtual ~SlimPhy ();

  /**
   * Return the minimum available transmission power level (dBm).
   *
   * \return the minimum available transmission power level in dBm
   */
  virtual double GetTxPowerStart (void) const = 0;
  /**
   * Return the maximum available transmission power level (dBm).
   *
   * \return the maximum available transmission power level in dBm
   */
  virtual double GetTxPowerEnd (void) const = 0;
  /**
   * \return the number of tx power levels available for this PHY.
   */
  virtual uint32_t GetNTxPower (void) const = 0;

  /**
   * \param callback the callback to invoke
   *        upon successful packet reception.
   */
  virtual void SetReceiveOkCallback (RxOkCallback callback) = 0;
  /**
   * \param callback the callback to invoke
   *        upon erroneous packet reception.
   */
  virtual void SetReceiveErrorCallback (RxErrorCallback callback) = 0;

  /**
   * \param packet the packet to send
   * \param txvector the txvector that has tx parameters such as mode, the transmission mode to use to send
   *        this packet, and txPowerLevel, a power level to use to send this packet. The real transmission
   *        power is calculated as txPowerMin + txPowerLevel * (txPowerMax - txPowerMin) / nTxLevels
   * \param preamble the type of preamble to use to send this packet.
   * \param packetType the type of the packet 0 is not A-MPDU, 1 is a MPDU that is part of an A-MPDU and 2 is the last MPDU in an A-MPDU
   */
  virtual void SendPacket (Ptr<const Packet> packet, SlimTxVector txvector, uint8_t packetType) = 0;

  /**
   * \param listener the new listener
   *
   * Add the input listener to the list of objects to be notified of
   * PHY-level events.
   */
  virtual void RegisterListener (SlimPhyListener *listener) = 0;
  /**
   * \param listener the listener to be unregistered
   *
   * Remove the input listener from the list of objects to be notified of
   * PHY-level events.
   */
  virtual void UnregisterListener (SlimPhyListener *listener) = 0;

  /**
   * Put in sleep mode.
   */
  virtual void SetSleepMode (void) = 0;
  /**
   * Resume from sleep mode.
   */
  virtual void ResumeFromSleep (void) = 0;

  /**
   * \return true of the current state of the PHY layer is SlimPhy::IDLE, false otherwise.
   */
  virtual bool IsStateIdle (void) = 0;
  /**
   * \return true of the current state of the PHY layer is SlimPhy::CCA_BUSY, false otherwise.
   */
  virtual bool IsStateCcaBusy (void) = 0;
  /**
   * \return true of the current state of the PHY layer is not SlimPhy::IDLE, false otherwise.
   */
  virtual bool IsStateBusy (void) = 0;
  /**
   * \return true of the current state of the PHY layer is SlimPhy::RX, false otherwise.
   */
  virtual bool IsStateRx (void) = 0;
  /**
   * \return true of the current state of the PHY layer is SlimPhy::TX, false otherwise.
   */
  virtual bool IsStateTx (void) = 0;
  /**
   * \return true of the current state of the PHY layer is SlimPhy::SWITCHING, false otherwise.
   */
  virtual bool IsStateSwitching (void) = 0;
  /**
   * \return true if the current state of the PHY layer is SlimPhy::SLEEP, false otherwise.
   */
  virtual bool IsStateSleep (void) = 0;
  /**
   * \return the amount of time since the current state has started.
   */
  virtual Time GetStateDuration (void) = 0;
  /**
   * \return the predicted delay until this PHY can become SlimPhy::IDLE.
   *
   * The PHY will never become SlimPhy::IDLE _before_ the delay returned by
   * this method but it could become really idle later.
   */
  virtual Time GetDelayUntilIdle (void) = 0;

  /**
   * Return the start time of the last received packet.
   *
   * \return the start time of the last received packet
   */
  virtual Time GetLastRxStartTime (void) const = 0;

  /**
    * \param size the number of bytes in the packet to send
    * \param txvector the transmission parameters used for this packet
    * \param preamble the type of preamble to use for this packet.
    * \param frequency the channel center frequency (MHz)
    * \param packetType the type of the packet 0 is not A-MPDU, 1 is a MPDU that is part of an A-MPDU  and 2 is the last MPDU in an A-MPDU
    * \param incFlag this flag is used to indicate that the static variables need to be update or not. This function is called a couple of times for the same packet so static variables should not be increased each time.
    * \return the total amount of time this PHY will stay busy for
    *          the transmission of these bytes.
    */
   Time CalculateTxDuration (SlimTxVector txvector);

   /*uint8_t GetNMcs (void) const;
   uint8_t GetMcs (uint8_t mcs) const;

   uint32_t SlimModeToMcs (SlimMode mode);

   SlimMode McsToSlimMode (uint8_t mcs);*/



  /**
   * The SlimPhy::GetNModes() and SlimPhy::GetMode() methods are used
   * (e.g., by a SlimRemoteStationManager) to determine the set of
   * transmission/reception modes that this SlimPhy(-derived class)
   * can support - a set of SlimMode objects which we call the
   * DeviceRateSet, and which is stored as SlimPhy::m_deviceRateSet.
   *
   * It is important to note that the DeviceRateSet is a superset (not
   * necessarily proper) of the OperationalRateSet (which is
   * logically, if not actually, a property of the associated
   * SlimRemoteStationManager), which itself is a superset (again, not
   * necessarily proper) of the BSSBasicRateSet.
   *
   * \return the number of transmission modes supported by this PHY.
   *
   * \sa SlimPhy::GetMode()
   */
 // virtual uint32_t GetNModes (void) const = 0;
  /**
   * The SlimPhy::GetNModes() and SlimPhy::GetMode() methods are used
   * (e.g., by a SlimRemoteStationManager) to determine the set of
   * transmission/reception modes that this SlimPhy(-derived class)
   * can support - a set of SlimMode objects which we call the
   * DeviceRateSet, and which is stored as SlimPhy::m_deviceRateSet.
   *
   * It is important to note that the DeviceRateSet is a superset (not
   * necessarily proper) of the OperationalRateSet (which is
   * logically, if not actually, a property of the associated
   * SlimRemoteStationManager), which itself is a superset (again, not
   * necessarily proper) of the BSSBasicRateSet.
   *
   * \param mode index in array of supported modes
   * \return the mode whose index is specified.
   *
   * \sa SlimPhy::GetNModes()
   */
  //virtual SlimMode GetMode (uint32_t mode) const = 0;
  /**
   * Check if the given SlimMode is supported by the PHY.
   *
   * \param mode the slim mode to check
   * \return true if the given mode is supported,
   *         false otherwise
   */
  virtual bool IsModeSupported (SlimTxVector txvector) = 0;
  /**
   * \param txMode the transmission mode
   * \param ber the probability of bit error rate
   * \return the minimum snr which is required to achieve
   *          the requested ber for the specified transmission mode. (W/W)
   */
  virtual double CalculateSnr (SlimMode txMode, double ber) const = 0;

  
  //virtual Time CalculateTxDuration (uint32_t size, SlimTxVector txvector, SlimPreamble preamble, double frequency, uint8_t packetType, uint8_t incFlag);

  /**
   * \brief Set channel number.
   *
   * Channel center frequency = Channel starting frequency + 5 MHz * (nch - 1)
   *
   * where Starting channel frequency is standard-dependent, see SetStandard()
   * as defined in (Section 18.3.8.4.2 "Channel numbering"; IEEE Std 802.11-2012).
   *
   * \param id the channel number
   */
  virtual void SetChannelNumber (uint16_t id) = 0;
  /**  
   * Return current channel number.
   *
   * \return the current channel number
   */
  virtual uint16_t GetChannelNumber (void) const = 0;
  /**
   * \return the required time for channel switch operation of this SlimPhy
   */
  virtual Time GetChannelSwitchDelay (void) const = 0;

  /**
   * Return the SlimChannel this SlimPhy is connected to.
   *
   * \return the SlimChannel this SlimPhy is connected to
   */
  virtual Ptr<SlimChannel> GetChannel (void) const = 0;


  //virtual Time CalculateTxDuration (uint32_t size, SlimTxVector txvector, SlimPreamble preamble, double frequency, uint8_t packetType, uint8_t incFlag) const =0;


  /**
   * Public method used to fire a PhyTxBegin trace.  Implemented for encapsulation
   * purposes.
   *
   * \param packet the packet being transmitted
   */
  void NotifyTxBegin (Ptr<const Packet> packet);

  /**
   * Public method used to fire a PhyTxEnd trace.  Implemented for encapsulation
   * purposes.
   *
   * \param packet the packet that was transmitted
   */
  void NotifyTxEnd (Ptr<const Packet> packet);

  /**
   * Public method used to fire a PhyTxDrop trace.  Implemented for encapsulation
   * purposes.
   *
   * \param packet the packet that was failed to transmitted
   */
  void NotifyTxDrop (Ptr<const Packet> packet);

  /**
   * Public method used to fire a PhyRxBegin trace.  Implemented for encapsulation
   * purposes.
   *
   * \param packet the packet being received
   */
  void NotifyRxBegin (Ptr<const Packet> packet);

  /**
   * Public method used to fire a PhyRxEnd trace.  Implemented for encapsulation
   * purposes.
   *
   * \param packet the packet received
   */
  void NotifyRxEnd (Ptr<const Packet> packet);

  /**
   * Public method used to fire a PhyRxDrop trace.  Implemented for encapsulation
   * purposes.
   *
   * \param packet the packet that was not successfully received
   */
  void NotifyRxDrop (Ptr<const Packet> packet);

  /**
   * Public method used to fire a MonitorSniffer trace for a slim packet
   * being received.  Implemented for encapsulation purposes.
   *
   * \param packet the packet being received
   * \param channelFreqMhz the frequency in MHz at which the packet is
   *        received. Note that in real devices this is normally the
   *        frequency to which  the receiver is tuned, and this can be
   *        different than the frequency at which the packet was originally
   *        transmitted. This is because it is possible to have the receiver
   *        tuned on a given channel and still to be able to receive packets
   *        on a nearby channel.
   * \param channelNumber the channel on which the packet is received
   * \param rate the PHY data rate in units of 500kbps (i.e., the same
   *        units used both for the radiotap and for the prism header)
   * \param isShortPreamble true if short preamble is used, false otherwise
   * \param signalDbm signal power in dBm
   * \param noiseDbm  noise power in dBm
   */
  void NotifyMonitorSniffRx (Ptr<const Packet> packet, SlimTxVector txvector,
                             uint16_t channelNumber, double signalDbm,
                             double noiseDbm);

  /**
   * TracedCallback signature for monitor mode receive events.
   *
   *
   * \param packet the packet being received
   * \param channelFreqMhz the frequency in MHz at which the packet is
   *        received. Note that in real devices this is normally the
   *        frequency to which  the receiver is tuned, and this can be
   *        different than the frequency at which the packet was originally
   *        transmitted. This is because it is possible to have the receiver
   *        tuned on a given channel and still to be able to receive packets
   *        on a nearby channel.
   * \param channelNumber the channel on which the packet is received
   * \param rate the PHY data rate in units of 500kbps (i.e., the same
   *        units used both for the radiotap and for the prism header)
   * \param isShortPreamble true if short preamble is used, false otherwise
   * \param signalDbm signal power in dBm
   * \param noiseDbm  noise power in dBm
   */
  typedef void (* MonitorSnifferRxCallback)
    (Ptr<const Packet> packet, SlimTxVector txvector,
     uint16_t channelNumber, double signalDbm,
     double noiseDbm);

  /**
   * Public method used to fire a MonitorSniffer trace for a slim packet
   * being transmitted.  Implemented for encapsulation purposes.
   *
   * \param packet the packet being transmitted
   * \param channelFreqMhz the frequency in MHz at which the packet is
   *        transmitted.
   * \param channelNumber the channel on which the packet is transmitted
   * \param rate the PHY data rate in units of 500kbps (i.e., the same
   *        units used both for the radiotap and for the prism header)
   * \param isShortPreamble true if short preamble is used, false otherwise
   * \param txPower the transmission power in dBm
   */
  void NotifyMonitorSniffTx (Ptr<const Packet> packet, SlimTxVector txvector,
                             uint16_t channelNumber, uint8_t txPower);

  /**
   * TracedCallback signature for monitor mode transmit events.
   *
   * \param packet the packet being transmitted
   * \param channelFreqMhz the frequency in MHz at which the packet is
   *        transmitted.
   * \param channelNumber the channel on which the packet is transmitted
   * \param rate the PHY data rate in units of 500kbps (i.e., the same
   *        units used both for the radiotap and for the prism header)
   * \param isShortPreamble true if short preamble is used, false otherwise
   * \param txPower the transmission power in dBm
   */
  typedef void (* MonitorSnifferTxCallback)
    (const Ptr<const Packet> packet, SlimTxVector txvector,
     uint16_t channelNumber, uint8_t txPower);


 /**
  * Assign a fixed random variable stream number to the random variables
  * used by this model.  Return the number of streams (possibly zero) that
  * have been assigned.
  *
  * \param stream first stream index to use
  * \return the number of stream indices assigned by this model
  */
  virtual int64_t AssignStreams (int64_t stream) = 0;

  /**
   * \param freq the operating frequency on this node.
   */
  virtual void SetFrequency (uint32_t freq)=0;
  /**
   * \return the operating frequency on this node
   */
  virtual uint32_t GetFrequency (void) const=0;


private:
  /**
   * The trace source fired when a packet begins the transmission process on
   * the medium.
   *
   * \see class CallBackTraceSource
   */
  TracedCallback<Ptr<const Packet> > m_phyTxBeginTrace;

  /**
   * The trace source fired when a packet ends the transmission process on
   * the medium.
   *
   * \see class CallBackTraceSource
   */
  TracedCallback<Ptr<const Packet> > m_phyTxEndTrace;

  /**
   * The trace source fired when the phy layer drops a packet as it tries
   * to transmit it.
   *
   * \see class CallBackTraceSource
   */
  TracedCallback<Ptr<const Packet> > m_phyTxDropTrace;

  /**
   * The trace source fired when a packet begins the reception process from
   * the medium.
   *
   * \see class CallBackTraceSource
   */
  TracedCallback<Ptr<const Packet> > m_phyRxBeginTrace;

  /**
   * The trace source fired when a packet ends the reception process from
   * the medium.
   *
   * \see class CallBackTraceSource
   */
  TracedCallback<Ptr<const Packet> > m_phyRxEndTrace;

  /**
   * The trace source fired when the phy layer drops a packet it has received.
   *
   * \see class CallBackTraceSource
   */
  TracedCallback<Ptr<const Packet> > m_phyRxDropTrace;

  /**
   * A trace source that emulates a slim device in monitor mode
   * sniffing a packet being received.
   *
   * As a reference with the real world, firing this trace
   * corresponds in the madslim driver to calling the function
   * ieee80211_input_monitor()
   *
   * \see class CallBackTraceSource
   */
  TracedCallback<Ptr<const Packet>, SlimTxVector, uint16_t, double, double> m_phyMonitorSniffRxTrace;

  /**
   * A trace source that emulates a slim device in monitor mode
   * sniffing a packet being transmitted.
   *
   * As a reference with the real world, firing this trace
   * corresponds in the madslim driver to calling the function
   * ieee80211_input_monitor()
   *
   * \see class CallBackTraceSource
   */
  TracedCallback<Ptr<const Packet>, SlimTxVector, uint16_t,uint8_t> m_phyMonitorSniffTxTrace;
    
  uint32_t m_totalAmpduNumSymbols; //!< Number of symbols previously transmitted for the MPDUs in an A-MPDU, used for the computation of the number of symbols needed for the last MPDU in the A-MPDU
  uint32_t m_totalAmpduSize;       //!< Total size of the previously transmitted MPDUs in an A-MPDU, used for the computation of the number of symbols needed for the last MPDU in the A-MPDU

};

/**
 * \param os          output stream
 * \param state       slim state to stringify
 * \return output stream
 */
std::ostream& operator<< (std::ostream& os, enum SlimPhy::State state);

} // namespace ns3

#endif /* SLIM_PHY_H */
