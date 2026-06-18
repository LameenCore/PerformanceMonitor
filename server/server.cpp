#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <ctime>
#include "sqlite3.h"

#pragma comment(lib, "ws2_32.lib")

std::string extractField(const std::string& packet, const std::string& label) {
    size_t start = packet.find(label);
    if (start == std::string::npos) return "";
    start += label.size();
    size_t end = packet.find("|", start);
    if (end == std::string::npos) return packet.substr(start);
    return packet.substr(start, end - start);
}

// The packet itself has no timestamp, so the Server stamps the time it RECEIVED the data
std::string getCurrentTimestamp() {
    time_t now = time(nullptr);
    char buf[20];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
    return std::string(buf);
}

void insertMetric(sqlite3* db, const std::string& timestamp, const std::string& cpu,
                   const std::string& mem, const std::string& proc, const std::string& procMem) {
    std::string sql = "INSERT INTO metrics (timestamp, cpu, mem, process_name, process_mem) VALUES ('"
                     + timestamp + "', " + cpu + ", " + mem + ", '" + proc + "', " + procMem + ");";

    char* errMsg = nullptr;
    int result = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
    if (result != SQLITE_OK) {
        std::cout << "DB insert failed: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }
}

int main() {
    sqlite3* db;
    sqlite3_open("bin/metrics.db", &db);

    const char* createSQL =
        "CREATE TABLE IF NOT EXISTS metrics ("
        "timestamp TEXT, cpu REAL, mem REAL, "
        "process_name TEXT, process_mem REAL);";
    sqlite3_exec(db, createSQL, nullptr, nullptr, nullptr);

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

        std::string cpu       = extractField(packet, "CPU:");
        std::string mem       = extractField(packet, "MEM:");
        std::string proc      = extractField(packet, "PROC:");
        std::string procMem   = extractField(packet, "PROCMEM:");
        std::string timestamp = getCurrentTimestamp();

        std::cout << "[" << timestamp << "] CPU: " << cpu << "% | MEM: " << mem
                   << "% | Top Process: " << proc << " (" << procMem << " MB)" << std::endl;

        insertMetric(db, timestamp, cpu, mem, proc, procMem);
    }

    sqlite3_close(db);
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}