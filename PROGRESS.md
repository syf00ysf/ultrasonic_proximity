# Engineering Progress Log

## 2026-11-24

### Built & Landed
- **`e5f32a2` — Monotonic Timestamp Validation & Live Loop Invariants**
  - Completed and integrated `valid_timestamp(...)` (`FirstReading`, `Normal`, `Duplicate`, `Discontinuity`) with unit tests in `tests/test_timestamp_validation.cpp`.
  - Reordered the live serial loop in `src/main.cpp` so MCU clock/timestamp validation runs on *every* well-formed `MeasurementReady` packet before `is_valid_distance(...)`, while keeping `last_valid_received` and stale-recovery strictly gated on valid physical distances.
  - Renamed sample counter to `valid_count` and fixed a double-increment bug so the loop counts valid filtered readings consistently.
- **`40ecac0` — Automated Test Runner (`make test`) with Sanitizers**
  - Refactored `Makefile` to centralize `CORE_OBJS` and `DEPS` (`$(CORE_OBJS:.o=.d)`).
  - Added `.PHONY: test` compiling and executing all 7 `tests/test_*.cpp` suites with `-g -Wall -Wextra -fsanitize=$(SANITIZERS)` (defaulting to `undefined` / UBSan, overridable via `SANITIZERS=address,undefined`), `set -e` fail-fast execution, and `trap` temp-dir cleanup.

