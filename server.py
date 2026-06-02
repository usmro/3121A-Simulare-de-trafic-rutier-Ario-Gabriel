"""
server.py — local bridge between C++ simulation and HTML frontend

Setup (run once):
    pip install flask flask-cors

Run:
    python server.py

Then open index.html in your browser.
The HTML will call this server at http://localhost:5000
"""

import subprocess
import json
import os
import sys
from flask import Flask, jsonify, send_from_directory, request
from flask_cors import CORS

app = Flask(__name__)
CORS(app)  # allow the HTML file to call us even if opened as file://

# ── paths ──────────────────────────────────────────────────────────────────
BASE_DIR   = os.path.dirname(os.path.abspath(__file__))  # F:\POO

# exe compiles to project root (as per Makefile: -o app)
IS_WINDOWS = sys.platform == "win32"
EXE_NAME   = "app.exe" if IS_WINDOWS else "./app"
EXE_PATH   = os.path.join(BASE_DIR, EXE_NAME)

# HTML lives in ui/
UI_DIR     = os.path.join(BASE_DIR, "ui")

# src files for compilation
SRC_DIR    = os.path.join(BASE_DIR, "src")

# JSON output goes to project root alongside the exe
LOG_DAY1_JSON = os.path.join(BASE_DIR, "log_day1.json")
LOG_DAY2_JSON = os.path.join(BASE_DIR, "log_day2.json")


def compile_cpp():
    """
    Compile using the same command as the Makefile.
    Works on both Windows (g++ via MinGW) and Linux.
    Returns (success: bool, error_message: str)
    """
    srcs = [
        "src/main.cpp",
        "src/Street.cpp",
        "src/Intersection.cpp",
        "src/RoadNetwork.cpp",
        "src/Vehicle.cpp",
        "src/Car.cpp",
        "src/Motorcycle.cpp",
        "src/Truck.cpp",
        "src/Simulation.cpp",
        "src/Logger.cpp",
    ]
    # paths relative to BASE_DIR, matching the Makefile exactly
    sources = [os.path.join(BASE_DIR, s) for s in srcs]
    output  = EXE_PATH  # F:\POO\app.exe

    cmd = ["g++", "-std=c++17", "-Wall", "-o", output] + sources

    try:
        result = subprocess.run(
            cmd,
            capture_output=True,
            text=True,
            cwd=BASE_DIR  # run from F:\POO just like make does
        )
        if result.returncode != 0:
            return False, result.stderr
        return True, ""
    except FileNotFoundError:
        return False, "g++ not found. Install MinGW (Windows) or build-essential (Linux)."


def run_simulation(args=None):
    """
    Run the compiled C++ executable, optionally with CLI args for settings.
    args = [maxTicks, congThresh, deliveryImmune, randomLights] as strings.
    Returns (success: bool, stdout: str, error: str)
    """
    exe = EXE_PATH
    cmd = [exe]
    if args:
        cmd += args
    try:
        result = subprocess.run(
            cmd,
            capture_output=True,
            text=True,
            cwd=BASE_DIR,
            timeout=30  # bail if sim hangs
        )
        return result.returncode == 0, result.stdout, result.stderr
    except subprocess.TimeoutExpired:
        return False, "", "Simulation timed out after 30 seconds"
    except FileNotFoundError:
        return False, "", f"Executable not found at {exe} — compile first"


def load_json(path):
    """Load a JSON file, return None if missing or malformed."""
    if not os.path.exists(path):
        return None
    try:
        with open(path, "r") as f:
            return json.load(f)
    except json.JSONDecodeError as e:
        return {"error": str(e)}


# ── routes ─────────────────────────────────────────────────────────────────

@app.route("/")
def index():
    """Serve the HTML frontend from the ui/ folder."""
    return send_from_directory(UI_DIR, "index.html")


@app.route("/compile", methods=["POST"])
def compile_route():
    """Compile the C++ project. Call this once before /run."""
    ok, err = compile_cpp()
    if ok:
        return jsonify({"success": True, "message": "Compiled successfully"})
    else:
        return jsonify({"success": False, "error": err}), 500


@app.route("/run", methods=["POST"])
def run_route():
    """
    Compile (if needed) and run the simulation with optional settings.
    Body JSON: {maxTicks, congThresh, deliveryImmune, randomLights}
    Returns both day1 and day2 tick data as JSON.
    """
    # auto-compile if exe doesn't exist
    if not os.path.exists(EXE_PATH):
        ok, err = compile_cpp()
        if not ok:
            return jsonify({"success": False, "error": "Compile failed: " + err}), 500

    # read settings from request body (all optional)
    body = request.get_json(silent=True) or {}
    max_ticks         = str(int(body.get("maxTicks", 80)))
    cong_thresh       = str(int(body.get("congThresh", 7)))
    delivery_immune   = "1" if body.get("deliveryImmune", True) else "0"
    delivery_immune_col = "1" if body.get("deliveryImmuneCol", True) else "0"
    random_lights     = "1" if body.get("randomLights", True) else "0"
    args = [max_ticks, cong_thresh, delivery_immune, delivery_immune_col, random_lights]

    ok, stdout, stderr = run_simulation(args)
    if not ok:
        return jsonify({"success": False, "error": stderr or "Run failed"}), 500

    day1 = load_json(LOG_DAY1_JSON)
    day2 = load_json(LOG_DAY2_JSON)

    return jsonify({
        "success": True,
        "console": stdout,
        "day1": day1,
        "day2": day2
    })


@app.route("/status", methods=["GET"])
def status():
    """Quick health check — also tells frontend if exe exists."""
    return jsonify({
        "server": "running",
        "compiled": os.path.exists(EXE_PATH),
        "platform": sys.platform
    })


if __name__ == "__main__":
    print("=" * 50)
    print("  TrafSim local server")
    print("  Open http://localhost:5000 in your browser")
    print("=" * 50)
    app.run(host="0.0.0.0", port=5000, debug=True)