#!/usr/bin/env python3
"""Plot current orbital positions from orbital_elements.csv."""

import csv
import math
from datetime import date

import click
import matplotlib.pyplot as plt
import numpy as np

J2000_JD = 2451545.0


def date_to_jd(d: date) -> float:
    y, m, day = d.year, d.month, d.day
    return (
        367 * y
        - int(7 * (y + int((m + 9) / 12)) / 4)
        + int(275 * m / 9)
        + day
        + 1721013.5
    )


def solve_kepler(M: np.ndarray, e: np.ndarray) -> np.ndarray:
    E = M.copy()
    for _ in range(50):
        dE = (M - E + e * np.sin(E)) / (1 - e * np.cos(E))
        E += dE
        if np.max(np.abs(dE)) < 1e-10:
            break
    return E


def elements_to_xy(a, e, i_deg, om_deg, w_deg, ma_deg, epoch_jd, period_yr, t_jd):
    """Vectorised orbital elements → ecliptic x, y positions (AU)."""
    n = 2 * np.pi / (period_yr * 365.25)
    M = (np.radians(ma_deg) + n * (t_jd - epoch_jd)) % (2 * np.pi)
    E = solve_kepler(M, e)
    nu = 2 * np.arctan2(np.sqrt(1 + e) * np.sin(E / 2), np.sqrt(1 - e) * np.cos(E / 2))
    r = a * (1 - e * np.cos(E))
    xo, yo = r * np.cos(nu), r * np.sin(nu)

    i, om, w = np.radians(i_deg), np.radians(om_deg), np.radians(w_deg)
    x = (np.cos(om) * np.cos(w) - np.sin(om) * np.sin(w) * np.cos(i)) * xo + (
        -np.cos(om) * np.sin(w) - np.sin(om) * np.cos(w) * np.cos(i)
    ) * yo
    y = (np.sin(om) * np.cos(w) + np.cos(om) * np.sin(w) * np.cos(i)) * xo + (
        -np.sin(om) * np.sin(w) + np.cos(om) * np.cos(w) * np.cos(i)
    ) * yo
    return x, y


def orbit_path(a, e, i_deg, om_deg, w_deg, n_pts=300):
    """x, y arrays tracing the full ellipse."""
    nu = np.linspace(0, 2 * np.pi, n_pts)
    r = a * (1 - e**2) / (1 + e * np.cos(nu))
    xo, yo = r * np.cos(nu), r * np.sin(nu)
    i, om, w = math.radians(i_deg), math.radians(om_deg), math.radians(w_deg)
    x = (math.cos(om) * math.cos(w) - math.sin(om) * math.sin(w) * math.cos(i)) * xo + (
        -math.cos(om) * math.sin(w) - math.sin(om) * math.cos(w) * math.cos(i)
    ) * yo
    y = (math.sin(om) * math.cos(w) + math.cos(om) * math.sin(w) * math.cos(i)) * xo + (
        -math.sin(om) * math.sin(w) + math.cos(om) * math.cos(w) * math.cos(i)
    ) * yo
    return x, y


PLANET_COLORS = {
    "Sun": "#FFD700",
    "Mercury": "#A0A0A0",
    "Venus": "#E8C88C",
    "Earth": "#4FA3E0",
    "Mars": "#C1440E",
    "Jupiter": "#C88B3A",
    "Saturn": "#E4D191",
    "Uranus": "#7DE8E8",
    "Neptune": "#4060FF",
}

BG = "#080810"


def load_csv(path):
    bodies = {"planet": [], "moon": [], "trojan": []}
    with open(path) as f:
        for row in csv.DictReader(f):
            try:
                bodies[row["body_type"]].append(
                    {
                        "name": row["name"],
                        "a": float(row["a_au"]),
                        "e": float(row["e"]),
                        "i": float(row["i_deg"]),
                        "om": float(row["om_deg"]),
                        "w": float(row["w_deg"]),
                        "ma": float(row["ma_deg"]),
                        "epoch": float(row["epoch_jd"]),
                        "period": (
                            float(row["period_yr"])
                            if row["period_yr"]
                            else float(row["a_au"]) ** 1.5
                        ),
                    }
                )
            except (ValueError, KeyError):
                continue
    return bodies


def body_xy(b, t_jd):
    return elements_to_xy(
        np.array([b["a"]]),
        np.array([b["e"]]),
        np.array([b["i"]]),
        np.array([b["om"]]),
        np.array([b["w"]]),
        np.array([b["ma"]]),
        np.array([b["epoch"]]),
        np.array([b["period"]]),
        t_jd,
    )


