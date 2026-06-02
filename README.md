# Road Traffic Simulation

A road traffic simulation built on a three-layer architecture: a **C++ engine**
that runs the actual simulation, a **Python (Flask) server** that bridges it to
the browser, and an **HTML/Canvas web viewer** that replays the results in real time.

The C++ engine is the brain — it runs pathfinding, traffic lights, congestion and
collisions, and writes per-tick JSON snapshots. The web UI does not simulate
anything; it animates the data the engine produces.

---

## Architecture

```
Browser  ->  /run (POST, settings)  ->  Flask  ->  app.exe
                                                       |
Browser  <-------------  JSON  <-------------  log_day1.json
```

1. The browser sends the current settings to the server (`POST /run`).
2. Flask launches `app.exe` with those settings as command-line arguments.
3. The C++ engine runs the full simulation and writes `log_day1.json` /
   `log_day2.json` (one snapshot per tick).
4. Flask reads the JSON back and returns it to the browser.
5. The UI replays it tick by tick, animating the vehicles on a canvas.

A local server is required because a browser cannot launch an executable on its
own — Flask is the component allowed to run the C++ engine on request.

---

## Project structure

```
.
├── app.exe              compiled engine (after build)
├── server.py            Flask local server
├── Makefile
├── README.md
├── log_day1.json/.txt   simulation output
├── src/                 all C++ sources (.cpp / .h)
└── ui/index.html        web interface
```

---

## Build

### Linux / Windows (Makefile)
```bash
make            # builds the engine -> app / app.exe
make tests      # builds the unit-test binary -> tests
make clean      # removes binaries (rm; use 'del' on Windows)
```

### Without the Makefile
```bash
g++ -std=c++17 -Wall -o app src/main.cpp src/Street.cpp src/Intersection.cpp \
    src/RoadNetwork.cpp src/Vehicle.cpp src/Car.cpp src/Motorcycle.cpp \
    src/Truck.cpp src/Simulation.cpp src/Logger.cpp
```

---

## Run

The intended way to run is through the server + web UI.

```bash
# 1. install server dependencies once
python -m pip install flask flask-cors

# 2. start the server
python server.py

# 3. open the interface
#    http://localhost:5000
```

Press **Start** in the browser. The engine is compiled automatically if `app.exe`
is missing, the simulation runs, and the result is animated.

You can also run the engine directly for the raw console/log output:
```bash
./app            # Linux
.\app.exe        # Windows
```

Optional command-line arguments (also sent automatically by the UI):
```
app [maxTicks] [congThresh] [deliveryImmune 0|1] [deliveryImmuneCol 0|1] [randomLights 0|1]
```

---

## What it does

Vehicles move across a **3×3 grid** of 9 intersections (A–I) connected by 12
bidirectional streets. The four corners (A, C, G, I) are the only spawn and
destination points.

**Four vehicle types**, each with its own speed and routing strategy:

| Vehicle    | Max speed | Routing strategy            | At a red light      |
|------------|-----------|-----------------------------|---------------------|
| Delivery   | 120 km/h  | shortest distance (Dijkstra)| stops               |
| Car        | 120 km/h  | shortest distance (Dijkstra)| stops               |
| Motorcycle | 160 km/h  | fewest hops                 | slows to half speed |
| Truck      |  90 km/h  | avoids busy streets         | stops               |

**Delivery route:** the delivery vehicle carries a random number of packages
(3–5). It drives to a corner, delivers, picks a new corner, and repeats until all
packages are delivered. The other three vehicles roam continuously, picking a new
random corner whenever they arrive.

**Traffic lights:** each intersection has an independent red/green light that
changes every 1–3 ticks (randomised).

**Congestion (directional, node-based):** a vehicle waits only if the node it is
heading toward is already occupied — 1 car there = 1 tick wait, 2+ = 2 ticks. The
vehicle already at that node, or one driving away, is not affected.

**Collisions:** when 3+ vehicles end a tick at the same node, they freeze for 2–3
ticks (an accident), are pulled aside so traffic flows past, then resume.

The delivery vehicle can optionally be made immune to congestion and/or
collisions independently, via the settings.

---

## Output

- `log_day1.txt`, `log_day2.txt` — human-readable event logs (intersection
  entry/exit, collisions, deliveries).
- `log_day1.json`, `log_day2.json` — per-tick state snapshots consumed by the UI.

---

## Tests

Unit tests cover movement logic and route validation (plus structural checks):

```bash
make tests
./tests
```

All tests verify vehicle movement, arrival detection, polymorphic speeds, and that
Dijkstra returns correct shortest paths (e.g. A→I is exactly 4 hops) and that
following a computed route actually reaches the destination.