#include "Car.h"
#include "Street.h"
#include "Intersection.h"

Car::Car(std::string id, Intersection* start, Intersection* dest)
    : Vehicle(id, start, dest) {}

int Car::getMaxSpeed() {
    return 120;
}

int Car::edgeCost(Street* s) {
    // cares about distance — classic shortest path
    return s->getLength();
}

void Car::behaviorAtIntersection(Intersection* i) {
    if (i->isRed()) {
        currentSpeed = 0;       // full stop at red
    } else {
        currentSpeed = getMaxSpeed();
    }
}