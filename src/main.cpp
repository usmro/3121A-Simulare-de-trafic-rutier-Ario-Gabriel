#include <iostream>
#include "RoadNetwork.h"

int main() {
    RoadNetwork network;
    network.build();

    // test 1: can we find an intersection?
    Intersection* A = network.getIntersection("A");
    std::cout << "Found intersection: " << A->getId() << "\n";

    // test 2: does A have the right streets?
    std::cout << "Streets from A: ";
    for (Street* s : A->getStreets())
        std::cout << s->getName() << " ";
    std::cout << "\n";

    // test 3: does getOtherEnd work?
    Street* firstStreet = A->getStreets()[0];
    Intersection* other = firstStreet->getOtherEnd(A);
    std::cout << "Other end of " << firstStreet->getName()
              << " from A is: " << other->getId() << "\n";

    // test 4: can we manually walk A -> B -> E?
    Intersection* current = network.getIntersection("A");
    std::cout << "\nManual walk: " << current->getId();

    // from A, take street AB
    Street* AB = A->getStreets()[0]; // AB should be first
    current = AB->getOtherEnd(current);
    std::cout << " -> " << current->getId();

    // from B, take street BE
    // B has 3 streets: AB, BC, BE — BE is index 2
    Street* BE = current->getStreets()[2];
    current = BE->getOtherEnd(current);
    std::cout << " -> " << current->getId() << "\n";

    return 0;
}
