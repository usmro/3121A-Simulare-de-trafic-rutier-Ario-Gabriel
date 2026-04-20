#include <iostream>
#include "Simulation.h"

int main() {
    std::cout << "=============================\n";
    std::cout << "  Traffic Simulation - Day 1 \n";
    std::cout << "=============================\n\n";
    Simulation sim1("log_day1.txt");
    sim1.run();

    std::cout << "\n\n";

    std::cout << "=============================\n";
    std::cout << "  Traffic Simulation - Day 2 \n";
    std::cout << "=============================\n\n";
    Simulation sim2("log_day2.txt");
    sim2.run();

    return 0;
}