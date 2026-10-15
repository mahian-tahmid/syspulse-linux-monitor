# SysPulse: Linux System Telemetry Monitor

A lightweight host telemetry tool built in C++17 designed to parse and monitor low-level Linux kernel metrics via the `/proc` virtual filesystem.

## Features
- **Real-Time CPU Tracking:** Calculates aggregate CPU utilization between delta ticks by parsing `/proc/stat`.
- **Memory Consumption:** Parses `/proc/meminfo` to report real-time available, free, and total system RAM in Megabytes.
- **Terminal Visualization:** Renders clean ASCII visualizer bars with ANSI escape sequences for in-place terminal updates.
- **Zero Third-Party Dependencies:** Implemented strictly using modern C++ Standard Library features (`<fstream>`, `<sstream>`, `<thread>`, `<chrono>`).

## Sample Output
```text
====================================================
         SysPulse: Host Linux Telemetry Monitor      
====================================================

 CPU Utilization : [======>                       ] 21.4%
 Memory Usage    : [============>                 ] 4120.5 MB / 16000.0 MB (25.8%)

----------------------------------------------------
 Polling interval: 1.0s | Source: /proc/stat, /proc/meminfo
 Press Ctrl+C to terminate monitor session.
====================================================
```

## Compilation & Run

Requires `g++` with C++17 support on any Linux distribution (Ubuntu, Debian, Arch, Raspberry Pi OS):

```bash
g++ -std=c++17 -O2 main.cpp -o syspulse
./syspulse
```
