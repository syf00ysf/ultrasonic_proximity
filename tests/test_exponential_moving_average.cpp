#include <cassert>
#include <exponentialMovingAverageFilter.h>

int main(){
    ExponentialMovingAverageFilter<float> filter{0.5};
    assert(filter.update(49.0f) == 49.0f);
    assert(filter.update(30.0f) == 39.5f);
    assert(filter.update(30.0f) == 34.75f);
    filter.reset();
     assert(filter.update(49.0f) == 49.0f);
}