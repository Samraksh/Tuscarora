/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/* Copyright: The Samraksh Company
 * SLIM-WF:Synchronous Lightweight Interfernce-aware Model-driven Wavefrm
 * The usage of this module is governed by the associated license file.
 * Authors: Bora Karaoglu, Mukundan Sridharan
 */

#include "ns3/packet.h"
#include "ns3/simulator.h"
#include "ns3/mobility-model.h"
#include "ns3/net-device.h"
#include "ns3/node.h"
#include "ns3/log.h"
#include "ns3/pointer.h"
#include "ns3/object-factory.h"
#include "slim-channel.h"
#include "slim-net-device.h"
#include "tuscarora-slim-phy.h"
#include "ns3/propagation-loss-model.h"
#include "ns3/propagation-delay-model.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("SlimChannel");

NS_OBJECT_ENSURE_REGISTERED (SlimChannel);

TypeId
SlimChannel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::SlimChannel")
    .SetParent<Channel> ()
  ;
  return tid;
}

} // namespace ns3
