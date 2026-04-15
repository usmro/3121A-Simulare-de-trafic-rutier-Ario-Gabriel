#pragma once
#include <vector>
#include <string>
#include <memory>   // for shared_ptr

#include "Intersection.h"
#include "Street.h"

class RoadNetwork {
private:
    // shared_ptr manages memory for us — no manual delete needed
    std::vector<std::shared_ptr<Intersection>> intersections;
    std::vector<std::shared_ptr<Street>> streets;

public:
    void build();   // hardcodes the entire map, called once

    // lookup helpers
    Intersection* getIntersection(std::string id);
    std::vector<std::shared_ptr<Street>>& getStreets();
    std::vector<std::shared_ptr<Intersection>>& getIntersections();
};
