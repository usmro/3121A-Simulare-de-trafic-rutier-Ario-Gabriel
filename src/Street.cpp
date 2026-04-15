#include "Street.h"
#include "Intersection.h"  // NOW we include it, in the .cpp not the .h
#include <algorithm>       // for std::remove

Street::Street(std::string name, Intersection* a, Intersection* b,
               int length, int speedLimit) {
    this->name = name;
    this->endA = a;
    this->endB = b;
    this->length = length;
    this->speedLimit = speedLimit;
    this->congestionLevel = 1; // starts clear, simulation will randomize
}

Intersection* Street::getOtherEnd(Intersection* from) {
    // if you're coming from A, return B and vice versa
    // this is how graph traversal works — you know where you came from
    if (from == endA) return endB;
    if (from == endB) return endA;
    return nullptr; // should never happen
}

void Street::addVehicle(Vehicle* v) {
    vehiclesOnStreet.push_back(v);
}

void Street::removeVehicle(Vehicle* v) {
    // std::remove shifts the element to the end, erase actually removes it
    // this is the standard C++ erase-remove idiom
    vehiclesOnStreet.erase(
        std::remove(vehiclesOnStreet.begin(), vehiclesOnStreet.end(), v),
        vehiclesOnStreet.end()
    );
}

std::vector<Vehicle*>& Street::getVehicles() {
    return vehiclesOnStreet;
}

std::string Street::getName() { return name; }
int Street::getLength() { return length; }
int Street::getSpeedLimit() { return speedLimit; }
int Street::getCongestionLevel() { return congestionLevel; }
void Street::setCongestionLevel(int level) { congestionLevel = level; }
Intersection* Street::getEndA() { return endA; }
Intersection* Street::getEndB() { return endB; }
