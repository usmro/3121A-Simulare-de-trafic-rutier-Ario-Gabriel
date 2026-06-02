#include <iostream>
#include <string>
#include <cstdlib>
#include "Simulation.h"

// Parse command-line args into a SimConfig.
// Usage: app [maxTicks] [congThresh] [deliveryImmune] [deliveryImmuneCol] [randomLights]
// All optional — defaults are used if omitted.
static SimConfig parseArgs(int argc, char** argv) {
    SimConfig cfg; // defaults
    if (argc > 1) cfg.maxTicks         = std::atoi(argv[1]);
    if (argc > 2) cfg.congThresh       = std::atoi(argv[2]);
    if (argc > 3) cfg.deliveryImmune   = (std::atoi(argv[3]) != 0);
    if (argc > 4) cfg.deliveryImmuneCol= (std::atoi(argv[4]) != 0);
    if (argc > 5) cfg.randomLights     = (std::atoi(argv[5]) != 0);

    // sanity clamps so bad input can't break the sim
    if (cfg.maxTicks   < 10)  cfg.maxTicks   = 10;
    if (cfg.maxTicks   > 500) cfg.maxTicks   = 500;
    if (cfg.congThresh < 1)   cfg.congThresh = 1;
    if (cfg.congThresh > 10)  cfg.congThresh = 10;
    return cfg;
}

int main(int argc, char** argv) {
    SimConfig cfg = parseArgs(argc, argv);

    std::cout << "Config: maxTicks=" << cfg.maxTicks
              << " congThresh=" << cfg.congThresh
              << " deliveryImmune=" << cfg.deliveryImmune
              << " deliveryImmuneCol=" << cfg.deliveryImmuneCol
              << " randomLights=" << cfg.randomLights << "\n";

    std::cout << "=============================\n";
    std::cout << "  Traffic Simulation - Day 1 \n";
    std::cout << "=============================\n\n";
    Simulation sim1("log_day1.txt", cfg);
    sim1.run();

    std::cout << "\n\n";

    std::cout << "=============================\n";
    std::cout << "  Traffic Simulation - Day 2 \n";
    std::cout << "=============================\n\n";
    Simulation sim2("log_day2.txt", cfg);
    sim2.run();

    return 0;
}