#include "../include/BottleneckReport.h"
#include <iostream>
#include <fstream>

BottleneckReport::BottleneckReport() {
    peakCpu        = 0.0;
    peakMemory     = 0.0;
    cpuBreaches    = 0;
    memBreaches    = 0;
    totalSamples   = 0;
    peakCpuTime    = "";
    peakMemoryTime = "";
}

void BottleneckReport::update(double cpu, double mem, bool cpuAlert, bool memAlert, std::string timestamp) {
    totalSamples++;

    if (cpu > peakCpu) {
        peakCpu     = cpu;
        peakCpuTime = timestamp;
    }

    if (mem > peakMemory) {
        peakMemory     = mem;
        peakMemoryTime = timestamp;
    }

    if (cpuAlert) cpuBreaches++;
    if (memAlert) memBreaches++;
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
    if (!file.is_open()) {
        std::cout << "ERROR: Could not create " << filename << std::endl;
        return;
    }

    std::string cpuColor = (cpuBreaches > 0) ? "#ff4444" : "#00ff00";
    std::string memColor = (memBreaches > 0) ? "#ff4444" : "#00ff00";

    file << "<!DOCTYPE html>" << std::endl;
    file << "<html>" << std::endl;
    file << "<head>" << std::endl;
    file << "<title>Performance Monitor Report</title>" << std::endl;
    file << "<style>" << std::endl;
    file << "  body { font-family: 'Segoe UI', monospace; background: #0d1117; color: #c9d1d9; padding: 40px; margin: 0; }" << std::endl;
    file << "  .container { max-width: 700px; margin: auto; }" << std::endl;
    file << "  h1 { color: #00ff00; font-size: 2em; border-bottom: 1px solid #30363d; padding-bottom: 10px; }" << std::endl;
    file << "  h2 { color: #58a6ff; margin-top: 30px; }" << std::endl;
    file << "  .card { background: #161b22; border: 1px solid #30363d; border-radius: 8px; padding: 20px; margin: 15px 0; }" << std::endl;
    file << "  .label { color: #8b949e; font-size: 0.85em; text-transform: uppercase; letter-spacing: 1px; }" << std::endl;
    file << "  .value { font-size: 1.4em; font-weight: bold; margin-top: 5px; }" << std::endl;
    file << "  .green { color: #00ff00; }" << std::endl;
    file << "  .red { color: #ff4444; }" << std::endl;
    file << "  .grid { display: grid; grid-template-columns: 1fr 1fr; gap: 15px; }" << std::endl;
    file << "  .footer { color: #8b949e; font-size: 0.8em; margin-top: 40px; border-top: 1px solid #30363d; padding-top: 10px; }" << std::endl;
    file << "</style>" << std::endl;
    file << "</head>" << std::endl;
    file << "<body>" << std::endl;
    file << "<div class='container'>" << std::endl;
    file << "  <h1>Smart App Performance Monitor</h1>" << std::endl;
    file << "  <h2>Bottleneck Report</h2>" << std::endl;

    file << "  <div class='card'>" << std::endl;
    file << "    <div class='label'>Total Samples</div>" << std::endl;
    file << "    <div class='value green'>" << totalSamples << "</div>" << std::endl;
    file << "  </div>" << std::endl;

    file << "  <div class='grid'>" << std::endl;

    file << "    <div class='card'>" << std::endl;
    file << "      <div class='label'>Peak CPU</div>" << std::endl;
    file << "      <div class='value' style='color:" << cpuColor << "'>" << peakCpu << "%</div>" << std::endl;
    file << "      <div style='color:#8b949e; font-size:0.85em;'>at " << peakCpuTime << "</div>" << std::endl;
    file << "    </div>" << std::endl;

    file << "    <div class='card'>" << std::endl;
    file << "      <div class='label'>Peak Memory</div>" << std::endl;
    file << "      <div class='value' style='color:" << memColor << "'>" << peakMemory << "%</div>" << std::endl;
    file << "      <div style='color:#8b949e; font-size:0.85em;'>at " << peakMemoryTime << "</div>" << std::endl;
    file << "    </div>" << std::endl;

    file << "    <div class='card'>" << std::endl;
    file << "      <div class='label'>CPU Breaches</div>" << std::endl;
    file << "      <div class='value' style='color:" << cpuColor << "'>" << cpuBreaches << "</div>" << std::endl;
    file << "    </div>" << std::endl;

    file << "    <div class='card'>" << std::endl;
    file << "      <div class='label'>Memory Breaches</div>" << std::endl;
    file << "      <div class='value' style='color:" << memColor << "'>" << memBreaches << "</div>" << std::endl;
    file << "    </div>" << std::endl;

    file << "  </div>" << std::endl;
    file << "  <div class='footer'>Generated by Smart App Performance Monitor</div>" << std::endl;
    file << "</div>" << std::endl;
    file << "</body></html>" << std::endl;

    file.close();
    std::cout << "Report saved to " << filename << std::endl;
}