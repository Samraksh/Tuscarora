////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef FRAMEWORK_STAT_H_
#define FRAMEWORK_STAT_H_


struct FrameworkStat{
    uint16_t positive_frameworkAck;
    uint16_t negative_frameworkAck;
    uint16_t total_valid_sent;  //total number of unicast which was on the neighbor table
    uint16_t total_sent;  //total number of unicast requested by pattern
    uint16_t total_invalid_destination;
};

struct NeighborStat{
    uint16_t neighbor_size;
};


struct locationStat{
    uint16_t x;
    uint16_t y;
};

struct FrameworkStatBroadcast{
    uint16_t positive_fw_ack;
    uint16_t negative_fw_ack;
    uint16_t data_broadcast_sent;
    uint16_t le_broadcast_sent;
};


#endif //FRAMEWORK_STAT_H_