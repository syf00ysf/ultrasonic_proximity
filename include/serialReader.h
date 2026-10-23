#pragma once

struct Measurement {
    unsigned long timestamp_ms;
    float distance_cm;
};
int open_serial(const char* port);
bool read_measurement(int connection, Measurement& result);
