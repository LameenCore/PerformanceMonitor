#include "SessionReport.h"
#include <iostream>
#include <fstream>
#include <sstream>

SessionReport::SessionReport(std::string fileName) {
    this->fileName  = fileName;
    avgCpu          = 0.0;
    avgMemory       = 0.0;
    peakCpu         = 0.0;
    peakMemory      = 0.0;
    cpuWarnings     = 0;
    memWarnings     = 0;
    totalSamples    = 0;
}

void SessionReport::parse() {
    std::ifstream file(fileName);
    if (!file.is_open()) {
        std::cout << "ERROR: Could not open " << fileName << std::endl;
        return;
    }

    double totalCpu = 0.0;
    double totalMem = 0.0;

    std::string line;
    while (std::getline(file, line)) {
        // ── Extract CPU value ─────────────────────────
        size_t cpuPos = line.find("CPU: ");
        if (cpuPos == std::string::npos) continue;

        std::string cpuStr = line.substr(cpuPos + 5);
        cpuStr = cpuStr.substr(0, cpuStr.find("%")); // cut off at %
        double cpu = std::stod(cpuStr);

        // ── Extract Memory value ──────────────────────
        size_t memPos = line.find("Memory: ");
        std::string memStr = line.substr(memPos + 8);
        memStr = memStr.substr(0, memStr.find("%")); // cut off at %
        double mem = std::stod(memStr);

        // ── Accumulate stats ──────────────────────────
        totalCpu += cpu;
        totalMem += mem;

        if (cpu > peakCpu) peakCpu = cpu;
        if (mem > peakMemory) peakMemory = mem;

        if (line.find("WARNING: CPU high!")  != std::string::npos) cpuWarnings++;
        if (line.find("WARNING: Memory high!") != std::string::npos) memWarnings++;

        totalSamples++;
    }

    file.close();

    if (totalSamples > 0) {
        avgCpu    = totalCpu / totalSamples;
        avgMemory = totalMem / totalSamples;
    }
}

void SessionReport::print() {
    std::cout << "File: "             << fileName     << std::endl;
    std::cout << "Total Samples:   "  << totalSamples << std::endl;
    std::cout << "Avg CPU:         "  << avgCpu       << "%" << std::endl;
    std::cout << "Avg Memory:      "  << avgMemory    << "%" << std::endl;
    std::cout << "Peak CPU:        "  << peakCpu      << "%" << std::endl;
    std::cout << "Peak Memory:     "  << peakMemory   << "%" << std::endl;
    std::cout << "CPU Warnings:    "  << cpuWarnings  << std::endl;
    std::cout << "Memory Warnings: "  << memWarnings  << std::endl;
}