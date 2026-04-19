#pragma once
#include <string>
#include <fstream>

class Vehicle;
class Intersection;
class Street;

class Logger {
private:
    std::ofstream logFile;
    int currentTick;

public:
    Logger(std::string filename);
    ~Logger();

    void setTick(int tick);

    void logIntersectionEntry(Vehicle* v, Intersection* i);
    void logIntersectionExit(Vehicle* v, Intersection* i);
    void logCollision(Vehicle* a, Vehicle* b, Street* s);
    void logDelivery(Vehicle* v, Intersection* dest);
    void logSummary(int totalTicks);
};