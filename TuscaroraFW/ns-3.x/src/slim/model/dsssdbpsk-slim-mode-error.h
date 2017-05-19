/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/* Copyright: The Samraksh Company
 * SLIM-WF:Synchronous Lightweight Interfernce-aware Model-driven Wavefrm
 * The usage of this module is governed by the associated license file.
 * Authors: Bora Karaoglu, Mukundan Sridharan
 */

#ifndef DsssDbpsk_SLIM_MODE_ERROR_H
#define DsssDbpsk_SLIM_MODE_ERROR_H

#include <stdint.h>
#include <string>
#include <vector>
#include <ostream>
#include "slim-mode-error.h"

namespace ns3 {

class DsssDbpskModeError : public SlimModeError {
private:

public:
  static TypeId GetTypeId (void);

  virtual double GetBER(double sinr);

};


} // namespace ns3

#endif /* SLIM_MODE_ERROR_H */
