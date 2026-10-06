# Synthetic spike and step experiment

These files are invented test data, not physical sensor measurements.

- Input: `synthetic_spike_step_100.log`
- Known true distances: `synthetic_spike_step_100_reference.log`
- Format: `timestamp_ms distance_cm`, with no header, compatible with `filter_log()`.
- Exactly 100 readings at a synthetic interval of 1,000 ms (1 Hz), from 0 to 99 seconds. This deliberately regular interval is not the approximately 1,030 ms interval in the hardware logs.

| Time | Input distance | Known true distance | Purpose |
|---|---:|---:|---|
| 0–19 s | 50 cm | 50 cm | Fill every filter and establish a baseline |
| 20 s | 150 cm | 50 cm | One deliberately injected measurement error |
| 21–49 s | 50 cm | 50 cm | Observe recovery after the spike |
| 50–99 s | 20 cm | 20 cm | Sustained genuine change in the synthetic reference |

There is no random noise or timeout sentinel. This isolates spike handling from response to a genuine change. The 150 cm spike is not a claim about any sensor's timeout behavior. The instantaneous step tests the algorithm; it is not a physically realistic movement trajectory.

## How to use

Pass the input log independently through each filter, using a fresh or reset object for every run. Keep output filenames separate. Do not feed one filter's output into another for this comparison.

Predict each filter's first output at 20 s and 50 s before plotting. Compare spike deviation from 50 cm, recovery after the spike, and delay after the step. For a measurable delay criterion, count readings from the first 20 cm input until the output first reaches within 1 cm of 20 cm; same-reading response is zero delay.

Use the full 0–99 s interval (the plotting function accepts `seconds=0`). Plot the reference as a curve if desired: it changes at 50 s, so a single horizontal reference line would be misleading. Raw-minus-filtered residuals measure disagreement with the input, not error against the known reference. Whole-recording standard deviation includes the deliberate step and is not a pure noise measurement.
