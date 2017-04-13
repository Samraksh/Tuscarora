////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 




#ifndef InfoLogger_H_
#define InfoLogger_H_



class FileLogElement{
public:
	virtual std::string PrintHeader() const = 0 ;
	virtual std::string PrintContents() const = 0 ;
};


class FileLogElementWTime : public FileLogElement{
protected:
	uint64_t CurTime;
public:
	FileLogElementWTime(){
		CurTime = PAL::SysTime::GetEpochTimeInMicroSec();
	};
	virtual std::string PrintHeader() const{
		std::string out("CurTime, ");
		return out;
	};
	virtual std::string PrintContents() const{
	    std::ostringstream ss;
	    ss << CurTime<< ", ";
	    return ss.str();;
	};
};


template <typename Obj>
class FileLoggerTypeEnforcer{
public:
	FileLoggerTypeEnforcer(){
		static_assert(std::is_base_of<FileLogElement, Obj>::value, "Derived not derived from BaseClass");
	}
};


template <class Obj>
class FileLogger : public FileLoggerTypeEnforcer<Obj>{
private:
	 class FileLoggerImpl;
	 FileLoggerImpl* implPtr;
public:
	  FileLogger(std::string filename, bool _record_in_txt = true, bool _record_in_memmap = false );
	  ~FileLogger();
	  void addRecord(const Obj& obj);
};
























#include "../../Lib/PAL/FileLoggerImpl.h"

#endif //InfoLogger_H_
