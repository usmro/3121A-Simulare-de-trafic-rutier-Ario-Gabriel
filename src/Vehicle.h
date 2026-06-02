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
    int waitTicks = 0;
    Street* lastWaitedStreet = nullptr; // street we last waited on (anti-deadlock)
    int freezeTicks = 0;                // collision freeze countdown (accident)

    Vehicle(std::string id, Intersection* start, Intersection* dest);

    virtual int getMaxSpeed() = 0;
    virtual void behaviorAtIntersection(Intersection* i) = 0;
    virtual int edgeCost(Street* s) = 0;
    virtual int getTickRate() = 0;

    void move(Street* nextStreet);
    bool hasArrived();

    std::string getId();
    Intersection* getCurrentPosition();
    Intersection* getDestination();
    Street* getCurrentStreet();
    int getCurrentSpeed();

    // NEW: change destination mid-run (for multi-package delivery)
    void setDestination(Intersection* dest);

    virtual ~Vehicle() {}
};  