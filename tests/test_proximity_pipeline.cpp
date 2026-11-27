#include <cassert>
#include <chrono>
#include <cmath>
#include "proximityPipeline.h"

void test_normal_braking(){
    const auto t0 = ProximityPipeline::TimePoint{};
    ProximityPipeline pipeline(5, 5, 25.0f, std::chrono::milliseconds(3000) ,t0);
    pipeline.process_measurement({0, 20.0}, t0);
    //enter stale and trigger brake
    assert(pipeline.update_stale(t0 + std::chrono::milliseconds(3100)) == true);
    assert(pipeline.brake_engaged() == true);
    
   

}

int main(){
    test_normal_braking();
}