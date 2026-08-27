# Proximity Detection System
A real-time proximity detection system that processes and characterizes ultrasonic sensor data using a custom ring buffer, sensor noise analysis, and windowed brake logic in C++20.

## Table of contents
- [Overview](#overview)
- [Architecture](#architecture)
- [RingBuffer](#ringbuffer)
- [UltrasonicScanner](#ultrasonicscanner)
- [UltrasonicCharacterizer](#ultrasoniccharacterizer)
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
├── Data
│   ├── readings01.log
│   ├── readings02_moving.log
│   └── readings03_static50.log
├── include
│   ├── ultrasonicBuffer.h
│   ├── ultrasonicScanner.h
│   └── ultrasonicCharacterizer.h
└── src
    └── main.cpp

```

## RingBuffer
A fixed-capacity ring buffer with full Rule of Five:
- Copy constructor and copy assignment
- Move constructor and move assignment
- Destructor with manual heap cleanup

Supports `push_to_ring()` (lvalue and rvalue overloads), `pop_from_ring()` returning `std::optional<T>`, and `get_recent_readings()` to get last n readings.


## UltrasonicScanner
Wraps the ring buffer and implements windowed brake detection:
- `push_to_ring()` pushes each new distance reading into the buffer
- It inspects the last 5 readings
- It sets brake state if **3 or more** readings are below `max_safe_distance` (2.5m)
- Brake state resets automatically when readings return to safe range

---

## UltrasonicCharacterizer

This is a summary of the characterization of different readings:
- ***readings01*** is an old legacy capture without timestamps and uncontrolled measurement.
- ***readings02_moving*** with a moving target.
- ***readings03_static50*** with a static wall at 50 cm reference.


| Stat                | readings01 | readings02_moving | readings03_static50 |
|---------------------|-----------:|------------------:|-------------------:|
| jitter_std (ms)     |     n/a    | 0.96              | 0.63               |
| delta_std (cm)      |     -      | 4.29              | 0.57               |
| corrupted (%)       |     2.4    | 0                 | 0                  |
| max_low_run(<10)    |     37     | 9                 | 0                  |
| max_spike_run (>= 798)|   3      | 0                 | 0                  |
| mean reading (cm)   |      -     | 28.76             | 49.33              |
| reading_std (cm)    |       -    | 14.22             | 0.47               |

1.  `noise floor <=1cm in readings03_static50 (measured 0.47)`

    In the ***readings03_static50*** test, the reading only varied about 0.47 cm which is less than 1 cm. The sensor can't really express tiny sub-centimeter changes, so the noise is basically limited by measurement resolution.

2. `bias -0.67 cm`

    Measured mean in the ***readings03_static50*** test shows that the sensor reads slightly short about: `49.33 - 50.00 = -0.67 cm` smaller than the sensor's 1 cm resolution.

3. `Jitter < 1ms validating constant-dt`

    Both ***readings03_static50*** and ***readings02_moving*** timestamps are spaced very consistently. Jitter standard deviation is under 1 ms so later filters can safely assume constant time steps.

4. `The filter parameter`, Kalman measurement noise variance is standard deviation squared:

    `reading_std²(static) ≈ 0.25 cm²`
 



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