#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <thread>
#include <chrono>
#include <iomanip>

struct CpuSnapshot {
    unsigned long long user{0};
    unsigned long long nice{0};
    unsigned long long system{0};
    unsigned long long idle{0};
    unsigned long long iowait{0};
    unsigned long long irq{0};
    unsigned long long softirq{0};
    unsigned long long steal{0};

    unsigned long long getTotalTime() const {
        return user + nice + system + idle + iowait + irq + softirq + steal;
    }

    unsigned long long getIdleTime() const {
        return idle + iowait;
    }
};

CpuSnapshot readCpuSnapshot() {
    std::ifstream file("/proc/stat");
    CpuSnapshot snapshot;
    if (!file.is_open()) return snapshot;

    std::string line;
    if (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string cpuLabel;
        ss >> cpuLabel;
        if (cpuLabel == "cpu") {
            ss >> snapshot.user >> snapshot.nice >> snapshot.system 
               >> snapshot.idle >> snapshot.iowait >> snapshot.irq 
               >> snapshot.softirq >> snapshot.steal;
        }
    }
    return snapshot;
}

double calculateCpuUsage(const CpuSnapshot& prev, const CpuSnapshot& curr) {
    unsigned long long prevTotal = prev.getTotalTime();
    unsigned long long currTotal = curr.getTotalTime();
    unsigned long long prevIdle = prev.getIdleTime();
    unsigned long long currIdle = curr.getIdleTime();

    unsigned long long totalDelta = currTotal - prevTotal;
    unsigned long long idleDelta = currIdle - prevIdle;

    if (totalDelta == 0) return 0.0;
    return (1.0 - (static_cast<double>(idleDelta) / totalDelta)) * 100.0;
}

struct MemInfo {
    double totalMB{0.0};
    double freeMB{0.0};
    double availableMB{0.0};
};

MemInfo readMemInfo() {
    std::ifstream file("/proc/meminfo");
    MemInfo mem;
    if (!file.is_open()) return mem;

    std::string key;
    unsigned long long value;
    std::string unit;

    while (file >> key >> value >> unit) {
        if (key == "MemTotal:") mem.totalMB = value / 1024.0;
        else if (key == "MemFree:") mem.freeMB = value / 1024.0;
        else if (key == "MemAvailable:") mem.availableMB = value / 1024.0;
    }
    return mem;
}

std::string renderBar(double percentage, int barWidth = 30) {
    int filled = static_cast<int>((percentage / 100.0) * barWidth);
    if (filled > barWidth) filled = barWidth;
    if (filled < 0) filled = 0;

    std::string bar = "[";
    for (int i = 0; i < barWidth; ++i) {
        if (i < filled) bar += "=";
        else if (i == filled) bar += ">";
        else bar += " ";
    }
    bar += "]";
    return bar;
}

int main() {
    std::cout << "\033[?25l";

    CpuSnapshot prevCpu = readCpuSnapshot();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    for (int cycle = 0; cycle < 100; ++cycle) {
        CpuSnapshot currCpu = readCpuSnapshot();
        double cpuUsage = calculateCpuUsage(prevCpu, currCpu);
        prevCpu = currCpu;

        MemInfo mem = readMemInfo();
        double usedMB = mem.totalMB - mem.availableMB;
        double memUsagePct = (mem.totalMB > 0) ? (usedMB / mem.totalMB) * 100.0 : 0.0;

        std::cout << "\033[2J\033[H";
        std::cout << "====================================================\n";
        std::cout << "         SysPulse: Host Linux Telemetry Monitor      \n";
        std::cout << "====================================================\n\n";

        std::cout << std::fixed << std::setprecision(1);
        std::cout << " CPU Utilization : " << renderBar(cpuUsage) << " " << cpuUsage << "%\n";
        std::cout << " Memory Usage    : " << renderBar(memUsagePct) << " " 
                  << usedMB << " MB / " << mem.totalMB << " MB (" << memUsagePct << "%)\n";

        std::cout << "\n----------------------------------------------------\n";
        std::cout << " Polling interval: 1.0s | Source: /proc/stat, /proc/meminfo\n";
        std::cout << " Press Ctrl+C to terminate monitor session.\n";
        std::cout << "====================================================\n";

        std::cout.flush();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    std::cout << "\033[?25h";
    return 0;
}
