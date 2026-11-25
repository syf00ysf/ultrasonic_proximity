#include "timestampValidation.h"




TimestampStatus valid_timestamp(unsigned long timestamp_ms, bool has_previous_timestamp ,unsigned long previous_timestamp){

    if(!has_previous_timestamp){
        return TimestampStatus::FirstReading;
    }
    else if(timestamp_ms == previous_timestamp){
        return TimestampStatus::Duplicate;
    }
    else if(timestamp_ms < previous_timestamp){
        return TimestampStatus::Discontinuity;
    }else{
        return TimestampStatus::Normal;
    }
}