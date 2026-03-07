#ifndef LIDARSCANNER_H
#define LIDARSCANNER_H

#include "lidarBuffer.h"

class LidarScanner {
    private:
    LidarRingBuffer <float> a_lidar_ring;
    float max_safe_distance = 2.5f;
    bool brake_object;

    public:
    LidarScanner(size_t max_size) : a_lidar_ring { max_size }, brake_object { false }{
    }

    void lidar_reading(float new_distance){
        a_lidar_ring.push_to_ring(new_distance);
        auto recent_readings = a_lidar_ring.get_recent_readings(5);
        int tally = 0;
        for(size_t i = 0; i < recent_readings.size(); i++){
            if (recent_readings[i] < max_safe_distance){
                tally++;
            }
        }
        
        brake_object = (tally >= 3);

    }


    bool get_brake_state()const{
        return brake_object;
    }

};

#endif