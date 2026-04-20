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

static std::mt19937 rng(std::random_device{}());

static std::string getVehiclesAt(
    Intersection* i,
    std::vector<std::shared_ptr<Vehicle>>& vehicles)
{
    std::string result = "";
    for (auto& v : vehicles) {
        if (v->getCurrentPosition() == i) {
            if (result != "") result += ",";
            result += v->getId();
        }
    }
    return result;
}

static std::string formatNode(
    Intersection* i,
    std::vector<std::shared_ptr<Vehicle>>& vehicles)
{
    std::string light = i->isRed() ? "RED" : "GREEN";
    std::string veh = getVehiclesAt(i, vehicles);
    std::string label = i->getId() + ":" + light;
    if (veh != "") label += ":" + veh;
    return "[" + label + "]";
}

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
    auto& streets = network.getStreets();
    std::uniform_int_distribution<int> congDist(1, 10);
    for (auto& s : streets)
        s->setCongestionLevel(congDist(rng));

    auto& intersections = network.getIntersections();
    std::uniform_int_distribution<int> nodeDist(0, intersections.size() - 1);

    std::vector<std::string> types = {"Car", "Motorcycle", "Truck"};

    for (int i = 0; i < 3; i++) {
        int startIdx = nodeDist(rng);
        int destIdx  = nodeDist(rng);
        while (destIdx == startIdx)
            destIdx = nodeDist(rng);

        Intersection* start = intersections[startIdx].get();
        Intersection* dest  = intersections[destIdx].get();

        if (types[i] == "Car") {
            vehicles.push_back(std::make_shared<Car>("Car", start, dest));
            std::cout << "Spawned Car at "
                      << start->getId() << " -> " << dest->getId() << "\n";
        } else if (types[i] == "Motorcycle") {
            vehicles.push_back(std::make_shared<Motorcycle>("Motorcycle", start, dest));
            std::cout << "Spawned Motorcycle at "
                      << start->getId() << " -> " << dest->getId() << "\n";
        } else {
            vehicles.push_back(std::make_shared<Truck>("Truck", start, dest));
            std::cout << "Spawned Truck at "
                      << start->getId() << " -> " << dest->getId() << "\n";
        }
    }
}

Street* Simulation::chooseNextStreet(Vehicle* v) {
    std::vector<Street*> path = network.findPath(
        v->getCurrentPosition(),
        v->getDestination(),
        v
    );
    if (!path.empty()) return path[0];
    return nullptr;
}

void Simulation::toggleLights() {
    if (currentTick % 5 == 0) {
        for (auto& i : network.getIntersections())
            i->toggleLight();
    }
}

void Simulation::checkDelivery() {
    if (deliveryCar->hasArrived()) {
        deliveryComplete = true;
        logger.logDelivery(deliveryCar.get(), deliveryCar->getDestination());
        std::cout << "  [DELIVERED in " << currentTick << " ticks]\n";
    }
}

