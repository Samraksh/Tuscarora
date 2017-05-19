/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/* Copyright: The Samraksh Company
 * SLIM-WF:Synchronous Lightweight Interfernce-aware Model-driven Wavefrm
 * The usage of this module is governed by the associated license file.
 * Authors: Bora Karaoglu, Mukundan Sridharan
 */

#include "slim-phy-state-helper.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/trace-source-accessor.h"
#include <algorithm>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("SlimPhyStateHelper");

NS_OBJECT_ENSURE_REGISTERED (SlimPhyStateHelper);

TypeId
SlimPhyStateHelper::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::SlimPhyStateHelper")
    .SetParent<Object> ()
    .AddConstructor<SlimPhyStateHelper> ()
    .AddTraceSource ("State",
                     "The state of the PHY layer",
                     MakeTraceSourceAccessor (&SlimPhyStateHelper::m_stateLogger),
                     "ns3::SlimPhyStateHelper::StateTracedCallback")
    .AddTraceSource ("RxOk",
                     "A packet has been received successfully.",
                     MakeTraceSourceAccessor (&SlimPhyStateHelper::m_rxOkTrace),
                     "ns3::SlimPhyStateHelper::RxOkTracedCallback")
    .AddTraceSource ("RxError",
                     "A packet has been received unsuccessfully.",
                     MakeTraceSourceAccessor (&SlimPhyStateHelper::m_rxErrorTrace),
                     "ns3::SlimPhyStateHelper::RxErrorTracedCallback")
    .AddTraceSource ("Tx", "Packet transmission is starting.",
                     MakeTraceSourceAccessor (&SlimPhyStateHelper::m_txTrace),
                     "ns3::SlimPhyStateHelper::TxTracedCallback")
  ;
  return tid;
}

SlimPhyStateHelper::SlimPhyStateHelper ()
  : m_rxing (false),
    m_sleeping (false),
    m_endTx (Seconds (0)),
    m_endRx (Seconds (0)),
    m_endCcaBusy (Seconds (0)),
    m_endSwitching (Seconds (0)),
    m_startTx (Seconds (0)),
    m_startRx (Seconds (0)),
    m_startCcaBusy (Seconds (0)),
    m_startSwitching (Seconds (0)),
    m_startSleep (Seconds (0)),
    m_previousStateChangeTime (Seconds (0))
{
  NS_LOG_FUNCTION (this);
}

void
SlimPhyStateHelper::SetReceiveOkCallback (SlimPhy::RxOkCallback callback)
{
  m_rxOkCallback = callback;
}
void
SlimPhyStateHelper::SetReceiveErrorCallback (SlimPhy::RxErrorCallback callback)
{
  m_rxErrorCallback = callback;
}
void
SlimPhyStateHelper::RegisterListener (SlimPhyListener *listener)
{
  m_listeners.push_back (listener);
}
void
SlimPhyStateHelper::UnregisterListener (SlimPhyListener *listener)
{
  ListenersI i = find (m_listeners.begin(), m_listeners.end(), listener);
  if (i != m_listeners.end())
    {
      m_listeners.erase(i);
    }
}

bool
SlimPhyStateHelper::IsStateIdle (void)
{
  return (GetState () == SlimPhy::IDLE);
}
bool
SlimPhyStateHelper::IsStateBusy (void)
{
  return (GetState () != SlimPhy::IDLE);
}
bool
SlimPhyStateHelper::IsStateCcaBusy (void)
{
  return (GetState () == SlimPhy::CCA_BUSY);
}
bool
SlimPhyStateHelper::IsStateRx (void)
{
  return (GetState () == SlimPhy::RX);
}
bool
SlimPhyStateHelper::IsStateTx (void)
{
  return (GetState () == SlimPhy::TX);
}
bool
SlimPhyStateHelper::IsStateSwitching (void)
{
  return (GetState () == SlimPhy::SWITCHING);
}
bool
SlimPhyStateHelper::IsStateSleep (void)
{
  return (GetState () == SlimPhy::SLEEP);
}



Time
SlimPhyStateHelper::GetStateDuration (void)
{
  return Simulator::Now () - m_previousStateChangeTime;
}

