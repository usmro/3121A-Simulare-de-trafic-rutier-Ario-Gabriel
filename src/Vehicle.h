#pragma once
#include <string>

class Street;
class Intersection;

class Vehicle {
protected:
    std::string id;
    int currentSpeed;
    Street* currentStreet;
    Intersection* currentPosition;
    Intersection* destination;

public:
    Vehicle(std::string id, Intersection* start, Intersection* dest);

    // pure virtual — every subclass MUST implement these
    virtual int getMaxSpeed() = 0;
    virtual void behaviorAtIntersection(Intersection* i) = 0;
    virtual int edgeCost(Street* s) = 0;

    // same for everyone, no need to override
    void move(Street* nextStreet);
    bool hasArrived();

    // getters
    std::string getId();
    Intersection* getCurrentPosition();
    Intersection* getDestination();
    Street* getCurrentStreet();
    int getCurrentSpeed();

    virtual ~Vehicle() {}
};