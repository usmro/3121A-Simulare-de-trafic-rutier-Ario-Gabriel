#pragma once
#include <vector>
#include <memory>
#include <string>
#include "RoadNetwork.h"
#include "Logger.h"

class Vehicle;
class Car;

class Simulation {
private:
    RoadNetwork network;
    std::vector<std::shared_ptr<Vehicle>> vehicles;
    std::shared_ptr<Car> deliveryCar;
    Logger logger;
    int currentTick;
    bool deliveryComplete;

    void spawnRandomTraffic();
    void spawnDeliveryCar();
    void tick();
    void checkCollisions();
    void checkDelivery();
    void toggleLights();
    Street* chooseNextStreet(Vehicle* v);

public:
    Simulation(std::string logFilename);
    void run();
};