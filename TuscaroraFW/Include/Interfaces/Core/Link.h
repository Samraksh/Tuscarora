////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef LINK_H_
#define LINK_H_

#include <Base/BasicTypes.h>
#include <Base/FixedPoint.h>

#include <Base/FrameworkTypes.h>
#include <Base/Delegate.h>
#include <Base/TimeI.h>

using namespace Types;
using namespace PAL;

namespace Core {
  
  ///Enum for different types of links
  enum LinkTypeE {
    ///Two way communicaiton possible using the link
    TwoWay,	
    ///Link can only be used to send messages, the node at the other end will not transmit
    SinkOnly,
    ///Link can only be use to receive message, the node at the other end will not receive messages.
    SourceOnly,
  };

  ///Enum for the type of neighbor change event.
  enum NeighborChangeTypeE {
    NBR_NEW, 	///New neighbor detected
    NBR_DEAD,	///A previously existing neighbor is now dead
    NBR_UPDATE 	///An update about an existing neighbor
  };
  
	
	
  /**
	 * Quality.  The probability of delivery of transmissions with successful receipt of acknowledgement on the link, expressed in [0, 1].
	 * Quality has type UFixed_8_7_t, where the last 7 of the 8 bit unsigned fixed point type are the radix point bits.  
	 * 
	 * DataRate.  Let $si$ be defined as the estimation of the average sustained rate in bits per second.  Then DataRate will be defined as log_2($si$)
	 * DataRate has type UFixed_8_2_t, where the 16 bit fixed point type consists, 6 bits before the radix point and 2 radix point bits. 
	 * Its maximum value is 63.75 and minimum value is 0.  This corresponds to data rates from 1 bps  to 15.5 Pb, with a representational accuracy of 
	 * plus or minus 25%. 
	 * 
	 * Latency.  Let \tau be defined as the latency in seconds.  Then the latency is defined to be log_2(\tau).Latency has type UFixed_8_2_t, which contains a sign 
	 * bit, 5 bits before the decimal point, and 2 after.  This can represent a number from 230 ps to 136 years to accuracies of plus or minus 25%. 
	 * 
	 * Energy.  Let \eta be the average network-wide number of pico-joules (e.g., combined energy of the transmitter and the receiver) per byte.  
	 * Then the energy is defined as log_2(\eta).Energy  has  type  UFixed_8_2_t,  which  defines  an  unsigned  number  with  6  bits  before  the decimal and 
	 * 2 after.  This would correspond to levels from 1 pJ/Byte to 15 MJ/Byte. 
	 *
	 * cost. The cost sending one packet using the waveform.
	 */
  struct WF_LinkMetrics {
    ///A quality metric expressed as a real number between [0,1]
    UFixed_7_8_t quality;
    ///Datarate expressed as log_2(bps)
    UFixed_2_8_t dataRate; 
    ///Average latency expressed as log_2(milliseconds)
    Fixed_7_16_t avgLatency;
    ///Average energy to transmit a packet expressed as log_2(pica-joules)
    UFixed_2_8_t energy;
    ///Average cost of sending a packet
    Fixed_14_16_t cost;
  };

  ///Structure used by the framwork to expose link metrics to the pattern, 28 bytes with the use of float for metrics
  struct LinkMetrics: public WF_LinkMetrics {

    ///Tuscarora's estimate of the latency for the next transmit
    //U64NanoTime expLatencyToXmit;
		
	LinkMetrics(){
		quality = 0;
		dataRate = 0;
		avgLatency = 0;
		energy = 0;
		cost = 0;
	}
	LinkMetrics (WF_LinkMetrics _param) {
		quality = _param.quality;
		dataRate = _param.dataRate;
		avgLatency = _param.avgLatency;
		energy = _param.energy;
		cost = _param.cost;
		//expLatencyToXmit = 0;
	}
		
