#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

int main() {
    // Step 1 — Initialize Winsock
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // Step 2 — Create socket
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    // Step 3 — Bind to port 8080
    sockaddr_in serverAddr;
    serverAddr.sin_family      = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY; // accept from anyone
    serverAddr.sin_port        = htons(8080);

    bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));

    // Step 4 — Listen for connections
    listen(serverSocket, 1);
    std::cout << "Server listening on port 8080..." << std::endl;

    // Step 5 — Accept a connection
    SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
    std::cout << "Agent connected!" << std::endl;

    // Step 6 — Receive data in a loop
    char buffer[256];
    while (true) {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) {
            std::cout << "Agent disconnected." << std::endl;
            break;
        }
        buffer[bytesReceived] = '\0';
        std::cout << "Received: " << buffer << std::endl;
    }

    // Step 7 — Cleanup
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}