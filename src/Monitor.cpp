#include "../include/Monitor.h"
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

    topProcess.name     = "Unknown";
    topProcess.memoryMB = 0.0;
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
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // CPU color
    if (cpuUsage >= cpuLimit)
        SetConsoleTextAttribute(hConsole, 12);
    else if (cpuUsage >= cpuLimit * 0.75)
        SetConsoleTextAttribute(hConsole, 14);
    else
        SetConsoleTextAttribute(hConsole, 10);

    std::cout << "CPU Usage:    " << cpuUsage << "%" << std::endl;

    // Memory color
    if (memUsage >= memLimit)
        SetConsoleTextAttribute(hConsole, 12);
    else if (memUsage >= memLimit * 0.75)
        SetConsoleTextAttribute(hConsole, 14);
    else
        SetConsoleTextAttribute(hConsole, 10);

    std::cout << "Memory Usage: " << memUsage << "%" << std::endl;

    // Warnings
    if (cpuAlert) {
        SetConsoleTextAttribute(hConsole, 12);
        std::cout << "WARNING: CPU usage is high! (" << cpuUsage << "%)" << std::endl;
    }
    if (memAlert) {
        SetConsoleTextAttribute(hConsole, 12);
        std::cout << "WARNING: Memory usage is high! (" << memUsage << "%)" << std::endl;
    }

    // Top process in cyan
    SetConsoleTextAttribute(hConsole, 11);
    std::cout << "Top Process:  " << topProcess.name
          << " - Memory: " << topProcess.memoryMB << " MB" << std::endl;

    // Reset to white
    SetConsoleTextAttribute(hConsole, 7);
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

void Monitor::sampleTopProcess() {
    DWORD processIds[1024];
    DWORD bytesReturned;

    if (!EnumProcesses(processIds, sizeof(processIds), &bytesReturned))
        return;

    int processCount = bytesReturned / sizeof(DWORD);

    double peakMemory = 0.0;
    std::string peakName = "Unknown";

    for (int i = 0; i < processCount; i++) {
        DWORD pid = processIds[i];
        if (pid == 0) continue;

        HANDLE hProcess = OpenProcess(
            PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid
        );
        if (!hProcess) continue;

        PROCESS_MEMORY_COUNTERS pmc;
        if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
            double memMB = pmc.WorkingSetSize / (1024.0 * 1024.0);

            if (memMB > peakMemory) {
                peakMemory = memMB;

                char nameBuffer[MAX_PATH];
                if (GetProcessImageFileNameA(hProcess, nameBuffer, MAX_PATH)) {
                    std::string fullPath(nameBuffer);
                    size_t pos = fullPath.rfind('\\');
                    peakName = (pos != std::string::npos)
                                ? fullPath.substr(pos + 1)
                                : fullPath;
                }
            }
        }

        CloseHandle(hProcess);
    }

    topProcess.name     = peakName;
    topProcess.memoryMB = peakMemory;
}