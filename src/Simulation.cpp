#include "Simulation.h"
#include "Vehicle.h"
#include "Car.h"
#include "Motorcycle.h"
#include "Truck.h"
#include "Street.h"
#include "Intersection.h"
#include <iostream>
#include <random>
#include <algorithm>

// random number generator
static std::mt19937 rng(std::random_device{}());

Simulation::Simulation(std::string logFilename) : logger(logFilename) {
    currentTick = 0;
    deliveryComplete = false;
}

void Simulation::spawnDeliveryCar() {
    Intersection* start = network.getIntersection("A");
    Intersection* goal  = network.getIntersection("I");
    deliveryCar = std::make_shared<Car>("DELIVERY", start, goal);
    vehicles.push_back(deliveryCar);
    std::cout << "Spawned delivery car at A, destination I\n";
}

void Simulation::spawnRandomTraffic() {
    // randomize congestion levels — this is what changes each run
    auto& streets = network.getStreets();
    std::uniform_int_distribution<int> congDist(1, 10);
    for (auto& s : streets)
        s->setCongestionLevel(congDist(rng));

    // random number of background vehicles (3 to 6)
    std::uniform_int_distribution<int> countDist(3, 6);
    int count = countDist(rng);

    // all intersections available as spawn points
    auto& intersections = network.getIntersections();
    std::uniform_int_distribution<int> nodeDist(0, intersections.size() - 1);

    for (int i = 0; i < count; i++) {
        // pick random start and destination
        int startIdx = nodeDist(rng);
        int destIdx  = nodeDist(rng);

        // make sure start != destination
        while (destIdx == startIdx)
            destIdx = nodeDist(rng);

        Intersection* start = intersections[startIdx].get();
        Intersection* dest  = intersections[destIdx].get();

        // spawn random mix of vehicles
        std::uniform_int_distribution<int> typeDist(0, 2);
        int type = typeDist(rng);

        std::string id = "V" + std::to_string(i + 1);

        if (type == 0) {
            vehicles.push_back(std::make_shared<Car>(id, start, dest));
            std::cout << "Spawned Car " << id << " at " 
                      << start->getId() << " -> " << dest->getId() << "\n";
        } else if (type == 1) {
            vehicles.push_back(std::make_shared<Motorcycle>(id, start, dest));
            std::cout << "Spawned Motorcycle " << id << " at " 
                      << start->getId() << " -> " << dest->getId() << "\n";
        } else {
            vehicles.push_back(std::make_shared<Truck>(id, start, dest));
            std::cout << "Spawned Truck " << id << " at " 
                      << start->getId() << " -> " << dest->getId() << "\n";
        }
    }
}

Street* Simulation::chooseNextStreet(Vehicle* v) {
    // find path from current position to destination
    std::vector<Street*> path = network.findPath(
        v->getCurrentPosition(),
        v->getDestination(),
        v
    );

    // return first street in path if it exists
    if (!path.empty()) return path[0];
    return nullptr;
}

void Simulation::toggleLights() {
    // toggle all lights every 5 ticks
    if (currentTick % 5 == 0) {
        for (auto& i : network.getIntersections())
            i->toggleLight();
    }
}

void Simulation::checkCollisions() {
    for (auto& street : network.getStreets()) {
        auto& vs = street->getVehicles();
        // if 2+ vehicles on same street — collision
        if (vs.size() >= 2) {
            for (int i = 0; i < (int)vs.size() - 1; i++) {
                logger.logCollision(vs[i], vs[i+1], street.get());
                std::cout << "[COLLISION] " << vs[i]->getId() 
                          << " and " << vs[i+1]->getId()
                          << " on " << street->getName() << "\n";
            }
        }
    }
}

void Simulation::checkDelivery() {
    if (deliveryCar->hasArrived()) {
        deliveryComplete = true;
        logger.logDelivery(deliveryCar.get(), deliveryCar->getDestination());
        std::cout << "[DELIVERY COMPLETE] Package delivered to I"
                  << " in " << currentTick << " ticks\n";
    }
}

void Simulation::tick() {
    currentTick++;
    logger.setTick(currentTick);

    toggleLights();

    for (auto& v : vehicles) {
        // skip vehicles that already arrived
        if (v->hasArrived()) continue;

        Intersection* prevPosition = v->getCurrentPosition();

        // vehicle decides behavior at current intersection
        v->behaviorAtIntersection(prevPosition);

        // choose next street based on pathfinding
        Street* next = chooseNextStreet(v.get());
        if (next == nullptr) continue;

        // remove from current street
        if (v->getCurrentStreet() != nullptr)
            v->getCurrentStreet()->removeVehicle(v.get());

        // move to next intersection
        v->move(next);

        // add to new street
        next->addVehicle(v.get());

        // log the movement
        Intersection* newPosition = v->getCurrentPosition();
        if (newPosition != prevPosition) {
            logger.logIntersectionExit(v.get(), prevPosition);
            logger.logIntersectionEntry(v.get(), newPosition);
            std::cout << "[TICK " << currentTick << "] " 
                      << v->getId() << " moved to " 
                      << newPosition->getId() << "\n";
        }
    }

    checkCollisions();
    checkDelivery();
}

void Simulation::run() {
    network.build();
    spawnDeliveryCar();
    spawnRandomTraffic();

    std::cout << "\n--- Simulation Start ---\n\n";

    // run until delivery done or 50 ticks max
    while (!deliveryComplete && currentTick < 50) {
        tick();
    }

    if (!deliveryComplete)
        std::cout << "\n[TIMEOUT] Delivery not completed in 50 ticks\n";

    logger.logSummary(currentTick);
    std::cout << "\n--- Simulation End --- Total ticks: " 
              << currentTick << " ---\n";
}