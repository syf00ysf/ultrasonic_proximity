#!/usr/bin/env python3
"""Compare raw logs with reusable lists of filtered logs.

Run: python3 plot_compare.py
Use --seconds 120 to show a longer interval, or --seconds 0 for all samples.
The --moving and --static options accept: RAW W3 W5 W10 EMA MEDIAN paths.
The --synthetic option accepts: RAW REFERENCE MA EMA MEDIAN paths.
Three separate images are generated; synthetic views use fixed event intervals.
"""
import argparse
from pathlib import Path

import numpy as np
import matplotlib.pyplot as plt


def load_log(path: Path):
    """Read timestamp_ms/value pairs, or legacy single-column readings."""
    data = np.loadtxt(path, ndmin=2)
    if data.size == 0:
        raise ValueError(f"Empty log: {path}")
    if data.shape[1] == 1:
        return np.arange(len(data)), data[:, 0]
    if data.shape[1] != 2:
        raise ValueError(f"Expected one or two columns: {path}")
    return data[:, 0] / 1000.0, data[:, 1]


def compare_log(raw_path, filtered_logs, title, out_path, ref_line=None,
                seconds=60, x_label="Time (s)"):
    """filtered_logs contains (path, label, color) tuples.

    Use x_label='Sample number', seconds=0 for legacy single-column logs.
    Statistics use the full recording, even when the displayed view is shorter.
    """
    t_raw, v_raw = load_log(Path(raw_path))
    curves = []
    for path, label, color in filtered_logs:
        t, v = load_log(Path(path))
        # Residuals only make sense when rows refer to the same measurements.
        if len(t) != len(t_raw) or not np.array_equal(t, t_raw):
            raise ValueError(f"Timestamps or sample count do not match raw log: {path}")
        curves.append((t, v, label, color))

    fig, (ax1, ax2) = plt.subplots(
        2, 1, figsize=(12, 8), sharex=True,
        gridspec_kw={"height_ratios": [3, 1]},
    )
    ax1.plot(t_raw, v_raw, label="Raw", color="gray", marker=".",
             linestyle="none", markersize=4, alpha=0.5)
    stats = [f"Full recording: n={len(v_raw)}",
             f"Raw std: {np.std(v_raw):.2f} cm"]

    # One loop handles any number of filters, including their stats and residuals.
    for t, v, label, color in curves:
        ax1.plot(t, v, label=label, color=color)
        ax2.plot(t, v_raw - v, label=f"Raw − {label}", color=color,
                 linewidth=1, alpha=0.8)
        stats.append(f"{label} std: {np.std(v):.2f} cm")

    if ref_line is not None:
        ax1.axhline(ref_line, color="gray", linestyle="--", linewidth=1,
                    label=f"Reference: {ref_line} cm")
    if seconds > 0:
        ax1.set_xlim(t_raw[0], min(t_raw[-1], t_raw[0] + seconds))
    ax1.set_title(title, fontweight="bold")
    ax1.set_ylabel("Distance (cm)")
    ax1.legend(loc="upper left", bbox_to_anchor=(1.02, 1), borderaxespad=0)
    # Keep statistics outside the curves so they don't obscure the data.
    ax1.text(1.02, 0.55, "\n".join(stats), transform=ax1.transAxes,
             va="top", fontsize=9,
             bbox=dict(boxstyle="round", facecolor="white", edgecolor="gray"))
    ax2.axhline(0, color="black", linewidth=0.8)
    ax2.set_xlabel(x_label)
    ax2.set_ylabel("Raw − filtered (cm)")
    ax2.legend(loc="upper left", bbox_to_anchor=(1.02, 1), borderaxespad=0,
               fontsize=8)
    for ax in (ax1, ax2):
        ax.grid(True, alpha=0.3)
    fig.subplots_adjust(right=0.73, hspace=0.12)
    out_path = Path(out_path)
    out_path.parent.mkdir(parents=True, exist_ok=True)
    fig.savefig(out_path, dpi=150, bbox_inches="tight")
    plt.close(fig)
    print(f"Saved: {out_path}")


