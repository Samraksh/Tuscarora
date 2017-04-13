////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef FTYPES_H_
#define FTYPES_H_

#ifdef NO_USE_TYPES_DIR
#include "BasicTypes.h"
#else
#include <Base/BasicTypes.h>
#endif

namespace Types {
	typedef uint16_t Nonce_t;
	typedef uint16_t NodeId_t;
	typedef uint32_t MessageId_t;
	typedef uint64_t WF_MessageId_t;
	//typedef uint16_t LinkId_t; //Included here rather than in frameworktypes so that it can be easily included in post processing scripts
	typedef uint16_t WaveformId_t; //Included here rather than in frameworktypes so that it can be easily included in post processing scripts
	typedef uint128_t GlobalId_t;
	typedef uint16_t PatternId_t;
	typedef uint32_t RequestId_t;
	typedef uint32_t DelegateId_t;

	const WaveformId_t NULL_WF_ID=0xFFFF;
	const NodeId_t NULL_NODE_ID=0xFFFF;

  
  struct NodeList{
    NodeId_t *array;
    uint16_t size;
  };
  
  enum MessageTypeE {
    NULL_MSG=0,
    ESTIMATION_MSG=1, ///Link Estimation
    DISCOVERY_MSG,	///Network Discovery
    //TIMESYNC_MSG,	///Time Synchronization
    PATTERN_MSG,	///Network Patterns
    //DATA_MSG,	///Data
    ACK_REPLY_MSG, // this is acknowledgement packet send from Framework.
    ACK_REQUEST_MSG,
  };
  
  enum SprayTypeE {
	SPRAY_ALL_NBR,
	SPRAY_ONE_NBR,
	SPRAY_THREE_NBR,
	SPRAY_FOUR_NBR,
	SPRAY_FIFTYPERCENT_NBR //send to 50% of neighbors if 50% greater than 3,else 
		//send to 3
  };
}


#endif //FTYPES_H_