Time
SlimPhyStateHelper::GetDelayUntilIdle (void)
{
  Time retval;

  switch (GetState ())
    {
    case SlimPhy::RX:
      retval = m_endRx - Simulator::Now ();
      break;
    case SlimPhy::TX:
      retval = m_endTx - Simulator::Now ();
      break;
    case SlimPhy::CCA_BUSY:
      retval = m_endCcaBusy - Simulator::Now ();
      break;
    case SlimPhy::SWITCHING:
      retval = m_endSwitching - Simulator::Now ();
      break;
    case SlimPhy::IDLE:
      retval = Seconds (0);
      break;
    case SlimPhy::SLEEP:
      NS_FATAL_ERROR ("Cannot determine when the device will wake up.");
      retval = Seconds (0);
      break;
    default:
      NS_FATAL_ERROR ("Invalid SlimPhy state.");
      retval = Seconds (0);
      break;
    }
  retval = Max (retval, Seconds (0));
  return retval;
}

Time
SlimPhyStateHelper::GetLastRxStartTime (void) const
{
  return m_startRx;
}

enum SlimPhy::State
SlimPhyStateHelper::GetState (void)
{
  if (m_sleeping)
    {
      return SlimPhy::SLEEP;
    }
  else if (m_endTx > Simulator::Now ())
    {
      return SlimPhy::TX;
    }
  else if (m_rxing)
    {
      return SlimPhy::RX;
    }
  else if (m_endSwitching > Simulator::Now ())
    {
      return SlimPhy::SWITCHING;
    }
  else if (m_endCcaBusy > Simulator::Now ())
    {
      return SlimPhy::CCA_BUSY;
    }
  else
    {
      return SlimPhy::IDLE;
    }
}


void
SlimPhyStateHelper::NotifyTxStart (Time duration, double txPowerDbm)
{
  for (Listeners::const_iterator i = m_listeners.begin (); i != m_listeners.end (); i++)
    {
      (*i)->NotifyTxStart (duration, txPowerDbm);
    }
}
void
SlimPhyStateHelper::NotifyRxStart (Time duration)
{
  for (Listeners::const_iterator i = m_listeners.begin (); i != m_listeners.end (); i++)
    {
      (*i)->NotifyRxStart (duration);
    }
}
void
SlimPhyStateHelper::NotifyRxEndOk (void)
{
  for (Listeners::const_iterator i = m_listeners.begin (); i != m_listeners.end (); i++)
    {
      (*i)->NotifyRxEndOk ();
    }
}
void
SlimPhyStateHelper::NotifyRxEndError (void)
{
  for (Listeners::const_iterator i = m_listeners.begin (); i != m_listeners.end (); i++)
    {
      (*i)->NotifyRxEndError ();
    }
}
void
SlimPhyStateHelper::NotifyMaybeCcaBusyStart (Time duration)
{
  for (Listeners::const_iterator i = m_listeners.begin (); i != m_listeners.end (); i++)
    {
      (*i)->NotifyMaybeCcaBusyStart (duration);
    }
}
void
SlimPhyStateHelper::NotifySwitchingStart (Time duration)
{
  for (Listeners::const_iterator i = m_listeners.begin (); i != m_listeners.end (); i++)
    {
      (*i)->NotifySwitchingStart (duration);
    }
}
void
SlimPhyStateHelper::NotifySleep (void)
{
  for (Listeners::const_iterator i = m_listeners.begin (); i != m_listeners.end (); i++)
    {
      (*i)->NotifySleep ();
    }
}
void
SlimPhyStateHelper::NotifyWakeup (void)
{
  for (Listeners::const_iterator i = m_listeners.begin (); i != m_listeners.end (); i++)
    {
      (*i)->NotifyWakeup ();
    }
}


