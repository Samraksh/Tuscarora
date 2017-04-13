////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef LinkEstimatorI_h
#define LinkEstimatorI_h

#include <Base/BasicTypes.h>
#include <Base/FixedPoint.h>
#include "Link.h"
#include "NetworkDiscoveryI.h"
#include "EstimationCallbackI.h"
#include <Interfaces/Waveform/WF_Types.h>

using namespace Types;
using namespace PAL;

/// Frameworks core services
namespace Core {
  
/// Estimation related classes and types
namespace Estimation {
	#define DEFAULT_ESTIMATION_PERIOD 1000000 //1sec, in micro seconds
	
	typedef struct SendEstMsgParam{
		WaveformId_t wfId; 
		FMessage_t *msg;
	}SendEstMsgParam;
  
	typedef Delegate<void, SendEstMsgParam> SendEstimationMessageDelegate_t;
	//typedef Delegate<void, WF_Message_n64_t&> SendEstimationMessageDelegate_t;
	
  ///Enum for type of framework estimator
  enum EstimatorTypeE {
    WF_PROXY, 	///provided by waveform
    FW_RANDOM_EST,	///Provided by framework, a randomized periodic estimator
    FW_SCHEDULED_EST, 	///Provided by framework, based on a schedule that is known
    FW_CONFLICTAWARE_EST,	///Provided by framework, based on a schedule that is known, also identifies message loss due to schedule collision
    FW_TEST_EST	//A test type used for testing the framework
  };
  
  ///Interface definition for a link estimator which is implemented by the framework.
  template <class AddressType>
  class LinkEstimatorI {
    EstimatorTypeE type;
    
  public:
	LinkEstimatorI() {}  
    LinkEstimatorI (EstimatorCallback_I<AddressType> &callback){}
    //LinkEstimatorI (PatternInterface::Pattern_I *parent);
    virtual bool RegisterDelegate (Core::WF_LinkChangeDelegate_n64_t& del)=0;
    virtual void SetParam(uint32_t beaconPeriod, uint32_t inactivePeriod) = 0;
    //virtual bool EvaluateNbr(NodeId_t node) =0 ;
    
    ///This function will be called each time a new Waveform packet arrives with that packet. This should return the new link metric of the src of the packet.
    virtual LinkMetrics* OnPacketReceive(Waveform::WF_MessageBase *rcvPkt)=0;
    virtual void PotentialNeighborUpdate (Discovery::PotentialNeighbor &pnbr, Discovery::PNbrUpdateTypeE type)=0;
    virtual ~LinkEstimatorI() {}
  };

  /*
  class EstimationManagerI {
  public:
    virtual bool RegisterCustomizer (LinkEstimatorI* estmator) = 0;
    virtual bool UpdatePatternNegihborhood () = 0;
    virtual ~EstimationManagerI (){}
  };
  */

  ///Defines the interface that must be implemented by the Pattern to customize its neighborhood.
  /*class LinkCustomizerI {
  public:
    LinkCustomizerI () {}
    /// This function will be called each time a message is received for the pattern. All of the customization logic should be implemented here.
    virtual LinkMetrics OnPacketReveive (FMessage_t *rcvMsg)=0;
    virtual ~LinkCustomizerI (){}
  };
  */
  
  /// Defines an interface that can be used to take information from a policy module about nodes
  class NeighborhoodControlI {
    //virtual void RegisterWhiteListCallback(Delegate <void, NodeList> *whitelistDelegate)=0;
  public:
    /// This API registers a callback, that will be called by the policy manager module to provide a blacklist of nodes.
    virtual void RegisterBlackListCallback (Delegate <void, NodeList> *blacklistDelegate) = 0;  
    virtual ~NeighborhoodControlI (){}
  };

  ///Defines an interface for co-operative link estimation in the waveform layer. Waveform implements the class, but framework decides when to run the methods to 
  /// to produce/update Link Metrics.
  //TODO::Do we need this in the Golden release
  /*class WF_CoopLinkEstimatorI {
    
  public:
    ///Method is called when an new packet is received on this waveform. Returns the new LinkMetrics from which the packet was received.
    virtual LinkMetrics OnWaveformPacketReveive (Waveform::WF_MessageBase *rcvMsg) = 0;
    
    ///Method is called by framework when it requires an update on some neighbor. For example this can be called periodically.
    virtual LinkMetrics OnDemandEstimation (NodeId_t nbr, LinkMetrics currentMetrics) = 0;
    virtual ~WF_CoopLinkEstimatorI (){}
  };*/

}//End of namespace
}


#endif // LinkEstimatorI_h