### Key SWE & Systems Concepts Covered
- **Clock Health vs. Physical Sensor Health:** Tracking MCU timestamps independently of acoustic echo failures prevents silent missed reboots (`4000 -> 5000 (0.0 cm) -> 4200`).
- **Discontinuity Re-Anchoring:** Updating `previous_timestamp` on `Discontinuity` (instead of `continue`) prevents every post-reboot packet from falsely triggering resets until the clock catches up.
- **Makefile Mechanics:** Variable expansion (`$(VAR)` vs literal text), substitution references (`$(CORE_OBJS:.o=.d)`), `.PHONY` targets, per-line subshells + `\` line continuations, `$$` shell escaping, `-c` (compile-only) vs linking, and `trap ... EXIT` cleanup.
- **Sanitizer Tooling:** Diagnosed macOS CLT ASan `malloc_zone` init abort vs. clean UBSan execution and designed overridable `SANITIZERS ?= undefined` flags for cross-platform/CI portability.

### Code-Review Takeaways
- Always check loop-bottom state mutations (`valid_count++`) after refactoring early-exit branches.
- Watch out for trailing whitespace/tabs after `\` in `Makefile` recipes—they silently break line continuation.

### Next Session
- **Step 3 (Design + Implement):** Add O(1) zero-allocation `RingBuffer::clear()` and fail-safe `UltrasonicScanner` reset/stale-sensor semantics + unit tests.
- **Step 4:** Wire `UltrasonicScanner` into the live pipeline and extract the per-sample pipeline step for deterministic offline testing.


## 2026-11-25

### Built & Landed
- **`0f4087e` — `RingBuffer::clear()` & Fail-Safe `UltrasonicScanner` Recovery**
  - Added O(1), zero-allocation, `noexcept` `clear()`, `size()`, and `empty()` to `RingBuffer<T>` in `include/ultrasonicBuffer.h` with unit tests in `tests/test_ring_buffer.cpp`.
  - Added `reset()`, `enter_failsafe()`, and `is_in_failsafe()` to `UltrasonicScanner` in `include/ultrasonicScanner.h`, plus quorum-based fail-safe recovery requiring 3 clear readings (`>= threshold_cm`) in the 5-sample window before releasing the brake.
  - Added unit tests in `tests/test_scanner.cpp` covering `reset()` history wiping, fail-safe activation/recovery, and mixed close/clear readings during fail-safe. All 7 test suites pass under UBSan with `-Wall -Wextra`.
- **Designed Step 4 (`ProximityPipeline`)**
  - Agreed on extracting the live loop's decision logic into `ProximityPipeline` with `update_stale(now)` and `process_measurement(reading, now)` taking injected `std::chrono::steady_clock::time_point` for deterministic offline testing.

### Key SWE & Systems Concepts Covered
- **Zero-Allocation Reset:** Resetting `head = tail = count = 0` reuses pre-allocated ring memory in O(1) without heap churn (safe for trivially destructible telemetry types).
- **Fail-Safe & Hysteresis Recovery:** Separating clean `reset()` from `enter_failsafe()` and requiring 3 clear readings to release the brake prevents a single noisy reading from releasing a safety stop; because `3 + 3 > 5`, close and clear quorums can never contradict each other.
- **Floating-Point `NaN` Safety:** Using `else if (val >= threshold_cm)` instead of a bare `else` ensures `NaN` never increments `clear_count` and cannot falsely release a fail-safe brake.
- **Clock Injection for Testability:** Passing `steady_clock::time_point` into the pipeline removes hidden `now()` calls so stale timeouts and reboots can be tested deterministically in microseconds.

### Code-Review Takeaways
- Verify that `reset()` tests actually prove old buffer history was wiped (e.g., 1 close reading after `reset()` should not re-trigger the brake), not just that a flag flipped.
- Keep member initialization style consistent in constructors and watch for subtle spelling typos in variable names.

### Next Session
- **Step 4 (Implement + Test):** Create `include/proximityPipeline.h`, `src/proximityPipeline.cpp`, wire `proximityPipeline.o` into `Makefile`, and add `tests/test_proximity_pipeline.cpp` (5 deterministic scenarios).
- **Step 4 (Integrate):** Simplify `src/main.cpp` to use `ProximityPipeline` and verify both `make` and `make test` pass cleanly.


## 2026-11-26

### Built & In Progress (Uncommitted)
- **Step 4 — `ProximityPipeline` Core Implementation**
  - Created `include/proximityPipeline.h` and `src/proximityPipeline.cpp` implementing `ProximityPipeline` (`update_stale(TimePoint)`, `process_measurement(const Measurement&, TimePoint)`, and `const noexcept` state getters) with zero internal `steady_clock::now()` calls.
  - Structured per-sample outputs via `SampleOutcome` and `PipelineStepResult` using C++20 designated initializers, decoupling decision logic from `std::cout` and serial I/O.
  - Wired `proximityPipeline.o` into `CORE_OBJS` and build rules in `Makefile`.
  - Started `tests/test_proximity_pipeline.cpp` with deterministic `TimePoint{}` clock injection verifying stale timeout -> fail-safe brake engagement.

### Key SWE & Systems Concepts Covered
- **Safety vs. Availability (Phantom Braking vs. Brownout Hazard):** Analyzed `scanner_.reset()` vs. `scanner_.enter_failsafe()` on `TimestampStatus::Discontinuity`; chose unconditional `enter_failsafe()` so an MCU reboot while actively braking never releases the brake in front of an obstacle.
- **C++ Class Naming & Shadowing Hygiene:** Trailing-underscore convention (`is_stale_`, `stale_after_`, etc.) prevents method/member name collisions and constructor parameter self-assignment bugs.
- **Aggregate Construction with C++20 Designated Initializers:** Explicit field naming (`.outcome = ...`) in declaration order prevents swapped boolean arguments across multi-field result structs.
- **Deterministic Time-Based Testing:** Using a synthetic base `TimePoint{}` + `std::chrono::milliseconds(...)` tests multi-second stale timeouts instantaneously without `sleep()`.

### Code-Review Takeaways
- Every early-exit branch (`Duplicate`, `!valid`) in a value-returning function must explicitly `return` a result struct—bare enum expressions silently fall through into downstream state mutations.
- Keep stale-exit state transitions (`is_stale_ = false`, `recovered_from_stale = true`) outside `if (has_valid_reading_)` so first-reading recovery after startup timeout clears stale state properly.
- Avoid hardcoding result struct fields (`tm_status`, `recovered_from_stale`, `filtered_distance`) on the happy path.

### Next Session
- **Step 4 (Finish Tests):** Expand `tests/test_proximity_pipeline.cpp` to cover all 5 deterministic scenarios (normal braking/release, stale -> fail-safe -> 3-clear recovery, discontinuity/reboot, duplicate timestamp, invalid distances not refreshing staleness) and verify `make && make test` passes under UBSan.
- **Step 4 (Integrate):** Simplify `src/main.cpp` to drive `ProximityPipeline`, then commit & push Step 4.
