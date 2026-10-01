#include <cassert>
#include <exponentialMovingAverageFilter.h>

int main(){
    ExponentialMovingAverageFilter<float> filter{0.5};
    assert(filter.update(49.0f) == 49.0f);
    assert(filter.update(30.0f) == 39.5f);
    assert(filter.update(30.0f) == 34.75f);
    filter.reset();
    assert(filter.update(49.0f) == 49.0f);

    ExponentialMovingAverageFilter<float> filter_a1{1};
    assert(filter_a1.update(49.0f) == 49.0f);
    assert(filter_a1.update(30.0f) == 30.0f);

    bool rejected_zero = false;

    try{
        ExponentialMovingAverageFilter<float> filter_a2{0};
    }catch (const std::invalid_argument&){
        rejected_zero = true;
    }

    
    assert(rejected_zero);



}