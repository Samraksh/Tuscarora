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

#include "slim-phy.h"
#include "slim-mode.h"
#include "slim-channel.h"
#include "ns3/simulator.h"
#include "ns3/packet.h"
#include "ns3/assert.h"
#include "ns3/log.h"
#include "ns3/double.h"
#include "ns3/uinteger.h"
#include "ns3/enum.h"
#include "ns3/trace-source-accessor.h"

#include <cmath>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("SlimPhy");

/****************************************************************
 *       This destructor is needed.
 ****************************************************************/

SlimPhyListener::~SlimPhyListener ()
{
}

/****************************************************************
 *       The actual SlimPhy class
 ****************************************************************/

NS_OBJECT_ENSURE_REGISTERED (SlimPhy);

TypeId
SlimPhy::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::SlimPhy")
    .SetParent<Object> ()
    .AddTraceSource ("PhyTxBegin",
                     "Trace source indicating a packet "
                     "has begun transmitting over the channel medium",
                     MakeTraceSourceAccessor (&SlimPhy::m_phyTxBeginTrace),
                     "ns3::Packet::TracedCallback")
    .AddTraceSource ("PhyTxEnd",
                     "Trace source indicating a packet "
                     "has been completely transmitted over the channel. "
                     "NOTE: the only official SlimPhy implementation "
                     "available to this date (YansSlimPhy) never fires "
                     "this trace source.",
                     MakeTraceSourceAccessor (&SlimPhy::m_phyTxEndTrace),
                     "ns3::Packet::TracedCallback")
    .AddTraceSource ("PhyTxDrop",
                     "Trace source indicating a packet "
                     "has been dropped by the device during transmission",
                     MakeTraceSourceAccessor (&SlimPhy::m_phyTxDropTrace),
                     "ns3::Packet::TracedCallback")
    .AddTraceSource ("PhyRxBegin",
                     "Trace source indicating a packet "
                     "has begun being received from the channel medium "
                     "by the device",
                     MakeTraceSourceAccessor (&SlimPhy::m_phyRxBeginTrace),
                     "ns3::Packet::TracedCallback")
    .AddTraceSource ("PhyRxEnd",
                     "Trace source indicating a packet "
                     "has been completely received from the channel medium "
                     "by the device",
                     MakeTraceSourceAccessor (&SlimPhy::m_phyRxEndTrace),
                     "ns3::Packet::TracedCallback")
    .AddTraceSource ("PhyRxDrop",
                     "Trace source indicating a packet "
                     "has been dropped by the device during reception",
                     MakeTraceSourceAccessor (&SlimPhy::m_phyRxDropTrace),
                     "ns3::Packet::TracedCallback")
    .AddTraceSource ("MonitorSnifferRx",
                     "Trace source simulating a slim device in monitor mode "
                     "sniffing all received frames",
                     MakeTraceSourceAccessor (&SlimPhy::m_phyMonitorSniffRxTrace),
                     "ns3::SlimPhy::MonitorSnifferRxCallback")
    .AddTraceSource ("MonitorSnifferTx",
                     "Trace source simulating the capability of a slim device "
                     "in monitor mode to sniff all frames being transmitted",
                     MakeTraceSourceAccessor (&SlimPhy::m_phyMonitorSniffTxTrace),
                     "ns3::SlimPhy::MonitorSnifferTxCallback")
  ;
  return tid;
}

SlimPhy::SlimPhy ()
{
  NS_LOG_FUNCTION (this);
  m_totalAmpduSize = 0;
  m_totalAmpduNumSymbols = 0;
}

SlimPhy::~SlimPhy ()
{
  NS_LOG_FUNCTION (this);
}




void
SlimPhy::NotifyTxBegin (Ptr<const Packet> packet)
{
  m_phyTxBeginTrace (packet);
}

void
SlimPhy::NotifyTxEnd (Ptr<const Packet> packet)
{
  m_phyTxEndTrace (packet);
}

void
SlimPhy::NotifyTxDrop (Ptr<const Packet> packet)
{
  m_phyTxDropTrace (packet);
}

void
SlimPhy::NotifyRxBegin (Ptr<const Packet> packet)
{
  m_phyRxBeginTrace (packet);
}

void
SlimPhy::NotifyRxEnd (Ptr<const Packet> packet)
{
  m_phyRxEndTrace (packet);
}

void
SlimPhy::NotifyRxDrop (Ptr<const Packet> packet)
{
  m_phyRxDropTrace (packet);
}

void
SlimPhy::NotifyMonitorSniffRx (Ptr<const Packet> packet, SlimTxVector txvector, uint16_t channelNumber, double signalDbm, double noiseDbm)
{
  m_phyMonitorSniffRxTrace (packet, txvector, channelNumber, signalDbm, noiseDbm);
}

void
SlimPhy::NotifyMonitorSniffTx (Ptr<const Packet> packet, SlimTxVector txvector, uint16_t channelNumber, uint8_t txPower)
{
  m_phyMonitorSniffTxTrace (packet, txvector, channelNumber,  txPower);
}

Time SlimPhy::CalculateTxDuration (SlimTxVector txvector)
{
//	Time duration = MicroSeconds(0);
//
//
//	for( SlimTxVector::ListofSlimModes::iterator i = txvector.GetListofModes().begin(); i != txvector.GetListofModes().end(); ++i ){
//		duration += i->GetDuration();
//	}
//	return duration;

	return (txvector.CalculateTxDuration());
}









std::ostream& operator<< (std::ostream& os, enum SlimPhy::State state)
{
  switch (state)
    {
    case SlimPhy::IDLE:
      return (os << "IDLE");
    case SlimPhy::CCA_BUSY:
      return (os << "CCA_BUSY");
    case SlimPhy::TX:
      return (os << "TX");
    case SlimPhy::RX:
      return (os << "RX");
    case SlimPhy::SWITCHING:
      return (os << "SWITCHING");
    case SlimPhy::SLEEP:
      return (os << "SLEEP");
    default:
      NS_FATAL_ERROR ("Invalid SlimPhy state");
      return (os << "INVALID");
    }
}



} // namespace ns3

namespace {

static class Constructor
{
public:
  Constructor ()
  {
  }
} g_constructor;
}
