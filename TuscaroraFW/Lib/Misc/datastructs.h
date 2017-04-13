////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef DATASTRUCTS_H_
#define DATASTRUCTS_H_

#include<map> 
#include<vector>
#include<set>

// encapsulate the std template datastructures
// see http://stackoverflow.com/a/2795024/268040
template <typename K, typename V> 
struct map_t {
  typedef std::map<K,V> type;
};

template <typename T> 
struct vector_t {
  typedef std::vector<T> type;
};

template <typename T> 
struct set_t {
  typedef std::set<T> type;
};

#endif
