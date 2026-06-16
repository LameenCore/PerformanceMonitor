#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

// Pulls one field's value out of the packet, given the field's label (e.g. "CPU:")
std::string extractField(const std::string& packet, const std::string& label) {
    size_t start = packet.find(label);
    if (start == std::string::npos) return "";

    start += label.size();                      // move past "CPU:" to the value itself
    size_t end = packet.find("|", start);        // find the next '|' after the value

    if (end == std::string::npos) {
        return packet.substr(start);              // last field — no trailing '|'
    }
    return packet.substr(start, end - start);
}

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddr;
    serverAddr.sin_family      = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port        = htons(8080);

    bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, 1);

    std::cout << "Ingestion Server listening on port 8080..." << std::endl;

    SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
    std::cout << "Agent connected!" << std::endl;

    char buffer[256];
    while (true) {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) {
            std::cout << "Agent disconnected." << std::endl;
            break;
        }
        buffer[bytesReceived] = '\0';
        std::string packet(buffer);

        std::string cpu     = extractField(packet, "CPU:");
        std::string mem     = extractField(packet, "MEM:");
        std::string proc    = extractField(packet, "PROC:");
        std::string procMem = extractField(packet, "PROCMEM:");

        std::cout << "CPU: " << cpu << "% | MEM: " << mem
                   << "% | Top Process: " << proc
                   << " (" << procMem << " MB)" << std::endl;
    }

    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}