#include "proximityPipeline.h"
#include "measurementValidation.h"

ProximityPipeline::ProximityPipeline(
        std::size_t filter_window, 
        std::size_t scanner_window,
        float threshold,
        std::chrono::milliseconds stale_after,
        TimePoint start_time) :
        filter_(filter_window),
        scanner_(scanner_window, threshold)
        {
        stale_after_ = stale_after;
        last_valid_received_ = start_time;
        has_valid_reading_ = false;
        is_stale_ = false;
        has_previous_timestamp_ = false;
        previous_timestamp_ = 0;
        
        }

bool ProximityPipeline::update_stale(TimePoint now){
    auto elapsed = now - last_valid_received_;
    if(elapsed >= stale_after_ && !is_stale_){
        is_stale_ = true;
        scanner_.enter_failsafe();
        return true;
    }
    return false;
    }

PipelineStepResult ProximityPipeline::process_measurement(const Measurement& reading, TimePoint now){
    TimestampStatus tm_status = valid_timestamp(reading.timestamp_ms, has_previous_timestamp_, previous_timestamp_);

    switch(tm_status) {
        case TimestampStatus::FirstReading:
             has_previous_timestamp_ = true;
             break;
        case TimestampStatus::Duplicate:
            return PipelineStepResult{
                .outcome = SampleOutcome::DuplicateTimestamp,
                .timestamp_status = TimestampStatus::Duplicate,
                .recovered_from_stale = false,
                .filtered_distance_cm = 0.0f,
                .brake_engaged = scanner_.get_brake_state(),
                .failsafe_active = scanner_.is_in_failsafe()
            };
        case TimestampStatus::Discontinuity:
            filter_.reset();
            scanner_.enter_failsafe();
            break;
        case TimestampStatus::Normal:
            break;
    }
    previous_timestamp_  = reading.timestamp_ms;

    bool valid = is_valid_distance(reading.distance_cm);
    bool recovered_from_stale = false;
    if(!valid){
        return PipelineStepResult{
                .outcome = SampleOutcome::InvalidDistance,
                .timestamp_status = tm_status,
                .recovered_from_stale = false,
                .filtered_distance_cm = 0.0f,
                .brake_engaged = scanner_.get_brake_state(),
                .failsafe_active = scanner_.is_in_failsafe()
            };
    }
    else{
        
        if(is_stale_){
            if(has_valid_reading_ == true){
                filter_.reset();
            }
            is_stale_ = false; 
            recovered_from_stale = true;
        }
    }
    
    last_valid_received_ = now;
    has_valid_reading_ = true;
    float filtered_distance = filter_.update(reading.distance_cm);
    scanner_.push_reading(filtered_distance);

    return PipelineStepResult{
                .outcome = SampleOutcome::Accepted,
                .timestamp_status = tm_status,
                .recovered_from_stale = recovered_from_stale,
                .filtered_distance_cm = filtered_distance,
                .brake_engaged = scanner_.get_brake_state(),
                .failsafe_active = scanner_.is_in_failsafe()
            };
    
}
bool ProximityPipeline::is_stale() const noexcept{
    return is_stale_;
}
bool ProximityPipeline::has_valid_reading()const noexcept {
    return has_valid_reading_;
} 
bool ProximityPipeline::brake_engaged() const noexcept {
    return scanner_.get_brake_state();
}
bool ProximityPipeline::failsafe_active() const noexcept{
    return scanner_.is_in_failsafe();
}
