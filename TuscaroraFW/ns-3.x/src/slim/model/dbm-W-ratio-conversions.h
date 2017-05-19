/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/* Copyright: The Samraksh Company
 * SLIM-WF: Flexible Adaptable TDMA Waveform
 * The usage of this module is governed by the associated license file.
 * Authors: Bora Karaoglu, Mukundan Sridharan
 */

#ifndef DBM_W_RATIO_CONV_H
#define DBM_W_RATIO_CONV_H

double DbToRatio (double dB) {
  double ratio = std::pow (10.0, dB / 10.0);
  return ratio;
}

double DbmToW (double dBm) {
  double mW = std::pow (10.0, dBm / 10.0);
  return mW / 1000.0;
}

double WToDbm (double w) {
  return 10.0 * std::log10 (w * 1000.0);
}

double RatioToDb (double ratio) {
  return 10.0 * std::log10 (ratio);
}

#endif
