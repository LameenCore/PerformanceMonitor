#include <iostream>
#include "sqlite3.h"

// Called once per row returned by a SELECT query
int printRow(void* notUsed, int columnCount, char** columnValues, char** columnNames) {
    for (int i = 0; i < columnCount; i++) {
        std::cout << columnNames[i] << " = " << columnValues[i] << "  ";
    }
    std::cout << std::endl;
    return 0;
}

int main() {
    sqlite3* db;
    char* errMsg = nullptr;

    int result = sqlite3_open("test.db", &db);
    if (result != SQLITE_OK) {
        std::cout << "Could not open database!" << std::endl;
        return 1;
    }
    std::cout << "Database opened successfully." << std::endl;

    // Create the table
    const char* createSQL =
        "CREATE TABLE IF NOT EXISTS metrics ("
        "timestamp TEXT, cpu REAL, mem REAL, "
        "process_name TEXT, process_mem REAL);";

    sqlite3_exec(db, createSQL, nullptr, nullptr, &errMsg);

    // Insert one test row
    const char* insertSQL =
        "INSERT INTO metrics (timestamp, cpu, mem, process_name, process_mem) "
        "VALUES ('2026-06-17 14:32:01', 24.5, 78.0, 'claude.exe', 533.5);";

    sqlite3_exec(db, insertSQL, nullptr, nullptr, &errMsg);

    std::cout << "Row inserted. Reading it back:" << std::endl;

    // Read it back
    const char* selectSQL = "SELECT * FROM metrics;";
    sqlite3_exec(db, selectSQL, printRow, nullptr, &errMsg);

    sqlite3_close(db);
    return 0;
}