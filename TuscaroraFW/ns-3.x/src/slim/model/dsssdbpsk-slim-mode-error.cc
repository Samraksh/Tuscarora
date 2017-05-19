/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/* Copyright: The Samraksh Company
 * SLIM-WF:Synchronous Lightweight Interfernce-aware Model-driven Wavefrm
 * The usage of this module is governed by the associated license file.
 * Authors: Bora Karaoglu, Mukundan Sridharan
 */

#include "dsssdbpsk-slim-mode-error.h"
#include "ns3/simulator.h"
#include "ns3/assert.h"
#include "ns3/log.h"


namespace ns3 {


TypeId DsssDbpskModeError::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::DsssDbpskModeError")
    .SetParent<SlimModeError> ()
    .AddConstructor<DsssDbpskModeError> ()
	;
  return tid;
}

double DsssDbpskModeError::GetBER(double sinr){
	  //NS_LOG_FUNCTION_NOARGS ();
	  double EbN0 = sinr * 22000000.0 / 1000000.0; // 1 bit per symbol with 1 MSPS
	  double ber = 0.5 * std::exp (-EbN0);
	  return ber;
}




} // namespace ns3
