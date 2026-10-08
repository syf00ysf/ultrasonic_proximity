#ifndef ULTRASONICSCANNER_H
#define ULTRASONICSCANNER_H

#include "ultrasonicBuffer.h"
#include "stdexcept"
 
class UltrasonicScanner {
    private:
    RingBuffer <float> a_ring;
    float threshold_cm;
    bool brake_object;
 
    public:
    UltrasonicScanner(std::size_t capacity, float threshold_cm) 
        : a_ring { capacity }, 
          threshold_cm { threshold_cm},
        brake_object { false }
    {
        if (threshold_cm <= 0 ){
            throw std::invalid_argument(
                "Threshold must be greater than 0"
            );
        }
    }

    void push_reading(float new_distance){
        a_ring.push_to_ring(new_distance);
        auto recent_readings = a_ring.get_recent_readings(5);
        int tally = 0;
        for(size_t i = 0; i < recent_readings.size(); i++){
            if (recent_readings[i] < threshold_cm){
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