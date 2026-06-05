#pragma once
#include <windows.h>

class Monitor {
private:
    // Previous CPU time snapshots
    FILETIME prevIdle;
    FILETIME prevKernel;
    FILETIME prevUser;

    // Current readings
    double cpuUsage;
    double memUsage;

    // Thresholds
    double cpuLimit;
    double memLimit;

    // Alert flags
    bool cpuAlert;
    bool memAlert;

public:
    Monitor(double cpuLimit, double memLimit); // Constructor with parameters
    void sample();
    void printStats();
};