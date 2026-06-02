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

Simulation::Simulation(std::string logFilename, SimConfig cfg) : logger(logFilename) {
    config = cfg;
    currentTick = 0;
    deliveryComplete = false;
    totalPackages = 0;
    packagesDelivered = 0;
}

void Simulation::spawnDeliveryCar() {
    // random number of packages to deliver this run (3-5)
    std::uniform_int_distribution<int> pkgDist(3, 5);
    totalPackages = pkgDist(rng);
    packagesDelivered = 0;

    // spawn at a random corner
    std::vector<std::string> corners = {"A", "C", "G", "I"};
    std::uniform_int_distribution<int> d(0, 3);
    std::string startId = corners[d(rng)];

    Intersection* start = network.getIntersection(startId);
    deliveryCar = std::make_shared<Car>("DELIVERY", start, start);
    vehicles.push_back(deliveryCar);

    // assign the first package destination (diagonal opposite for a long first leg)
    assignNextPackage();

    std::cout << "Spawned DELIVERY at " << startId
              << " with " << totalPackages << " packages\n";
}

void Simulation::assignNextPackage() {
    // pick a new corner different from current position
    std::vector<std::string> corners = {"A", "C", "G", "I"};
    std::uniform_int_distribution<int> d(0, 3);
    std::string curId = deliveryCar->getCurrentPosition()->getId();
    std::string destId;
    do { destId = corners[d(rng)]; } while (destId == curId);

    Intersection* goal = network.getIntersection(destId);
    deliveryCar->setDestination(goal);
    std::cout << "  Package " << (packagesDelivered + 1) << "/" << totalPackages
              << " — deliver to " << destId << "\n";
}