@click.command()
@click.option(
    "-i", "--input", "csv_path", default="orbital_elements.csv", show_default=True
)
@click.option(
    "-d", "--date", "ref_date", default=date.today().isoformat(), show_default=True
)
@click.option(
    "-s",
    "--save",
    "save_path",
    default=None,
    help="Save to file instead of displaying.",
)
def main(csv_path, ref_date, save_path):
    """Plot current positions of planets, Jupiter moons, and Trojans.

    The interactive window supports zoom (scroll wheel or toolbar) and pan.
    Trojan orbits are omitted intentionally — only their current positions
    are shown to keep the plot readable.
    """
    t_jd = date_to_jd(date.fromisoformat(ref_date))
    bodies = load_csv(csv_path)

    fig, ax = plt.subplots(figsize=(11, 11), facecolor=BG)
    ax.set_facecolor(BG)
    ax.set_aspect("equal")
    for spine in ax.spines.values():
        spine.set_color("#1a1a2e")
    ax.tick_params(colors="#333355", labelsize=7)
    ax.grid(True, color="#111128", linewidth=0.4, zorder=0)

    # ── Trojan cloud (positions only — ~16k orbit lines would be unreadable) ──
    if bodies["trojan"]:
        click.echo(f"Computing {len(bodies['trojan'])} Trojan positions...")
        t = bodies["trojan"]
        tx, ty = elements_to_xy(
            np.fromiter((b["a"] for b in t), float),
            np.fromiter((b["e"] for b in t), float),
            np.fromiter((b["i"] for b in t), float),
            np.fromiter((b["om"] for b in t), float),
            np.fromiter((b["w"] for b in t), float),
            np.fromiter((b["ma"] for b in t), float),
            np.fromiter((b["epoch"] for b in t), float),
            np.fromiter((b["period"] for b in t), float),
            t_jd,
        )
        ax.scatter(
            tx,
            ty,
            s=0.5,
            c="#f0c040",
            alpha=0.20,
            linewidths=0,
            label=f"Jupiter Trojans ({len(t):,})",
            rasterized=True,
        )

    # ── Planets ───────────────────────────────────────────────────────────────
    jupiter_pos = None
    for b in bodies["planet"]:
        name = b["name"]
        color = PLANET_COLORS.get(name, "#ffffff")

        ox, oy = orbit_path(b["a"], b["e"], b["i"], b["om"], b["w"])
        ax.plot(ox, oy, color=color, alpha=0.20, linewidth=0.8, zorder=2)

        px, py = body_xy(b, t_jd)
        ax.scatter(px, py, s=35, c=color, zorder=6)
        ax.annotate(
            name,
            (px[0], py[0]),
            color=color,
            fontsize=7,
            xytext=(6, 3),
            textcoords="offset points",
            zorder=7,
        )

        if name == "Jupiter":
            jupiter_pos = (float(px[0]), float(py[0]))

    # ── Jupiter-system inset ──────────────────────────────────────────────────
    if bodies["moon"] and jupiter_pos:
        ins = ax.inset_axes([0.02, 0.02, 0.24, 0.24])
        ins.set_facecolor("#0b0b1a")
        ins.set_aspect("equal")
        ins.tick_params(labelsize=5, colors="#333355")
        for spine in ins.spines.values():
            spine.set_color("#1a1a2e")
        ins.set_title("Jupiter system (AU)", color="#666688", fontsize=6, pad=3)
        ins.scatter(0, 0, s=80, c=PLANET_COLORS["Jupiter"], zorder=5)
        ins.annotate(
            "Jupiter",
            (0, 0),
            color=PLANET_COLORS["Jupiter"],
            fontsize=5,
            xytext=(4, 3),
            textcoords="offset points",
        )

        for b in bodies["moon"]:
            color = "#8888bb"
            ox, oy = orbit_path(b["a"], b["e"], b["i"], b["om"], b["w"])
            ins.plot(ox, oy, color=color, alpha=0.35, linewidth=0.7)
            px, py = body_xy(b, t_jd)
            ins.scatter(px, py, s=18, c=color, zorder=5)
            ins.annotate(
                b["name"],
                (px[0], py[0]),
                color=color,
                fontsize=5,
                xytext=(3, 2),
                textcoords="offset points",
            )

    ax.set_xlabel("X (AU)", color="#333355", fontsize=8)
    ax.set_ylabel("Y (AU)", color="#333355", fontsize=8)
    ax.set_title(f"Solar System  ·  {ref_date}", color="#8888aa", fontsize=12, pad=10)
    ax.legend(
        loc="upper right",
        fontsize=7,
        facecolor="#0d0d1e",
        edgecolor="#1a1a2e",
        labelcolor="#aaaacc",
        markerscale=6,
    )

    plt.tight_layout()
    if save_path:
        plt.savefig(save_path, dpi=150, facecolor=BG)
        click.echo(f"Saved to {save_path}")
    else:
        plt.show()


if __name__ == "__main__":
    main()
