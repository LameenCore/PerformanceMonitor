#include "../include/BottleneckReport.h"
#include <iostream>

BottleneckReport::BottleneckReport() {
    peakCpu      = 0.0;
    peakMemory   = 0.0;
    cpuBreaches  = 0;
    memBreaches  = 0;
    totalSamples = 0;
    peakCpuTime  = "";
    peakMemoryTime = "";
}

void BottleneckReport::update(double cpu, double mem, bool cpuAlert, bool memAlert, std::string timestamp) {
    totalSamples++;

    // Track peak CPU
    if (cpu > peakCpu) {
        peakCpu     = cpu;
        peakCpuTime = timestamp;
    }

    // Track peak Memory
    if (mem > peakMemory) {
        peakMemory     = mem;
        peakMemoryTime = timestamp;
    }

    // Count breaches
    if (cpuAlert) cpuBreaches++;
    if (memAlert) memBreaches++;
}

void BottleneckReport::print() {
    std::cout << "\n===== Bottleneck Report =====" << std::endl;
    std::cout << "Total Samples:    " << totalSamples               << std::endl;
    std::cout << "Peak CPU:         " << peakCpu      << "% at " << peakCpuTime    << std::endl;
    std::cout << "Peak Memory:      " << peakMemory   << "% at " << peakMemoryTime << std::endl;
    std::cout << "CPU Breaches:     " << cpuBreaches               << std::endl;
    std::cout << "Memory Breaches:  " << memBreaches               << std::endl;
    std::cout << "=============================" << std::endl;
}