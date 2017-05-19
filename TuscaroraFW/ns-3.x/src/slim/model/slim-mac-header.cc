/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2006, 2009 INRIA
 * Copyright (c) 2009 MIRKO BANCHI
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
 * Author: Mirko Banchi <mk.banchi@gmail.com>
 */
#include "ns3/assert.h"
#include "ns3/address-utils.h"
#include "slim-mac-header.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (SlimMacHeader);

enum
{
  TYPE_MGT = 0,
  TYPE_CTL  = 1,
  TYPE_DATA = 2
};

enum
{
  SUBTYPE_CTL_BACKREQ = 8,
  SUBTYPE_CTL_BACKRESP = 9,
  SUBTYPE_CTL_RTS = 11,
  SUBTYPE_CTL_CTS = 12,
  SUBTYPE_CTL_ACK = 13,
  SUBTYPE_CTL_CTLWRAPPER=7

};

SlimMacHeader::SlimMacHeader ()
  :
    m_ctrlMoreData (0),
    m_ctrlWep (0),
    m_ctrlOrder (1),
    m_amsduPresent (0)
{
}
SlimMacHeader::~SlimMacHeader ()
{
}

void
SlimMacHeader::SetDsFrom (void)
{
  m_ctrlFromDs = 1;
}
void
SlimMacHeader::SetDsNotFrom (void)
{
  m_ctrlFromDs = 0;
}
void
SlimMacHeader::SetDsTo (void)
{
  m_ctrlToDs = 1;
}
void
SlimMacHeader::SetDsNotTo (void)
{
  m_ctrlToDs = 0;
}

void
SlimMacHeader::SetAddr1 (Mac48Address address)
{
  m_addr1 = address;
}
void
SlimMacHeader::SetAddr2 (Mac48Address address)
{
  m_addr2 = address;
}
void
SlimMacHeader::SetAddr3 (Mac48Address address)
{
  m_addr3 = address;
}
void
SlimMacHeader::SetAddr4 (Mac48Address address)
{
  m_addr4 = address;
}
void
SlimMacHeader::SetAssocReq (void)
{
  m_ctrlType = TYPE_MGT;
  m_ctrlSubtype = 0;
}
void
SlimMacHeader::SetAssocResp (void)
{
  m_ctrlType = TYPE_MGT;
  m_ctrlSubtype = 1;
}
void
SlimMacHeader::SetProbeReq (void)
{
  m_ctrlType = TYPE_MGT;
  m_ctrlSubtype = 4;
}
void
SlimMacHeader::SetProbeResp (void)
{
  m_ctrlType = TYPE_MGT;
  m_ctrlSubtype = 5;
}
void
SlimMacHeader::SetBeacon (void)
{
  m_ctrlType = TYPE_MGT;
  m_ctrlSubtype = 8;
}
void
SlimMacHeader::SetBlockAckReq (void)
{
  m_ctrlType = TYPE_CTL;
  m_ctrlSubtype = 8;
}
void
SlimMacHeader::SetBlockAck (void)
{
  m_ctrlType = TYPE_CTL;
  m_ctrlSubtype = 9;
}

