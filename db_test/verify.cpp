#include <iostream>
#include "sqlite3.h"

int printRow(void* notUsed, int columnCount, char** columnValues, char** columnNames) {
    for (int i = 0; i < columnCount; i++) {
        std::cout << columnNames[i] << " = " << columnValues[i] << "  ";
    }
    std::cout << std::endl;
    return 0;
}

int main() {
    sqlite3* db;
    int result = sqlite3_open("bin/metrics.db", &db);
    if (result != SQLITE_OK) {
        std::cout << "Could not open database!" << std::endl;
        return 1;
    }

    std::cout << "=== Row Count ===" << std::endl;
    sqlite3_exec(db, "SELECT COUNT(*) AS total_rows FROM metrics;", printRow, nullptr, nullptr);

    std::cout << "\n=== Last 5 Rows ===" << std::endl;
    sqlite3_exec(db, "SELECT * FROM metrics ORDER BY timestamp DESC LIMIT 5;", printRow, nullptr, nullptr);

    sqlite3_close(db);
    return 0;
}