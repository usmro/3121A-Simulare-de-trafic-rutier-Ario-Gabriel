#include <iostream>
#include "RoadNetwork.h"
#include "Car.h"
#include "Motorcycle.h"
#include "Truck.h"

int main() {
    RoadNetwork network;
    network.build();

    Intersection* start = network.getIntersection("A");
    Intersection* goal  = network.getIntersection("I");

    // Car — shortest distance
    Car car("CAR-1", start, goal);
    std::vector<Street*> carPath = network.findPath(start, goal, &car);
    std::cout << "Car path (shortest distance): ";
    for (Street* s : carPath)
        std::cout << s->getName() << " ";
    std::cout << "\n";

    // Motorcycle — fewest hops
    Motorcycle moto("MOTO-1", start, goal);
    std::vector<Street*> motoPath = network.findPath(start, goal, &moto);
    std::cout << "Motorcycle path (fewest hops): ";
    for (Street* s : motoPath)
        std::cout << s->getName() << " ";
    std::cout << "\n";

    // Truck — avoids congestion
    network.getStreets()[0]->setCongestionLevel(8);
    network.getStreets()[3]->setCongestionLevel(7);

    Truck truck("TRUCK-1", start, goal);
    std::vector<Street*> truckPath = network.findPath(start, goal, &truck);
    std::cout << "Truck path (avoids congestion): ";
    for (Street* s : truckPath)
        std::cout << s->getName() << " ";
    std::cout << "\n";

    return 0;
}