void
SlimMacHeader::SetTypeData (void)
{
  m_ctrlType = TYPE_DATA;
  m_ctrlSubtype = 0;
}
void
SlimMacHeader::SetAction (void)
{
  m_ctrlType = TYPE_MGT;
  m_ctrlSubtype = 0x0D;
}
void
SlimMacHeader::SetMultihopAction (void)
{
  m_ctrlType = TYPE_MGT;
  m_ctrlSubtype = 0x0F;
}
void
SlimMacHeader::SetType (enum SlimMacType type)
{
  switch (type)
    {
    case SLIM_MAC_CTL_BACKREQ:
      m_ctrlType = TYPE_CTL;
      m_ctrlSubtype = SUBTYPE_CTL_BACKREQ;
      break;
    case SLIM_MAC_CTL_BACKRESP:
      m_ctrlType = TYPE_CTL;
      m_ctrlSubtype = SUBTYPE_CTL_BACKRESP;
      break;
    case SLIM_MAC_CTL_RTS:
      m_ctrlType = TYPE_CTL;
      m_ctrlSubtype = SUBTYPE_CTL_RTS;
      break;
    case SLIM_MAC_CTL_CTS:
      m_ctrlType = TYPE_CTL;
      m_ctrlSubtype = SUBTYPE_CTL_CTS;
      break;
    case SLIM_MAC_CTL_ACK:
      m_ctrlType = TYPE_CTL;
      m_ctrlSubtype = SUBTYPE_CTL_ACK;
      break;
    case SLIM_MAC_CTL_CTLWRAPPER:
      m_ctrlType = TYPE_CTL;
      m_ctrlSubtype = SUBTYPE_CTL_CTLWRAPPER;
      break;
    case SLIM_MAC_MGT_ASSOCIATION_REQUEST:
      m_ctrlType = TYPE_MGT;
      m_ctrlSubtype = 0;
      break;
    case SLIM_MAC_MGT_ASSOCIATION_RESPONSE:
      m_ctrlType = TYPE_MGT;
      m_ctrlSubtype = 1;
      break;
    case SLIM_MAC_MGT_REASSOCIATION_REQUEST:
      m_ctrlType = TYPE_MGT;
      m_ctrlSubtype = 2;
      break;
    case SLIM_MAC_MGT_REASSOCIATION_RESPONSE:
      m_ctrlType = TYPE_MGT;
      m_ctrlSubtype = 3;
      break;
    case SLIM_MAC_MGT_PROBE_REQUEST:
      m_ctrlType = TYPE_MGT;
      m_ctrlSubtype = 4;
      break;
    case SLIM_MAC_MGT_PROBE_RESPONSE:
      m_ctrlType = TYPE_MGT;
      m_ctrlSubtype = 5;
      break;
    case SLIM_MAC_MGT_BEACON:
      m_ctrlType = TYPE_MGT;
      m_ctrlSubtype = 8;
      break;
    case SLIM_MAC_MGT_DISASSOCIATION:
      m_ctrlType = TYPE_MGT;
      m_ctrlSubtype = 10;
      break;
    case SLIM_MAC_MGT_AUTHENTICATION:
      m_ctrlType = TYPE_MGT;
      m_ctrlSubtype = 11;
      break;
    case SLIM_MAC_MGT_DEAUTHENTICATION:
      m_ctrlType = TYPE_MGT;
      m_ctrlSubtype = 12;
      break;
    case SLIM_MAC_MGT_ACTION:
      m_ctrlType = TYPE_MGT;
      m_ctrlSubtype = 13;
      break;
    case SLIM_MAC_MGT_ACTION_NO_ACK:
      m_ctrlType = TYPE_MGT;
      m_ctrlSubtype = 14;
      break;
    case SLIM_MAC_MGT_MULTIHOP_ACTION:
      m_ctrlType = TYPE_MGT;
      m_ctrlSubtype = 15;
      break;

    case SLIM_MAC_DATA:
      m_ctrlType = TYPE_DATA;
      m_ctrlSubtype = 0;
      break;
    case SLIM_MAC_DATA_CFACK:
      m_ctrlType = TYPE_DATA;
      m_ctrlSubtype = 1;
      break;
    case SLIM_MAC_DATA_CFPOLL:
      m_ctrlType = TYPE_DATA;
      m_ctrlSubtype = 2;
      break;
    case SLIM_MAC_DATA_CFACK_CFPOLL:
      m_ctrlType = TYPE_DATA;
      m_ctrlSubtype = 3;
      break;
    case SLIM_MAC_DATA_NULL:
      m_ctrlType = TYPE_DATA;
      m_ctrlSubtype = 4;
      break;
    case SLIM_MAC_DATA_NULL_CFACK:
      m_ctrlType = TYPE_DATA;
      m_ctrlSubtype = 5;
      break;
    case SLIM_MAC_DATA_NULL_CFPOLL:
      m_ctrlType = TYPE_DATA;
      m_ctrlSubtype = 6;
      break;
    case SLIM_MAC_DATA_NULL_CFACK_CFPOLL:
      m_ctrlType = TYPE_DATA;
      m_ctrlSubtype = 7;
      break;
    case SLIM_MAC_QOSDATA:
      m_ctrlType = TYPE_DATA;
      m_ctrlSubtype = 8;
      break;
    case SLIM_MAC_QOSDATA_CFACK:
      m_ctrlType = TYPE_DATA;
      m_ctrlSubtype = 9;
      break;
    case SLIM_MAC_QOSDATA_CFPOLL:
      m_ctrlType = TYPE_DATA;
      m_ctrlSubtype = 10;
      break;
    case SLIM_MAC_QOSDATA_CFACK_CFPOLL:
      m_ctrlType = TYPE_DATA;
      m_ctrlSubtype = 11;
      break;
    case SLIM_MAC_QOSDATA_NULL:
      m_ctrlType = TYPE_DATA;
      m_ctrlSubtype = 12;
      break;
    case SLIM_MAC_QOSDATA_NULL_CFPOLL:
      m_ctrlType = TYPE_DATA;
      m_ctrlSubtype = 14;
      break;
    case SLIM_MAC_QOSDATA_NULL_CFACK_CFPOLL:
      m_ctrlType = TYPE_DATA;
      m_ctrlSubtype = 15;
      break;
    }
  m_ctrlToDs = 0;
  m_ctrlFromDs = 0;
}
void
SlimMacHeader::SetRawDuration (uint16_t duration)
{
  m_duration = duration;
}
void
SlimMacHeader::SetDuration (Time duration)
{
  int64_t duration_us = ceil((double)duration.GetNanoSeconds ()/1000);
  NS_ASSERT (duration_us >= 0 && duration_us <= 0x7fff);
  m_duration = static_cast<uint16_t> (duration_us);
}

