#pragma once
#include <windows.h>
#include <string>

class Monitor {
private:
    FILETIME prevIdle;
    FILETIME prevKernel;
    FILETIME prevUser;

    double cpuUsage;
    double memUsage;

    double cpuLimit;
    double memLimit;

    bool cpuAlert;
    bool memAlert;

    std::string logFileName; // Name of the log file

public:
    Monitor(double cpuLimit, double memLimit, std::string logFileName = "monitor.log");
    void sample();
    void printStats();
    void log(); // New — saves current reading to file
};