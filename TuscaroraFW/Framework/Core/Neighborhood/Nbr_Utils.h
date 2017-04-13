////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef NBR_UTILS_H_
#define NBR_UTILS_H_

#include <stdio.h>
#include <string>

namespace Core {
namespace Neighborhood {

class NbrUtils {
	
public:

	static void ConvertU64ToEthAddress(uint64_t x, char *address){
		char *y = (char *)&x;
		sprintf(address, "%02X:%02X:%02X:%02X:%02X:%02X",y[5],y[4],y[3],y[2],y[1],y[0]);
		address[18]='\0';
		//printf("ConvertU64ToEthAddress:: Input: %lu, output: %s \n", x,(char*)address);
	}	

	static string ConvertU64ToEthAddress(uint64_t x){
		uint8_t *y = (uint8_t *)&x;
		char address[18];
		sprintf(address, "%02X:%02X:%02X:%02X:%02X:%02X",y[5],y[4],y[3],y[2],y[1],y[0]);
		address[18]='\0';
		//printf("ConvertU64ToEthAddress:: Input: %lu, output: %s \n", x,(char*)address);
		return string(address);
	}
	
	static uint64_t ConvertEthAddressToU64(string x){
		
		char b[64];
		uint32_t a[6];
		strcpy(b,x.c_str());
		uint64_t ret=0;
		int s=6;
		sscanf(b, "%02X:%02X:%02X:%02X:%02X:%02X",&a[5],&a[4],&a[3],&a[2],&a[1],&a[0]);
		for (int i=0; i< s; i++){
			ret = ret | ((0xffffffffffffffff&a[i])<<((i*8)));
			//printf ("%02X\n", a[i]);
		}
		//printf("ConvertEthAddressToU64:: Input %s, Output: %lu \n",b, ret);
		return ret;
	}
};

} //End namespace
}
#endif //NBR_UTILS_H_