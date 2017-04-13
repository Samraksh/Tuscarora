////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef FixedPointT_h
#define FixedPointT_h

#include <stdio.h>

namespace Types {

//#define FULLINT(X) uint ## X ## _t
  
  //X is the number of bits for part before radix point, Y is the number of bits for radix point
  
  //A is the integer part and B is the decimal part
#define FCONST(TYPE, X, Y, A, B) (TYPE)((A<<Y) + ((B + 1/(2^(Y+1)))*2^Y))
#define FCONST_1_7(A,B) FCONST(uint8_t, 1, 7, A, B)

#define FMULT(TYPE, X, Y, A, B) (TYPE)(A.full*B.full+ 2^(Y-1))>>Y 
#define FMULT_1_7(A, B) FMULT(uint8_t, 1,7, A, B)
  
#define FDIV(TYPE, A, B, X , Y) (TYPE)((A.full<<Y+1)/B.full)+1)/2
#define FDIV_1_7(A, B) FDIV(uint8_t, 1, 7, A,B)
  
  template<bool signbit, uint8_t radix_bits, uint8_t total_bits>
  class FixedPointT {
    bool sign; 
    //FULLINT(total_bits) fp_state;
    uint32_t FULLINT;

  public:
    FixedPointT(){
      sign=signbit;
      //uint8_t totalBytes = total_bits/8;
      //fp_state = new uint8_t[totalBytes];
    }

    FixedPointT& operator = (const FixedPointT& rhs){
      FixedPointT& ret = *this;
      return ret;
    }

    bool operator == (const FixedPointT& rhs){
      return false;
    }
    
    bool operator > (const FixedPointT& rhs){
      return false;
    }
    
    bool operator < (const FixedPointT& rhs){
      return false;
    }
    
    /*const uint8_t& operator [] (const uint32_t index) {
      return &(*fp_state);
    }*/

    uint32_t ByteLen() {
      return 0;
    }

  };

  
  template<bool sign, uint8_t radix_bits>
  class FixedPointT<sign, radix_bits, 8> {
    typedef union Fixed1_7tag {
      unsigned char full;
      struct part1_7tag {
	unsigned char fraction: 7;
	unsigned char integer: 1;
      }part;
    }Fixed1_7;
    
  public:
     Fixed1_7 state;
    
  public:
    FixedPointT(){
      state.full = 0;
    }
    FixedPointT(uint8_t _full){
      state.full = _full;
    }
    FixedPointT(uint8_t _x, uint8_t _y){
      state.full = FCONST_1_7(_x, _y);
    }
    
    //assignment
    void operator = (const FixedPointT& rhs){
      state.full = rhs.state.full;
    }
    
    //Multiply
    FixedPointT operator * (const FixedPointT& rhs){
      return FixedPointT<sign, radix_bits, 8>(FMULT_1_7(*this, rhs));
    }
    
    FixedPointT operator + (const FixedPointT& rhs){
      return FixedPointT<sign, radix_bits, 8>(this->state.full + rhs.state.full);
    }
    
    bool operator == (const FixedPointT& rhs){
      return false;
    }
    
    bool operator > (const FixedPointT& rhs){
      return false;
    }
    
    bool operator < (const FixedPointT& rhs){
      return false;
    }

    char* print () {
      char * ret = new char[5];
      sprintf(ret,"%d.%2.2d", state.part.integer, (state.part.fraction*100+64)/128);  
      return ret;
    }
  };
  
  //below are not used in framework
  //typedef FixedPointT<true,20,32> Fixed_20_32_t;
  //typedef FixedPointT<false,20,32> UFixed_12_16_t;
  //typedef FixedPointT<true,7,8> Fixed_7_8_t;


  //these four are the key types used in framework, that needs to be implemented.
  //But will type cast them to float temporarily
  //typedef FixedPointT<true,15,16> Fixed_15_16_t;
  //typedef FixedPointT<true,2,8> Fixed_2_8_t;
  //typedef FixedPointT<false,2,8> UFixed_2_8_t;
  //typedef FixedPointT<false,7,8> UFixed_7_8_t;

	typedef float Fixed_7_16_t;
	typedef float Fixed_14_16_t;
	typedef float Fixed_15_16_t;
	typedef float Fixed_2_8_t;
	typedef float UFixed_2_8_t;
	typedef float UFixed_7_8_t;
}
#endif //FixedPointT_h
