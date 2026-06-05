#include "Monitor.h"
#include <iostream>
#include <psapi.h>

Monitor::Monitor(double cpuLimit, double memLimit) {
    ZeroMemory(&prevIdle,   sizeof(FILETIME));
    ZeroMemory(&prevKernel, sizeof(FILETIME));
    ZeroMemory(&prevUser,   sizeof(FILETIME));

    cpuUsage = 0.0;
    memUsage = 0.0;

    this->cpuLimit = cpuLimit;
    this->memLimit = memLimit;

    cpuAlert = false;
    memAlert = false;
}

static unsigned long long toULL(FILETIME ft) {
    return (static_cast<unsigned long long>(ft.dwHighDateTime) << 32)
         | ft.dwLowDateTime;
}

void Monitor::sample() {
    // ── CPU ──────────────────────────────────────────
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

    // ── Memory ───────────────────────────────────────
    MEMORYSTATUSEX memStatus;
    memStatus.dwLength = sizeof(memStatus);
    if (GlobalMemoryStatusEx(&memStatus))
        memUsage = static_cast<double>(memStatus.dwMemoryLoad);

    // ── Check thresholds ─────────────────────────────
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