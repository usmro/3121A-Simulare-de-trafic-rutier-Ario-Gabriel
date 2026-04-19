#pragma once
#include "Vehicle.h"

class Truck : public Vehicle {
public:
    Truck(std::string id, Intersection* start, Intersection* dest);

    int getMaxSpeed() override;
    void behaviorAtIntersection(Intersection* i) override;
    int edgeCost(Street* s) override;
};