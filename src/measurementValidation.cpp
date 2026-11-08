#include <cmath>
#include "measurementValidation.h"

const int MIN_SENSOR_DIST = 2;
const int MAX_SENSOR_DIST = 400;

bool is_valid_distance(float distance_cm){
    if(distance_cm >= MIN_SENSOR_DIST && distance_cm <= MAX_SENSOR_DIST && std::isfinite(distance_cm)){
        return true;
    }
    return false;
}