		/*void operator = (WF_LinkMetrics rhs) {
			quality = rhs.quality;
			dataRate = rhs.dataRate;
			avgLatency = rhs.avgLatency;
			energy = rhs.energy;
			cost = -1;
			expLatencyToXmit = 0;
		}*/
  };
  
  template <class WF_AddressType>
  class WF_LinkId {
  public:
    WF_AddressType nodeId;
    WaveformId_t waveformId;
  
    WF_LinkId (){}
    WF_LinkId (WF_AddressType _id){
		 nodeId = _id; waveformId = NULL_WF_ID;
	}
    WF_LinkId (WF_AddressType _id, WaveformId_t _wid){
		nodeId = _id; waveformId = _wid;
	}
    
	
	
	bool operator == (const WF_LinkId<WF_AddressType> rhs) const {
		if (rhs.nodeId == nodeId ){
			if(rhs.waveformId == waveformId || rhs.waveformId == NULL_WF_ID || waveformId == NULL_WF_ID){return true;}
		else {return false;}
		}else {
			return false;
		}
	}

	/*bool EqualTo (const WF_LinkId<WF_AddressType> &lhs, const WF_LinkId<WF_AddressType> &rhs) {
		return lhs == rhs;
	}*/

	
    bool operator < (const WF_LinkId<WF_AddressType> rhs) const{
		if(rhs.nodeId != nodeId) return nodeId < rhs.nodeId;
		else if(rhs.waveformId == NULL_WF_ID || waveformId == NULL_WF_ID){ return false;}
		else return waveformId < rhs.waveformId;
	}
};
  
  typedef WF_LinkId<NodeId_t> LinkId;
  
  
  ///Structure to store information about a link in the framework and pattern layers including its metrics, 33b
  struct Link {
    LinkId linkId; //4b
    //WaveformId_t waveformId;
    LinkTypeE type;//1b
    LinkMetrics metrics;//28b
    Link(){
    	linkId.waveformId = NULL_WF_ID;
    	linkId.nodeId = NULL_NODE_ID;
    }

  };

  ///Structure to be used as the parameter for the function handling the neighbor change events.
template <class WF_AddressType>
struct WF_LinkEstimationParam {
	LinkTypeE type;
	NodeId_t nodeId;
	WF_LinkMetrics metrics;
	WF_AddressType linkAddress;
	NeighborChangeTypeE changeType;
	WaveformId_t wfid; //To store waveform Id.
	
	WF_LinkEstimationParam() {
		nodeId = NULL_NODE_ID;
		wfid = NULL_WF_ID;
	}
}__attribute__((packed, aligned(1)));

	
  ///A delegate definition which will be called to notify events about a node or link 
  typedef WF_LinkEstimationParam<uint64_t> WF_LinkEstimationParam_n64_t;
  typedef Delegate<void, WF_LinkEstimationParam<uint64_t> > WF_LinkChangeDelegate_n64_t;
  
	
  ///Structure to be used as the parameter for the function handling the neighbor change events.
  struct NeighborUpdateParam {
    Link link;//33b
    NodeId_t nodeId;//2b
    NeighborChangeTypeE changeType;//1b
	
		NeighborUpdateParam (){}
		NeighborUpdateParam (WF_LinkEstimationParam_n64_t _param, NodeId_t nodeid){
			link.linkId.nodeId = nodeid;
			link.linkId.waveformId = _param.wfid;
			link.metrics = _param.metrics;
			link.type = _param.type;
			
			nodeId = nodeid;
			changeType = _param.changeType;
		}
  };

	typedef Delegate<void, NeighborUpdateParam> NeighborDelegate;

  ///Structure to store information about a link received from the waveform layer
	template <class WF_AddressType>
	struct WF_Link {
		WF_LinkId<WF_AddressType> linkId;
		LinkTypeE type;
		WF_LinkMetrics metrics;
	};

  
	template <class WF_AddressType>
	struct WF_LinkEstimates {
		WF_Link<WF_AddressType> *linkArray;
		uint16_t size;
	};
  

}//End of namespace


#endif // LINK_H_
