// src/tests.cpp
// Unit tests for the traffic simulation logic.
// Covers: street/intersection basics, vehicle MOVEMENT, and ROUTE VALIDATION
// (pathfinding), as required by the project brief.
#include <cassert>
#include <iostream>
#include "Street.h"
#include "Intersection.h"
#include "RoadNetwork.h"
#include "Vehicle.h"
#include "Car.h"
#include "Motorcycle.h"
#include "Truck.h"

// ─────────────────────────────────────────────────────────────
// BASIC STRUCTURE TESTS
// ─────────────────────────────────────────────────────────────

void testGetOtherEnd() {
    Intersection a("A");
    Intersection b("B");
    Street ab("AB", &a, &b, 100, 50);

    assert(ab.getOtherEnd(&a) == &b);
    assert(ab.getOtherEnd(&b) == &a);
    std::cout << "testGetOtherEnd PASSED\n";
}

void testIntersectionLight() {
    Intersection a("A");
    assert(a.isRed() == false); // starts green
    a.toggleLight();
    assert(a.isRed() == true);
    a.toggleLight();
    assert(a.isRed() == false);
    std::cout << "testIntersectionLight PASSED\n";
}

void testNetworkBuild() {
    RoadNetwork network;
    network.build();
    assert(network.getIntersection("A") != nullptr);
    assert(network.getIntersection("I") != nullptr);
    assert(network.getIntersection("E") != nullptr); // centre
    assert(network.getIntersection("Z") == nullptr);  // doesn't exist
    assert(network.getIntersections().size() == 9);
    assert(network.getStreets().size() == 12);
    std::cout << "testNetworkBuild PASSED\n";
}

// ─────────────────────────────────────────────────────────────
// MOVEMENT LOGIC TESTS (required by brief)
// ─────────────────────────────────────────────────────────────

void testVehicleStartsAtStart() {
    Intersection a("A");
    Intersection b("B");
    Car car("TEST", &a, &b);
    // a freshly spawned vehicle is at its start, not yet arrived
    assert(car.getCurrentPosition() == &a);
    assert(car.getDestination() == &b);
    assert(car.hasArrived() == false);
    std::cout << "testVehicleStartsAtStart PASSED\n";
}

void testVehicleMovesToCorrectNode() {
    Intersection a("A");
    Intersection b("B");
    Street ab("AB", &a, &b, 100, 50);

    Car car("TEST", &a, &b);
    car.move(&ab);
    // after moving along AB from A, the car must now be at B
    assert(car.getCurrentPosition() == &b);
    std::cout << "testVehicleMovesToCorrectNode PASSED\n";
}

void testVehicleArrivesAtDestination() {
    Intersection a("A");
    Intersection b("B");
    Street ab("AB", &a, &b, 100, 50);

    Car car("TEST", &a, &b);
    assert(car.hasArrived() == false);
    car.move(&ab);
    // destination was B, car is now at B => arrived
    assert(car.hasArrived() == true);
    std::cout << "testVehicleArrivesAtDestination PASSED\n";
}

void testSetDestinationResetsArrival() {
    Intersection a("A");
    Intersection b("B");
    Car car("TEST", &a, &a); // starts already "arrived" (start == dest)
    assert(car.hasArrived() == true);
    // give it a new destination — it should no longer be arrived
    car.setDestination(&b);
    assert(car.hasArrived() == false);
    std::cout << "testSetDestinationResetsArrival PASSED\n";
}

void testPolymorphicSpeeds() {
    Intersection a("A");
    Intersection b("B");
    Car car("C", &a, &b);
    Motorcycle moto("M", &a, &b);
    Truck truck("T", &a, &b);
    // each type reports its own max speed (polymorphism)
    assert(car.getMaxSpeed() == 120);
    assert(moto.getMaxSpeed() == 160);
    assert(truck.getMaxSpeed() == 90);
    std::cout << "testPolymorphicSpeeds PASSED\n";
}

// ─────────────────────────────────────────────────────────────
// ROUTE VALIDATION TESTS (required by brief)
// ─────────────────────────────────────────────────────────────

void testPathExistsBetweenCorners() {
    RoadNetwork network;
    network.build();
    Intersection* a = network.getIntersection("A");
    Intersection* i = network.getIntersection("I");

    Car car("TEST", a, i);
    std::vector<Street*> path = network.findPath(a, i, &car);
    // A to I must have a route on this connected grid
    assert(!path.empty());
    std::cout << "testPathExistsBetweenCorners PASSED\n";
}

void testPathShortestLengthAtoI() {
    RoadNetwork network;
    network.build();
    Intersection* a = network.getIntersection("A");
    Intersection* i = network.getIntersection("I");

    // Car routes by distance. On the 3x3 grid the corner-to-opposite-corner
    // trip (A->I) is 4 hops minimum regardless of the exact path taken.
    Car car("TEST", a, i);
    std::vector<Street*> path = network.findPath(a, i, &car);
    assert(path.size() == 4);
    std::cout << "testPathShortestLengthAtoI PASSED\n";
}

void testPathStartEqualsGoal() {
    RoadNetwork network;
    network.build();
    Intersection* a = network.getIntersection("A");

    Car car("TEST", a, a);
    std::vector<Street*> path = network.findPath(a, a, &car);
    // no movement needed when already at the goal
    assert(path.empty());
    std::cout << "testPathStartEqualsGoal PASSED\n";
}

void testPathAdjacentNodes() {
    RoadNetwork network;
    network.build();
    Intersection* a = network.getIntersection("A");
    Intersection* b = network.getIntersection("B");

    // A and B are directly connected => path is exactly one street
    Car car("TEST", a, b);
    std::vector<Street*> path = network.findPath(a, b, &car);
    assert(path.size() == 1);
    // and that street must actually connect A to B
    Street* s = path[0];
    assert(s->getOtherEnd(a) == b);
    std::cout << "testPathAdjacentNodes PASSED\n";
}

void testPathFollowedReachesGoal() {
    RoadNetwork network;
    network.build();
    Intersection* a = network.getIntersection("A");
    Intersection* i = network.getIntersection("I");

    // walk the whole returned path and confirm it actually lands on the goal —
    // this validates movement + routing together
    Car car("TEST", a, i);
    int guard = 0;
    while (!car.hasArrived() && guard < 50) {
        std::vector<Street*> path = network.findPath(
            car.getCurrentPosition(), i, &car);
        assert(!path.empty());      // a route must always exist
        car.move(path[0]);          // take the first step
        guard++;
    }
    assert(car.hasArrived());        // following the route reaches I
    std::cout << "testPathFollowedReachesGoal PASSED\n";
}

// ─────────────────────────────────────────────────────────────

int main() {
    std::cout << "=== STRUCTURE ===\n";
    testGetOtherEnd();
    testIntersectionLight();
    testNetworkBuild();

    std::cout << "\n=== MOVEMENT ===\n";
    testVehicleStartsAtStart();
    testVehicleMovesToCorrectNode();
    testVehicleArrivesAtDestination();
    testSetDestinationResetsArrival();
    testPolymorphicSpeeds();

    std::cout << "\n=== ROUTE VALIDATION ===\n";
    testPathExistsBetweenCorners();
    testPathShortestLengthAtoI();
    testPathStartEqualsGoal();
    testPathAdjacentNodes();
    testPathFollowedReachesGoal();

    std::cout << "\nAll tests passed.\n";
    return 0;
}