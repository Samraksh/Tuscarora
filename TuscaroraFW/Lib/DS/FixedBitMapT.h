#ifndef FIXED_BIT_MAP_H_
#define FIXED_BIT_MAP_H_

#include <string.h>


/*
 * Purpose: Efficiently store a single bit information. The storage is size/8 + 2 bytes. Stores information as bits rather than as bool. Counting of bits across an array is not very efficient and hence couple of bytes is used to store the count.
 * Author: Mukundan Sridharan
 */


using namespace std;

/*static uint16_t FindArraySize(uint16_t inputsize){
		uint16_t arraySize =0;
		arraySize = inputsize/8;
		if(inputsize % 8) arraySize++;
		if (!arraySize) arraySize++;
		
		return arraySize;
}*/

uint16_t FindArraySize(uint16_t size){
	return size%8 ? size/8 + 1 : size/8;
}	

template <uint16_t size>
class FixedBitMapT {
	uint16_t setCount;
	volatile uint8_t array[size%8 ? size/8 + 1 : size/8];

public:	
	FixedBitMapT(){
		setCount = 0;
		memset((void*)array,0,FindArraySize(size));
	}

	void Set(uint16_t index){
		ASSERT_MSG(index<=size, "Index is out of bounds");

		uint16_t byte = index/8;
		uint8_t offset = index%8;
		
		uint8_t read = array[byte];
		
		uint8_t mask =0x01 << offset;
		array[byte] = read | mask;
		
		setCount++;
	}
	
	void UnSet(uint16_t index){
		ASSERT_MSG(index<=size, "Index is out of bounds");
		volatile uint16_t byte = index/8;
		volatile  uint8_t offset = index%8;
		
		volatile uint8_t read = array[byte];
		
		volatile uint8_t mask =0xFE << offset;
		array[byte] = read & mask;
		setCount--;
	}
	
	bool Read(uint16_t index){
		uint16_t byte = index/8;
		volatile uint8_t offset = index%8;
		volatile uint8_t read = array[byte];
		volatile uint8_t mask =0x01 << offset;
		return (read & mask);
	}
	
	uint16_t CountSet(){
		return setCount;
	}
	
	uint16_t CountUnSet(){
		return size-setCount;
	}
	
	uint16_t CountTotal(){
		return size;
	}
};


#endif //FIXED_BIT_MAP_H_