#pragma once
#include <string>
#include <vector>

class Intersection; // forward declaration, not #include
class Vehicle;      // same for Vehicle

class Street {
private:
    std::string name;
    int length;          // in meters, used by Car's edgeCost
    int speedLimit;      // km/h
    int congestionLevel; // 1-10, randomized each simulation run

    Intersection* endA; // one end of the street
    Intersection* endB; // other end of the street

    std::vector<Vehicle*> vehiclesOnStreet; // who's currently on this street

public:
    Street(std::string name, Intersection* a, Intersection* b,
           int length, int speedLimit);

    // given you're coming FROM one end, return the OTHER end
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