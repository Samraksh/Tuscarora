/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/* Copyright: The Samraksh Company
 * SLIM-WF:Synchronous Lightweight Interfernce-aware Model-driven Wavefrm
 * The usage of this module is governed by the associated license file.
 * Authors: Bora Karaoglu, Mukundan Sridharan
 */

#ifndef SLIM_TX_VECTOR_H
#define SLIM_TX_VECTOR_H

#include "slim-mode.h"
#include <ostream>

namespace ns3 {


/**
 * This class mimics the TXVECTOR which is to be
 * passed to the PHY in order to define the parameters which are to be
 * used for a transmission.
 *
 * This class is constructed with the constructor that takes no
 * arguments, then the client must explicitly set
 * the transmit power level and the the list of mode parameters before using them.
 *
 */
class SlimTxVector : public Object
{
public:

	static TypeId GetTypeId (void);

  typedef std::vector <SlimMode> ListofSlimModes;

  SlimTxVector ();
  /**
   * Create a TXVECTOR with the given parameters.
   *
   * \param mode SlimMode
   * \param powerLevel transmission power level
   * \param retries retries
   * \param shortGuardInterval enable or disable short guard interval
   */
//  SlimTxVector (SlimMode mode, uint8_t powerLevel, uint8_t retries, bool shortGuardInterval, uint8_t nss, uint8_t ness, bool stbc);
  /**
   *  \returns the txvector payload mode
   */
  SlimMode GetMode (void) const;
  /**
  * Sets the selected payload transmission mode
  *
  * \param mode
  */
  void SetPayloadModePosition (uint8_t k);
  void SetPayloadSize(uint64_t size);

  ListofSlimModes& GetListofModes();

  /**
   *  \returns the transmission power level
   */
  uint8_t GetTxPowerLevel (void) const;
  /**
   * Sets the selected transmission power level
   *
   * \param powerlevel
   */
  void SetTxPowerLevel (uint8_t powerlevel);

  std::string GetUniqueName (void) const;

  void SetUniqueName(std::string _uniqueUid);

  Time CalculateTxDuration();

private:

  ListofSlimModes m_listofmodes;
  std::string uniqueUid;
  uint8_t  m_txPowerLevel;  /**< The TXPWR_LEVEL parameter in Table 15-4. 
                           It is the value that will be passed
                           to PMD_TXPWRLVL.request */ 

  uint8_t m_PayloadModePosition;

  //bool     m_modeInitialized; //*< Internal initialization flag */
  bool     m_txPowerLevelInitialized; //*< Internal initialization flag */
};

bool operator == (const SlimTxVector &a, const SlimTxVector &b);
std::ostream & operator << (std::ostream & os,const SlimTxVector &v);
std::istream & operator >> (std::istream &is, SlimTxVector &mode);

} // namespace ns3

#endif // SLIM_TX_VECTOR_H
