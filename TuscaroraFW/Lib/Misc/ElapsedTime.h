////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef ELAPSEDTIME_H
#define ELAPSEDTIME_H

/// ElapsedTime::toMicroseconds returns the number of microseconds
/// since the start of the simulation as a 64 bit unsigned int. This
/// is enough bits to represent 126,755 years, so it should be
/// enough. We use a static variable usecs0 to represent the time in
/// microseconds of the start of the simulation and subtract from this
/// the current time in microseconds to get the elapsed time. To allow
/// initialization of usecs0, we declare it in a template class
/// (ElapsedTime_Base) which allows (requires?) the definition to be
/// in the same file as the declaration. The ElapsedTime class simply
/// derives from ElapsedTime_Base.


template <class Dummy>
struct ElapsedTime_Base {
  static uint64_t usecs0;  // start of the simulation in microseconds.
  static uint64_t CurrentTimeInMicroseconds() { // returns 
    struct timeval now;
    gettimeofday(&now, NULL);
    return now.tv_sec * 1000000 + now.tv_usec;
  };
};

// This definition in a header file would not be allowed in a non-template class.
template <class Dummy>
uint64_t ElapsedTime_Base<Dummy>::usecs0 = ElapsedTime_Base<Dummy>::CurrentTimeInMicroseconds();


// returns the time elapsed since the start of the simulation 
struct ElapsedTime:ElapsedTime_Base<void> {
  static uint64_t inMicroseconds() {
    return ElapsedTime::CurrentTimeInMicroseconds() - ElapsedTime::usecs0;
  };

  static double seconds() {
    uint64_t t = ElapsedTime::inMicroseconds();
    double u = 1000000;
    return t / u;
  };
  
  /// Warning: this code is not thread-safe: http://stackoverflow.com/a/453741/268040
  /// It returns a constant pointer to mutable memory.
  static const char* timestring() {
    static char _timeString[22]; // 64 bits gives 20 decimal digits + decimal pt + \0
    long long unsigned t = ElapsedTime::inMicroseconds(), u=1000000;
    sprintf ( _timeString, "%llu.%06llu", t/u, t%u );
    return _timeString;
  };
};

#endif // ELAPSEDTIME_H
