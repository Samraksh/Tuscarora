/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/* Copyright: The Samraksh Company
 * SLIM-WF:Synchronous Lightweight Interfernce-aware Model-driven Wavefrm
 * The usage of this module is governed by the associated license file.
 * Authors: Bora Karaoglu, Mukundan Sridharan
 */

#ifndef DsssDqpskCCk11_SLIM_MODE_ERROR_H
#define DsssDqpskCCk11_SLIM_MODE_ERROR_H

#include <stdint.h>
#include <string>
#include <vector>
#include <ostream>
#include "slim-mode-error.h"

namespace ns3 {

class DsssDqpskCCk11ModeError : public SlimModeError {
private:

public:
  static TypeId GetTypeId (void);

  virtual double GetBER(double sinr);

};


} // namespace ns3

#endif /* DsssDqpskCCk11_SLIM_MODE_ERROR_H */
