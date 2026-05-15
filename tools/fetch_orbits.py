#!/usr/bin/env python3
"""
Download precise orbital elements from JPL for planets, Jupiter moons, and
Jupiter Trojans.
"""

import csv
import sys
from datetime import date, timedelta

import click
import requests

HORIZONS_URL = "https://ssd.jpl.nasa.gov/api/horizons.api"
SBDB_URL = "https://ssd-api.jpl.nasa.gov/sbdb_query.api"

DEFAULT_EPOCH = "2000-01-01"

PLANETS = [
    ("Sun", "10", "@0"),
    ("Mercury", "199", "@10"),
    ("Venus", "299", "@10"),
    ("Earth", "399", "@10"),
    ("Mars", "499", "@10"),
    ("Jupiter", "599", "@10"),
    ("Saturn", "699", "@10"),
    ("Uranus", "799", "@10"),
    ("Neptune", "899", "@10"),
]

JUPITER_MOONS = [
    ("Io", "501", "@599"),
    ("Europa", "502", "@599"),
    ("Ganymede", "503", "@599"),
    ("Callisto", "504", "@599"),
]

FIELDS = [
    "name",
    "body_type",
    "epoch_jd",
    "a_au",
    "e",
    "i_deg",
    "om_deg",
    "w_deg",
    "ma_deg",
    "period_yr",
]


def _stop(epoch: str) -> str:
    d = date.fromisoformat(epoch) + timedelta(days=1)
    return d.isoformat()


def horizons_elements(body_id: str, center: str, epoch: str) -> dict:
    resp = requests.get(
        HORIZONS_URL,
        params={
            "format": "json",
            "COMMAND": f"'{body_id}'",
            "OBJ_DATA": "NO",
            "MAKE_EPHEM": "YES",
            "EPHEM_TYPE": "ELEMENTS",
            "CENTER": f"'{center}'",
            "START_TIME": f"'{epoch}'",
            "STOP_TIME": f"'{_stop(epoch)}'",
            "STEP_SIZE": "'1d'",
            "OUT_UNITS": "'AU-D'",
            "REF_PLANE": "ECLIPTIC",
            "REF_SYSTEM": "J2000",
            "CSV_FORMAT": "YES",
        },
        timeout=30,
    )
    resp.raise_for_status()
    text = resp.json()["result"]

    soe = text.index("$$SOE")
    eoe = text.index("$$EOE")
    pre_lines = text[:soe].rstrip().split("\n")
    header_line = next(
        line.strip() for line in reversed(pre_lines) if not line.strip().startswith("*")
    )
    data_line = text[soe + 5 : eoe].strip().split("\n")[0]

    headers = [h.strip() for h in header_line.split(",")]
    values = [v.strip() for v in data_line.split(",")]
    return dict(zip(headers, values))


def trojans_from_sbdb() -> list[dict]:
    resp = requests.get(
        SBDB_URL,
        params={
            "sb-class": "TJN",
            "fields": "full_name,epoch,a,e,i,om,w,ma,per",
            "limit": 100000,
        },
        timeout=120,
    )
    resp.raise_for_status()
    body = resp.json()
    return [dict(zip(body["fields"], row)) for row in body["data"]]


def row_from_horizons(name: str, body_type: str, el: dict) -> dict:
    pr = el.get("PR", "")
    return {
        "name": name,
        "body_type": body_type,
        "epoch_jd": el.get("JDTDB", ""),
        "a_au": el.get("A", ""),
        "e": el.get("EC", ""),
        "i_deg": el.get("IN", ""),
        "om_deg": el.get("OM", ""),
        "w_deg": el.get("W", ""),
        "ma_deg": el.get("MA", ""),
        "period_yr": round(float(pr) / 365.25, 6) if pr else "",
    }


def row_from_sbdb(t: dict) -> dict:
    return {
        "name": (t.get("full_name") or "").strip(),
        "body_type": "trojan",
        "epoch_jd": t.get("epoch", ""),
        "a_au": t.get("a", ""),
        "e": t.get("e", ""),
        "i_deg": t.get("i", ""),
        "om_deg": t.get("om", ""),
        "w_deg": t.get("w", ""),
        "ma_deg": t.get("ma", ""),
        "period_yr": round(float(t["per"]) / 365.25, 6) if t.get("per") else "",
    }


@click.group()
def cli():
    pass


@cli.command()
@click.argument("body_id", default="399")
@click.option("--center", default="@10", show_default=True)
@click.option("--epoch", default=DEFAULT_EPOCH, show_default=True)
def debug_horizons(body_id: str, center: str, epoch: str):
    """Print the raw Horizons API response for a body (default: Earth=399)."""
    resp = requests.get(
        HORIZONS_URL,
        params={
            "format": "json",
            "COMMAND": f"'{body_id}'",
            "OBJ_DATA": "NO",
            "MAKE_EPHEM": "YES",
            "EPHEM_TYPE": "ELEMENTS",
            "CENTER": f"'{center}'",
            "START_TIME": f"'{epoch}'",
            "STOP_TIME": f"'{_stop(epoch)}'",
            "STEP_SIZE": "'1d'",
            "OUT_UNITS": "'AU-D'",
            "REF_PLANE": "ECLIPTIC",
            "REF_SYSTEM": "J2000",
            "CSV_FORMAT": "YES",
        },
        timeout=30,
    )
    resp.raise_for_status()
    click.echo(resp.json().get("result", resp.text))


@cli.command()
@click.option(
    "-o",
    "--output",
    default="orbital_elements.csv",
    show_default=True,
    help="Output CSV path.",
)
@click.option(
    "--epoch",
    default=DEFAULT_EPOCH,
    show_default=True,
    help="Reference epoch (YYYY-MM-DD) for major bodies.",
)
def fetch(output: str, epoch: str):
    """Download orbital elements from JPL for planets, Jupiter moons, and Jupiter Trojans.

    Major bodies (planets, moons) come from JPL Horizons. Jupiter Trojans come from the
    JPL Small-Body Database (all ~15k TJN objects; L4/L5 distinction is determined by
    the orbital simulator from mean longitude at a given epoch).

    Orbital elements reference frame: J2000 ecliptic.
    """
    with open(output, "w", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=FIELDS, extrasaction="ignore")
        writer.writeheader()

        for label, bodies, body_type in [
            ("planets", PLANETS, "planet"),
            ("Jupiter moons", JUPITER_MOONS, "moon"),
        ]:
            click.echo(f"Fetching {label} from JPL Horizons...")
            for name, body_id, center in bodies:
                click.echo(f"  {name} ", nl=False)
                try:
                    el = horizons_elements(body_id, center, epoch)
                    writer.writerow(row_from_horizons(name, body_type, el))
                    click.echo("ok")
                except Exception as exc:
                    click.echo(f"FAILED: {exc}", err=True)

        click.echo("Fetching Jupiter Trojans from JPL SBDB...")
        try:
            trojans = trojans_from_sbdb()
            for t in trojans:
                writer.writerow(row_from_sbdb(t))
            click.echo(f"  {len(trojans)} trojans written.")
        except Exception as exc:
            click.echo(f"FAILED: {exc}", err=True)
            sys.exit(1)

    click.echo(f"Saved to {output}")


if __name__ == "__main__":
    cli()