void SlimMacHeader::SetId (uint16_t id)
{
  m_duration = id;
}
void SlimMacHeader::SetSequenceNumber (uint16_t seq)
{
  m_seqSeq = seq;
}
void SlimMacHeader::SetFragmentNumber (uint8_t frag)
{
  m_seqFrag = frag;
}
void SlimMacHeader::SetNoMoreFragments (void)
{
  m_ctrlMoreFrag = 0;
}
void SlimMacHeader::SetMoreFragments (void)
{
  m_ctrlMoreFrag = 1;
}
void SlimMacHeader::SetOrder (void)
{
  m_ctrlOrder = 1;
}
void SlimMacHeader::SetNoOrder (void)
{
  m_ctrlOrder = 0;
}
void SlimMacHeader::SetRetry (void)
{
  m_ctrlRetry = 1;
}
void SlimMacHeader::SetNoRetry (void)
{
  m_ctrlRetry = 0;
}
void SlimMacHeader::SetQosTid (uint8_t tid)
{
  m_qosTid = tid;
}
void SlimMacHeader::SetQosEosp ()
{
  m_qosEosp = 1;
}
void SlimMacHeader::SetQosNoEosp ()
{
  m_qosEosp = 0;
}
void SlimMacHeader::SetQosAckPolicy (enum QosAckPolicy policy)
{
  switch (policy)
    {
    case NORMAL_ACK:
      m_qosAckPolicy = 0;
      break;
    case NO_ACK:
      m_qosAckPolicy = 1;
      break;
    case NO_EXPLICIT_ACK:
      m_qosAckPolicy = 2;
      break;
    case BLOCK_ACK:
      m_qosAckPolicy = 3;
      break;
    }
}
void
SlimMacHeader::SetQosNormalAck ()
{
  m_qosAckPolicy = 0;
}
void
SlimMacHeader::SetQosBlockAck ()
{
  m_qosAckPolicy = 3;
}
void
SlimMacHeader::SetQosNoAck ()
{
  m_qosAckPolicy = 1;
}
void SlimMacHeader::SetQosAmsdu (void)
{
  m_amsduPresent = 1;
}
void SlimMacHeader::SetQosNoAmsdu (void)
{
  m_amsduPresent = 0;
}
void SlimMacHeader::SetQosTxopLimit (uint8_t txop)
{
  m_qosStuff = txop;
}

Mac48Address
SlimMacHeader::GetAddr1 (void) const
{
  return m_addr1;
}
Mac48Address
SlimMacHeader::GetAddr2 (void) const
{
  return m_addr2;
}
Mac48Address
SlimMacHeader::GetAddr3 (void) const
{
  return m_addr3;
}
Mac48Address
SlimMacHeader::GetAddr4 (void) const
{
  return m_addr4;
}

