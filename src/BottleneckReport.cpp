#include "../include/BottleneckReport.h"
#include <iostream>
#include <fstream>


void BottleneckReport::setThresholds(double cpuLim, double memLim) {
    cpuLimit = cpuLim;
    memLimit = memLim;
}

void BottleneckReport::update(double cpu, double mem, bool cpuAlert, bool memAlert,
                               const std::string& timestamp,
                               const std::string& procName, double procMem) {
    totalSamples++;

    if (cpu > peakCpu) { peakCpu = cpu; peakCpuTime = timestamp; }
    if (mem > peakMemory) { peakMemory = mem; peakMemoryTime = timestamp; }
    if (cpuAlert) cpuBreaches++;
    if (memAlert) memBreaches++;

    if (procMem > peakProcessMem) {
        peakProcessMem = procMem;
        peakProcessName = procName;
    }

    bool breach = cpuAlert || memAlert;
    history.push_back({timestamp, cpu, mem, breach});
}

void BottleneckReport::print() {
    std::cout << "\n===== Bottleneck Report =====" << std::endl;
    std::cout << "Total Samples:    " << totalSamples                          << std::endl;
    std::cout << "Peak CPU:         " << peakCpu     << "% at " << peakCpuTime    << std::endl;
    std::cout << "Peak Memory:      " << peakMemory  << "% at " << peakMemoryTime << std::endl;
    std::cout << "CPU Breaches:     " << cpuBreaches                           << std::endl;
    std::cout << "Memory Breaches:  " << memBreaches                           << std::endl;
    std::cout << "=============================" << std::endl;
}

void BottleneckReport::exportTxt(std::string filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cout << "ERROR: Could not create " << filename << std::endl;
        return;
    }

    file << "===== Bottleneck Report =====" << std::endl;
    file << "Total Samples:    " << totalSamples                          << std::endl;
    file << "Peak CPU:         " << peakCpu     << "% at " << peakCpuTime    << std::endl;
    file << "Peak Memory:      " << peakMemory  << "% at " << peakMemoryTime << std::endl;
    file << "CPU Breaches:     " << cpuBreaches                           << std::endl;
    file << "Memory Breaches:  " << memBreaches                           << std::endl;
    file << "=============================" << std::endl;

    file.close();
    std::cout << "Report saved to " << filename << std::endl;
}

