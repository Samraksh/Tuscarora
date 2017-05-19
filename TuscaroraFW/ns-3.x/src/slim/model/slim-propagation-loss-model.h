/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2005,2006,2007 INRIA
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as 
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 * Contributions: Timo Bingmann <timo.bingmann@student.kit.edu>
 * Contributions: Gary Pei <guangyu.pei@boeing.com> for fixed RSS
 * Contributions: Tom Hewer <tomhewer@mac.com> for two ray ground model
 *                Pavel Boyko <boyko@iitp.ru> for matrix
 */

#ifndef PROPAGATION_LOSS_MODEL_H
#define PROPAGATION_LOSS_MODEL_H

#include "ns3/object.h"
#include "ns3/random-variable-stream.h"
#include <map>

namespace ns3 {

/**
 * \defgroup propagation Propagation Models
 *
 */

class MobilityModel;

/**
 * \ingroup propagation
 *
 * \brief Models the propagation loss through a transmission medium
 *
 * Calculate the receive power (dbm) from a transmit power (dbm)
 * and a mobility model for the source and destination positions.
 */
class PropagationLossModel : public Object
{
public:
  /**
   * Get the type ID.
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  PropagationLossModel ();
  virtual ~PropagationLossModel ();

  /**
   * \brief Enables a chain of loss models to act on the signal
   * \param next The next PropagationLossModel to add to the chain
   *
   * This method of chaining propagation loss models only works commutatively
   * if the propagation loss of all models in the chain are independent
   * of transmit power.
   */
  void SetNext (Ptr<PropagationLossModel> next);

  /**
   * \brief Gets the next PropagationLossModel in the chain of loss models
   * that act on the signal.
   * \returns The next PropagationLossModel in the chain
   *
   * This method of chaining propagation loss models only works commutatively
   * if the propagation loss of all models in the chain are independent
   * of transmit power.
   */
  Ptr<PropagationLossModel> GetNext ();

  /**
   * Returns the Rx Power taking into account all the PropagatinLossModel(s)
   * chained to the current one.
   *
   * \param txPowerDbm current transmission power (in dBm)
   * \param a the mobility model of the source
   * \param b the mobility model of the destination
   * \returns the reception power after adding/multiplying propagation loss (in dBm)
   */
  double CalcRxPower (double txPowerDbm,
                      Ptr<MobilityModel> a,
                      Ptr<MobilityModel> b) const;

  /**
   * If this loss model uses objects of type RandomVariableStream,
   * set the stream numbers to the integers starting with the offset
   * 'stream'. Return the number of streams (possibly zero) that
   * have been assigned.  If there are PropagationLossModels chained
   * together, this method will also assign streams to the
   * downstream models.
   *
   * \param stream
   * \return the number of stream indices assigned by this model
   */
  int64_t AssignStreams (int64_t stream);

private:
  /**
   * \brief Copy constructor
   *
   * Defined and unimplemented to avoid misuse
   */
  PropagationLossModel (const PropagationLossModel &);
  /**
   * \brief Copy constructor
   *
   * Defined and unimplemented to avoid misuse
   * \returns
   */
  PropagationLossModel &operator = (const PropagationLossModel &);

  /**
   * Returns the Rx Power taking into account only the particular
   * PropagatinLossModel.
   *
   * \param txPowerDbm current transmission power (in dBm)
   * \param a the mobility model of the source
   * \param b the mobility model of the destination
   * \returns the reception power after adding/multiplying propagation loss (in dBm)
   */
  virtual double DoCalcRxPower (double txPowerDbm,
                                Ptr<MobilityModel> a,
                                Ptr<MobilityModel> b) const = 0;

  /**
   * Subclasses must implement this; those not using random variables
   * can return zero
   */
  virtual int64_t DoAssignStreams (int64_t stream) = 0;

  Ptr<PropagationLossModel> m_next; //!< Next propagation loss model in the list
};

/**
 * \ingroup propagation
 *
 * \brief The propagation loss follows a random distribution.
 */ 
class RandomPropagationLossModel : public PropagationLossModel
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  RandomPropagationLossModel ();
  virtual ~RandomPropagationLossModel ();

private:
  /**
   * \brief Copy constructor
   *
   * Defined and unimplemented to avoid misuse
   */
  RandomPropagationLossModel (const RandomPropagationLossModel &);
  /**
   * \brief Copy constructor
   *
   * Defined and unimplemented to avoid misuse
   * \returns
   */
  RandomPropagationLossModel & operator = (const RandomPropagationLossModel &);
  virtual double DoCalcRxPower (double txPowerDbm,
                                Ptr<MobilityModel> a,
                                Ptr<MobilityModel> b) const;
  virtual int64_t DoAssignStreams (int64_t stream);
  Ptr<RandomVariableStream> m_variable; //!< random generator
};

