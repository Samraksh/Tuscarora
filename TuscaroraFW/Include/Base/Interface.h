////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef INTERFACE_H_
#define INTERFACE_H_


//This is a combination of 
//http://programmers.stackexchange.com/questions/235674/what-is-the-pattern-for-a-safe-interface-in-c
//http://stackoverflow.com/questions/10273227/instantiating-a-class-that-derives-from-an-interface-thats-inside-another-class


#define interface class
#define abstract_class class
#define implements : public 


#define DECLARE_AS_INTERFACE(ClassName)                                \
   public :                                                                  \
      virtual ~ClassName() {}                                                \
   protected :                                                               \
      ClassName() {}                                                         \
      ClassName(const ClassName & ) {}                                       \
      ClassName & operator = (const ClassName & ) { return *this ; }         \
   private :
	   
	   
	  
#define INTERFACE_H_	   