#pragma once

#include <chrono>
#include <cstddef>
#include "medianFilter.h"
#include "serialReader.h"
#include "timestampValidation.h"
#include "ultrasonicScanner.h"

enum class SampleOutcome {
    Accepted,
    DuplicateTimestamp,
    InvalidDistance
};

struct PipelineStepResult {
    SampleOutcome outcome;
    TimestampStatus timestamp_status;
    bool recovered_from_stale;
    float filtered_distance_cm;
    bool brake_engaged;
    bool failsafe_active;
};

class ProximityPipeline {
    public:
        using Clock = std::chrono::steady_clock;
        using TimePoint = Clock::time_point;

        ProximityPipeline(
            std::size_t filter_window, 
            std::size_t scanner_window,
            float threshold,
            std::chrono::milliseconds stale_after,
            TimePoint start_time);

        bool update_stale(TimePoint now);

        PipelineStepResult process_measurement(const Measurement& reading, TimePoint now);

        bool is_stale() const noexcept;
        bool has_valid_reading() const noexcept;
        bool brake_engaged() const noexcept;
        bool failsafe_active() const noexcept;
        
    
    private:
        MedianFilter<float> filter_;
        UltrasonicScanner scanner_;
        std::chrono::milliseconds stale_after_;
        TimePoint last_valid_received_;
        bool has_valid_reading_;
        bool is_stale_;
        bool has_previous_timestamp_;
        unsigned long previous_timestamp_;

   

    

};