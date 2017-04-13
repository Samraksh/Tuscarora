////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef HISTORY_I_H_
#define HISTORY_I_H_

namespace PAL {
	///Generic interface class for an History class. The History class enables storing objects of some type
	///associated with a index for retrival latter
	template <class ObjectType>
	class HistoryI{
	public:
		///Store an object in the History store using its reference
		virtual bool Store(ObjectType &object)=0;

		///Returns the size of the History
		virtual uint16_t Size()=0;

		///Prunes the oldest x objects to limit the size of the History to the number of elements specified as parameter.
		/// If the parameter specified is greater than number of objects in the History, method has no effect.
		virtual bool Prune(uint16_t noOfElements)=0;

		///Returns an object reference indicated by the index
		virtual ObjectType& GetElement(uint16_t index)=0;

		///Virtual destructor for the interface.
		virtual inline ~HistoryI(){}
	};
}

#endif /* HISTORY_I_H_ */
