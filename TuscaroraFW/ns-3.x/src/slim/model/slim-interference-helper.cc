/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/* Copyright: The Samraksh Company
 * SLIM-WF:Synchronous Lightweight Interfernce-aware Model-driven Wavefrm
 * The usage of this module is governed by the associated license file.
 * Authors: Bora Karaoglu, Mukundan Sridharan
 */

#include "slim-interference-helper.h"

#include "slim-phy.h"
#include "ns3/simulator.h"
#include "ns3/log.h"



#include <algorithm>
namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("SlimInterferenceHelper");

/****************************************************************
 *       Phy event class
 ****************************************************************/
 SlimInterferenceHelper::Event::Event (uint32_t size, SlimMode* payloadMode,
		Time duration, double rxPower, SlimTxVector txVector)
: m_size (size),
  m_startTime (Simulator::Now ()),
  m_endTime (m_startTime + duration),
  m_rxPowerW (rxPower),
  m_txVector (txVector)
{
	 m_payloadMode = payloadMode;
}
SlimInterferenceHelper::Event::~Event ()
{
}

Time SlimInterferenceHelper::Event::GetDuration (void) const
{
	return m_endTime - m_startTime;
}
Time SlimInterferenceHelper::Event::GetStartTime (void) const
{
	return m_startTime;
}
Time SlimInterferenceHelper::Event::GetEndTime (void) const
{
	return m_endTime;
}
double SlimInterferenceHelper::Event::GetRxPowerW (void) const
{
	return m_rxPowerW;
}
uint32_t SlimInterferenceHelper::Event::GetSize (void) const
{
	return m_size;
}
SlimMode* SlimInterferenceHelper::Event::GetPayloadMode (void) const
{
	return m_payloadMode;
}

SlimTxVector SlimInterferenceHelper::Event::GetTxVector (void) const
{
	return m_txVector;
}


/****************************************************************
 *       Class which records ssnir change events for a
 *       short period of time.
 ****************************************************************/
 SlimInterferenceHelper::NiChange::NiChange (Time time, double delta)
: m_time (time),
  m_delta (delta)
{
}
Time SlimInterferenceHelper::NiChange::GetTime (void) const
{
	return m_time;
}
double SlimInterferenceHelper::NiChange::GetDelta (void) const
{
	return m_delta;
}
bool SlimInterferenceHelper::NiChange::operator < (const NiChange& o) const
{
	return (m_time < o.m_time);
}

/****************************************************************
 *       The actual SlimInterferenceHelper
 ****************************************************************/
 SlimInterferenceHelper::SlimInterferenceHelper ()
: m_firstPower (0.0),
  m_rxing (false)
{
}
SlimInterferenceHelper::~SlimInterferenceHelper ()
{
	EraseEvents ();
}

Ptr<SlimInterferenceHelper::Event> SlimInterferenceHelper::Add (uint32_t size, SlimMode* payloadMode,
		Time duration, double rxPowerW, SlimTxVector txVector)
{
	Ptr<SlimInterferenceHelper::Event> event;

	event = Create<SlimInterferenceHelper::Event> (size,
			payloadMode,
			duration,
			rxPowerW,
			txVector);
	AppendEvent (event);
	return event;
}


void SlimInterferenceHelper::SetNoiseFigure (double value)
{
	m_noiseFigure = value;
}

double SlimInterferenceHelper::GetNoiseFigure (void) const
{
	return m_noiseFigure;
}


Time SlimInterferenceHelper::GetEnergyDuration (double energyW)
{
	Time now = Simulator::Now ();
	double noiseInterferenceW = 0.0;
	Time end = now;
	noiseInterferenceW = m_firstPower;
	for (NiChanges::const_iterator i = m_niChanges.begin (); i != m_niChanges.end (); i++)
	{
		noiseInterferenceW += i->GetDelta ();
		end = i->GetTime ();
		if (end < now)
		{
			continue;
		}
		if (noiseInterferenceW < energyW)
		{
			break;
		}
	}
	return end > now ? end - now : MicroSeconds (0);
}

void SlimInterferenceHelper::AppendEvent (Ptr<SlimInterferenceHelper::Event> event)
{
	Time now = Simulator::Now ();
	if (!m_rxing)
	{
		NiChanges::iterator nowIterator = GetPosition (now);
		for (NiChanges::iterator i = m_niChanges.begin (); i != nowIterator; i++)
		{
			m_firstPower += i->GetDelta ();
		}
		m_niChanges.erase (m_niChanges.begin (), nowIterator);
		m_niChanges.insert (m_niChanges.begin (), NiChange (event->GetStartTime (), event->GetRxPowerW ()));
	}
	else
	{
		AddNiChangeEvent (NiChange (event->GetStartTime (), event->GetRxPowerW ()));
	}
	AddNiChangeEvent (NiChange (event->GetEndTime (), -event->GetRxPowerW ()));

}


