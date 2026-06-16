# Proximity Detection System
A real-time proximity detection system to simulate ultrasonic sensor input using a custom ring buffer and a windowed brake logic in C++ 20.

## Table of contents
- [Overview](#overview)
- [Architecture](#architecture)
- [RingBuffer](#ringbuffer-includes)
- [UltrasonicScanner](#ultrasonicscanner-includes)
- [Brake logic](#brake-logic)
- [How to use](#how-to-use)
- [Sample Output](#sample-output)
- [Performance](#performance)
- [Design Decision](#design-decision)
- [Future Work](#future-work)
- [Author](#author)

## Overview
This project models a core problem in perception systems: ***how to make reliable decisions from noisy, high-frequency sensor data?***
My first implementation of this would trigger a brake on a single unsafe reading, which would cause false readings from sensor noise. Instead this system uses a **sliding window to get last n readings** and only triggers a brake when a majority breach the safety threshold. 

## Architecture

``` 
.
├── Makefile
├── README.md
├── include
│   ├── ultrasonicBuffer.h
│   └── ultrasonicScanner.h
└── src
    └── main.cpp

```

## RingBuffer (ultrasonicBuffer.h)
A fixed-capacity ring buffer with full Rule of Five:
- Copy constructor and copy assignment
- Move constructor and move assignment
- Destructor with manual heap cleanup

Supports `push_to_ring()` (lvalue and rvalue overloads), `pop_from_ring()` returning `std::optional<T>`, and `get_recent_readings()` to get last n readings.


## UltrasonicScanner (ultrasonicScanner.h)
Wraps the ring buffer and implements windowed brake detection:
- `push_to_ring()` pushes each new distance reading into the buffer
- It inspects the last 5 readings
- It sets brake state if **3 or more** readings are below `max_safe_distance` (2.5m)
- Brake state resets automatically when readings return to safe range

---
## Brake Logic

```cpp
// Triggers brake if 3 of the last 5 readings breach threshold
brake_object = (tally >= 3);
```

This windowed majority approach prevents single noisy readings from triggering a false brake, while still responding quickly to a genuine obstacle.


## How to use

***Require g++ with C++20 support***

```bash
# Build
make

# Run 
./scanner
```

## Sample Output

```
Let's go
Object stopped!         ← 3 unsafe readings at i=900000, brake triggers
Object still running!   ← 99,999 safe readings follow, brake resets
It took : 43.21ms       ← 1,000,000+ iterations processed
```

## Performance

Benchmarked with `std::chrono::high_resolution_clock` over 1,000,000+ iterations. The ring buffer operates in **O(1)** for push and window retrieval.

## Design Decision

| Decision | Rationale |
|---|---|
| Manual heap allocation over `std::vector` | Demonstrates explicit memory management and ownership semantics |
| Rule of Five | Ensures correct behavior when scanner objects are copied or moved |
| `std::optional` on `pop_from_ring` | Avoids undefined behavior on empty buffer without exceptions |
| Windowed majority vote | More robust to sensor noise than single-reading threshold |
| Threshold configurable via member variable | Easy to extend with runtime configuration |


---

## Future Work

- Configurable window size and threshold at runtime
- ROS2 node wrapper for integration with real sensor hardware
- Multi-sensor fusion across multiple `UltrasonicScanner` instances
- Unit test suite with edge case coverage

---

## Author
Youssouf - [syf00ysf](https://github.com/syf00ysf)