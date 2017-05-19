/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/* Copyright: The Samraksh Company
 * SLIM-WF:Synchronous Lightweight Interfernce-aware Model-driven Wavefrm
 * The usage of this module is governed by the associated license file.
 * Authors: Bora Karaoglu, Mukundan Sridharan
 */

#include "slim-mode.h"
#include "ns3/simulator.h"
#include "ns3/assert.h"
#include "ns3/log.h"
#include "ns3/trace-source-accessor.h"
#include "dsssdbpsk-slim-mode-error.h"
#include "ns3/pointer.h"


namespace ns3 {


bool operator == (const SlimMode &a, const SlimMode &b){
	return a.GetUniqueName() == b.GetUniqueName();
}

std::ostream & operator << (std::ostream & os, const SlimMode &mode)
{
	os << mode.GetUniqueName ();
	return os;
}

std::istream & operator >> (std::istream &is, SlimMode &mode)
{
	std::string str;
	is >> str;
	mode.SetUniqueName(str);
	return is;
}

NS_LOG_COMPONENT_DEFINE ("SlimMode");



TypeId SlimMode::GetTypeId (void) {
	static TypeId tid = TypeId ("ns3::SlimMode")
		   .SetParent<Object> ()
		   .AddConstructor<SlimMode> ()
		   .AddAttribute ("UniqueName", "The unique ID of the mode being modeled. Used for comparison purposes.",
				   StringValue ("UNINITIALIZEDMODE"),
				   MakeStringAccessor (&SlimMode::GetUniqueName,
						   &SlimMode::SetUniqueName),
				   MakeStringChecker())
		   .AddAttribute ("Bandwidth", "The bandwidth(Hz) of the signal being modeled",
				   UintegerValue (20000000),
				   MakeUintegerAccessor (&SlimMode::GetBandwidth,
						   &SlimMode::SetBandwidth),
						   MakeUintegerChecker<uint32_t> ())
		   .AddAttribute ("BitTime",
				   "Time it takes to transmit one bit of information.",
				   TimeValue (MicroSeconds (1)),
				   MakeTimeAccessor (&SlimMode::dataRate),
				   MakeTimeChecker ())
		   .AddAttribute ("Duration",
				   "Time it takes to transmit entire section. Must be set for fixed-length sections. Calculated internally for payload.",
				   TimeValue (MicroSeconds (1)),
				   MakeTimeAccessor (&SlimMode::Duration),
				   MakeTimeChecker ())
			.AddAttribute ("FecF",
						   "The ratio of bits that the FEC can correct",
						   DoubleValue (0.0),
						   MakeDoubleAccessor (&SlimMode::SetCodeRate,
											   &SlimMode::GetCodeRate),
						   MakeDoubleChecker<double> ())
			.AddAttribute ("ErrorModPtr", "Pointer to the SlimModeError object implementing SINR vs. BER characteristics. The default is TuscaroraDsssDbpskModeError.",
						   PointerValue (Create<DsssDbpskModeError>()),
						   MakePointerAccessor (&SlimMode::error),
						   MakePointerChecker<SlimModeError> ())
;
	return tid;
}

NS_OBJECT_ENSURE_REGISTERED (SlimMode);

SlimMode::SlimMode (){
	NS_LOG_FUNCTION (this);
	m_random_ptr = CreateObject<UniformRandomVariable> ();
}
uint32_t SlimMode::GetBandwidth (void) const
{
	return bandwidth;
}
void SlimMode::SetBandwidth (uint32_t _bandwidth){
	bandwidth = _bandwidth;
}

Time SlimMode::GetDataRate (void) const
{
	return dataRate;
}
void SlimMode::SetDataRate(Time _dataRate){
	dataRate = _dataRate;
}

double SlimMode::GetCodeRate (void) const
{
	return codingRate;
}
void SlimMode::SetCodeRate (double cr)
{
	codingRate = cr;
}



std::string SlimMode::GetUniqueName (void) const
{
	return uniqueUid;
}
void SlimMode::SetUniqueName(std::string _uniqueUid){
	uniqueUid = _uniqueUid;
}



Time SlimMode::GetDuration ()
{
	return Duration;
}
void SlimMode::SetDuration(Time dur){
	Duration = dur;
}

void SlimMode::SetSlimModeError(Ptr<SlimModeError> er){
	error = er;
}


double SlimMode::GetNumberofBits(Time duration){
	return((uint64_t)(duration/GetDataRate()));
}

uint32_t SlimMode::CalculateNumErrorsInChunk(double ssnir, Time duration){
	if (duration == NanoSeconds (0))
	{
		return 0.0;
	}
	double ber = error->GetBER(ssnir);
	//ToDO: Randomly draw bits to determine whether they are in error or not
	//double numer = (double) GetNumberofBits(duration) * ber;
	uint32_t numer = 0;
	for (uint32_t t= 0; t < GetNumberofBits(duration); ++t){
		if(m_random_ptr -> GetValue () < ber ) ++numer;
	}

	return numer;
}


Ptr<RandomVariableStream> SlimMode::GetTheRandomVariableStream(){
	return m_random_ptr;
}

void SlimMode::SetTheRandomVariableStream(Ptr<RandomVariableStream> _p){
	m_random_ptr = _p;
}

} // namespace ns3
