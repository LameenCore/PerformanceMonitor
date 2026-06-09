#include "Monitor.h"
#include <iostream>
#include <fstream>
#include <ctime>
#include <psapi.h>

Monitor::Monitor(double cpuLimit, double memLimit, std::string logFileName) {
    ZeroMemory(&prevIdle,   sizeof(FILETIME));
    ZeroMemory(&prevKernel, sizeof(FILETIME));
    ZeroMemory(&prevUser,   sizeof(FILETIME));

    cpuUsage = 0.0;
    memUsage = 0.0;

    this->cpuLimit    = cpuLimit;
    this->memLimit    = memLimit;
    this->logFileName = logFileName;

    cpuAlert = false;
    memAlert = false;
}

static unsigned long long toULL(FILETIME ft) {
    return (static_cast<unsigned long long>(ft.dwHighDateTime) << 32)
         | ft.dwLowDateTime;
}

void Monitor::sample() {
    FILETIME idle, kernel, user;
    if (GetSystemTimes(&idle, &kernel, &user)) {
        unsigned long long idleDiff   = toULL(idle)   - toULL(prevIdle);
        unsigned long long kernelDiff = toULL(kernel) - toULL(prevKernel);
        unsigned long long userDiff   = toULL(user)   - toULL(prevUser);

        prevIdle   = idle;
        prevKernel = kernel;
        prevUser   = user;

        unsigned long long total = kernelDiff + userDiff;
        if (total > 0)
            cpuUsage = (static_cast<double>(total - idleDiff) / total) * 100.0;
    }

    MEMORYSTATUSEX memStatus;
    memStatus.dwLength = sizeof(memStatus);
    if (GlobalMemoryStatusEx(&memStatus))
        memUsage = static_cast<double>(memStatus.dwMemoryLoad);

    cpuAlert = (cpuUsage > cpuLimit);
    memAlert = (memUsage > memLimit);
}

void Monitor::printStats() {
    std::cout << "CPU Usage:    " << cpuUsage << "%" << std::endl;
    if (cpuAlert)
        std::cout << "WARNING: CPU usage is high! (" << cpuUsage << "%)" << std::endl;

    std::cout << "Memory Usage: " << memUsage << "%" << std::endl;
    if (memAlert)
        std::cout << "WARNING: Memory usage is high! (" << memUsage << "%)" << std::endl;

    std::cout << "-------------------------" << std::endl;
}

void Monitor::log() {
    time_t now = time(0);
    tm* localTime = localtime(&now);

    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%H:%M:%S", localTime);

    std::ofstream logFile(logFileName, std::ios::app);
    if (!logFile.is_open()) {
        std::cout << "ERROR: Could not open log file." << std::endl;
        return;
    }

    logFile << "[" << timestamp << "] "
            << "CPU: " << cpuUsage << "% | "
            << "Memory: " << memUsage << "%";

    if (cpuAlert) logFile << " | WARNING: CPU high!";
    if (memAlert) logFile << " | WARNING: Memory high!";

    logFile << std::endl;
    logFile.close();
}

std::string Monitor::getTimestamp() {
    time_t now = time(0);
    tm* localTime = localtime(&now);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%H:%M:%S", localTime);
    return std::string(timestamp);
}