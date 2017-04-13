////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 



#ifndef APPS_APPBASE_H_
#define APPS_APPBASE_H_

namespace Apps {

class AppBase {
public:
	virtual void StartApp() = 0;
};

} /* namespace Apps */

#endif /* APPS_APPBASE_H_ */
