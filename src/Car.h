#pragma once
#include "Vehicle.h"

class Car : public Vehicle {
public:
    Car(std::string id, Intersection* start, Intersection* dest);

    int getMaxSpeed() override;
    void behaviorAtIntersection(Intersection* i) override;
    int edgeCost(Street* s) override;
};