void
SlimPhyStateHelper::LogPreviousIdleAndCcaBusyStates (void)
{
  Time now = Simulator::Now ();
  Time idleStart = Max (m_endCcaBusy, m_endRx);
  idleStart = Max (idleStart, m_endTx);
  idleStart = Max (idleStart, m_endSwitching);
  NS_ASSERT (idleStart <= now);
  if (m_endCcaBusy > m_endRx
      && m_endCcaBusy > m_endSwitching
      && m_endCcaBusy > m_endTx)
    {
      Time ccaBusyStart = Max (m_endTx, m_endRx);
      ccaBusyStart = Max (ccaBusyStart, m_startCcaBusy);
      ccaBusyStart = Max (ccaBusyStart, m_endSwitching);
      m_stateLogger (ccaBusyStart, idleStart - ccaBusyStart, SlimPhy::CCA_BUSY);
    }
  m_stateLogger (idleStart, now - idleStart, SlimPhy::IDLE);
}

void
SlimPhyStateHelper::SwitchToTx (Time txDuration, Ptr<const Packet> packet, double txPowerDbm,
                                SlimTxVector txVector)
{
  m_txTrace (packet, txVector.GetMode(), txVector.GetTxPowerLevel());
  Time now = Simulator::Now ();
  switch (GetState ())
    {
    case SlimPhy::RX:
      /* The packet which is being received as well
       * as its endRx event are cancelled by the caller.
       */
      m_rxing = false;
      m_stateLogger (m_startRx, now - m_startRx, SlimPhy::RX);
      m_endRx = now;
      break;
    case SlimPhy::CCA_BUSY:
      {
        Time ccaStart = Max (m_endRx, m_endTx);
        ccaStart = Max (ccaStart, m_startCcaBusy);
        ccaStart = Max (ccaStart, m_endSwitching);
        m_stateLogger (ccaStart, now - ccaStart, SlimPhy::CCA_BUSY);
      } break;
    case SlimPhy::IDLE:
      LogPreviousIdleAndCcaBusyStates ();
      break;
    case SlimPhy::SWITCHING:
    case SlimPhy::SLEEP:
    default:
      NS_FATAL_ERROR ("Invalid SlimPhy state.");
      break;
    }
  m_stateLogger (now, txDuration, SlimPhy::TX);
  m_previousStateChangeTime = now;
  m_endTx = now + txDuration;
  m_startTx = now;
  NotifyTxStart (txDuration, txPowerDbm);
}
void
SlimPhyStateHelper::SwitchToRx (Time rxDuration)
{
  NS_ASSERT (IsStateIdle () || IsStateCcaBusy ());
  NS_ASSERT (!m_rxing);
  Time now = Simulator::Now ();
  switch (GetState ())
    {
    case SlimPhy::IDLE:
      LogPreviousIdleAndCcaBusyStates ();
      break;
    case SlimPhy::CCA_BUSY:
      {
        Time ccaStart = Max (m_endRx, m_endTx);
        ccaStart = Max (ccaStart, m_startCcaBusy);
        ccaStart = Max (ccaStart, m_endSwitching);
        m_stateLogger (ccaStart, now - ccaStart, SlimPhy::CCA_BUSY);
      } break;
    case SlimPhy::SWITCHING:
    case SlimPhy::RX:
    case SlimPhy::TX:
    case SlimPhy::SLEEP:
      NS_FATAL_ERROR ("Invalid SlimPhy state.");
      break;
    }
  m_previousStateChangeTime = now;
  m_rxing = true;
  m_startRx = now;
  m_endRx = now + rxDuration;
  NotifyRxStart (rxDuration);
  NS_ASSERT (IsStateRx ());
}

void
SlimPhyStateHelper::SwitchToChannelSwitching (Time switchingDuration)
{
  Time now = Simulator::Now ();
  switch (GetState ())
    {
    case SlimPhy::RX:
      /* The packet which is being received as well
       * as its endRx event are cancelled by the caller.
       */
      m_rxing = false;
      m_stateLogger (m_startRx, now - m_startRx, SlimPhy::RX);
      m_endRx = now;
      break;
    case SlimPhy::CCA_BUSY:
      {
        Time ccaStart = Max (m_endRx, m_endTx);
        ccaStart = Max (ccaStart, m_startCcaBusy);
        ccaStart = Max (ccaStart, m_endSwitching);
        m_stateLogger (ccaStart, now - ccaStart, SlimPhy::CCA_BUSY);
      } break;
    case SlimPhy::IDLE:
      LogPreviousIdleAndCcaBusyStates ();
      break;
    case SlimPhy::TX:
    case SlimPhy::SWITCHING:
    case SlimPhy::SLEEP:
    default:
      NS_FATAL_ERROR ("Invalid SlimPhy state.");
      break;
    }

  if (now < m_endCcaBusy)
    {
      m_endCcaBusy = now;
    }

  m_stateLogger (now, switchingDuration, SlimPhy::SWITCHING);
  m_previousStateChangeTime = now;
  m_startSwitching = now;
  m_endSwitching = now + switchingDuration;
  NotifySwitchingStart (switchingDuration);
  NS_ASSERT (IsStateSwitching ());
}