void BottleneckReport::exportHtml(std::string filename) {
    std::ofstream file(filename);
    if (!file.is_open()) return;

    double cpuPercent = (peakCpu / cpuLimit) * 100.0;
    if (cpuPercent > 100.0) cpuPercent = 100.0;
    double memPercent = (peakMemory / memLimit) * 100.0;
    if (memPercent > 100.0) memPercent = 100.0;
    double breachPercent = (totalSamples > 0)
        ? ((double)(cpuBreaches + memBreaches) / totalSamples) * 100.0
        : 0.0;
    if (breachPercent > 100.0) breachPercent = 100.0;

    std::string systemStatus = (cpuBreaches > 0 || memBreaches > 0)
        ? "STATUS: RESOURCE EXHAUSTED" : "STATUS: NOMINAL";
    std::string statusColor = (cpuBreaches > 0 || memBreaches > 0)
        ? "#f43f5e" : "#10b981";

    file << R"(<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<title>Performance Monitor Dashboard</title>
<style>
    * { margin: 0; padding: 0; box-sizing: border-box; }
    body { background-color: #0b0f19; color: #e2e8f0; font-family: 'Courier New', monospace; padding: 30px; }
    .container { max-width: 1100px; margin: 0 auto; }
    .status-bar { display: flex; flex-direction: column; background: #131b2e;
        border: 1px solid #1e293b; border-radius: 10px; padding: 16px 20px; margin-bottom: 24px; gap: 14px; }
    .status-bar .title { font-weight: bold; font-size: 22px; letter-spacing: 0.15em; text-align: center;
        color: #22d3ee; text-shadow: 0 0 10px rgba(34,211,238,0.45); }
    .badge-row { display: flex; justify-content: space-between; flex-wrap: wrap; gap: 10px; }
    .badge { background: #1e293b; padding: 6px 14px; border-radius: 6px; font-size: 14px; color: #94a3b8; }
    .badge.live { color: #10b981; text-shadow: 0 0 6px rgba(16,185,129,0.6); }
    .badge.alert { font-weight: bold; padding: 6px 16px; font-size: 14px; }
    .cards { display: grid; grid-template-columns: repeat(3, 1fr); gap: 16px; margin-bottom: 24px; }
    .card { background: #131b2e; border: 1px solid #1e293b; border-radius: 10px; padding: 18px 20px;
        transition: transform 0.15s ease, box-shadow 0.15s ease; }
    .card:hover { transform: translateY(-3px); box-shadow: 0 6px 18px rgba(0,0,0,0.4); }
    .card .micro-label { font-size: 11px; color: #64748b; letter-spacing: 0.05em; text-transform: uppercase; }
    .card .value { font-size: 28px; font-weight: bold; margin: 6px 0 12px 0; }
    .meter-track { width: 100%; height: 6px; background: #1e293b; border-radius: 4px; overflow: hidden; }
    .meter-fill { height: 100%; border-radius: 4px; }
    .section { background: #131b2e; border: 1px solid #1e293b; border-radius: 10px; padding: 18px 20px; margin-bottom: 24px; }
    .section-title { font-size: 13px; color: #94a3b8; margin-bottom: 14px; letter-spacing: 0.03em; }
    .sparkline { display: flex; flex-wrap: wrap; gap: 3px; }
    .spark-dot { width: 10px; height: 10px; border-radius: 2px; }
    .hog-readout { display: flex; align-items: center; gap: 16px; }
    .hog-badge { background: rgba(244,63,94,0.12); color: #f43f5e; border: 1px solid rgba(244,63,94,0.4);
        padding: 8px 14px; border-radius: 8px; font-weight: bold; text-shadow: 0 0 6px rgba(244,63,94,0.4); }
    .hog-detail { color: #94a3b8; font-size: 13px; }
    table { width: 100%; border-collapse: collapse; font-size: 13px; }
    th { text-align: left; color: #64748b; font-size: 11px; text-transform: uppercase; padding: 8px 10px; border-bottom: 1px solid #1e293b; }
    td { padding: 8px 10px; color: #cbd5e1; }
    tbody tr:nth-child(even) { background: #161f33; }
    tbody tr:hover { background: #1c2740; }
    .footer { text-align: center; color: #ffffff; font-size: 12px; margin-top: 30px; }
</style>
</head>
<body>
<div class="container">
    <div class="status-bar">
        <div class="title">PERFORMANCE MONITOR DASHBOARD</div>
        <div class="badge-row">
            <div class="badge">SESSION: LOCAL</div>
            <div class="badge live">LOGS: ACTIVE</div>
            <div class="badge alert" style="color:)" << statusColor << R"(; background: rgba(244,63,94,0.1);">)" << systemStatus << R"(</div>
        </div>
    </div>
    <div class="cards">
        <div class="card">
            <div class="micro-label">Peak CPU Load</div>
            <div class="value">)" << peakCpu << R"(%</div>
            <div class="meter-track"><div class="meter-fill" style="width:)" << cpuPercent << R"(%; background:)"
                << (cpuBreaches > 0 ? "#f43f5e" : "#10b981") << R"(;"></div></div>
        </div>
        <div class="card">
            <div class="micro-label">Peak Memory Load</div>
            <div class="value">)" << peakMemory << R"(%</div>
            <div class="meter-track"><div class="meter-fill" style="width:)" << memPercent << R"(%; background:)"
                << (memBreaches > 0 ? "#f43f5e" : "#10b981") << R"(;"></div></div>
        </div>
        <div class="card">
            <div class="micro-label">Total Threshold Breaches</div>
            <div class="value">)" << (cpuBreaches + memBreaches) << R"(</div>
            <div class="meter-track"><div class="meter-fill" style="width:)" << breachPercent << R"(%; background:#f43f5e;"></div></div>
        </div>
    </div>
    <div class="section">
        <div class="section-title">METRIC STREAM TIMELINE (1-SEC INTERVALS)</div>
        <div class="sparkline">
)";

    for (const auto& rec : history) {
        std::string color = rec.breach ? "#f43f5e" : "#10b981";
        file << "            <div class=\"spark-dot\" style=\"background:" << color
             << "; box-shadow: 0 0 4px " << color << ";\" title=\"" << rec.timestamp
             << " | CPU: " << rec.cpu << "% MEM: " << rec.mem << "%\"></div>\n";
    }

    file << R"(        </div>
    </div>
    <div class="section">
        <div class="section-title">PROCESS RESOURCE HOG</div>
        <div class="hog-readout">
            <div class="hog-badge">)" << peakProcessName << R"(</div>
            <div class="hog-detail">Peak memory consumption: )" << peakProcessMem << R"( MB</div>
        </div>
    </div>
    <div class="section">
        <div class="section-title">SESSION LOG</div>
        <table>
            <thead><tr><th>Timestamp</th><th>CPU Utilization</th><th>Memory Utilization</th><th>Status</th></tr></thead>
            <tbody>
)";

    for (const auto& rec : history) {
        std::string status = rec.breach ? "BREACH" : "OK";
        std::string statusColor2 = rec.breach ? "#f43f5e" : "#10b981";
        file << "                <tr><td>" << rec.timestamp << "</td><td>" << rec.cpu
             << "%</td><td>" << rec.mem << "%</td><td style=\"color:" << statusColor2
             << ";\">" << status << "</td></tr>\n";
    }

    file << R"(            </tbody>
        </table>
    </div>
    <div class="footer">Generated by Smart App Performance Monitor</div>
</div>
</body>
</html>
)";

    file.close();
}