#!/usr/bin/env python3
"""
Raw vs Filtered ultrasonic comparison plots.
Usage:
    python3 plot_compare.py
    python3 plot_compare.py --moving Data/readings02_moving.log Data/readings02_moving_filtered.log
"""
import argparse
from pathlib import Path
import numpy as np
import matplotlib.pyplot as plt

def load_log(path: Path):
    """Load log with format: 'timestamp_ms value' or single 'value' column."""
    data = np.loadtxt(path)
    if data.ndim == 1:
        # readings01.log legacy: just values, no timestamp
        t = np.arange(len(data))
        v = data
    else:
        t = data[:, 0] / 1000.0  # ms -> s for readable x-axis
        v = data[:, 1]
    return t, v

def plot_pair(raw_path, path_w3, path_w5, path_w10, title, out_path, ylabel="Distance (cm)", ref_line=None):
    t_raw, v_raw = load_log(Path(raw_path))
    #plotting other
    t3, v3 = load_log(Path(path_w3))
    t5, v5 = load_log(Path(path_w5))
    t10, v10 = load_log(Path(path_w10))

    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(11, 7), sharex=True,
                                   gridspec_kw={"height_ratios": [3, 1], "hspace": 0.08})

    # --- Top: curves ---
    ax1.plot(t_raw, v_raw, label="raw",color="gray", marker=".", linestyle="none", markersize=3, alpha=0.25, linewidth=1.2)
    # others plots
    ax1.plot(t3, v3, label="Window 3", color="tab:blue")
    ax1.plot(t5, v5, label="Window 5", color="tab:orange")
    ax1.plot(t10, v10, label="Window 10", color="tab:green")
    ax1.set_xlim(t_raw[0], t_raw[0] + 60)
   
    if ref_line is not None:
        ax1.axhline(ref_line, color="gray", linestyle="--", linewidth=1, label=f"ref {ref_line}cm")
    ax1.set_title(title, fontsize=13, fontweight="bold")
    ax1.set_ylabel(ylabel)
    ax1.legend(loc="best")
    ax1.grid(True, alpha=0.3)

    # stats in box
    stats_text = (
        f"Full recording: n={len(v_raw)}\n"
        f"Raw std: {np.std(v_raw):.2f} cm\n"
        f"W3 std: {np.std(v3):.2f} cm\n"
        f"W5 std: {np.std(v5):.2f} cm\n"
        f"W10 std: {np.std(v10):.2f} cm\n"
    )
    ax1.text(
        0.01, 0.95, stats_text,
        transform =ax1.transAxes,
        va="top",
        fontsize=9,
        bbox=dict(boxstyle="round", facecolor="white", alpha=0.8)
    )


    # --- Bottom: residual (raw - filtered) ---
    n = min(len(v_raw), len(v3))
    resid = v_raw[:n] - v3[:n]
    ax2.plot(t_raw[:n], resid, color="tab:red", linewidth=1, label="residual (raw-W3)")
    ax2.axhline(0, color="black", linewidth=0.8)
    ax2.set_xlabel("Time (s)")
    ax2.set_ylabel("Residual (cm)")
    ax2.grid(True, alpha=0.3)
    ax2.legend(fontsize=8)

    fig.subplots_adjust(hspace=0.12)
    fig.savefig(out_path, dpi=150, bbox_inches="tight")
    print(f"saved -> {out_path}")
    plt.close(fig)

def main():
    ap = argparse.ArgumentParser(description="Plot raw vs filtered ultrasonic logs")
    ap.add_argument("--moving", nargs=4, default=["Data/readings02_moving.log", "Data/readings02_moving_filtered_w3.log", "Data/readings02_moving_filtered_w5.log", "Data/readings02_moving_filtered_w10.log"])
    ap.add_argument("--static", nargs=4, default=["Data/readings03_static50.log", "Data/readings03_static50_filtered_w3.log", "Data/readings03_static50_filtered_w5.log", "Data/readings03_static50_filtered_w10.log"])
    ap.add_argument("--outdir", default="Data")
    args = ap.parse_args()

    outdir = Path(args.outdir)
    outdir.mkdir(parents=True, exist_ok=True)

    plot_pair(args.moving[0], args.moving[1], args.moving[2], args.moving[3],
              "Moving target: raw vs 3 filtered",
              outdir / "compare_moving.png")

    plot_pair(args.static[0], args.static[1], args.static[2],  args.static[3],
              "Static wall @50cm: raw vs 3 filtered (noise floor check)",
              outdir / "compare_static50.png",
              ref_line=50)

if __name__ == "__main__":
    main()
