#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <chrono>

#pragma comment(lib, "ws2_32.lib")

int main() {
    // Step 1 — Initialize Winsock
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // Step 2 — Create socket
    SOCKET agentSocket = socket(AF_INET, SOCK_STREAM, 0);

    // Step 3 — Connect to server
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port   = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

    if (connect(agentSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) != 0) {
        std::cout << "ERROR: Could not connect to server." << std::endl;
        return 1;
    }

    std::cout << "Connected to server!" << std::endl;

    // Step 4 — Send fake data every second
    int count = 1;
    while (count <= 10) {t
        std::string message = "CPU:23." + std::to_string(count) 
                            + "|MEM:85";
        send(agentSocket, message.c_str(), message.size(), 0);
        std::cout << "Sent: " << message << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(1));
        count++;
    }

    // Step 5 — Cleanup
    closesocket(agentSocket);
    WSACleanup();
    return 0;
}