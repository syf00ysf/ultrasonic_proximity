#pragma once
#include <string>

struct SerialReaderState {
    std::string pending_line;
};

enum class ReadStatus {
    MeasurementReady,
    InvalidLine,
    Timeout,
    Disconnected,
    Error
};

struct Measurement {
    unsigned long timestamp_ms;
    float distance_cm;
};
int open_serial(const char* port);
ReadStatus read_measurement(int connection, Measurement& result, SerialReaderState& state);
 