/**
 * \ingroup propagation
 *
 * \brief a Friis propagation loss model
 *
 * The Friis propagation loss model was first described in
 * "A Note on a Simple Transmission Formula", by 
 * "Harald T. Friis".
 * 
 * The original equation was described as:
 *  \f$ \frac{P_r}{P_t} = \frac{A_r A_t}{d^2\lambda^2} \f$
 *  with the following equation for the case of an
 *  isotropic antenna with no heat loss:
 *  \f$ A_{isotr.} = \frac{\lambda^2}{4\pi} \f$
 *
 * The final equation becomes:
 * \f$ \frac{P_r}{P_t} = \frac{\lambda^2}{(4 \pi d)^2} \f$
 *
 * Modern extensions to this original equation are:
 * \f$ P_r = \frac{P_t G_t G_r \lambda^2}{(4 \pi d)^2 L}\f$
 *
 * With:
 *  - \f$ P_r \f$ : reception power (W)
 *  - \f$ P_t \f$ : transmission power (W)
 *  - \f$ G_t \f$ : transmission gain (unit-less)
 *  - \f$ G_r \f$ : reception gain (unit-less)
 *  - \f$ \lambda \f$ : wavelength (m)
 *  - \f$ d \f$ : distance (m)
 *  - \f$ L \f$ : system loss (unit-less)
 * 
 * In the implementation,  \f$ \lambda \f$ is calculated as 
 * \f$ \frac{C}{f} \f$, where  \f$ C = 299792458\f$ m/s is the speed of light in
 * vacuum, and \f$ f \f$ is the frequency in Hz which can be configured by
 * the user via the Frequency attribute.
 *
 * The Friis model is valid only for propagation in free space within
 * the so-called far field region, which can be considered
 * approximately as the region for \f$ d > 3 \lambda \f$.
 * The model will still return a value for \f$ d < 3 \lambda \f$, as
 * doing so (rather than triggering a fatal error) is practical for
 * many simulation scenarios. However, we stress that the values
 * obtained in such conditions shall not be considered realistic. 
 * 
 * Related with this issue, we note that the Friis formula is
 * undefined for \f$ d = 0 \f$, and results in 
 * \f$ P_r > P_t \f$ for \f$ d < \lambda / 2 \sqrt{\pi} \f$.
 * Both these conditions occur outside of the far field region, so in
 * principle the Friis model shall not be used in these conditions. 
 * In practice, however, Friis is often used in scenarios where accurate
 * propagation modeling is not deemed important, and values of \f$ d =
 * 0 \f$ can occur. To allow practical use of the model in such
 * scenarios, we have to 1) return some value for \f$ d = 0 \f$, and
 * 2) avoid large discontinuities in propagation loss values (which
 * could lead to artifacts such as bogus capture effects which are
 * much worse than inaccurate propagation loss values). The two issues
 * are conflicting, as, according to the Friis formula, 
 * \f$\lim_{d \to 0 }  P_r = +\infty \f$;
 * so if, for \f$ d = 0 \f$, we use a fixed loss value, we end up with an infinitely large
 * discontinuity, which as we discussed can cause undesireable
 * simulation artifacts.
 *
 * To avoid these artifact, this implmentation of the Friis model
 * provides an attribute called MinLoss which allows to specify the
 * minimum total loss (in dB) returned by the model. This is used in
 * such a way that 
 * \f$ P_r \f$ continuously increases for \f$ d \to 0 \f$, until
 * MinLoss is reached, and then stay constant; this allow to
 * return a value for \f$ d  = 0 \f$ and at the same time avoid
 * discontinuities. The model won't be much realistic, but at least
 * the simulation artifacts discussed before are avoided. The default value of
 * MinLoss is 0 dB, which means that by default the model will return 
 * \f$ P_r = P_t \f$ for \f$ d <= \lambda / 2 \sqrt{\pi} \f$. We note
 * that this value of \f$ d \f$ is outside of the far field
 * region, hence the validity of the model in the far field region is
 * not affected.
 * 
 */
class DirectionalFriisPropagationLossModel : public FriisPropagationLossModel
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);
  FriisPropagationLossModel ();
  /**
   * \param frequency (Hz)
   *
   * Set the carrier frequency used in the Friis model 
   * calculation.
   */
  void SetFrequency (double frequency);
  /**
   * \param systemLoss (dimension-less)
   *
   * Set the system loss used by the Friis propagation model.
   */
  void SetSystemLoss (double systemLoss);

  /**
   * \param minLoss the minimum loss (dB)
   *
   * no matter how short the distance, the total propagation loss (in
   * dB) will always be greater or equal than this value 
   */
  void SetMinLoss (double minLoss);

  /**
   * \return the minimum loss.
   */
  double GetMinLoss (void) const;

  /**
   * \returns the current frequency (Hz)
   */
  double GetFrequency (void) const;
  /**
   * \returns the current system loss (dimension-less)
   */
  double GetSystemLoss (void) const;

private:
  /**
   * \brief Copy constructor
   *
   * Defined and unimplemented to avoid misuse
   */
  FriisPropagationLossModel (const FriisPropagationLossModel &);
  /**
   * \brief Copy constructor
   *
   * Defined and unimplemented to avoid misuse
   * \returns
   */
  FriisPropagationLossModel & operator = (const FriisPropagationLossModel &);

  virtual double DoCalcRxPower (double txPowerDbm,
                                Ptr<MobilityModel> a,
                                Ptr<MobilityModel> b) const;
  virtual int64_t DoAssignStreams (int64_t stream);

  /**
   * Transforms a Dbm value to Watt
   * \param dbm the Dbm value
   * \return the Watts
   */
  double DbmToW (double dbm) const;

  /**
   * Transforms a Watt value to Dbm
   * \param w the Watt value
   * \return the Dbm
   */
  double DbmFromW (double w) const;

  double m_lambda;        //!< the carrier wavelength
  double m_frequency;     //!< the carrier frequency
  double m_systemLoss;    //!< the system loss
  double m_minLoss;       //!< the minimum loss
};

} // namespace ns3

#endif /* PROPAGATION_LOSS_MODEL_H */