enum SlimMacType
SlimMacHeader::GetType (void) const
{
  switch (m_ctrlType)
    {
    case TYPE_MGT:
      switch (m_ctrlSubtype)
        {
        case 0:
          return SLIM_MAC_MGT_ASSOCIATION_REQUEST;
          break;
        case 1:
          return SLIM_MAC_MGT_ASSOCIATION_RESPONSE;
          break;
        case 2:
          return SLIM_MAC_MGT_REASSOCIATION_REQUEST;
          break;
        case 3:
          return SLIM_MAC_MGT_REASSOCIATION_RESPONSE;
          break;
        case 4:
          return SLIM_MAC_MGT_PROBE_REQUEST;
          break;
        case 5:
          return SLIM_MAC_MGT_PROBE_RESPONSE;
          break;
        case 8:
          return SLIM_MAC_MGT_BEACON;
          break;
        case 10:
          return SLIM_MAC_MGT_DISASSOCIATION;
          break;
        case 11:
          return SLIM_MAC_MGT_AUTHENTICATION;
          break;
        case 12:
          return SLIM_MAC_MGT_DEAUTHENTICATION;
          break;
        case 13:
          return SLIM_MAC_MGT_ACTION;
          break;
        case 14:
          return SLIM_MAC_MGT_ACTION_NO_ACK;
          break;
        case 15:
          return SLIM_MAC_MGT_MULTIHOP_ACTION;
          break;

        }
      break;
    case TYPE_CTL:
      switch (m_ctrlSubtype)
        {
        case SUBTYPE_CTL_BACKREQ:
          return SLIM_MAC_CTL_BACKREQ;
          break;
        case SUBTYPE_CTL_BACKRESP:
          return SLIM_MAC_CTL_BACKRESP;
          break;
        case SUBTYPE_CTL_RTS:
          return SLIM_MAC_CTL_RTS;
          break;
        case SUBTYPE_CTL_CTS:
          return SLIM_MAC_CTL_CTS;
          break;
        case SUBTYPE_CTL_ACK:
          return SLIM_MAC_CTL_ACK;
          break;
        }
      break;
    case TYPE_DATA:
      switch (m_ctrlSubtype)
        {
        case 0:
          return SLIM_MAC_DATA;
          break;
        case 1:
          return SLIM_MAC_DATA_CFACK;
          break;
        case 2:
          return SLIM_MAC_DATA_CFPOLL;
          break;
        case 3:
          return SLIM_MAC_DATA_CFACK_CFPOLL;
          break;
        case 4:
          return SLIM_MAC_DATA_NULL;
          break;
        case 5:
          return SLIM_MAC_DATA_NULL_CFACK;
          break;
        case 6:
          return SLIM_MAC_DATA_NULL_CFPOLL;
          break;
        case 7:
          return SLIM_MAC_DATA_NULL_CFACK_CFPOLL;
          break;
        case 8:
          return SLIM_MAC_QOSDATA;
          break;
        case 9:
          return SLIM_MAC_QOSDATA_CFACK;
          break;
        case 10:
          return SLIM_MAC_QOSDATA_CFPOLL;
          break;
        case 11:
          return SLIM_MAC_QOSDATA_CFACK_CFPOLL;
          break;
        case 12:
          return SLIM_MAC_QOSDATA_NULL;
          break;
        case 14:
          return SLIM_MAC_QOSDATA_NULL_CFPOLL;
          break;
        case 15:
          return SLIM_MAC_QOSDATA_NULL_CFACK_CFPOLL;
          break;

        }
      break;
    }
  // NOTREACHED
  NS_ASSERT (false);
  return (enum SlimMacType)-1;
}
bool
SlimMacHeader::IsFromDs (void) const
{
  return m_ctrlFromDs == 1;
}
bool
SlimMacHeader::IsToDs (void) const
{
  return m_ctrlToDs == 1;
}

bool
SlimMacHeader::IsData (void) const
{
  return (m_ctrlType == TYPE_DATA);

}
bool
SlimMacHeader::IsQosData (void) const
{
  return (m_ctrlType == TYPE_DATA && (m_ctrlSubtype & 0x08));
}
bool
SlimMacHeader::IsCtl (void) const
{
  return (m_ctrlType == TYPE_CTL);
}
bool
SlimMacHeader::IsMgt (void) const
{
  return (m_ctrlType == TYPE_MGT);
}
bool
SlimMacHeader::IsCfpoll (void) const
{
  switch (GetType ())
    {
    case SLIM_MAC_DATA_CFPOLL:
    case SLIM_MAC_DATA_CFACK_CFPOLL:
    case SLIM_MAC_DATA_NULL_CFPOLL:
    case SLIM_MAC_DATA_NULL_CFACK_CFPOLL:
    case SLIM_MAC_QOSDATA_CFPOLL:
    case SLIM_MAC_QOSDATA_CFACK_CFPOLL:
    case SLIM_MAC_QOSDATA_NULL_CFPOLL:
    case SLIM_MAC_QOSDATA_NULL_CFACK_CFPOLL:
      return true;
      break;
    default:
      return false;
      break;
    }
}
bool
SlimMacHeader::IsRts (void) const
{
  return (GetType () == SLIM_MAC_CTL_RTS);
}
bool
SlimMacHeader::IsCts (void) const
{
  return (GetType () == SLIM_MAC_CTL_CTS);
}
bool
SlimMacHeader::IsAck (void) const
{
  return (GetType () == SLIM_MAC_CTL_ACK);
}
bool
SlimMacHeader::IsAssocReq (void) const
{
  return (GetType () == SLIM_MAC_MGT_ASSOCIATION_REQUEST);
}
bool
SlimMacHeader::IsAssocResp (void) const
{
  return (GetType () == SLIM_MAC_MGT_ASSOCIATION_RESPONSE);
}
bool
SlimMacHeader::IsReassocReq (void) const
{
  return (GetType () == SLIM_MAC_MGT_REASSOCIATION_REQUEST);
}
bool
SlimMacHeader::IsReassocResp (void) const
{
  return (GetType () == SLIM_MAC_MGT_REASSOCIATION_RESPONSE);
}
bool
SlimMacHeader::IsProbeReq (void) const
{
  return (GetType () == SLIM_MAC_MGT_PROBE_REQUEST);
}
bool
SlimMacHeader::IsProbeResp (void) const
{
  return (GetType () == SLIM_MAC_MGT_PROBE_RESPONSE);
}
bool
SlimMacHeader::IsBeacon (void) const
{
  return (GetType () == SLIM_MAC_MGT_BEACON);
}
bool
SlimMacHeader::IsDisassociation (void) const
{
  return (GetType () == SLIM_MAC_MGT_DISASSOCIATION);
}
bool
SlimMacHeader::IsAuthentication (void) const
{
  return (GetType () == SLIM_MAC_MGT_AUTHENTICATION);
}
bool
SlimMacHeader::IsDeauthentication (void) const
{
  return (GetType () == SLIM_MAC_MGT_DEAUTHENTICATION);
}
bool
SlimMacHeader::IsAction (void) const
{
  return (GetType () == SLIM_MAC_MGT_ACTION);
}
bool
SlimMacHeader::IsMultihopAction (void) const
{
  return (GetType () == SLIM_MAC_MGT_MULTIHOP_ACTION);
}
bool
SlimMacHeader::IsBlockAckReq (void) const
{
  return (GetType () == SLIM_MAC_CTL_BACKREQ) ? true : false;
}
bool
SlimMacHeader::IsBlockAck (void) const
{
  return (GetType () == SLIM_MAC_CTL_BACKRESP) ? true : false;
}


