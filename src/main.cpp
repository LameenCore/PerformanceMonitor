#include <iostream>
#include <thread>
#include <chrono>
#include <csignal>
#include "../include/Monitor.h"
#include "../include/SessionReport.h"
#include "../include/BottleneckReport.h"

bool running = true;

void handleSignal(int signal) {
    running = false;
}

int main() {
    std::cout << "=== Smart App Performance Monitor ===" << std::endl;
    std::cout << "1. Run Monitor" << std::endl;
    std::cout << "2. Compare Two Sessions" << std::endl;
    std::cout << "Choose (1 or 2): ";

    int choice;
    std::cin >> choice;

    if (choice == 1) {
        std::cout << "\nLogging to monitor.log" << std::endl;
        std::cout << "Sampling every 1 second. Press Ctrl+C to stop.\n" << std::endl;

        signal(SIGINT, handleSignal);

        Monitor monitor(80.0, 85.0);
        BottleneckReport report;

        monitor.sample();
        std::this_thread::sleep_for(std::chrono::seconds(1));

        while (running) {
            monitor.sample();
            monitor.sampleTopProcess();
            monitor.printStats();
            monitor.log();

            report.update(
                monitor.getCpu(),
                monitor.getMem(),
                monitor.isCpuAlert(),
                monitor.isMemAlert(),
                monitor.getTimestamp()
            );

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        report.print();
        report.exportTxt();
        report.exportHtml();
    }
    else if (choice == 2) {
        std::string file1, file2;
        std::cout << "\nEnter first log file name:  ";
        std::cin >> file1;
        std::cout << "Enter second log file name: ";
        std::cin >> file2;

        SessionReport s1(file1);
        SessionReport s2(file2);

        s1.parse();
        s2.parse();

        std::cout << "\n===== Session 1 =====" << std::endl;
        s1.print();

        std::cout << "\n===== Session 2 =====" << std::endl;
        s2.print();
    }

    return 0;
}