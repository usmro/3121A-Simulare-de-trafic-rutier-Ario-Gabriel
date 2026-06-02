#pragma once
#include "Vehicle.h"

class Truck : public Vehicle {
public:
    Truck(std::string id, Intersection* start, Intersection* dest);

    int getMaxSpeed() override;
    void behaviorAtIntersection(Intersection* i) override;
    int edgeCost(Street* s) override;
    int getTickRate() override;
    // BUG FIX: removed duplicate waitTicks — it's already in Vehicle base class
};