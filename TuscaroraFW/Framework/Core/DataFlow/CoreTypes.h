////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef CORE_TYPES_H_
#define CORE_TYPES_H_


#include <Base/BasicTypes.h>
#include <Base/FrameworkTypes.h>
#include <Interfaces/PWI/Framework_I.h>
#include <Interfaces/Waveform/WF_I.h>
#include "../../../Include/Interfaces/Core/EstimationLogI.h"
#include "Framework/PWI/PatternClient.h"
//#include "Framework/Core/Estimation/EstimationManager.h"

using namespace PWI;
using namespace Waveform;
using namespace Core;

namespace Core {
  
namespace Dataflow {

typedef Delegate<void, WF_MessageBase&> WaveformMessageDelegate_t;
typedef BSTMapT<WaveformId_t, Waveform_I<uint64_t>* > WaveformMap_t;
typedef BSTMapT<WaveformId_t, WF_Attributes> WF_AttributeMap_t;
typedef BSTMapT<WaveformId_t, PacketAdaptor<uint64_t>*> WF_MsgAdaptorMap_t;
typedef BSTMapT<PatternId_t, PatternClient*> PatternClientMap_t;
typedef BSTMapT<WaveformId_t, LinkEstimatorI<uint64_t> *> EstimatorHash_t;

//typedef BSTMapT<WF_MessageId_t, FMessageId_t > WFMsgIdToFragmentListMap_t;

//typedef FragmentStatusTypeE fragmentStatus_t;
//struct FragmentDeliveryStatusPerDest{
//	fragmentStatus_t fragmentStatus;
//	uint16_t n_ACK_FRAG_FRAG_CREATED;
//	uint16_t n_ACK_FRAG_WF_RECV;
//	uint16_t n_ACK_FRAG_WF_SENT;
//	uint16_t n_ACK_FRAG_DST_RECV;
//	FragmentDeliveryStatusPerDest() :
//		n_ACK_FRAG_FRAG_CREATED(0),
//		n_ACK_FRAG_WF_RECV(0),
//		n_ACK_FRAG_WF_SENT(0),
//		n_ACK_FRAG_DST_RECV(0) {
//	};
//};
typedef FragmentStatusTypeE FragmentDeliveryStatusPerDest;
typedef BSTMapT<uint64_t, FragmentDeliveryStatusPerDest> FragmentDeliveryStatusMap_t ;

struct FragmentList{
	FMessageId_t frameworkMsgID;
	BSTMapT<uint64_t, WF_DataStatusTypeE> lastSentCombinedStatusMap;
	BSTMapT<WF_MessageId_t, FragmentDeliveryStatusMap_t> fragmentmap;
};


class WFMsgIdToFragmentListMap_t: public BSTMapT<WF_MessageId_t, FragmentList*> {
public:
	bool Erase(const WF_MessageId_t &_key){ //Use shadowing to prevent memory leak on FragmentList
		auto it = this->Find(_key);
		if(it != this->End()){
			FragmentList* fragstatptr = (it->Second());
			if(this->BSTMapT<WF_MessageId_t, FragmentList*>::Erase(it)){
				delete fragstatptr;
			}
			else return false;

		}
		return false;
	};
	bool Erase(Iterator& it){
		if(it != this->End()){
			FragmentList* fragstatptr = (it->Second());
			if(this->BSTMapT<WF_MessageId_t, FragmentList*>::Erase(it)){
				delete fragstatptr;
			}
			else return false;

		}
		return false;
	};
	bool Erase(Reverse_Iterator& it){
		if(it != this->REnd()){
			FragmentList* fragstatptr = (it->Second());
			if(this->BSTMapT<WF_MessageId_t, FragmentList*>::Erase(it)){
				delete fragstatptr;
			}
			else return false;
		}
		return false;
	};
};

//typedef AVLBST_T<WF_MessageId_t> WF_Message_FragmentIdList_t; // This is a list of WF_MessageIds corresponding to the same fragment group
//typedef BSTMapT<WF_MessageId_t, DataStatusTypeE> WF_Message_FragmentIdList_t; // This is a map of WF message Ids belonging to the same fragment group and their associated statuses
//
//
//class WF_Message_FragmentIdListComparator_t { //Comparator class for the WF_Message_FragmentIdList_t that is based on the first element in the list
//public:
//	static bool LessThan (const WF_Message_FragmentIdList_t*& A, const WF_Message_FragmentIdList_t*& B) {
//		auto aitr = A->Begin();
//		auto bitr = B->Begin();
//
//		if(aitr != A->End()){
//			if(bitr != B->End()){
//				return (aitr->First() < bitr->First());
//			}
//			else return true;
//		}
//		else{
//			return false;
//		}
//	}
//
//	static bool EqualTo (const WF_Message_FragmentIdList_t*& A, const WF_Message_FragmentIdList_t*& B) {
//		if(A->Size() == 0){
//			if(B->Size() == 0) return true;
//			else return false;
//		}
//		else if(B->Size() == 0) return false;
//		else if(A->Size() ==1 ){ //If one of them has one element compare all elements against it. This hack for
//			auto aitr = A->Begin();
//			for(auto bitr = B->Begin(); bitr != B->End(); ++bitr){
//				if(aitr->First() == bitr->First()) return true;
//			}
//			return false;
//		}
//		else if(B->Size() ==1 ){
//			auto aitr = B->Begin();
//			for(auto bitr = A->Begin(); bitr != A->End(); ++bitr){
//				if(aitr->First() == bitr->First()) return true;
//			}
//			return false;
//		}
//
//		else if(A->Size() != B->Size()) return false;
//		else{
//			auto aitr = A->Begin();
//			auto bitr = B->Begin();
//			while(aitr != A->End() && bitr != B->End()){
//				if(aitr->First() != )
//				++aitr;++bitr;
//			}
//		}
//
//	}
//};
//typedef BSTMapT<WF_Message_FragmentIdList_t*, FMessageId_t , WF_Message_FragmentIdListComparator_t> WFMsgIdToFragmentListMap_t;


//class WF_MessageIdArray_t{
//
//public:
//	std::size_t size;
//	WF_MessageId_t* ptr;
//
//	WF_MessageIdArray_t(std::size_t s = 1) : size(s){
//		ptr = new WF_MessageId_t[s];
//	}
//
//	~WF_MessageIdArray_t(){
//		if(ptr) delete[] ptr;
//	}
//
//};
//
//class WF_MessageIdArrayComparator {
//public:
//	static bool LessThan (const WF_MessageIdArray_t& A, const WF_MessageIdArray_t& B) {
//		return (A.ptr[0] < B.ptr[0]);
//	}
//	static bool EqualTo (const WF_MessageIdArray_t& A, const WF_MessageIdArray_t& B) {
//		return (A.ptr[0] == B.ptr[0]);
//	}
//};

//typedef BSTMapT<WF_MessageIdArray_t, FMessageId_t , WF_MessageIdArrayComparator > WFMsgIdToFragmentListMap_t;

struct LinkEstimationStore{
	EstimatorTypeE linkEstimationType;
	WaveformMessageDelegate_t *linkEstimationDelegate;
	WaveformMessageDelegate_t *discoveryDelegate;
	EstimationLogI *logger;
	//EstimationManager linkManager;
	EstimatorHash_t estimatorHash;
	//int discoveryType;
	NetworkDiscoveryTypeE discoveryType;
	bool longLinkActive;
};	
	

struct GenericSendDataDelegateParam {
	WaveformId_t wid;
	uint16_t dataSize;
	FMessage_t &msg;
};


typedef  Delegate<bool, WaveformId_t, uint64_t, MessageTypeE, FMessage_t& , bool > U64AddressSendDataDelegate_t;

} //End namespace
}

#endif // CORE_TYPES_H_
