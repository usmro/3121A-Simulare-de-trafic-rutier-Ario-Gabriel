// src/tests.cpp
#include <cassert>
#include <iostream>
#include "Street.h"
#include "Intersection.h"
#include "RoadNetwork.h"

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
    assert(a.isRed() == false);   // starts green
    a.toggleLight();
    assert(a.isRed() == true);
    std::cout << "testIntersectionLight PASSED\n";
}

void testNetworkBuild() {
    RoadNetwork network;
    network.build();
    assert(network.getIntersection("A") != nullptr);
    assert(network.getIntersection("I") != nullptr);
    assert(network.getIntersection("Z") == nullptr); // doesn't exist
    std::cout << "testNetworkBuild PASSED\n";
}

int main() {
    testGetOtherEnd();
    testIntersectionLight();
    testNetworkBuild();

    std::cout << "\nAll tests passed.\n";
    return 0;
}
