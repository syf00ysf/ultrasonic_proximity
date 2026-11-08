#include <cassert>
#include <limits>
#include "measurementValidation.h"

int main(){
    assert(is_valid_distance(19.0f));
    assert(is_valid_distance(2.0f));
    assert(is_valid_distance(400.0f));
    assert(!is_valid_distance(-5.0f));
    assert(!is_valid_distance(1.0f));
    assert(!is_valid_distance(401.0f));
    assert(!is_valid_distance(0.0f));
    assert(!is_valid_distance(std::numeric_limits<float>::infinity()));
    assert(!is_valid_distance(std::numeric_limits<float>::quiet_NaN()));
}