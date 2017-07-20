////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef LIB_PAL_FILELOGGERIMPL_H_
#define LIB_PAL_FILELOGGERIMPL_H_

#include <PAL/FileLogger.h>
#include "../Logs/MemMap.h"

template <class Obj>
class FileLogger<Obj>::FileLoggerImpl{
public:
      friend class FileLogger<Obj>;
	  bool record_in_memmap;
	  bool record_in_txt;
	  std::string fname2;
	  std::ofstream file2;
	  MemMap<Obj>* m_memmap_ptr;
};

template <class Obj>
FileLogger<Obj>::FileLogger(std::string filename, bool _record_in_txt , bool _record_in_memmap  ){
	  static_assert(std::is_base_of<FileLogElement, Obj>::value, "Derived not derived from BaseClass");
	  implPtr->record_in_txt = _record_in_txt;
	  implPtr->record_in_memmap = _record_in_memmap;

	  if(implPtr->record_in_memmap) implPtr->m_memmap_ptr = new MemMap<Obj> (filename.append(".bin"));

//		  implPtr->fname2 = filename.substr(0,filename.size()-4) + ".txt";
	  implPtr->fname2 = filename + ".txt";
	  //implPtr->fname2.append
	  implPtr->file2.open(implPtr->fname2.c_str(), std::ios::out| std::ios::app);
	  Obj obj;
	  implPtr->file2 << (obj.PrintHeader()).c_str() << "\n";
	  implPtr->file2.close();
	  printf("Opened log file %s \n",implPtr->fname2.c_str() );
};

template <class Obj>
FileLogger<Obj>::~FileLogger(){
	  implPtr->file2.close();
	  printf("Closed log file %s",implPtr->fname2.c_str() );
}

template <class Obj>
void FileLogger<Obj>::addRecord(const Obj& obj) {
	  if(implPtr->record_in_memmap) implPtr->m_memmap_ptr->addRecord(obj);
	  if(implPtr->record_in_txt) {
		  if ( !implPtr->file2.is_open() ) implPtr->file2.open(implPtr->fname2.c_str(), std::ios::out| std::ios::app);
		  implPtr->file2 << obj.PrintContents() << "\n";
		  implPtr->file2.close();
	  }
};


//FileLogElementWTime::FileLogElementWTime() : FileLogElement(){
//	CurTime = PAL::SysTime::GetTimeMicro();
//};
//
//std::string FileLogElementWTime::PrintHeader() const {
//	std::string out("CurTime, ");
//	return out;
//};
//std::string FileLogElementWTime::PrintContents() const {
//    std::ostringstream ss;
//    ss << CurTime<< ", ";
//    return ss.str();;
//};

#endif /* LIB_PAL_FILELOGGERIMPL_H_ */
