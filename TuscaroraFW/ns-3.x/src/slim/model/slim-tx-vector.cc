/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/* Copyright: The Samraksh Company
 * SLIM-WF:Synchronous Lightweight Interfernce-aware Model-driven Wavefrm
 * The usage of this module is governed by the associated license file.
 * Authors: Bora Karaoglu, Mukundan Sridharan
 */

#include "ns3/slim-tx-vector.h"
#include "ns3/fatal-error.h"
#include "ns3/nstime.h"

namespace ns3 {

bool operator == (const SlimTxVector &a, const SlimTxVector &b){
	return a.GetUniqueName() == b.GetUniqueName();
}

std::ostream & operator << ( std::ostream &os, const SlimTxVector &v)
{
  os << "mode:" << v.GetMode() <<
    " txpwrlvl:" << (uint32_t)v.GetTxPowerLevel();
  return os;
}

std::istream & operator >> (std::istream &is, SlimTxVector &mode)
{
	std::string str;
	is >> str;
	mode.SetUniqueName(str);
	return is;
}




TypeId SlimTxVector::GetTypeId (void) {
	static TypeId tid = TypeId ("ns3::SlimTxVector")
		   .SetParent<Object> ()
		   .AddConstructor<SlimTxVector> ()
		   .AddAttribute ("UniqueName", "A string containing a uniqueName of the TxVector. Used for comparison purposes.",
				   StringValue ("UNINITIALIZEDTXVECTOR"),
				   MakeStringAccessor (&SlimTxVector::GetUniqueName, &SlimTxVector::SetUniqueName),
				   MakeStringChecker())
		   .AddAttribute ("PayloadModePosition", "The index of the section containing the payload. ",
				   UintegerValue (0),
				   MakeUintegerAccessor (&SlimTxVector::GetTxPowerLevel, &SlimTxVector::SetTxPowerLevel),
				   MakeUintegerChecker<uint8_t> ())
//		   .AddAttribute ("txPowerLevel", "The index of the txPower level defined in the PHY",
//				   UintegerValue (1),
//				   MakeUintegerAccessor (&SlimTxVector::GetTxPowerLevel, &SlimTxVector::SetTxPowerLevel),
//				   MakeUintegerChecker<uint8_t> ())
;
	return tid;
}

NS_OBJECT_ENSURE_REGISTERED (SlimTxVector);

SlimTxVector::SlimTxVector ()
  : m_txPowerLevelInitialized (false)
{
}

std::string SlimTxVector::GetUniqueName (void) const
{
	return uniqueUid;
}
void SlimTxVector::SetUniqueName(std::string _uniqueUid){
	uniqueUid = _uniqueUid;
}


SlimMode SlimTxVector::GetMode (void) const{
	return m_listofmodes[m_PayloadModePosition];
}

void SlimTxVector::SetPayloadModePosition (uint8_t k){
	m_PayloadModePosition = k;
}

void SlimTxVector::SetPayloadSize(uint64_t size){
	m_listofmodes[m_PayloadModePosition].SetDuration(size * 8 *  m_listofmodes[m_PayloadModePosition].GetDataRate()  );
}


SlimTxVector::ListofSlimModes& SlimTxVector::GetListofModes (void)
{

  return m_listofmodes;
}

uint8_t  SlimTxVector::GetTxPowerLevel (void) const
{
  if (!m_txPowerLevelInitialized)
    {
      NS_FATAL_ERROR ("SlimTxVector txPowerLevel must be set before using");
    }
  return m_txPowerLevel;
}

void SlimTxVector::SetTxPowerLevel (uint8_t powerlevel){
	m_txPowerLevel = powerlevel;
	m_txPowerLevelInitialized = true;
}

Time SlimTxVector::CalculateTxDuration ()
{
	Time duration = MicroSeconds(0);


	for( SlimTxVector::ListofSlimModes::iterator i = this->GetListofModes().begin(); i != this->GetListofModes().end(); ++i ){
		duration += i->GetDuration();
	}
	return duration;
}

} // namespace ns3