void
SlimPhyStateHelper::SwitchFromRxEndOk (Ptr<Packet> packet, double snr, SlimTxVector txVector)
{
  m_rxOkTrace (packet, snr, txVector);
  NotifyRxEndOk ();
  DoSwitchFromRx ();
  if (!m_rxOkCallback.IsNull ())
    {
      m_rxOkCallback (packet, snr, txVector);
    }

}
void
SlimPhyStateHelper::SwitchFromRxEndError (Ptr<const Packet> packet, double snr)
{
  m_rxErrorTrace (packet, snr);
  NotifyRxEndError ();
  DoSwitchFromRx ();
  if (!m_rxErrorCallback.IsNull ())
    {
      m_rxErrorCallback (packet, snr);
    }
}

void
SlimPhyStateHelper::DoSwitchFromRx (void)
{
  NS_ASSERT (IsStateRx ());
  NS_ASSERT (m_rxing);

  Time now = Simulator::Now ();
  m_stateLogger (m_startRx, now - m_startRx, SlimPhy::RX);
  m_previousStateChangeTime = now;
  m_rxing = false;

  NS_ASSERT (IsStateIdle () || IsStateCcaBusy ());
}
void
SlimPhyStateHelper::SwitchMaybeToCcaBusy (Time duration)
{
  NotifyMaybeCcaBusyStart (duration);
  Time now = Simulator::Now ();
  switch (GetState ())
    {
    case SlimPhy::SWITCHING:
      break;
    case SlimPhy::SLEEP:
      break;
    case SlimPhy::IDLE:
      LogPreviousIdleAndCcaBusyStates ();
      break;
    case SlimPhy::CCA_BUSY:
      break;
    case SlimPhy::RX:
      break;
    case SlimPhy::TX:
      break;
    }
  if (GetState () != SlimPhy::CCA_BUSY)
    {
      m_startCcaBusy = now;
    }
  m_endCcaBusy = std::max (m_endCcaBusy, now + duration);
}
void
SlimPhyStateHelper::SwitchToSleep (void)
{
  Time now = Simulator::Now ();
  switch (GetState ())
    {
    case SlimPhy::IDLE:
      LogPreviousIdleAndCcaBusyStates ();
      break;
    case SlimPhy::CCA_BUSY:
      {
        Time ccaStart = Max (m_endRx, m_endTx);
        ccaStart = Max (ccaStart, m_startCcaBusy);
        ccaStart = Max (ccaStart, m_endSwitching);
        m_stateLogger (ccaStart, now - ccaStart, SlimPhy::CCA_BUSY);
      } break;
    case SlimPhy::RX:
    case SlimPhy::SWITCHING:
    case SlimPhy::TX:
    case SlimPhy::SLEEP:
      NS_FATAL_ERROR ("Invalid SlimPhy state.");
      break;
    }
  m_previousStateChangeTime = now;
  m_sleeping = true;
  m_startSleep = now;
  NotifySleep ();
  NS_ASSERT (IsStateSleep ());
}
void
SlimPhyStateHelper::SwitchFromSleep (Time duration)
{
  NS_ASSERT (IsStateSleep ());
  Time now = Simulator::Now ();
  m_stateLogger (m_startSleep, now - m_startSleep, SlimPhy::SLEEP);
  m_previousStateChangeTime = now;
  m_sleeping = false;
  NotifyWakeup ();
  // update m_endCcaBusy after the sleep period
  m_endCcaBusy = std::max (m_endCcaBusy, now + duration);
  if (m_endCcaBusy > now)
    NotifyMaybeCcaBusyStart (m_endCcaBusy - now);
}

} // namespace ns3
