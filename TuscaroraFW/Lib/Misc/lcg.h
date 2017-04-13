////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 


/// Linear Congruential Generator
///
/// See http://en.wikipedia.org/wiki/Linear_congruential_generator.

/// This type is used in all the internal calculations.q
typedef unsigned long long internal_lcg_t;

struct lcg_base_t {
  internal_lcg_t a, c, m;
  internal_lcg_t seed;
  lcg_base_t (): m_is_power_of_two(false) {};
  internal_lcg_t operator() () {
    return this->next(0);
  };
  internal_lcg_t operator() ( internal_lcg_t x ) {
    return this->next(x);
  };
  internal_lcg_t next ( internal_lcg_t x ) {
    seed = ( a * (seed+x) + c );
    if ( m_is_power_of_two ) seed &= (m-1);
    else seed %= m;
    return seed;
  };
  // hash function
  internal_lcg_t operator() ( void* buff, unsigned long long len ) {
    return hash ( buff, len );
  };
  internal_lcg_t hash ( void* buff, unsigned long long len ) {
    unsigned char *b = (unsigned char*) buff;
    for ( unsigned long long i = 0; i < len; i++ ) {      
      this->next ( b[i] );
    }
    return this->seed;
  };

protected:
  bool m_is_power_of_two;
  void set_power_of_two () {
    if ( m == 0 ) {
      throw "m should not be zero.";
    }
    for ( internal_lcg_t x = this->m; x > 0; x >>= 1 ) {
      if ( x & 1ULL ) {
	m_is_power_of_two = (x == 1ULL);
	return;
      }
    }
    m_is_power_of_two = true;
  };
};
    
struct lcg_visual6_t: lcg_base_t {
  lcg_visual6_t() {
    m = 1ULL << 24;
    a = 1140671485;
    c = 12820163;
    seed = 0;
    set_power_of_two ();
  };
};

typedef lcg_visual6_t lcg_t; // use microsoft visual basic 6 and earlier for portability (24 bit)

struct lcg_numerical_recipes_t: lcg_base_t {
  lcg_numerical_recipes_t() {
    m = 1ULL << 32;
    a = 1664525;
    c = 1013904223;
    seed = 0;
    set_power_of_two ();
  };
};
  
struct lcg_borland_t: lcg_base_t {
  lcg_borland_t() {
    m = 1ULL << 32;
    a = 22695477;
    c = 1;
    seed = 0;
    set_power_of_two ();
  };
};
  
struct lcg_glibc_t: lcg_base_t {
  lcg_glibc_t() {
    m = 1ULL << 31;
    a = 1103515245;
    c = 12345;
    seed = 0;
    set_power_of_two ();
  };
};
  
struct lcg_ansi_t: lcg_base_t {
  lcg_ansi_t() {
    m = 1ULL << 31;
    a = 1103515245;
    c = 12345;
    seed = 0;
    set_power_of_two ();
  };
};
  
struct lcg_delphi_t: lcg_base_t {
  lcg_delphi_t() {
    m = 1ULL << 32;
    a = 134775813;
    c = 1;
    seed = 0;
    set_power_of_two ();
  };
};

struct lcg_visual_t: lcg_base_t {
  lcg_visual_t() {
    m = 1ULL << 32;
    a = 134775813;
    c = 2531011;
    seed = 0;
    set_power_of_two ();
  };
};

struct lcg_rtl_t: lcg_base_t {
  lcg_rtl_t() {
    m = 1; m <<= 31; m--;
    a = 2147483629;
    c = 2147483587;
    seed = 0;
    set_power_of_two ();
  };
};

struct lcg_vax_t: lcg_base_t {
  lcg_vax_t() {
    m = 1; m <<= 32;
    a = 69069;
    c = 1;
    seed = 0;
    set_power_of_two ();
  };
};

