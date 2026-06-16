#ifndef ULTRASONICSCANNER_H
#define ULTRASONICSCANNER_H

#include "ultrasonicBuffer.h"
 
class UltrasonicScanner {
    private:
    RingBuffer <float> a_ring;
    float max_safe_distance = 2.5f;
    bool brake_object;
 
    public:
    UltrasonicScanner(size_t max_size) : a_ring { max_size }, brake_object { false }{
    }

    void push_reading(float new_distance){
        a_ring.push_to_ring(new_distance);
        auto recent_readings = a_ring.get_recent_readings(5);
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