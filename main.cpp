#include <iostream>
#include <thread>
#include <chrono>
#include "Monitor.h"

int main() {
    std::cout << "=== Smart App Performance Monitor ===" << std::endl;
    std::cout << "Sampling every 1 second. Press Ctrl+C to stop.\n" << std::endl;

    Monitor monitor;

    // First sample seeds the baseline — discard it
    monitor.sample();
    std::this_thread::sleep_for(std::chrono::seconds(1));

    while (true) {
        monitor.sample();
        monitor.printStats();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}