#!/usr/bin/env python3
"""Capture sc2-pdc Serial NDJSON debug lines into debug-03e5a9.log."""

import argparse
import json
import sys
import time
from pathlib import Path

try:
    import serial
    from serial.tools import list_ports
except ImportError:
    print("Install pyserial: pip install pyserial", file=sys.stderr)
    raise SystemExit(1)


def default_log_path() -> Path:
    return Path(__file__).resolve().parents[2] / "debug-03e5a9.log"


def pick_port(explicit: str | None) -> str:
    if explicit:
        return explicit
    ports = [p.device for p in list_ports.comports()]
    if not ports:
        raise SystemExit("No serial ports found. Pass --port COMx")
    if len(ports) == 1:
        return ports[0]
    raise SystemExit(f"Multiple ports found: {ports}. Pass --port explicitly.")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--port", help="Serial port, e.g. COM3")
    parser.add_argument("--baud", type=int, default=115200)
    parser.add_argument("--duration", type=float, default=20.0)
    parser.add_argument("--log", type=Path, default=default_log_path())
    args = parser.parse_args()

    port = pick_port(args.port)
    args.log.parent.mkdir(parents=True, exist_ok=True)

    print(f"Capturing from {port} for {args.duration:.0f}s -> {args.log}")
    with serial.Serial(port, args.baud, timeout=0.2) as ser, args.log.open(
        "a", encoding="utf-8"
    ) as log_file:
        end = time.time() + args.duration
        while time.time() < end:
            line = ser.readline().decode("utf-8", errors="ignore").strip()
            if not line.startswith("{"):
                continue
            try:
                payload = json.loads(line)
            except json.JSONDecodeError:
                continue
            if payload.get("sessionId") != "03e5a9":
                continue
            log_file.write(line + "\n")
            log_file.flush()
            print(line)

    print("Capture complete.")


if __name__ == "__main__":
    main()
