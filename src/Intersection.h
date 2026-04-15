#pragma once
#include <string>
#include <vector>

class Street;   // forward declaration again

class Intersection {
private:
    std::string id;                      // "A", "B", "C" etc.
    std::vector<Street*> connectedStreets;
    bool trafficLightRed;                // true = red, false = green
    int lightTimer;                      // counts ticks until toggle

public:
    Intersection(std::string id);

    // connect a street to this intersection
    // called during map building in RoadNetwork
    void addStreet(Street* s);

    // pathfinding needs this — all streets you can take from here
    std::vector<Street*>& getStreets();

    // traffic light
    bool isRed();
    void toggleLight();

    std::string getId();
};
