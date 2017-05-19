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
#ifndef TUSCARORA_SLIM_CHANNEL_H
#define TUSCARORA_SLIM_CHANNEL_H

#include <vector>
#include <stdint.h>
#include "ns3/packet.h"
#include "slim-channel.h"
#include "slim-mode.h"
#include "slim-tx-vector.h"
#include "ns3/nstime.h"

namespace ns3 {

class NetDevice;
class PropagationLossModel;
class PropagationDelayModel;
class TuscaroraSlimPhy;

/**
 * \brief A Tuscarora slim channel
 * \ingroup slim
 */
class TuscaroraSlimChannel : public SlimChannel
{
public:
  static TypeId GetTypeId (void);

  TuscaroraSlimChannel ();
  virtual ~TuscaroraSlimChannel ();

  // inherited from Channel.
  virtual uint32_t GetNDevices (void) const;
  virtual Ptr<NetDevice> GetDevice (uint32_t i) const;

  /**
   * Adds the given TuscaroraSlimPhy to the PHY list
   *
   * \param phy the TuscaroraSlimPhy to be added to the PHY list
   */
  void Add (Ptr<TuscaroraSlimPhy> phy);

  /**
   * \param loss the new propagation loss model.
   */
  void SetPropagationLossModel (Ptr<PropagationLossModel> loss);
  /**
   * \param delay the new propagation delay model.
   */
  void SetPropagationDelayModel (Ptr<PropagationDelayModel> delay);

  /**
   * \param sender the device from which the packet is originating.
   * \param packet the packet to send
   * \param txPowerDbm the tx power associated to the packet
   * \param txVector the TXVECTOR associated to the packet

   * \param packetType the type of packet, used for A-MPDU to say whether it's the last MPDU or not
   * \param duration the transmission duration associated to the packet
   *
   * This method should not be invoked by normal users. It is
   * currently invoked only from SlimPhy::Send. TuscaroraSlimChannel
   * delivers packets only between PHYs with the same m_channelNumber,
   * e.g. PHYs that are operating on the same channel.
   */
  void Send (Ptr<TuscaroraSlimPhy> sender, Ptr<const Packet> packet, double txPowerDbm,
             SlimTxVector txVector, uint8_t packetType, Time duration) const;

 /**
  * Assign a fixed random variable stream number to the random variables
  * used by this model.  Return the number of streams (possibly zero) that
  * have been assigned.
  *
  * \param stream first stream index to use
  * \return the number of stream indices assigned by this model
  */
  int64_t AssignStreams (int64_t stream);

private:
  //TuscaroraSlimChannel& operator = (const TuscaroraSlimChannel &);
  //TuscaroraSlimChannel (const TuscaroraSlimChannel &);

  /**
   * A vector of pointers to TuscaroraSlimPhy.
   */
  typedef std::vector<Ptr<TuscaroraSlimPhy> > PhyList;
  /**
   * This method is scheduled by Send for each associated TuscaroraSlimPhy.
   * The method then calls the corresponding TuscaroraSlimPhy that the first
   * bit of the packet has arrived.
   *
   * \param i index of the corresponding TuscaroraSlimPhy in the PHY list
   * \param packet the packet being sent
   * \param atts a vector containing the received power in dBm and the packet type
   * \param txVector the TXVECTOR of the packet
   */
  void Receive (uint32_t i, Ptr<Packet> packet, double *atts,
                SlimTxVector txVector) const;


  PhyList m_phyList; //!< List of TuscaroraSlimPhys connected to this TuscaroraSlimChannel
  Ptr<PropagationLossModel> m_loss; //!< Propagation loss model
  Ptr<PropagationDelayModel> m_delay; //!< Propagation delay model
};

} // namespace ns3


#endif /* TUSCARORA_SLIM_CHANNEL_H */
