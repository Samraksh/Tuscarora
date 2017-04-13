////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#include "IO_Signals.h"

BSTMapT<int32_t, SocketSignalDelegate_t*>  IO_Signals::sockMap;
bool IO_Signals::lock;
bool IO_Signals::pending;
uint16_t IO_Signals::registered;
IO_Signals ioSignals;

void ReceiveSigIO(int sig){
	//printf("ReceiveSigIO::Kernal generated sigio signal %d\n", sig);
	ioSignals.CheckSocketsAndSignal();
}

void ReceiveIOSignal(int sig, siginfo_t *si, void *uc){
	//printf("ReceiveIOSignal::Kernal generated sigio signal %d\n", sig);
	ioSignals.CheckSocketsAndSignal();
}
