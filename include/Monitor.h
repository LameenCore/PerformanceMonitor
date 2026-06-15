#pragma once
#include <windows.h>
#include <string>

struct ProcessInfo {
    std::string name;
    double memoryMB;
};

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

    std::string logFileName;
    ProcessInfo topProcess;

public:
    Monitor(double cpuLimit, double memLimit, std::string logFileName = "monitor.log");
    void sample();
    void printStats();
    void log();
    void sampleTopProcess();

    double      getCpu()       { return cpuUsage; }
    double      getMem()       { return memUsage; }
    bool        isCpuAlert()   { return cpuAlert; }
    bool        isMemAlert()   { return memAlert; }
    std::string getTopProcessName() { return topProcess.name; }
    double      getTopProcessMem()  { return topProcess.memoryMB; } 
    std::string getTimestamp();
};