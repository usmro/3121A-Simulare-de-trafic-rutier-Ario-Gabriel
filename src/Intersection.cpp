#include "Intersection.h"
#include "Street.h"

Intersection::Intersection(std::string id) {
    this->id = id;
    this->trafficLightRed = false; // starts green
    this->lightTimer = 0;
}

void Intersection::addStreet(Street* s) {
    connectedStreets.push_back(s);
}

std::vector<Street*>& Intersection::getStreets() {
    return connectedStreets;
}

bool Intersection::isRed() {
    return trafficLightRed;
}

void Intersection::toggleLight() {
    trafficLightRed = !trafficLightRed;
}

std::string Intersection::getId() {
    return id;
}
