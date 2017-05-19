/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/* Copyright: The Samraksh Company
 * SLIM-WF:Synchronous Lightweight Interfernce-aware Model-driven Wavefrm
 * The usage of this module is governed by the associated license file.
 * Authors: Bora Karaoglu, Mukundan Sridharan
 */

#include "dsssdqpskcck11-slim-mode-error.h"
#include "ns3/simulator.h"
#include "ns3/assert.h"
#include "ns3/log.h"


namespace ns3 {


TypeId DsssDqpskCCk11ModeError::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::DsssDqpskCCk11ModeError")
    .SetParent<SlimModeError> ()
    .AddConstructor<DsssDqpskCCk11ModeError> ()
	;
  return tid;
}

double DsssDqpskCCk11ModeError::GetBER(double sinr){
	  //NS_LOG_FUNCTION_NOARGS ();
	  double ber;
	  if (sinr > 10)
	    {
	      ber = 0.0;
	    }
	  else if (sinr < 0.1)
	    {
	      ber = 0.5;
	    }
	  else
	    {
	      // fitprops.coeff from matlab berfit
	      double a1 =  7.9056742265333456e-003;
	      double a2 = -1.8397449399176360e-001;
	      double a3 =  1.0740689468707241e+000;
	      double a4 =  1.0523316904502553e+000;
	      double a5 =  3.0552298746496687e-001;
	      double a6 =  2.2032715128698435e+000;
	      ber =  (a1 * sinr * sinr + a2 * sinr + a3) / (sinr * sinr * sinr + a4 * sinr * sinr + a5 * sinr + a6);
	    }
	return ber;
}




} // namespace ns3
