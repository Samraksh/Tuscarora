////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef EstimatorCallbackI_h
#define EstimatorCallbackI_h

#include <Base/BasicTypes.h>
#include <Base/FixedPoint.h>
#include "Link.h"
#include "NetworkDiscoveryI.h"
#include <Interfaces/Waveform/WF_Types.h>

using namespace Waveform;
using namespace PAL;

namespace Core {
namespace Estimation {
	
template <class AddressType>
class EstimatorCallback_I{
	
public:
	virtual bool SendEstimationMessage (WaveformId_t wfId, 	FMessage_t *msg) =0;
	virtual WF_Attributes GetWaveformAttributes (WaveformId_t wfId) = 0;
	virtual void LinkEstimatorChangeHandler(WF_LinkEstimationParam<AddressType> _param)=0;
	virtual ~EstimatorCallback_I() {}
};
	
	
} //End namespace
}



#endif //EstimatorCallbackI_h