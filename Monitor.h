#pragma once
#include <windows.h>

class Monitor {
private:
    // Store previous CPU time snapshots as member variables
    FILETIME prevIdle;
    FILETIME prevKernel;
    FILETIME prevUser;

    double cpuUsage;
    double memUsage;

public:
    Monitor();              // Constructor — sets everything to zero
    void sample();          // Takes one reading of CPU + Memory
    void printStats();      // Prints the results to console
};