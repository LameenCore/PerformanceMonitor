#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "../include/Monitor.h"

#pragma comment(lib, "ws2_32.lib")

// Build the data packet string from monitor readings
std::string buildPacket(Monitor& monitor) {
    return "CPU:"     + std::to_string(monitor.getCpu())
         + "|MEM:"    + std::to_string(monitor.getMem())
         + "|PROC:"   + monitor.getTopProcessName()
         + "|PROCMEM:"+ std::to_string(monitor.getTopProcessMem())
         + "\n";
}

int main() {
    const char* SERVER_IP   = "127.0.0.1";
    const int   SERVER_PORT = 8080;

    // Initialize Winsock
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    Monitor monitor(80.0, 85.0);

    // Seed baseline
    monitor.sample();
    monitor.sampleTopProcess();
    std::this_thread::sleep_for(std::chrono::seconds(1));

    while (true) {
        // Create socket
        SOCKET agentSocket = socket(AF_INET, SOCK_STREAM, 0);

        // Connect to server
        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port   = htons(SERVER_PORT);
        inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);

        std::cout << "Connecting to server..." << std::endl;

        if (connect(agentSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) != 0) {
            std::cout << "Could not connect. Retrying in 3 seconds..." << std::endl;
            closesocket(agentSocket);
            std::this_thread::sleep_for(std::chrono::seconds(3));
            continue; // retry
        }

        std::cout << "Connected! Streaming data..." << std::endl;

        // Stream data until connection drops
        while (true) {
            monitor.sample();
            monitor.sampleTopProcess();

            std::string packet = buildPacket(monitor);
            int result = send(agentSocket, packet.c_str(), packet.size(), 0);

            if (result == SOCKET_ERROR) {
                std::cout << "Connection lost. Reconnecting..." << std::endl;
                break;
            }

            std::cout << "Sent: " << packet << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        closesocket(agentSocket);
    }

    WSACleanup();
    return 0;
}