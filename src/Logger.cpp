#include "Logger.h"
#include "Vehicle.h"
#include "Intersection.h"
#include "Street.h"

Logger::Logger(std::string filename) {
    logFile.open(filename);
    currentTick = 0;
}

Logger::~Logger() {
    if (logFile.is_open())
        logFile.close();
}

void Logger::setTick(int tick) {
    currentTick = tick;
}

void Logger::logIntersectionEntry(Vehicle* v, Intersection* i) {
    logFile << "[TICK " << currentTick << "] [ENTRY] Vehicle " 
            << v->getId() << " entered intersection " 
            << i->getId() << "\n";
}

void Logger::logIntersectionExit(Vehicle* v, Intersection* i) {
    logFile << "[TICK " << currentTick << "] [EXIT]  Vehicle " 
            << v->getId() << " left intersection " 
            << i->getId() << "\n";
}

void Logger::logCollision(Vehicle* a, Vehicle* b, Street* s) {
    logFile << "[TICK " << currentTick << "] [COLLISION] " 
            << a->getId() << " and " << b->getId() 
            << " collided on street " << s->getName() << "\n";
}

void Logger::logDelivery(Vehicle* v, Intersection* dest) {
    logFile << "[TICK " << currentTick << "] [DELIVERY] " 
            << v->getId() << " delivered package to " 
            << dest->getId() << "\n";
}

void Logger::logSummary(int totalTicks) {
    logFile << "\n=== SIMULATION COMPLETE ===\n";
    logFile << "Total ticks: " << totalTicks << "\n";
}