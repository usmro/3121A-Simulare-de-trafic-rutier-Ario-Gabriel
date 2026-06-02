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
from flask import Flask, jsonify, send_from_directory
from flask_cors import CORS

app = Flask(__name__)
CORS(app)  # allow the HTML file to call us even if opened as file://

# ── paths ──────────────────────────────────────────────────────────────────
BASE_DIR = os.path.dirname(os.path.abspath(__file__))

# where your compiled exe will be (Windows: traffic.exe, Linux: ./traffic)
IS_WINDOWS = sys.platform == "win32"
EXE_NAME   = "traffic.exe" if IS_WINDOWS else "./traffic"
EXE_PATH   = os.path.join(BASE_DIR, EXE_NAME)

LOG_DAY1_JSON = os.path.join(BASE_DIR, "log_day1.json")
LOG_DAY2_JSON = os.path.join(BASE_DIR, "log_day2.json")


def compile_cpp():
    """
    Compile all .cpp files into a single executable.
    Works on both Windows (g++ via MinGW) and Linux.
    Returns (success: bool, error_message: str)
    """
    cpp_files = [
        "main.cpp",
        "Simulation.cpp",
        "RoadNetwork.cpp",
        "Car.cpp",
        "Motorcycle.cpp",
        "Truck.cpp",
        "Vehicle.cpp",
        "Street.cpp",
        "Intersection.cpp",
        "Logger.cpp",
    ]

    # build full paths
    sources = [os.path.join(BASE_DIR, f) for f in cpp_files]

    output = EXE_PATH if IS_WINDOWS else os.path.join(BASE_DIR, "traffic")

    cmd = ["g++", "-std=c++17", "-o", output] + sources

    try:
        result = subprocess.run(
            cmd,
            capture_output=True,
            text=True,
            cwd=BASE_DIR
        )
        if result.returncode != 0:
            return False, result.stderr
        return True, ""
    except FileNotFoundError:
        return False, "g++ not found. Install MinGW (Windows) or build-essential (Linux)."


def run_simulation():
    """
    Run the compiled C++ executable.
    Returns (success: bool, stdout: str, error: str)
    """
    exe = EXE_PATH if IS_WINDOWS else os.path.join(BASE_DIR, "traffic")
    try:
        result = subprocess.run(
            [exe],
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
    """Serve the HTML frontend directly so you can open localhost:5000."""
    return send_from_directory(BASE_DIR, "index.html")


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
    Compile (if needed) and run the simulation.
    Returns both day1 and day2 tick data as JSON.
    """
    # auto-compile if exe doesn't exist
    if not os.path.exists(EXE_PATH if IS_WINDOWS else os.path.join(BASE_DIR, "traffic")):
        ok, err = compile_cpp()
        if not ok:
            return jsonify({"success": False, "error": "Compile failed: " + err}), 500

    ok, stdout, stderr = run_simulation()
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
    exe = EXE_PATH if IS_WINDOWS else os.path.join(BASE_DIR, "traffic")
    return jsonify({
        "server": "running",
        "compiled": os.path.exists(exe),
        "platform": sys.platform
    })


if __name__ == "__main__":
    print("=" * 50)
    print("  TrafSim local server")
    print("  Open http://localhost:5000 in your browser")
    print("=" * 50)
    app.run(host="0.0.0.0", port=5000, debug=True)