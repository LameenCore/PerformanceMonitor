#pragma once
#include <string>

class BottleneckReport {
public:
    double peakCpu;
    double peakMemory;
    std::string peakCpuTime;
    std::string peakMemoryTime;
    int cpuBreaches;
    int memBreaches;
    int totalSamples;

    BottleneckReport();
    void update(double cpu, double mem, bool cpuAlert, bool memAlert, std::string timestamp);
    void print();
};