#pragma once

enum class TimestampStatus {
    FirstReading,
    Normal, 
    Duplicate,
    Discontinuity
};

TimestampStatus valid_timestamp(unsigned long timestamp_ms, bool has_previous_timestamp, unsigned long previous_timestampc);