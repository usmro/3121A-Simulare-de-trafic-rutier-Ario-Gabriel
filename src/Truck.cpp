#include "Truck.h"
#include "Street.h"
#include "Intersection.h"

Truck::Truck(std::string id, Intersection* start, Intersection* dest)
    : Vehicle(id, start, dest) {}

int Truck::getMaxSpeed() {
    return 90;
}

int Truck::edgeCost(Street* s) {
    // avoids busy streets — congestion level is the cost
    return s->getCongestionLevel();
}

void Truck::behaviorAtIntersection(Intersection* i) {
    // always stops, slow and cautious
    currentSpeed = 0;
}