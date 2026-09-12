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
    """Load log with format: 'timestamp_us value' or single 'value' column."""
    data = np.loadtxt(path)
    if data.ndim == 1:
        # readings01.log legacy: just values, no timestamp
        t = np.arange(len(data))
        v = data
    else:
        t = data[:, 0] / 1000.0  # us -> ms for readable x-axis
        v = data[:, 1]
    return t, v

def plot_pair(raw_path, filt_path, title, out_path, ylabel="Distance (cm)", ref_line=None):
    t_raw, v_raw = load_log(Path(raw_path))
    t_filt, v_filt = load_log(Path(filt_path))

    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(11, 7), sharex=True,
                                   gridspec_kw={"height_ratios": [3, 1], "hspace": 0.08})

    # --- Top: curves ---
    ax1.plot(t_raw, v_raw, label="raw", alpha=0.6, linewidth=1.2)
    ax1.plot(t_filt, v_filt, label="filtered", linewidth=2)
    if ref_line is not None:
        ax1.axhline(ref_line, color="gray", linestyle="--", linewidth=1, label=f"ref {ref_line}cm")
    ax1.set_title(title, fontsize=13, fontweight="bold")
    ax1.set_ylabel(ylabel)
    ax1.legend(loc="best")
    ax1.grid(True, alpha=0.3)

    # stats in box
    rmse = np.sqrt(np.mean((v_raw[:len(v_filt)] - v_filt) ** 2))
    ax1.text(0.01, 0.95, f"n={len(v_raw)}  raw std={np.std(v_raw):.2f}cm  filt std={np.std(v_filt):.2f}cm\nRMSE raw-vs-filt={rmse:.2f}cm",
             transform=ax1.transAxes, va="top", fontsize=9,
             bbox=dict(boxstyle="round", facecolor="white", alpha=0.8))

    # --- Bottom: residual (raw - filtered) ---
    n = min(len(v_raw), len(v_filt))
    resid = v_raw[:n] - v_filt[:n]
    ax2.plot(t_raw[:n], resid, color="tab:red", linewidth=1, label="residual (raw-filt)")
    ax2.axhline(0, color="black", linewidth=0.8)
    ax2.set_xlabel("Time (ms)")
    ax2.set_ylabel("Residual (cm)")
    ax2.grid(True, alpha=0.3)
    ax2.legend(fontsize=8)

    fig.subplots_adjust(hspace=0.12)
    fig.savefig(out_path, dpi=150, bbox_inches="tight")
    print(f"saved -> {out_path}")
    plt.close(fig)

def main():
    ap = argparse.ArgumentParser(description="Plot raw vs filtered ultrasonic logs")
    ap.add_argument("--moving", nargs=2, default=["Data/readings02_moving.log", "Data/readings02_moving_filtered.log"])
    ap.add_argument("--static", nargs=2, default=["Data/readings03_static50.log", "Data/readings03_static50_filtered.log"])
    ap.add_argument("--outdir", default="Data")
    args = ap.parse_args()

    outdir = Path(args.outdir)
    outdir.mkdir(parents=True, exist_ok=True)

    plot_pair(args.moving[0], args.moving[1],
              "Moving target: raw vs filtered",
              outdir / "compare_moving.png")

    plot_pair(args.static[0], args.static[1],
              "Static wall @50cm: raw vs filtered (noise floor check)",
              outdir / "compare_static50.png",
              ref_line=50)

if __name__ == "__main__":
    main()
