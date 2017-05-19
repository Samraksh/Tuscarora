/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/* Copyright: The Samraksh Company
 * SLIM-WF: Flexible Adaptable TDMA Waveform
 * The usage of this module is governed by the associated license file.
 * Authors: Bora Karaoglu, Mukundan Sridharan
 */

#ifndef SLIM_MODE_H
#define SLIM_MODE_H

#include <stdint.h>
#include <string>
#include <vector>
#include <ostream>
#include "ns3/object.h"
#include "ns3/double.h"
#include "ns3/uinteger.h"
#include "ns3/double.h"
#include "ns3/nstime.h"
#include "ns3/string.h"
#include "ns3/attribute-helper.h"
#include "ns3/simulator.h"
#include "slim-mode-error.h"
#include "ns3/random-variable-stream.h"

//#include "ns3/object.h"



namespace ns3 {

/**
 * \brief represent a single transmission mode
 * \ingroup slim
 *
 *	This class specifies the transmission characteristics of a given mode.
 *
 *	This class is constructed with the constructor that takes no
 * arguments, then the client must explicitly set parameters. Default
 * member initializers are provided for the other parameters, to
 * conform to a 1 Mbps DBPSK modulation with no FEC, although these
 * may also be explicitly set after object construction.
 */
class SlimMode : public Object
{
private:

		std::string uniqueUid;
		uint32_t bandwidth;
		Time dataRate;
		double codingRate;

		Ptr<SlimModeError> error;

		Time Duration;

		Ptr<RandomVariableStream> m_random_ptr;  //!< Provides uniform random variables.

public:

		static TypeId GetTypeId (void);

  /**
   * \Sets/gets a unique name for the slim mode. This is used to compare to see objects implementing the same mode are identical.
   * instance.
   */
  std::string GetUniqueName (void) const;
  void SetUniqueName(std::string _uniqueUid);

  /**
   * \Sets/gets total duration for the mode. The total duration is proportional to the amount of information in the mode.
   * instance.
   */
  Time GetDuration();
  void SetDuration(Time dur);

  /**
   * Set/Get the noise figure.
   *
   * \param value noise figure
   */
  void SetNoiseFigure (double value);
  double GetNoiseFigure (void) const;

  /**
   * \sets/gets the number of Hz used by this signal
   */
  uint32_t GetBandwidth (void) const;
  void SetBandwidth(uint32_t _bandwidth);

  /**
   * \returns the Time to transmit one bit of information. This is used to calculate the amoun of time it takes to transmit the information content. Used for payload
   */
  Time GetDataRate (void) const;
  void SetDataRate(Time _dataRate);

  /**
   * \Sets/gets an error mode specifying the relationship between SINR and the BER.
   * instance.
   */
  void SetSlimModeError (Ptr<SlimModeError> _er);
  //SlimModeError* GetSlimModeError() const;
  /**
   * Set/get the error correction capabilities of the underlying FEC. This is the ratio of information bits that can be corrected in hard-decision decoding system
   */
  double GetCodeRate (void) const;
  void SetCodeRate(double cr);

  /**
   * Set/Get the underlying random variable stream used by this module
   *
   * \param value noise figure
   */
   Ptr<RandomVariableStream> GetTheRandomVariableStream();
   void SetTheRandomVariableStream(Ptr<RandomVariableStream> _p);



   // The following methods are consumed internally


  /**
   * Creates a slim mode with the default values.
   */
  SlimMode ();

  /**
   * CalculateNumErrorsInChunk calculates the number of errors for the given duration over which number of interferers stays constant resulting in the given average SINR. The number of errors is calculation involves instantiation of the input random variable stream.
   */
  uint32_t CalculateNumErrorsInChunk(double ssnir, Time duration);
  double GetNumberofBits(Time duration);
};
bool operator == (const SlimMode &a, const SlimMode &b);
std::ostream & operator << (std::ostream & os, const SlimMode &mode);
std::istream & operator >> (std::istream &is, SlimMode &mode);

} // namespace ns3

#endif /* SLIM_MODE_H */
