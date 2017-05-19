/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/* Copyright: The Samraksh Company
 * SLIM-WF:Synchronous Lightweight Interfernce-aware Model-driven Wavefrm
 * The usage of this module is governed by the associated license file.
 * Authors: Bora Karaoglu, Mukundan Sridharan
 */

#ifndef SLIM_CHANNEL_H
#define SLIM_CHANNEL_H

#include "ns3/channel.h"

namespace ns3 {

class SlimNetDevice;
class SlimPhy;

/**
 * \brief Slim Channel interface specification
 * \ingroup slim
 *
 * This class works in tandem with the ns3::SlimPhy class. If you want to
 * provide a new Slim PHY layer, you have to subclass both ns3::SlimChannel
 * and ns3::SlimPhy.
 *
 * Typically, MySlimChannel will define a Send method whose job is to distribute
 * packets from a MySlimPhy source to a set of MySlimPhy destinations. MySlimPhy
 * also typically defines a Receive method which is invoked by MySlimPhy.
 */
class SlimChannel : public Channel
{
public:
  static TypeId GetTypeId (void);
};

} // namespace ns3


#endif /* SLIM_CHANNEL_H */
