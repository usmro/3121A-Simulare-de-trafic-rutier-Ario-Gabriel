// at the top of Street.h, instead of #include "Intersection.h"
class Intersection;  // just declares it exists, no details needed yet
#ifndef STREET_H_INCLUDED
#define STREET_H_INCLUDED



#endif // STREET_H_INCLUDED
#pragma once
#include <string>
#include <vector>

class Intersection;  // forward declaration, not #include
class Vehicle;       // same for Vehicle, we'll use it later

class Street {
private:
    std::string name;
    int length;          // in meters, used by Car's edgeCost
    int speedLimit;      // km/h
    int congestionLevel; // 1-10, randomized each simulation run

    Intersection* endA;  // one end of the street
    Intersection* endB;  // other end of the street

    std::vector<Vehicle*> vehiclesOnStreet; // who's currently on this street

public:
    // constructor
    Street(std::string name, Intersection* a, Intersection* b,
           int length, int speedLimit);

    // the most important method — given you're coming FROM one end,
    // return the OTHER end. This is how vehicles traverse the graph
    Intersection* getOtherEnd(Intersection* from);

    // vehicle tracking
    void addVehicle(Vehicle* v);
    void removeVehicle(Vehicle* v);
    std::vector<Vehicle*>& getVehicles();

    // getters
    std::string getName();
    int getLength();
    int getSpeedLimit();
    int getCongestionLevel();
    void setCongestionLevel(int level);

    Intersection* getEndA();
    Intersection* getEndB();
};
