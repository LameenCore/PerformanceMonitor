#pragma once
#include <string>

class SessionReport {
public:
    std::string fileName;

    double avgCpu;
    double avgMemory;
    double peakCpu;
    double peakMemory;

    int cpuWarnings;
    int memWarnings;
    int totalSamples;

    SessionReport(std::string fileName);
    void parse();    // Reads and parses the log file
    void print();    // Prints the stats for this session
};