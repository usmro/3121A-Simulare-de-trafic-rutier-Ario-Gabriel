# Road Traffic Simulation

## Build & Run

### Linux
```bash
make
./app
```

### Windows
```bash
make
.\app.exe
```

### Without Makefile (both platforms)
```bash
# Linux
g++ -std=c++17 -o app src/main.cpp src/Street.cpp src/Intersection.cpp src/RoadNetwork.cpp src/Vehicle.cpp src/Car.cpp src/Motorcycle.cpp src/Truck.cpp src/Simulation.cpp src/Logger.cpp
./app

# Windows
g++ -std=c++17 -o app src/main.cpp src/Street.cpp src/Intersection.cpp src/RoadNetwork.cpp src/Vehicle.cpp src/Car.cpp src/Motorcycle.cpp src/Truck.cpp src/Simulation.cpp src/Logger.cpp
.\app.exe
```

## What it does
Simulates vehicles (Car, Motorcycle, Truck) moving through a 3x3
grid of streets. Each run generates random traffic.
A delivery car navigates from A to I while other vehicles
follow different pathfinding strategies.

## Output
Generates log_day1.txt and log_day2.txt with all events.
