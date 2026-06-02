#pragma once
#include <vector>
#include <memory>
#include <string>
#include <map>
#include "RoadNetwork.h"
#include "Logger.h"

class Vehicle;
class Car;
class Street;

// configurable simulation parameters, passed in from main (via CLI args)
struct SimConfig {
    int  maxTicks         = 80;    // hard cap on ticks
    int  congThresh       = 7;     // (legacy, kept for compatibility)
    bool deliveryImmune   = true;  // delivery bypasses congestion (car-count slowdown)
    bool deliveryImmuneCol= true;  // delivery never frozen in a collision
    bool randomLights     = true;  // randomise starting light states
};

class Simulation {
private:
    RoadNetwork network;
    std::vector<std::shared_ptr<Vehicle>> vehicles;
    std::shared_ptr<Car> deliveryCar;
    Logger logger;
    int currentTick;
    bool deliveryComplete;

    SimConfig config; // the run's settings

    // package delivery route
    int totalPackages;
    int packagesDelivered;

    // per-intersection tick countdown for lights
    std::map<std::string, int> lightTicks;

    void spawnRandomTraffic();
    void spawnDeliveryCar();
    void assignNextPackage();
    void tick();
    void checkDelivery();
    void toggleLights();
    Street* chooseNextStreet(Vehicle* v);
    TickSnapshot buildSnapshot(
        const std::vector<std::string>& collisions,
        bool delivered,
        const std::string& deliveredTo);

public:
    Simulation(std::string logFilename, SimConfig cfg = SimConfig());
    void run();
};