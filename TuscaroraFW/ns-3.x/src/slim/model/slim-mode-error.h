/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/* Copyright: The Samraksh Company
 * SLIM-WF: Flexible Adaptable TDMA Waveform
 * The usage of this module is governed by the associated license file.
 * Authors: Bora Karaoglu, Mukundan Sridharan
 */

#ifndef SLIM_MODE_ERROR_H
#define SLIM_MODE_ERROR_H

#include <stdint.h>
#include <string>
#include <vector>
#include <ostream>
#include "ns3/object.h"
#include "ns3/ptr.h"

namespace ns3 {

class SlimModeError : public Object
{
private:

public:
	static TypeId GetTypeId (void);
	SlimModeError(){};
	~SlimModeError(){};
    virtual double GetBER(double sinr) = 0;

};


} // namespace ns3

#endif /* SLIM_MODE_ERROR_H */
