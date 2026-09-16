#include <cassert>
#include "movingAverageFilter.h"

int main(){
    MovingAverageFilter<float> filter{3};
    assert(filter.update(5.0f) == 5.0f);
    assert(filter.update(15.0f) == 10.0f);
    assert(filter.update(16.0f) == 12.0f);
    assert(filter.update(20.0f) == 17.0f);
    filter.reset();
    assert(filter.update(100.0f) == 100.0f);
}
