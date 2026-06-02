#include "Motorcycle.h"
#include "Street.h"
#include "Intersection.h"

int Motorcycle::getTickRate() { return 1; }

Motorcycle::Motorcycle(std::string id, Intersection* start, Intersection* dest)
    : Vehicle(id, start, dest) {}

int Motorcycle::getMaxSpeed() {
    return 160;
}

int Motorcycle::edgeCost(Street* s) {
    // greedy fewest hops — every street costs 1
    return 1;
}

void Motorcycle::behaviorAtIntersection(Intersection* i) {
    if (i->isRed()) {
        // BUG FIX: was currentSpeed/2 which is 0 on first tick
        // now correctly uses getMaxSpeed()/2 so it always slows, never freezes
        currentSpeed = getMaxSpeed() / 2;
    } else {
        currentSpeed = getMaxSpeed();
    }
}