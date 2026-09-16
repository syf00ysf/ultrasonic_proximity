#include <cassert>
#include "ultrasonicBuffer.h"

int main(){
    RingBuffer<int> ring{3};
    assert(ring.pop_from_ring() == std::nullopt);
    ring.push_to_ring(10);
    assert(ring.pop_from_ring() == 10);
    ring.push_to_ring(20);
    ring.push_to_ring(30);
    ring.push_to_ring(40);
    ring.push_to_ring(50);
    assert(ring.pop_from_ring() == 30);
    assert(ring.pop_from_ring() == 40);
    assert(ring.pop_from_ring() == 50);
    assert(ring.pop_from_ring() == std::nullopt);
}