uint16_t
SlimMacHeader::GetRawDuration (void) const
{
  return m_duration;
}
Time
SlimMacHeader::GetDuration (void) const
{
  return MicroSeconds (m_duration);
}
uint16_t
SlimMacHeader::GetSequenceControl (void) const
{
  return (m_seqSeq << 4) | m_seqFrag;
}
uint16_t
SlimMacHeader::GetSequenceNumber (void) const
{
  return m_seqSeq;
}
uint16_t
SlimMacHeader::GetFragmentNumber (void) const
{
  return m_seqFrag;
}
bool
SlimMacHeader::IsRetry (void) const
{
  return (m_ctrlRetry == 1);
}
bool
SlimMacHeader::IsMoreFragments (void) const
{
  return (m_ctrlMoreFrag == 1);
}
bool
SlimMacHeader::IsQosBlockAck (void) const
{
  NS_ASSERT (IsQosData ());
  return (m_qosAckPolicy == 3);
}
bool
SlimMacHeader::IsQosNoAck (void) const
{
  NS_ASSERT (IsQosData ());
  return (m_qosAckPolicy == 1);
}
bool
SlimMacHeader::IsQosAck (void) const
{
  NS_ASSERT (IsQosData ());
  return (m_qosAckPolicy == 0);
}
bool
SlimMacHeader::IsQosEosp (void) const
{
  NS_ASSERT (IsQosData ());
  return (m_qosEosp == 1);
}
bool
SlimMacHeader::IsQosAmsdu (void) const
{
  NS_ASSERT (IsQosData ());
  return (m_amsduPresent == 1);
}
uint8_t
SlimMacHeader::GetQosTid (void) const
{
  NS_ASSERT (IsQosData ());
  return m_qosTid;
}
enum SlimMacHeader::QosAckPolicy
SlimMacHeader::GetQosAckPolicy (void) const
{
  switch (m_qosAckPolicy)
    {
    case 0:
      return NORMAL_ACK;
      break;
    case 1:
      return NO_ACK;
      break;
    case 2:
      return NO_EXPLICIT_ACK;
      break;
    case 3:
      return BLOCK_ACK;
      break;
    }
  // NOTREACHED
  NS_ASSERT (false);
  return (enum QosAckPolicy)-1;
}

uint8_t
SlimMacHeader::GetQosTxopLimit (void) const
{
  NS_ASSERT (IsQosData ());
  return m_qosStuff;
}

uint16_t
SlimMacHeader::GetFrameControl (void) const
{
  uint16_t val = 0;
  val |= (m_ctrlType << 2) & (0x3 << 2);
  val |= (m_ctrlSubtype << 4) & (0xf << 4);
  val |= (m_ctrlToDs << 8) & (0x1 << 8);
  val |= (m_ctrlFromDs << 9) & (0x1 << 9);
  val |= (m_ctrlMoreFrag << 10) & (0x1 << 10);
  val |= (m_ctrlRetry << 11) & (0x1 << 11);
  val |= (m_ctrlMoreData << 13) & (0x1 << 13);
  val |= (m_ctrlWep << 14) & (0x1 << 14);
  val |= (m_ctrlOrder << 15) & (0x1 << 15);
  return val;
}

uint16_t
SlimMacHeader::GetQosControl (void) const
{
  uint16_t val = 0;
  val |= m_qosTid;
  val |= m_qosEosp << 4;
  val |= m_qosAckPolicy << 5;
  val |= m_amsduPresent << 7;
  val |= m_qosStuff << 8;
  return val;
}

