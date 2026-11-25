#include <cassert>
#include "timestampValidation.h"

int main(){
    assert(valid_timestamp(100, false, 0) == TimestampStatus::FirstReading);
    assert(valid_timestamp(50, false, 100) == TimestampStatus::FirstReading);
    assert(valid_timestamp(0, false, 0) == TimestampStatus::FirstReading);
    assert(valid_timestamp(10, true, 10) == TimestampStatus::Duplicate);
    assert(valid_timestamp(50, true, 100) == TimestampStatus::Discontinuity);
    assert(valid_timestamp(200, true, 150) == TimestampStatus::Normal);
}