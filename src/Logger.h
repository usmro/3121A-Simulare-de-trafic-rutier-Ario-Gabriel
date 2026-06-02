#pragma once
#include <string>
#include <fstream>
#include <vector>

class Vehicle;
class Intersection;
class Street;

// One tick's worth of data — everything the frontend needs to render a frame
struct TickSnapshot {
    int tick;

    struct VehicleState {
        std::string id;
        std::string pos;
        std::string dest;
        std::string status; // "moving", "RED", "TRAFFIC", "done"
        int speed;
    };
    std::vector<VehicleState> vehicles;

    struct LightState {
        std::string id;
        bool isRed;
        int ticksRemaining;
    };
    std::vector<LightState> lights;

    std::vector<std::string> collisions;

    bool delivered;        // a package was delivered THIS tick
    std::string deliveredTo;

    // package route progress
    int packagesDelivered; // total dropped off so far
    int totalPackages;     // total packages this run
};

class Logger {
private:
    std::ofstream logFile;  // human readable .txt
    std::ofstream jsonFile; // structured JSON for frontend
    int currentTick;
    bool firstSnapshot;

public:
    Logger(std::string filename);
    ~Logger();

    void setTick(int tick);

    void logIntersectionEntry(Vehicle* v, Intersection* i);
    void logIntersectionExit(Vehicle* v, Intersection* i);
    void logCollision(Vehicle* a, Vehicle* b, Street* s);
    void logDelivery(Vehicle* v, Intersection* dest);
    void logSummary(int totalTicks);

    void logSnapshot(const TickSnapshot& snap);
    void flushJSON();
};