def compare_synthetic(raw_path, reference_path, filtered_logs, out_path):
    """Show the full synthetic experiment and details around its two events."""
    t_raw, v_raw = load_log(Path(raw_path))
    t_ref, v_ref = load_log(Path(reference_path))
    curves = []
    for path, label, color in filtered_logs:
        t, v = load_log(Path(path))
        if not np.array_equal(t, t_raw):
            raise ValueError(f"Timestamps do not match synthetic input: {path}")
        curves.append((t, v, label, color))
    if not np.array_equal(t_ref, t_raw):
        raise ValueError("Synthetic reference timestamps do not match input")

    fig, axes = plt.subplots(3, 1, figsize=(12, 10))
    views = [
        ("Full recording · 100 samples at 1 Hz", (t_raw[0], t_raw[-1]), None),
        ("Isolated error at 20 s · true distance stays at 50 cm", (17, 32), (45, 155)),
        ("Sustained change at 50 s · true distance becomes 20 cm", (47, 62), (17, 53)),
    ]
    for ax, (title, limits, y_limits) in zip(axes, views):
        ax.plot(t_raw, v_raw, ".", color="gray", markersize=5,
                alpha=0.7, label="Raw input", zorder=4)
        for t, v, label, color in curves:
            # Hold each output until the next sample; don't imply an early response.
            ax.step(t, v, where="post", color=color, label=label, linewidth=1.7)
        # A changing reference needs a curve, not a horizontal reference line.
        ax.step(t_ref, v_ref, where="post", color="black", linestyle="--",
                linewidth=1.4, label="Known true distance", zorder=5)
        ax.set_xlim(*limits)
        if y_limits is not None:
            ax.set_ylim(*y_limits)
        ax.set_title(title, fontsize=11, loc="left")
        ax.set_ylabel("Distance (cm)")
        ax.set_xlabel("Time (s)")
        ax.grid(True, alpha=0.25)
    axes[1].axvline(20, color="gray", linestyle=":", linewidth=1)
    axes[2].axvline(50, color="gray", linestyle=":", linewidth=1)
    axes[2].axhspan(19, 21, color="gray", alpha=0.12)
    axes[2].text(0.98, 0.88, "Shaded band: 20 ± 1 cm", ha="right",
                 transform=axes[2].transAxes, fontsize=9)
    fig.suptitle("Synthetic experiment: one spike, then a sustained change",
                 fontsize=14, fontweight="bold", y=0.985)
    handles, labels = axes[0].get_legend_handles_labels()
    fig.legend(handles, labels, loc="upper center", bbox_to_anchor=(0.5, 0.956),
               ncol=5, frameon=False, fontsize=9)
    fig.text(0.5, 0.012, "Invented test data, not a physical recording. Detail panels use different vertical scales.",
             ha="center", fontsize=9)
    fig.subplots_adjust(top=0.89, bottom=0.08, hspace=0.48, left=0.08, right=0.98)
    out_path = Path(out_path)
    out_path.parent.mkdir(parents=True, exist_ok=True)
    fig.savefig(out_path, dpi=150, bbox_inches="tight")
    plt.close(fig)
    print(f"Saved: {out_path}")


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--moving", nargs=6, default=[
        "Data/readings02_moving.log",
        "Data/readings02_moving_filtered_w3.log",
        "Data/readings02_moving_filtered_w5.log",
        "Data/readings02_moving_filtered_w10.log",
        "Data/readings02_moving_ema_filtered_0_5.log",
        "Data/readings02_moving_median_filtered_w5.log",
    ])
    ap.add_argument("--static", nargs=6, default=[
        "Data/readings03_static50.log",
        "Data/readings03_static50_filtered_w3.log",
        "Data/readings03_static50_filtered_w5.log",
        "Data/readings03_static50_filtered_w10.log",
        "Data/readings03_static50_ema_filtered_0_5.log",
        "Data/readings03_static50_median_filtered_w5.log",
    ])
    ap.add_argument("--synthetic", nargs=5, default=[
        "Data/synthetic_spike_step_100.log",
        "Data/synthetic_spike_step_100_reference.log",
        "Data/synthetic_spike_step_100_moving_average_w10.log",
        "Data/synthetic_spike_step_100_ema_0_5.log",
        "Data/synthetic_spike_step_100_median_filtered_w5.log",
    ])
    ap.add_argument("--outdir", default="Data")
    ap.add_argument("--seconds", type=float, default=60,
                    help="Visible interval in seconds; 0 shows the full recording")
    args = ap.parse_args()
    if args.seconds < 0:
        ap.error("--seconds must be zero or positive")

    for paths, title, filename, reference in [
        (args.moving, "Moving target: moving average, EMA and MEDIAN", "compare_moving.png", None),
        (args.static, "Static wall at 50 cm: moving average, EMA and MEDIAN", "compare_static50.png", 50),
    ]:
        filtered_logs = [
            (paths[1], "MA window 3", "tab:blue"),
            (paths[2], "MA window 5", "tab:orange"),
            (paths[3], "MA window 10", "tab:green"),
            (paths[4], "EMA α=0.5", "tab:purple"),
            (paths[5], "Median window 5", "tab:brown"),
        ]
        compare_log(paths[0], filtered_logs, title, Path(args.outdir) / filename,
                    ref_line=reference, seconds=args.seconds)

    synthetic_filters = [
        (args.synthetic[2], "MA window 10", "tab:green"),
        (args.synthetic[3], "EMA α=0.5", "tab:purple"),
        (args.synthetic[4], "Median window 5", "tab:brown"),
    ]
    compare_synthetic(args.synthetic[0], args.synthetic[1], synthetic_filters,
                      Path(args.outdir) / "compare_synthetic.png")


if __name__ == "__main__":
    main()
