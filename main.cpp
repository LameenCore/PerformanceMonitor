#include <iostream>
#include <windows.h>
#include <psapi.h>
#include <thread>
#include <chrono>

// ─── CPU Usage ────────────────────────────────────────────────────────────────

double getCpuUsage() {
    static FILETIME prevIdle   = {}, prevKernel = {}, prevUser = {};

    FILETIME idle, kernel, user;
    if (!GetSystemTimes(&idle, &kernel, &user)) return -1.0;

    // Combine the two halves of FILETIME into one big number
    auto toULL = [](FILETIME ft) -> unsigned long long {
        return (static_cast<unsigned long long>(ft.dwHighDateTime) << 32)
             | ft.dwLowDateTime;
    };

    unsigned long long idleDiff   = toULL(idle)   - toULL(prevIdle);
    unsigned long long kernelDiff = toULL(kernel) - toULL(prevKernel);
    unsigned long long userDiff   = toULL(user)   - toULL(prevUser);

    prevIdle   = idle;
    prevKernel = kernel;
    prevUser   = user;

    unsigned long long total = kernelDiff + userDiff;
    if (total == 0) return 0.0;

    double cpuUsage = (static_cast<double>(total - idleDiff) / total) * 100.0;
    return cpuUsage;
}

// ─── Memory Usage ─────────────────────────────────────────────────────────────

double getMemoryUsage() {
    MEMORYSTATUSEX memStatus;
    memStatus.dwLength = sizeof(memStatus);
    if (!GlobalMemoryStatusEx(&memStatus)) return -1.0;
    return static_cast<double>(memStatus.dwMemoryLoad);
}

// ─── Main Loop ────────────────────────────────────────────────────────────────

int main() {
    std::cout << "=== Smart App Performance Monitor ===" << std::endl;
    std::cout << "Sampling every 1 second. Press Ctrl+C to stop.\n" << std::endl;

    // First call seeds the baseline — discard it
    getCpuUsage();
    std::this_thread::sleep_for(std::chrono::seconds(1));

    while (true) {
        double cpu = getCpuUsage();
        double mem = getMemoryUsage();

        std::cout << "CPU Usage:    " << cpu << "%" << std::endl;
        std::cout << "Memory Usage: " << mem << "%" << std::endl;
        std::cout << "-------------------------" << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}