void Simulation::spawnRandomTraffic() {
    // randomise congestion on all streets
    auto& streets = network.getStreets();
    std::uniform_int_distribution<int> congDist(1, 10);
    for (auto& s : streets)
        s->setCongestionLevel(congDist(rng));

    // BUG FIX: other vehicles also spawn at corners only, not any intersection
    std::vector<std::string> corners = {"A", "C", "G", "I"};
    std::uniform_int_distribution<int> cornerDist(0, 3);

    std::vector<std::string> types = {"Car", "Motorcycle", "Truck"};
    for (auto& type : types) {
        std::string startId = corners[cornerDist(rng)];
        std::string destId;
        do { destId = corners[cornerDist(rng)]; } while (destId == startId);

        Intersection* start = network.getIntersection(startId);
        Intersection* dest  = network.getIntersection(destId);

        if (type == "Car") {
            vehicles.push_back(std::make_shared<Car>("Car", start, dest));
        } else if (type == "Motorcycle") {
            vehicles.push_back(std::make_shared<Motorcycle>("Motorcycle", start, dest));
        } else {
            vehicles.push_back(std::make_shared<Truck>("Truck", start, dest));
        }
        std::cout << "Spawned " << type << " at "
                  << startId << " -> " << destId << "\n";
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
    // BUG FIX: use per-intersection random tick counter, not fixed modulo
    for (auto& i : network.getIntersections()) {
        int& remaining = lightTicks[i->getId()];
        if (remaining <= 0) {
            i->toggleLight();
            // pick a new random duration 1-3 ticks
            std::uniform_int_distribution<int> d(1, 3);
            remaining = d(rng);
        }
        remaining--;
    }
}

void Simulation::checkDelivery() {
    if (deliveryCar->hasArrived()) {
        packagesDelivered++;
        logger.logDelivery(deliveryCar.get(), deliveryCar->getDestination());
        std::cout << "  [PACKAGE " << packagesDelivered << "/" << totalPackages
                  << " DELIVERED at tick " << currentTick << "]\n";

        if (packagesDelivered >= totalPackages) {
            // all packages done — run complete
            deliveryComplete = true;
            std::cout << "  [ALL " << totalPackages
                      << " PACKAGES DELIVERED in " << currentTick << " ticks]\n";
        } else {
            // more to deliver — pick the next target and keep going
            assignNextPackage();
        }
    }
}

// build a snapshot of current state for the JSON log
TickSnapshot Simulation::buildSnapshot(
    const std::vector<std::string>& collisions,
    bool delivered,
    const std::string& deliveredTo)
{
    TickSnapshot snap;
    snap.tick = currentTick;
    snap.delivered = delivered;
    snap.deliveredTo = deliveredTo;
    snap.collisions = collisions;
    snap.packagesDelivered = packagesDelivered;
    snap.totalPackages = totalPackages;

    for (auto& v : vehicles) {
        TickSnapshot::VehicleState vs;
        vs.id    = v->getId();
        vs.pos   = v->getCurrentPosition()->getId();
        vs.dest  = v->getDestination()->getId();
        vs.speed = v->getCurrentSpeed();

        // delivery car is only "done" when ALL packages are delivered
        if (v->getId() == "DELIVERY" && deliveryComplete) {
            vs.status = "done";
        }
        else if (v->getId() != "DELIVERY" && v->hasArrived()) {
            vs.status = "done";
        }
        else if (v->freezeTicks > 0) {
            vs.status = "CRASHED";
        }
        else if (v->waitTicks > 0) {
            vs.status = "TRAFFIC";
        }
        else if (v->getCurrentSpeed() == 0
                 && v->getCurrentPosition()->isRed()) {
            vs.status = "RED";
        }
        else {
            vs.status = "moving";
        }

        snap.vehicles.push_back(vs);
    }

    for (auto& i : network.getIntersections()) {
        TickSnapshot::LightState ls;
        ls.id             = i->getId();
        ls.isRed          = i->isRed();
        ls.ticksRemaining = lightTicks.count(i->getId())
                            ? lightTicks[i->getId()] : 0;
        snap.lights.push_back(ls);
    }

    return snap;
}

void Simulation::tick() {
    currentTick++;
    logger.setTick(currentTick);

    std::string tickLine = "TICK " + std::to_string(currentTick) + ": ";
    std::vector<std::string> collisions;
    bool firstMove = true;
    bool deliveredThisTick = false;
    std::string deliveredTo = "";

    // OPTION C congestion model (LIVE occupancy):
    // Track which nodes are occupied, updated AS cars move within the tick.
    // A car heading toward an occupied node waits. When the car ahead vacates
    // a node, the node frees up immediately so the car behind can follow —
    // a line of traffic flows forward smoothly instead of shuffling.
    // Directional: only the car moving toward an occupied spot yields.
    std::map<std::string, int> nodeOccupied;
    for (auto& v : vehicles) {
        if (v->freezeTicks > 0) continue; // crashed cars are pulled aside, don't block
        if (v->hasArrived() && v->getId() != "DELIVERY") continue;
        nodeOccupied[v->getCurrentPosition()->getId()]++;
    }

    for (auto& v : vehicles) {
        // frozen in a collision? tick it down, stay put (pulled aside, doesn't block)
        if (v->freezeTicks > 0) {
            v->freezeTicks--;
            if (!firstMove) tickLine += "  ";
            tickLine += v->getId() + "[CRASHED]";
            firstMove = false;
            continue;
        }

        // background vehicles (not the delivery) re-roam when they arrive:
        // pick a fresh random corner so the map stays alive all run
        if (v->hasArrived()) {
            if (v->getId() != "DELIVERY") {
                std::vector<std::string> corners = {"A", "C", "G", "I"};
                std::uniform_int_distribution<int> cd(0, 3);
                std::string curId = v->getCurrentPosition()->getId();
                std::string newDest;
                do { newDest = corners[cd(rng)]; } while (newDest == curId);
                v->setDestination(network.getIntersection(newDest));
            } else {
                // delivery handled by checkDelivery()
                continue;
            }
        }

        // car-count congestion wait (set last tick)
        if (v->waitTicks > 0) {
            v->waitTicks--;
            if (!firstMove) tickLine += "  ";
            tickLine += v->getId() + "[TRAFFIC]";
            firstMove = false;
            continue;
        }

        Intersection* prevPos = v->getCurrentPosition();
        v->behaviorAtIntersection(prevPos);

        // red light block
        if (v->getCurrentSpeed() == 0) {
            if (!firstMove) tickLine += "  ";
            tickLine += v->getId() + "[RED]";
            firstMove = false;
            continue;
        }

        Street* next = chooseNextStreet(v.get());
        if (next == nullptr) continue;

        // the node this car would arrive at if it takes this street
        Intersection* targetNode = next->getOtherEnd(v->getCurrentPosition());
        std::string targetId = targetNode ? targetNode->getId() : "";
        std::string fromId = v->getCurrentPosition()->getId();

        // OPTION C CONGESTION (live): a car waits if the node it's heading toward
        // is occupied RIGHT NOW. Because the map updates as cars move, once the
        // car ahead vacates a node it frees up immediately for the car behind —
        // so a line of traffic flows forward instead of stalling.
        int occThere = nodeOccupied.count(targetId) ? nodeOccupied[targetId] : 0;
        bool immuneCong = (v->getId() == "DELIVERY") && config.deliveryImmune;
        if (occThere >= 1 && !immuneCong && v->lastWaitedStreet != next) {
            v->waitTicks = (occThere >= 2) ? 2 : 1;
            v->lastWaitedStreet = next;
            if (!firstMove) tickLine += "  ";
            tickLine += v->getId() + "[TRAFFIC]";
            firstMove = false;
            continue;
        }
        v->lastWaitedStreet = nullptr;

        if (v->getCurrentStreet() != nullptr)
            v->getCurrentStreet()->removeVehicle(v.get());

        logger.logIntersectionExit(v.get(), prevPos);
        v->move(next);
        next->addVehicle(v.get());

        // update live occupancy: this car left fromId and now sits at targetId
        if (nodeOccupied.count(fromId) && nodeOccupied[fromId] > 0)
            nodeOccupied[fromId]--;
        nodeOccupied[targetId]++;

        Intersection* newPos = v->getCurrentPosition();
        logger.logIntersectionEntry(v.get(), newPos);

        if (!firstMove) tickLine += "  ";
        tickLine += v->getId() + "->" + newPos->getId();
        firstMove = false;

        if (v->hasArrived() && v->getId() == "DELIVERY") {
            deliveredThisTick = true;
            deliveredTo = newPos->getId();
        }
    }

    // COLLISION (Option C): 3+ live cars ending the tick at the SAME node = accident.
    // The crash is about cars converging on the same spot, not sharing a road segment.
    // Crashed cars freeze 2-3 ticks, pulled aside (don't block), then resume.
    // Delivery can be exempted via the collision-immunity setting.
    std::map<std::string, std::vector<Vehicle*>> atNode;
    for (auto& v : vehicles) {
        if (v->freezeTicks > 0) continue; // already crashed, pulled aside
        if (v->hasArrived() && v->getId() != "DELIVERY") continue;
        atNode[v->getCurrentPosition()->getId()].push_back(v.get());
    }

    for (auto& [nodeId, here] : atNode) {
        if (here.size() >= 3) {
            std::uniform_int_distribution<int> fd(2, 3);
            std::string names;
            int crashedCount = 0;
            for (int i = 0; i < (int)here.size(); i++) {
                Vehicle* vv = here[i];
                bool immuneCol = (vv->getId() == "DELIVERY") && config.deliveryImmuneCol;
                if (!immuneCol) {
                    vv->freezeTicks = fd(rng);
                    // pull aside: remove from its current street so it doesn't block
                    if (vv->getCurrentStreet() != nullptr)
                        vv->getCurrentStreet()->removeVehicle(vv);
                    crashedCount++;
                }
                if (i) names += "+";
                names += vv->getId();
            }
            if (crashedCount > 0) {
                logger.logCollision(here[0], here[1], nullptr);
                collisions.push_back(names + " at " + nodeId);
            }
        }
    }

    std::cout << tickLine;
    if (!collisions.empty()) {
        std::cout << "  [COLLISION: ";
        for (int i = 0; i < (int)collisions.size(); i++) {
            if (i) std::cout << ", ";
            std::cout << collisions[i];
        }
        std::cout << "]";
    }
    std::cout << "\n";

    // BUG FIX: lights flip AFTER vehicles move
    toggleLights();

    // check delivery FIRST so package counts are current for the snapshot
    checkDelivery();

    // log full snapshot for JSON (now reflects updated package count)
    TickSnapshot snap = buildSnapshot(collisions, deliveredThisTick, deliveredTo);
    logger.logSnapshot(snap);
}

void Simulation::run() {
    network.build();

    // initialise light states and tick counters
    std::uniform_int_distribution<int> lightDist(0, 1);
    std::uniform_int_distribution<int> tickDist(1, 3);
    for (auto& i : network.getIntersections()) {
        if (config.randomLights && lightDist(rng)) i->toggleLight();
        lightTicks[i->getId()] = tickDist(rng);
    }

    spawnDeliveryCar();
    spawnRandomTraffic();

    std::cout << "\n--- Simulation Start ---\n";

    // honour the configured max-tick cap
    int maxTicks = config.maxTicks;

    while (!deliveryComplete && currentTick < maxTicks) {
        tick();
    }

    if (!deliveryComplete)
        std::cout << "\n[TIMEOUT] Only " << packagesDelivered << "/"
                  << totalPackages << " packages delivered in "
                  << currentTick << " ticks\n";

    logger.logSummary(currentTick);
    logger.flushJSON();

    std::cout << "\n--- Simulation End --- " << packagesDelivered << "/"
              << totalPackages << " packages, " << currentTick << " ticks ---\n";
}