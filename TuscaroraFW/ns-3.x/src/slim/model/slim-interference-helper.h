/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/* Copyright: The Samraksh Company
 * SLIM-WF:Synchronous Lightweight Interfernce-aware Model-driven Wavefrm
 * The usage of this module is governed by the associated license file.
 * Authors: Bora Karaoglu, Mukundan Sridharan
 */

#ifndef SLIM_INTERFERENCE_HELPER_H
#define SLIM_INTERFERENCE_HELPER_H

#include <stdint.h>
#include <vector>
#include <list>
#include "slim-mode.h"
#include "ns3/nstime.h"
#include "ns3/simple-ref-count.h"
#include "slim-tx-vector.h"

namespace ns3 {

/**
 * \ingroup slim
 * \brief handles interference calculations
 */
class SlimInterferenceHelper
{
public:
  /**
   * Signal event for a packet.
   */
  class Event : public SimpleRefCount<SlimInterferenceHelper::Event>
  {
public:
    /**
     * Create an Event with the given parameters.
     *
     * \param size packet size
     * \param payloadMode Wi-Fi mode used for the payload
     * \param duration duration of the signal
     * \param rxPower the receive power (w)
     * \param txvector TXVECTOR of the packet
     */
    Event (uint32_t size, SlimMode* payloadMode,
           Time duration, double rxPower, SlimTxVector txvector);
    ~Event ();

    /**
     * Return the duration of the signal.
     *
     * \return the duration of the signal
     */
    Time GetDuration (void) const;
    /**
     * Return the start time of the signal.
     *
     * \return the start time of the signal
     */
    Time GetStartTime (void) const;
    /**
     * Return the end time of the signal.
     *
     * \return the end time of the signal
     */
    Time GetEndTime (void) const;
    /**
     * Return the receive power (w).
     *
     * \return the receive power (w)
     */
    double GetRxPowerW (void) const;
    /**
     * Return the size of the packet (bytes).
     *
     * \return the size of the packet (bytes)
     */
    uint32_t GetSize (void) const;
    /**
     * Return the Wi-Fi mode used for the payload.
     *
     * \return the Wi-Fi mode used for the payload
     */
    SlimMode* GetPayloadMode (void) const;

    /**
     * Return the TXVECTOR of the packet.
     *
     * \return the TXVECTOR of the packet
     */
    SlimTxVector GetTxVector (void) const;
private:
    uint32_t m_size;
    SlimMode *m_payloadMode;
    Time m_startTime;
    Time m_endTime;
    double m_rxPowerW;
    SlimTxVector m_txVector;
  };
  /**
   * A struct for both SNR and PER
   */
  struct SnrPer
  {
    double snr;
    double per;
  };

  SlimInterferenceHelper ();
  ~SlimInterferenceHelper ();

  /**
   * Set the noise figure.
   *
   * \param value noise figure
   */
  void SetNoiseFigure (double value);

  /**
   * Return the noise figure.
   *
   * \return the noise figure
   */
  double GetNoiseFigure (void) const;


  /**
   * \param energyW the minimum energy (W) requested
   * \returns the expected amount of time the observed
   *          energy on the medium will be higher than
   *          the requested threshold.
   */
  Time GetEnergyDuration (double energyW);

  /**
   * Add the packet-related signal to interference helper.
   *
   * \param size packet size
   * \param payloadMode Wi-Fi mode for the payload
   * \param duration the duration of the signal
   * \param rxPower receive power (w)
   * \param txvector TXVECTOR of the packet
   * \return SlimInterferenceHelper::Event
   */
  Ptr<SlimInterferenceHelper::Event> Add (uint32_t size, SlimMode* payloadMode,
                                      Time duration, double rxPower, SlimTxVector txvector);

  /**
   * Calculate the SNIR at the start of the packet and accumulate
   * all SNIR changes in the snir vector.
   *
   * \param event the event corresponding to the first time the packet arrives
   * \return struct of SNR and PER
   */
  struct SlimInterferenceHelper::SnrPer CalculateSnrPer (Ptr<SlimInterferenceHelper::Event> event);
  /**
   * Notify that RX has started.
   */
  void NotifyRxStart ();
  /**
   * Notify that RX has ended.
   */
  void NotifyRxEnd ();
  /**
   * Erase all events.
   */
  void EraseEvents (void);
private:
  /**
   * Noise and Interference (thus Ni) event.
   */
  class NiChange
  {
public:
    /**
     * Create a NiChange at the given time and the amount of NI change.
     *
     * \param time time of the event
     * \param delta the power
     */
    NiChange (Time time, double delta);
    /**
     * Return the event time.
     *
     * \return the event time.
     */
    Time GetTime (void) const;
    /**
     * Return the power
     *
     * \return the power
     */
    double GetDelta (void) const;
    /**
     * Compare the event time of two NiChange objects (a < o).
     *
     * \param o
     * \return true if a < o.time, false otherwise
     */
    bool operator < (const NiChange& o) const;
private:
    Time m_time;
    double m_delta;
  };
  /**
   * typedef for a vector of NiChanges
   */
  typedef std::vector <NiChange> NiChanges;
  /**
   * typedef for a list of Events
   */
  typedef std::list<Ptr<Event> > Events;

  //SlimInterferenceHelper (const SlimInterferenceHelper &o);
  //SlimInterferenceHelper &operator = (const SlimInterferenceHelper &o);
  /**
   * Append the given Event.
   *
   * \param event
   */
  void AppendEvent (Ptr<Event> event);
  /**
   * Calculate noise and interference power in W.
   *
   * \param event
   * \param ni
   * \return noise and interference power
   */
  double CalculateNoiseInterferenceW (Ptr<Event> event, NiChanges *ni) const;
  /**
   * Calculate SNR (linear ratio) from the given signal power and noise+interference power.
   * (Mode is not currently used)
   *
   * \param signal
   * \param noiseInterference
   * \param mode
   * \return SNR in liear ratio
   */
  double CalculateSnr (double signal, double noiseInterference, SlimMode* mode) const;
  /**
   * Calculate the number of errors for a chunk with the given the SINR, duration, and transmission mode.
   * The duration and mode are used to calculate how many bits are present in the chunk.
   *
   * \param snir SINR
   * \param duration
   * \param mode
   * \return the success rate
   */
  //double CalculateNumErrorsInChunk (double snir, Time duration, SlimMode mode) const;


  double m_noiseFigure; /**< noise figure (linear) */
  //Ptr<SlimErrorRateModel> m_errorRateModel;
  /// Experimental: needed for energy duration calculation
  NiChanges m_niChanges;
  double m_firstPower;
  bool m_rxing;
  /// Returns an iterator to the first nichange, which is later than moment
  NiChanges::iterator GetPosition (Time moment);
  /**
   * Add NiChange to the list at the appropriate position.
   *
   * \param change
   */
  void AddNiChangeEvent (NiChange change);

  bool IsPacketSuccessful(Ptr<const SlimInterferenceHelper::Event> event, NiChanges *ni);
  bool IsSectionSuccessfull(SlimMode* mode, NiChanges *ni, NiChanges::iterator &j, Time &current, Time &previous, double &noiseInterferenceW, double &powerW);

};

} // namespace ns3

#endif /* INTERFERENCE_HELPER_H */