void
SlimMacHeader::SetFrameControl (uint16_t ctrl)
{
  m_ctrlType = (ctrl >> 2) & 0x03;
  m_ctrlSubtype = (ctrl >> 4) & 0x0f;
  m_ctrlToDs = (ctrl >> 8) & 0x01;
  m_ctrlFromDs = (ctrl >> 9) & 0x01;
  m_ctrlMoreFrag = (ctrl >> 10) & 0x01;
  m_ctrlRetry = (ctrl >> 11) & 0x01;
  m_ctrlMoreData = (ctrl >> 13) & 0x01;
  m_ctrlWep = (ctrl >> 14) & 0x01;
  m_ctrlOrder = (ctrl >> 15) & 0x01;
}
void
SlimMacHeader::SetSequenceControl (uint16_t seq)
{
  m_seqFrag = seq & 0x0f;
  m_seqSeq = (seq >> 4) & 0x0fff;
}
void
SlimMacHeader::SetQosControl (uint16_t qos)
{
  m_qosTid = qos & 0x000f;
  m_qosEosp = (qos >> 4) & 0x0001;
  m_qosAckPolicy = (qos >> 5) & 0x0003;
  m_amsduPresent = (qos >> 7) & 0x0001;
  m_qosStuff = (qos >> 8) & 0x00ff;
}

uint32_t
SlimMacHeader::GetSize (void) const
{
  uint32_t size = 0;
  switch (m_ctrlType)
    {
    case TYPE_MGT:
      size = 2 + 2 + 6 + 6 + 6 + 2;
      break;
    case TYPE_CTL:
      switch (m_ctrlSubtype)
        {
        case SUBTYPE_CTL_RTS:
          size = 2 + 2 + 6 + 6;
          break;
        case SUBTYPE_CTL_CTS:
        case SUBTYPE_CTL_ACK:
          size = 2 + 2 + 6;
          break;
        case SUBTYPE_CTL_BACKREQ:
        case SUBTYPE_CTL_BACKRESP:
          size = 2 + 2 + 6 + 6;
          break;
        case SUBTYPE_CTL_CTLWRAPPER:
          size = 2 +2 +6 +2 +4;
          break;
        }
      break;
    case TYPE_DATA:
      size = 2 + 2 + 6 + 6 + 6 + 2;
      if (m_ctrlToDs && m_ctrlFromDs)
        {
          size += 6;
        }
      if (m_ctrlSubtype & 0x08)
        {
          size += 2;
        }
      break;
    }
  return size;
}
const char *
SlimMacHeader::GetTypeString (void) const
{
#define FOO(x) \
case SLIM_MAC_ ## x: \
  return # x; \
  break;

  switch (GetType ())
    {
      FOO (CTL_RTS);
      FOO (CTL_CTS);
      FOO (CTL_ACK);
      FOO (CTL_BACKREQ);
      FOO (CTL_BACKRESP);

      FOO (MGT_BEACON);
      FOO (MGT_ASSOCIATION_REQUEST);
      FOO (MGT_ASSOCIATION_RESPONSE);
      FOO (MGT_DISASSOCIATION);
      FOO (MGT_REASSOCIATION_REQUEST);
      FOO (MGT_REASSOCIATION_RESPONSE);
      FOO (MGT_PROBE_REQUEST);
      FOO (MGT_PROBE_RESPONSE);
      FOO (MGT_AUTHENTICATION);
      FOO (MGT_DEAUTHENTICATION);
      FOO (MGT_ACTION);
      FOO (MGT_ACTION_NO_ACK);
      FOO (MGT_MULTIHOP_ACTION);

      FOO (DATA);
      FOO (DATA_CFACK);
      FOO (DATA_CFPOLL);
      FOO (DATA_CFACK_CFPOLL);
      FOO (DATA_NULL);
      FOO (DATA_NULL_CFACK);
      FOO (DATA_NULL_CFPOLL);
      FOO (DATA_NULL_CFACK_CFPOLL);
      FOO (QOSDATA);
      FOO (QOSDATA_CFACK);
      FOO (QOSDATA_CFPOLL);
      FOO (QOSDATA_CFACK_CFPOLL);
      FOO (QOSDATA_NULL);
      FOO (QOSDATA_NULL_CFPOLL);
      FOO (QOSDATA_NULL_CFACK_CFPOLL);
    default:
      return "ERROR";
    }
#undef FOO
  // needed to make gcc 4.0.1 ppc darwin happy.
  return "BIG_ERROR";
}

TypeId
SlimDbmHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::SlimDbmHeader")
    .SetParent<Header> ()
    .AddConstructor<SlimDbmHeader> ()
  ;
  return tid;
}
TypeId
SlimMacHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::SlimMacHeader")
    .SetParent<Header> ()
    .AddConstructor<SlimMacHeader> ()
  ;
  return tid;
}

