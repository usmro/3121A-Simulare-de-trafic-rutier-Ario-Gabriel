#include "Vehicle.h"
#include "Street.h"
#include "Intersection.h"

Vehicle::Vehicle(std::string id, Intersection* start, Intersection* dest) {
    this->id = id;
    this->currentPosition = start;
    this->destination = dest;
    this->currentStreet = nullptr;
    this->currentSpeed = 0;
    this->waitTicks = 0;
    this->lastWaitedStreet = nullptr;
    this->freezeTicks = 0;
}

void Vehicle::move(Street* nextStreet) {
    if (nextStreet == nullptr) return;

    Intersection* next = nextStreet->getOtherEnd(currentPosition);
    if (next == nullptr) return;

    currentStreet = nextStreet;
    currentPosition = next;
    currentSpeed = getMaxSpeed();
}

bool Vehicle::hasArrived() {
    return currentPosition == destination;
}

void Vehicle::setDestination(Intersection* dest) {
    destination = dest;
}

std::string Vehicle::getId() { return id; }
Intersection* Vehicle::getCurrentPosition() { return currentPosition; }
Intersection* Vehicle::getDestination() { return destination; }
Street* Vehicle::getCurrentStreet() { return currentStreet; }
int Vehicle::getCurrentSpeed() { return currentSpeed; }