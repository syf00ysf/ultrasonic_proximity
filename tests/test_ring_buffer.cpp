#include <cassert>
#include "ultrasonicBuffer.h"

int main(){
    RingBuffer<int> ring{3};
    assert(ring.empty() == true);
    assert(ring.size() == 0);
    assert(ring.pop_from_ring() == std::nullopt);
    ring.push_to_ring(10);
    assert(ring.pop_from_ring() == 10);
    ring.push_to_ring(20);
    ring.push_to_ring(30);
    ring.push_to_ring(40);
    ring.push_to_ring(50);
    assert(ring.empty() == false);
    assert(ring.size() == 3);
    assert(ring.pop_from_ring() == 30);
    assert(ring.pop_from_ring() == 40);
    assert(ring.pop_from_ring() == 50);
    ring.push_to_ring(20);
    ring.clear();
    ring.push_to_ring(77);
    ring.push_to_ring(78);
    assert(ring.pop_from_ring() == 77);
    assert(ring.pop_from_ring() == 78);
    assert(ring.empty() == true);
    assert(ring.pop_from_ring() == std::nullopt);
}