#pragma once
#include <vector>
#include <string>
#include <memory>

#include "Intersection.h"
#include "Street.h"

class Vehicle;

class RoadNetwork {
private:
    std::vector<std::shared_ptr<Intersection>> intersections;
    std::vector<std::shared_ptr<Street>> streets;

public:
    void build();

    // pathfinding — vehicle passed in so we can call its edgeCost()
    std::vector<Street*> findPath(Intersection* start, 
                                   Intersection* goal, 
                                   Vehicle* vehicle);

    Intersection* getIntersection(std::string id);
    std::vector<std::shared_ptr<Street>>& getStreets();
    std::vector<std::shared_ptr<Intersection>>& getIntersections();
};