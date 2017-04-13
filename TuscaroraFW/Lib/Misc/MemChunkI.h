////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 




#ifndef MemChunkI_h
#define MemChunkI_h


#include "Base/BasicTypes.h"

class MemChunkI {
public:
	virtual MemChunkI& operator = (const MemChunkI& rhs) =0;
	virtual bool operator == (MemChunkI& rhs) =0;
	virtual const uint8_t& operator [] (const uint32_t index);
	virtual uint32_t ByteLen() =0;
    virtual ~MemChunkI() {}
};

#endif // MemChunkI_h
