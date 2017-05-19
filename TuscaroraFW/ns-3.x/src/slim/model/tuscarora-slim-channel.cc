/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2006,2007 INRIA
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
 * Author: Mathieu Lacage, <mathieu.lacage@sophia.inria.fr>
 */
#include "ns3/packet.h"
#include "ns3/simulator.h"
#include "ns3/mobility-model.h"
#include "ns3/net-device.h"
#include "ns3/node.h"
#include "ns3/log.h"
#include "ns3/pointer.h"
#include "ns3/object-factory.h"
#include "tuscarora-slim-channel.h"
#include "tuscarora-slim-phy.h"
#include "ns3/propagation-loss-model.h"
#include "ns3/propagation-delay-model.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("TuscaroraSlimChannel");

NS_OBJECT_ENSURE_REGISTERED (TuscaroraSlimChannel);

TypeId
TuscaroraSlimChannel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::TuscaroraSlimChannel")
    .SetParent<SlimChannel> ()
    .AddConstructor<TuscaroraSlimChannel> ()
    .AddAttribute ("PropagationLossModel", "A pointer to the propagation loss model attached to this channel.",
                   PointerValue (),
                   MakePointerAccessor (&TuscaroraSlimChannel::m_loss),
                   MakePointerChecker<PropagationLossModel> ())
    .AddAttribute ("PropagationDelayModel", "A pointer to the propagation delay model attached to this channel.",
                   PointerValue (),
                   MakePointerAccessor (&TuscaroraSlimChannel::m_delay),
                   MakePointerChecker<PropagationDelayModel> ())
  ;
  return tid;
}

TuscaroraSlimChannel::TuscaroraSlimChannel ()
{
}
TuscaroraSlimChannel::~TuscaroraSlimChannel ()
{
  NS_LOG_FUNCTION_NOARGS ();
  m_phyList.clear ();
}

void
TuscaroraSlimChannel::SetPropagationLossModel (Ptr<PropagationLossModel> loss)
{
  m_loss = loss;
}
void
TuscaroraSlimChannel::SetPropagationDelayModel (Ptr<PropagationDelayModel> delay)
{
  m_delay = delay;
}

void
TuscaroraSlimChannel::Send (Ptr<TuscaroraSlimPhy> sender, Ptr<const Packet> packet, double txPowerDbm,
                       SlimTxVector txVector, uint8_t packetType, Time duration) const
{
  Ptr<MobilityModel> senderMobility = sender->GetMobility ()->GetObject<MobilityModel> ();
  Ptr<AntennaModel> senderAntenna = sender->GetAntenna ()->GetObject<AntennaModel> ();
  double antennaGain;
  Angles relative_angle;
  NS_ASSERT (senderMobility != 0);
  uint32_t j = 0;
  for (PhyList::const_iterator i = m_phyList.begin (); i != m_phyList.end (); i++, j++)
    {
      if (sender != (*i))
        {
          // For now don't account for inter channel interference
          if ((*i)->GetChannelNumber () != sender->GetChannelNumber ())
            {
              continue;
            }

          Ptr<MobilityModel> receiverMobility = (*i)->GetMobility ()->GetObject<MobilityModel> ();
          Ptr<AntennaModel> receiverAntenna = (*i)->GetAntenna ()->GetObject<AntennaModel> ();
          Time delay = m_delay->GetDelay (senderMobility, receiverMobility);
          antennaGain = senderAntenna->GetGainDb(Angles(senderMobility->GetPosition(), receiverMobility->GetPosition())) + receiverAntenna->GetGainDb(Angles(receiverMobility->GetPosition(), senderMobility->GetPosition()));
          double rxPowerDbm = m_loss->CalcRxPower (txPowerDbm + antennaGain, senderMobility, receiverMobility);
          NS_LOG_DEBUG ("propagation: txPower=" << txPowerDbm << "dbm, rxPower=" << rxPowerDbm << "dbm, " <<
                        "distance=" << senderMobility->GetDistanceFrom (receiverMobility) << "m, delay=" << delay);
          Ptr<Packet> copy = packet->Copy ();
          Ptr<Object> dstNetDevice = m_phyList[j]->GetDevice ();
          uint32_t dstNode;
          if (dstNetDevice == 0)
            {
              dstNode = 0xffffffff;
            }
          else
            {
              dstNode = dstNetDevice->GetObject<NetDevice> ()->GetNode ()->GetId ();
            }

          double *atts = new double[3];
          *atts = rxPowerDbm;
          *(atts+1)= packetType;
          *(atts+2)= duration.GetNanoSeconds();

          Simulator::ScheduleWithContext (dstNode,
                                          delay, &TuscaroraSlimChannel::Receive, this,
                                          j, copy, atts, txVector);
        }
    }
}

void
TuscaroraSlimChannel::Receive (uint32_t i, Ptr<Packet> packet, double *atts,
                          SlimTxVector txVector) const
{
  m_phyList[i]->StartReceivePacket (packet, *atts, txVector,*(atts+1), NanoSeconds(*(atts+2)));
  delete[] atts;
}

uint32_t
TuscaroraSlimChannel::GetNDevices (void) const
{
  return m_phyList.size ();
}
Ptr<NetDevice>
TuscaroraSlimChannel::GetDevice (uint32_t i) const
{
  return m_phyList[i]->GetDevice ()->GetObject<NetDevice> ();
}

void
TuscaroraSlimChannel::Add (Ptr<TuscaroraSlimPhy> phy)
{
  m_phyList.push_back (phy);
}

int64_t
TuscaroraSlimChannel::AssignStreams (int64_t stream)
{
  int64_t currentStream = stream;
  currentStream += m_loss->AssignStreams (stream);
  return (currentStream - stream);
}

} // namespace ns3
