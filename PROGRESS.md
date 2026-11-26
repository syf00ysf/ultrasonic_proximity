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
