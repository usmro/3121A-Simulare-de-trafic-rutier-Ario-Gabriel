#include "Motorcycle.h"
#include "Street.h"
#include "Intersection.h"

Motorcycle::Motorcycle(std::string id, Intersection* start, Intersection* dest)
    : Vehicle(id, start, dest) {}

int Motorcycle::getMaxSpeed() {
    return 160;
}

int Motorcycle::edgeCost(Street* s) {
    // every hop costs 1 — fewest intersections wins
    // doesn't care about distance at all
    return 1;
}

void Motorcycle::behaviorAtIntersection(Intersection* i) {
    if (i->isRed()) {
        currentSpeed = currentSpeed / 2; // slows down but doesn't fully stop
    } else {
        currentSpeed = getMaxSpeed();
    }
}