double SlimInterferenceHelper::CalculateSnr (double signal, double noiseInterference, SlimMode* mode) const
{
	// thermal noise at 290K in J/s = W
			static const double BOLTZMANN = 1.3803e-23;
	// Nt is the power of thermal noise in W
	double Nt = BOLTZMANN * 290.0 * mode->GetBandwidth ();
	// receiver noise Floor (W) which accounts for thermal noise and non-idealities of the receiver
			double noiseFloor = m_noiseFigure * Nt;
	double noise = noiseFloor + noiseInterference;
	double snr = signal / noise;
	return snr;
}

double SlimInterferenceHelper::CalculateNoiseInterferenceW (Ptr<SlimInterferenceHelper::Event> event, NiChanges *ni) const
{
	double noiseInterference = m_firstPower;
	NS_ASSERT (m_rxing);
	for (NiChanges::const_iterator i = m_niChanges.begin () + 1; i != m_niChanges.end (); i++)
	{
		if ((event->GetEndTime () == i->GetTime ()) && event->GetRxPowerW () == -i->GetDelta ())
		{
			break;
		}
		ni->push_back (*i);
	}
	ni->insert (ni->begin (), NiChange (event->GetStartTime (), noiseInterference));
	ni->push_back (NiChange (event->GetEndTime (), 0));
	return noiseInterference;
}



bool SlimInterferenceHelper::IsPacketSuccessful(Ptr<const SlimInterferenceHelper::Event> event, NiChanges *ni){

	NiChanges::iterator j = ni->begin ();
	Time previous = (*j).GetTime ();
	Time current = previous;
	double noiseInterferenceW = (*j).GetDelta ();
	double powerW = event->GetRxPowerW ();
	j++;


	SlimTxVector::ListofSlimModes lofmodes = event->GetTxVector().GetListofModes();
	SlimTxVector::ListofSlimModes::iterator i = lofmodes.begin ();

	while (lofmodes.end () != i){
		if( !(IsSectionSuccessfull(&(*i), ni, j, current, previous, noiseInterferenceW, powerW)) ){
			return(false);
		}
		++i;
	}
	return true;
}

bool SlimInterferenceHelper::IsSectionSuccessfull(SlimMode *mode, NiChanges *ni, NiChanges::iterator &j, Time &current, Time &previous, double &noiseInterferenceW, double &powerW){
	Time section_start_time = current;
	double snr = 0.0;
	double numerrors = 0.0; /* Number of bit errors*/
	while (ni->end () != j){
		current = (*j).GetTime();
		if(current > section_start_time + mode->GetDuration()) current = section_start_time + mode->GetDuration();
		snr = CalculateSnr (powerW, noiseInterferenceW, mode);
		numerrors += mode->CalculateNumErrorsInChunk (
				snr,
				current - previous);


		//Increment j or if it is the end of the mode exit
		if(current == section_start_time + mode->GetDuration()){
			previous = current;
			break;
		}
		else{
			noiseInterferenceW += (*j).GetDelta ();
			previous = current;
			j++;
		}
	}

	//Determine if the section is successfull or not
	if( (double)numerrors/(double)mode->GetNumberofBits(current-section_start_time) > mode->GetCodeRate() ){
		return false;
	}
	else{
		return true;
	}
}

struct SlimInterferenceHelper::SnrPer SlimInterferenceHelper::CalculateSnrPer (Ptr<SlimInterferenceHelper::Event> event)
{
	NiChanges ni;
	double noiseInterferenceW = CalculateNoiseInterferenceW (event, &ni);
	double snr = CalculateSnr (event->GetRxPowerW (), noiseInterferenceW, event->GetPayloadMode() );

	/* calculate the ssnir at the start of the packet and accumulate
	 * all ssnir changes in the ssnir vector.
	 */
	//double per = CalculatePer (event, &ni);
	double per;
	if(IsPacketSuccessful(event, &ni)) per = 0.0;
	else per = 1.0;


	struct SnrPer snrPer;
	snrPer.snr = snr;
	snrPer.per = per;
	return snrPer;
}

void SlimInterferenceHelper::EraseEvents (void)
{
	m_niChanges.clear ();
	m_rxing = false;
	m_firstPower = 0.0;
}
SlimInterferenceHelper::NiChanges::iterator SlimInterferenceHelper::GetPosition (Time moment)
{
	return std::upper_bound (m_niChanges.begin (), m_niChanges.end (), NiChange (moment, 0));

}
void SlimInterferenceHelper::AddNiChangeEvent (NiChange change)
{
	m_niChanges.insert (GetPosition (change.GetTime ()), change);
}
void SlimInterferenceHelper::NotifyRxStart ()
{
	m_rxing = true;
}
void SlimInterferenceHelper::NotifyRxEnd ()
{
	m_rxing = false;
}
} // namespace ns3
