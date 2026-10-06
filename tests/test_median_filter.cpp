#include <cassert>
#include <medianFilter.h>

int main(){
    MedianFilter<float> odd_filter{5};
    assert(odd_filter.update(49.0f) == 49.0f);
    assert(odd_filter.update(50.0f) == 49.5f);
    assert(odd_filter.update(798.0f) == 50.0f);
    assert(odd_filter.update(49.0f) == 49.5f);
    assert(odd_filter.update(50.0f) == 50.0f);
    odd_filter.reset();
    assert(odd_filter.update(100.0f) == 100.0f);

    MedianFilter<float> even_filter{4};
    assert(even_filter.update(10.0f) == 10.0f);
    assert(even_filter.update(20.0f) == 15.0f);
    assert(even_filter.update(30.0f) == 20.0f);
    assert(even_filter.update(40.0f) == 25.0f);
    assert(even_filter.update(50.0f) == 35.0f);

    bool rejected_zero = false;
    try{
        MedianFilter<float> filter{0};
    }catch (const std::invalid_argument&){
        rejected_zero = true;
    }
    assert(rejected_zero);

}