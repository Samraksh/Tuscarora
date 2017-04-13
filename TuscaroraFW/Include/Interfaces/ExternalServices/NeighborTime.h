////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef NEIGHBOR_TIME_H_
#define NEIGHBOR_TIME_H_

namespace ExternalServices {
	template <class TimeA, class TimeB, class NodeIDType>
	class NeighborTimeI {
	public:
		virtual TimeB GetNeighborTime(NodeIDType nbrId, TimeA localtime)=0;
		virtual TimeA GetLocalTime( NodeIDType nbrId, TimeB nbrTime=0);
		virtual ~NeighborTimeI() {}
	};
}

#endif /* NEIGHBOR_TIME_H_ */
