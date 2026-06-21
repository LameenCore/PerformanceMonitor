#pragma once
#include <string>
#include <vector>

struct SecondRecord {
    std::string timestamp;
    double cpu;
    double mem;
    bool breach;
};

class BottleneckReport {
private:
    double peakCpu = 0.0;
    double peakMemory = 0.0;
    std::string peakCpuTime;
    std::string peakMemoryTime;

    int cpuBreaches = 0;
    int memBreaches = 0;
    int totalSamples = 0;

    double cpuLimit = 80.0;
    double memLimit = 85.0;

    std::string peakProcessName;
    double peakProcessMem = 0.0;

    std::vector<SecondRecord> history;

public:
    void setThresholds(double cpuLim, double memLim);
    void update(double cpu, double mem, bool cpuAlert, bool memAlert,
                const std::string& timestamp,
                const std::string& procName, double procMem);
    void print();
    void exportTxt(std::string filename = "report.txt");
    void exportHtml(std::string filename = "report.html");
};