TypeId
SlimDbmHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

TypeId
SlimMacHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void
SlimMacHeader::PrintFrameControl (std::ostream &os) const
{
  os << "ToDS=" << std::hex << (int) m_ctrlToDs << ", FromDS=" << std::hex << (int) m_ctrlFromDs
     << ", MoreFrag=" <<  std::hex << (int) m_ctrlMoreFrag << ", Retry=" << std::hex << (int) m_ctrlRetry
     << ", MoreData=" <<  std::hex << (int) m_ctrlMoreData << std::dec
  ;
}

void
SlimMacHeader::Print (std::ostream &os) const
{
  os << GetTypeString () << " ";
  switch (GetType ())
    {
    case SLIM_MAC_CTL_RTS:
      os << "Duration/ID=" << m_duration << "us"
         << ", RA=" << m_addr1 << ", TA=" << m_addr2;
      break;
    case SLIM_MAC_CTL_CTS:
    case SLIM_MAC_CTL_ACK:
      os << "Duration/ID=" << m_duration << "us"
         << ", RA=" << m_addr1;
      break;
    case SLIM_MAC_CTL_BACKREQ:
      break;
    case SLIM_MAC_CTL_BACKRESP:
      break;
    case SLIM_MAC_CTL_CTLWRAPPER:
      break;

    case SLIM_MAC_MGT_BEACON:
    case SLIM_MAC_MGT_ASSOCIATION_REQUEST:
    case SLIM_MAC_MGT_ASSOCIATION_RESPONSE:
    case SLIM_MAC_MGT_DISASSOCIATION:
    case SLIM_MAC_MGT_REASSOCIATION_REQUEST:
    case SLIM_MAC_MGT_REASSOCIATION_RESPONSE:
    case SLIM_MAC_MGT_PROBE_REQUEST:
    case SLIM_MAC_MGT_PROBE_RESPONSE:
    case SLIM_MAC_MGT_AUTHENTICATION:
    case SLIM_MAC_MGT_DEAUTHENTICATION:
      PrintFrameControl (os);
      os << " Duration/ID=" << m_duration << "us"
         << ", DA=" << m_addr1 << ", SA=" << m_addr2
         << ", BSSID=" << m_addr3 << ", FragNumber=" << std::hex << (int) m_seqFrag << std::dec
         << ", SeqNumber=" << m_seqSeq;
      break;
    case SLIM_MAC_MGT_ACTION:
    case SLIM_MAC_MGT_ACTION_NO_ACK:
      PrintFrameControl (os);
      os << " Duration/ID=" << m_duration << "us"
         << "DA=" << m_addr1 << ", SA=" << m_addr2 << ", BSSID=" << m_addr3
         << ", FragNumber=" << std::hex << (int) m_seqFrag << std::dec << ", SeqNumber=" << m_seqSeq;
      break;
    case SLIM_MAC_MGT_MULTIHOP_ACTION:
      os << " Duration/ID=" << m_duration << "us"
         << "RA=" << m_addr1 << ", TA=" << m_addr2 << ", DA=" << m_addr3
         << ", FragNumber=" << std::hex << (int) m_seqFrag << std::dec << ", SeqNumber=" << m_seqSeq;
      break;
    case SLIM_MAC_DATA:
      PrintFrameControl (os);
      os << " Duration/ID=" << m_duration << "us";
      if (!m_ctrlToDs && !m_ctrlFromDs)
        {
          os << "DA=" << m_addr1 << ", SA=" << m_addr2 << ", BSSID=" << m_addr3;
        }
      else if (!m_ctrlToDs && m_ctrlFromDs)
        {
          os << "DA=" << m_addr1 << ", SA=" << m_addr3 << ", BSSID=" << m_addr2;
        }
      else if (m_ctrlToDs && !m_ctrlFromDs)
        {
          os << "DA=" << m_addr3 << ", SA=" << m_addr2 << ", BSSID=" << m_addr1;
        }
      else if (m_ctrlToDs && m_ctrlFromDs)
        {
          os << "DA=" << m_addr3 << ", SA=" << m_addr4 << ", RA=" << m_addr1 << ", TA=" << m_addr2;
        }
      else
        {
          NS_FATAL_ERROR ("Impossible ToDs and FromDs flags combination");
        }
      os << ", FragNumber=" << std::hex << (int) m_seqFrag << std::dec
         << ", SeqNumber=" << m_seqSeq;
      break;
    case SLIM_MAC_DATA_CFACK:
    case SLIM_MAC_DATA_CFPOLL:
    case SLIM_MAC_DATA_CFACK_CFPOLL:
    case SLIM_MAC_DATA_NULL:
    case SLIM_MAC_DATA_NULL_CFACK:
    case SLIM_MAC_DATA_NULL_CFPOLL:
    case SLIM_MAC_DATA_NULL_CFACK_CFPOLL:
    case SLIM_MAC_QOSDATA:
    case SLIM_MAC_QOSDATA_CFACK:
    case SLIM_MAC_QOSDATA_CFPOLL:
    case SLIM_MAC_QOSDATA_CFACK_CFPOLL:
    case SLIM_MAC_QOSDATA_NULL:
    case SLIM_MAC_QOSDATA_NULL_CFPOLL:
    case SLIM_MAC_QOSDATA_NULL_CFACK_CFPOLL:
      break;
    }
}
uint32_t
SlimMacHeader::GetSerializedSize (void) const
{
  return GetSize ();
}
uint32_t
SlimDbmHeader::GetSerializedSize (void) const
{
  return 2 + sizeof m_data;
}
 void 
 SlimDbmHeader::Serialize (Buffer::Iterator start) const
 {
   // The 2 byte-constant
   start.WriteU8 (0xdb);
   start.WriteU8 (0xdb);
   // The data.
   start.Write((uint8_t*)(&m_data), sizeof m_data);
 }
   uint32_t 
   SlimDbmHeader::Deserialize (Buffer::Iterator start)
   {
     uint8_t tmp;
     tmp = start.ReadU8 ();
     NS_ASSERT (tmp == 0xdb);
     tmp = start.ReadU8 ();
     NS_ASSERT (tmp == 0xdb);
     start.Read((uint8_t*)(&m_data), sizeof m_data);
     return 2 + sizeof m_data; // the number of bytes consumed.
   }
   void 
   SlimDbmHeader::Print (std::ostream &os) const
   {
     os << "data=" << m_data;
   }
