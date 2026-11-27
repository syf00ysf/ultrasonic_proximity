#ifndef ULTRASONICSCANNER_H
#define ULTRASONICSCANNER_H

#include "ultrasonicBuffer.h"
#include "stdexcept"

class UltrasonicScanner {
    private:
    RingBuffer <float> a_ring;
    float threshold_cm;
    bool brake_object;
    bool failsafe_active;
 
    public:
    UltrasonicScanner(std::size_t capacity, float threshold_cm) 
        : a_ring { capacity }, 
        threshold_cm { threshold_cm},
        brake_object { false },
        failsafe_active { false }
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
        int close_count = 0, clear_count = 0;
        for(size_t i = 0; i < recent_readings.size(); i++){
            if (recent_readings[i] < threshold_cm){
                close_count++;

            }
            else if(recent_readings[i] >= threshold_cm){
                clear_count++;
            }
        }
        if(is_in_failsafe()){
            if(clear_count >= 3){
                failsafe_active = false;
                brake_object = false;
            }else{
                brake_object = true;
            }
        }else{
            brake_object = (close_count >= 3);
        }

    }


    bool get_brake_state()const{
        return brake_object;
    }

    void reset() noexcept {
        a_ring.clear();
        brake_object = false;
        failsafe_active = false;
    }
    void enter_failsafe() noexcept {
        a_ring.clear();
        brake_object = true;
        failsafe_active = true;


    }
    bool is_in_failsafe()const noexcept{
        return failsafe_active;
    }

};

#endif