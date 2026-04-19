#pragma once
#include "Vehicle.h"

class Motorcycle : public Vehicle {
public:
    Motorcycle(std::string id, Intersection* start, Intersection* dest);

    int getMaxSpeed() override;
    void behaviorAtIntersection(Intersection* i) override;
    int edgeCost(Street* s) override;
};