void Simulation::printMap() {
    Intersection* A = network.getIntersection("A");
    Intersection* B = network.getIntersection("B");
    Intersection* C = network.getIntersection("C");
    Intersection* D = network.getIntersection("D");
    Intersection* E = network.getIntersection("E");
    Intersection* F = network.getIntersection("F");
    Intersection* G = network.getIntersection("G");
    Intersection* H = network.getIntersection("H");
    Intersection* I = network.getIntersection("I");

    auto& streets = network.getStreets();
    int sAB = streets[0]->getSpeedLimit();
    int sBC = streets[1]->getSpeedLimit();
    int sDE = streets[2]->getSpeedLimit();
    int sEF = streets[3]->getSpeedLimit();
    int sGH = streets[4]->getSpeedLimit();
    int sHI = streets[5]->getSpeedLimit();
    int sAD = streets[6]->getSpeedLimit();
    int sDG = streets[7]->getSpeedLimit();
    int sBE = streets[8]->getSpeedLimit();
    int sEH = streets[9]->getSpeedLimit();
    int sCF = streets[10]->getSpeedLimit();
    int sFI = streets[11]->getSpeedLimit();

    std::cout << "\n";
    std::cout << "=============================================================\n";
    std::cout << "TICK " << currentTick << " - Traffic Map\n";
    std::cout << "=============================================================\n\n";

    std::cout << "  " << formatNode(A, vehicles)
              << "===" << sAB << "==="
              << formatNode(B, vehicles)
              << "===" << sBC << "==="
              << formatNode(C, vehicles) << "\n";

    std::cout << "       |" << sAD
              << "              |" << sBE
              << "              |" << sCF << "\n";

    std::cout << "  " << formatNode(D, vehicles)
              << "===" << sDE << "==="
              << formatNode(E, vehicles)
              << "===" << sEF << "==="
              << formatNode(F, vehicles) << "\n";

    std::cout << "       |" << sDG
              << "              |" << sEH
              << "              |" << sFI << "\n";

    std::cout << "  " << formatNode(G, vehicles)
              << "===" << sGH << "==="
              << formatNode(H, vehicles)
              << "===" << sHI << "==="
              << formatNode(I, vehicles) << "\n";

    std::cout << "\n";
    std::cout << "-------------------------------------------------------------\n";

    std::cout << "Vehicles:\n";
    for (auto& v : vehicles) {
        std::string type = "";
        if (v->getId() == "DELIVERY")
            type = "Car(Delivery)";
        else if (dynamic_cast<Motorcycle*>(v.get()))
            type = "Motorcycle";
        else if (dynamic_cast<Truck*>(v.get()))
            type = "Truck";
        else
            type = "Car";

        std::string light = v->getCurrentPosition()->isRed() ? "RED" : "GREEN";

        std::cout << "  " << v->getId()
                  << " @ " << v->getCurrentPosition()->getId()
                  << "[" << light << "]"
                  << "  type=" << type
                  << "  dest=" << v->getDestination()->getId()
                  << "  speed=" << v->getCurrentSpeed() << "km/h\n";
    }

    std::cout << "\n";
    std::cout << "Streets:\n";
    std::cout << "  AB:speed=" << sAB << "  "
              << "BC:speed=" << sBC << "  "
              << "DE:speed=" << sDE << "\n";
    std::cout << "  EF:speed=" << sEF << "  "
              << "GH:speed=" << sGH << "  "
              << "HI:speed=" << sHI << "\n";
    std::cout << "  AD:speed=" << sAD << "  "
              << "DG:speed=" << sDG << "  "
              << "BE:speed=" << sBE << "\n";
    std::cout << "  EH:speed=" << sEH << "  "
              << "CF:speed=" << sCF << "  "
              << "FI:speed=" << sFI << "\n";
    std::cout << "=============================================================\n\n";
}

void Simulation::tick() {
    currentTick++;
    logger.setTick(currentTick);
    toggleLights();

    std::string tickLine = "TICK " + std::to_string(currentTick) + ": ";
    std::string collisionLine = "";
    bool firstMove = true;

    for (auto& v : vehicles) {
        if (v->hasArrived()) continue;

        Intersection* prevPosition = v->getCurrentPosition();
        v->behaviorAtIntersection(prevPosition);

        Street* next = chooseNextStreet(v.get());
        if (next == nullptr) continue;

        if (v->getCurrentStreet() != nullptr)
            v->getCurrentStreet()->removeVehicle(v.get());

        v->move(next);
        next->addVehicle(v.get());

        Intersection* newPosition = v->getCurrentPosition();
        if (newPosition != prevPosition) {
            logger.logIntersectionExit(v.get(), prevPosition);
            logger.logIntersectionEntry(v.get(), newPosition);
            if (!firstMove) tickLine += "  ";
            tickLine += v->getId() + "->" + newPosition->getId();
            firstMove = false;
        }
    }

    // collision detection
    for (auto& street : network.getStreets()) {
        auto& vs = street->getVehicles();
        if (vs.size() >= 2) {
            for (int i = 0; i < (int)vs.size() - 1; i++) {
                logger.logCollision(vs[i], vs[i+1], street.get());
                if (collisionLine != "") collisionLine += ", ";
                collisionLine += vs[i]->getId() + "+"
                              + vs[i+1]->getId()
                              + " on " + street->getName();
            }
        }
    }

    std::cout << tickLine;
    if (collisionLine != "")
        std::cout << "  [COLLISION: " << collisionLine << "]";
    std::cout << "\n";

    checkDelivery();
}

void Simulation::run() {
    network.build();
    spawnDeliveryCar();
    spawnRandomTraffic();

    std::cout << "\n--- Simulation Start ---\n";
    printMap();

    while (!deliveryComplete && currentTick < 50) {
        tick();
    }

    if (!deliveryComplete)
        std::cout << "\n[TIMEOUT] Delivery not completed in 50 ticks\n";

    logger.logSummary(currentTick);
    std::cout << "\n--- Simulation End --- Total ticks: "
              << currentTick << " ---\n";
}