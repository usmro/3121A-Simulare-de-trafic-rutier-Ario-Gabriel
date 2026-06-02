#include "Logger.h"
#include "Vehicle.h"
#include "Intersection.h"
#include "Street.h"
#include <sstream>

Logger::Logger(std::string filename) {
    logFile.open(filename);
    // JSON file goes alongside the log, e.g. "log_day1.json"
    std::string jsonFilename = filename.substr(0, filename.find_last_of('.')) + ".json";
    jsonFile.open(jsonFilename);
    currentTick = 0;
    firstSnapshot = true;
    // open the JSON array
    jsonFile << "{\n  \"ticks\": [\n";
}

Logger::~Logger() {
    if (logFile.is_open()) logFile.close();
    if (jsonFile.is_open()) {
        // close the JSON array and object
        jsonFile << "\n  ]\n}\n";
        jsonFile.close();
    }
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
            << a->getId() << " and " << b->getId();
    if (s != nullptr)
        logFile << " collided on street " << s->getName() << "\n";
    else
        logFile << " collided at an intersection\n";
}

void Logger::logDelivery(Vehicle* v, Intersection* dest) {
    logFile << "[TICK " << currentTick << "] [DELIVERY] "
            << v->getId() << " delivered to "
            << dest->getId() << "\n";
}

void Logger::logSummary(int totalTicks) {
    logFile << "\n=== SIMULATION COMPLETE ===\n";
    logFile << "Total ticks: " << totalTicks << "\n";
}

// helper to escape strings for JSON
static std::string esc(const std::string& s) {
    return "\"" + s + "\"";
}
static std::string boolStr(bool b) {
    return b ? "true" : "false";
}

void Logger::logSnapshot(const TickSnapshot& snap) {
    // write comma separator between snapshots
    if (!firstSnapshot) jsonFile << ",\n";
    firstSnapshot = false;

    jsonFile << "    {\n";
    jsonFile << "      \"tick\": " << snap.tick << ",\n";
    jsonFile << "      \"delivered\": " << boolStr(snap.delivered) << ",\n";
    jsonFile << "      \"deliveredTo\": " << esc(snap.deliveredTo) << ",\n";
    jsonFile << "      \"packagesDelivered\": " << snap.packagesDelivered << ",\n";
    jsonFile << "      \"totalPackages\": " << snap.totalPackages << ",\n";

    // vehicles
    jsonFile << "      \"vehicles\": [\n";
    for (int i = 0; i < (int)snap.vehicles.size(); i++) {
        const auto& v = snap.vehicles[i];
        jsonFile << "        {"
                 << "\"id\":" << esc(v.id) << ","
                 << "\"pos\":" << esc(v.pos) << ","
                 << "\"dest\":" << esc(v.dest) << ","
                 << "\"status\":" << esc(v.status) << ","
                 << "\"speed\":" << v.speed
                 << "}";
        if (i < (int)snap.vehicles.size() - 1) jsonFile << ",";
        jsonFile << "\n";
    }
    jsonFile << "      ],\n";

    // lights
    jsonFile << "      \"lights\": [\n";
    for (int i = 0; i < (int)snap.lights.size(); i++) {
        const auto& l = snap.lights[i];
        jsonFile << "        {"
                 << "\"id\":" << esc(l.id) << ","
                 << "\"isRed\":" << boolStr(l.isRed) << ","
                 << "\"ticksRemaining\":" << l.ticksRemaining
                 << "}";
        if (i < (int)snap.lights.size() - 1) jsonFile << ",";
        jsonFile << "\n";
    }
    jsonFile << "      ],\n";

    // collisions
    jsonFile << "      \"collisions\": [";
    for (int i = 0; i < (int)snap.collisions.size(); i++) {
        jsonFile << esc(snap.collisions[i]);
        if (i < (int)snap.collisions.size() - 1) jsonFile << ",";
    }
    jsonFile << "]\n";

    jsonFile << "    }";
}

void Logger::flushJSON() {
    // already streaming directly, just need the destructor to close
    // this is a no-op now but kept for explicit call in Simulation::run()
    jsonFile.flush();
}