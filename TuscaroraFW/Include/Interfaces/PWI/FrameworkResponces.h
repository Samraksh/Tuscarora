////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef FRAMEWORK_RESPONCES_H_
#define FRAMEWORK_RESPONCES_H_

#include <Base/FrameworkTypes.h>
#include <Interfaces/Core/PatternNamingI.h>
#include <Interfaces/Core/DataFlowI.h>

using namespace Core::Naming;
using namespace Core::Dataflow;

namespace PWI {
  
//const unsigned int16_t MAX_DEST = 16;

//CancelDataRequest
struct CancelDataResponse_Data{
    FMessageId_t  msgId; // Stores message Id
    bool        status;  //Set to true when cancel was successful for all destinations
    NodeId_t    destArray[Core::MAX_DEST]; //Copy of destination array passed from Pattern
    bool        cancel_status[Core::MAX_DEST];// cancel_status[i] is set to true if destArray[i] is canceled successfully.
    uint16_t   noOfDest;

    void SetAllStatusNegative(){
  	status = false;
  	for(uint16_t i = 0; i < Core::MAX_DEST; ++i){
  		cancel_status[i] = false;
  	}
    }
};

struct ReplacePayloadResponse_Data{
  FMessageId_t  msgId;
  bool status;            //indicates payload was replaced sccessfully or not
  NodeId_t noOfDest;      //no of node in destArray
  NodeId_t destArray[Core::MAX_DEST]; //stores nodeIds.
  bool replace_status[Core::MAX_DEST];

  void SetAllStatusNegative(){
	status = false;
	for(uint16_t i = 0; i < Core::MAX_DEST; ++i){
		replace_status[i] = false;
	}
  }
};

struct DataStatusResponse_Data{
  FMessageId_t  msgId;
  NodeId_t destArray[MAX_DEST];
  DataStatusTypeE ackStatus[MAX_DEST];
  bool status; 
};  

struct RegistrationResponse_Data {
  bool status;
  uint8_t priority;
  uint16_t noOfOutStandingPkts;
  PatternId_t patternId;
};

struct NewPatternInstanceResponse_Data{
  bool status; 
  char uniqueName[128];
  PatternId_t pid;
  PatternTypeE type;
};  

struct AddDestinationResponse_Data{
  FMessageId_t  msgId;
  uint16_t noOfDest;
  NodeId_t destArray[Core::MAX_DEST];
  bool add_status[Core::MAX_DEST];
  bool status;
};

struct SelectDataNotificationResponse_Data {
  bool status;
};

struct SetLinkThresholdResponse_Data {
  bool status;
};

} //End of namespace

#endif //PATTERN_EVENT_DISPATCH_I_H_