void
SlimMacHeader::Serialize (Buffer::Iterator i) const
{
  i.WriteHtolsbU16 (GetFrameControl ());
  i.WriteHtolsbU16 (m_duration);
  WriteTo (i, m_addr1);
  switch (m_ctrlType)
    {
    case TYPE_MGT:
      WriteTo (i, m_addr2);
      WriteTo (i, m_addr3);
      i.WriteHtolsbU16 (GetSequenceControl ());
      break;
    case TYPE_CTL:
      switch (m_ctrlSubtype)
        {
        case SUBTYPE_CTL_RTS:
          WriteTo (i, m_addr2);
          break;
        case SUBTYPE_CTL_CTS:
        case SUBTYPE_CTL_ACK:
          break;
        case SUBTYPE_CTL_BACKREQ:
        case SUBTYPE_CTL_BACKRESP:
          WriteTo (i, m_addr2);
          break;
        default:
          //NOTREACHED
          NS_ASSERT (false);
          break;
        }
      break;
    case TYPE_DATA:
      {
        WriteTo (i, m_addr2);
        WriteTo (i, m_addr3);
        i.WriteHtolsbU16 (GetSequenceControl ());
        if (m_ctrlToDs && m_ctrlFromDs)
          {
            WriteTo (i, m_addr4);
          }
        if (m_ctrlSubtype & 0x08)
          {
            i.WriteHtolsbU16 (GetQosControl ());
          }
      } break;
    default:
      //NOTREACHED
      NS_ASSERT (false);
      break;
    }
}
uint32_t
SlimMacHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  uint16_t frame_control = i.ReadLsbtohU16 ();
  SetFrameControl (frame_control);
  m_duration = i.ReadLsbtohU16 ();
  ReadFrom (i, m_addr1);
  switch (m_ctrlType)
    {
    case TYPE_MGT:
      ReadFrom (i, m_addr2);
      ReadFrom (i, m_addr3);
      SetSequenceControl (i.ReadLsbtohU16 ());
      break;
    case TYPE_CTL:
      switch (m_ctrlSubtype)
        {
        case SUBTYPE_CTL_RTS:
          ReadFrom (i, m_addr2);
          break;
        case SUBTYPE_CTL_CTS:
        case SUBTYPE_CTL_ACK:
          break;
        case SUBTYPE_CTL_BACKREQ:
        case SUBTYPE_CTL_BACKRESP:
          ReadFrom (i, m_addr2);
          break;
        }
      break;
    case TYPE_DATA:
      ReadFrom (i, m_addr2);
      ReadFrom (i, m_addr3);
      SetSequenceControl (i.ReadLsbtohU16 ());
      if (m_ctrlToDs && m_ctrlFromDs)
        {
          ReadFrom (i, m_addr4);
        }
      if (m_ctrlSubtype & 0x08)
        {
          SetQosControl (i.ReadLsbtohU16 ());
        }
      break;
    }
  return i.GetDistanceFrom (start);
}

} // namespace ns3
