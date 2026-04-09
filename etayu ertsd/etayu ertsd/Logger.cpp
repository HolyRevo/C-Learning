#include "Logger.h"
#include <fstream>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <mutex>

static std::ofstream logFile;
static std::mutex logMutex;

void Logger::Init() {
    logFile.open("cs2_redbox_esp.log", std::ios::app);
    Log("Logger initialized - CS2 Red Box ESP started");
}

void Logger::Log(const std::string& message) {
    WriteToFile("INFO", message);
}

void Logger::Error(const std::string& message) {
    WriteToFile("ERROR", message);
    std::cerr << "[ERROR] " << message << std::endl;
}

void Logger::WriteToFile(const std::string& level, const std::string& msg) {
    std::lock_guard<std::mutex> lock(logMutex);
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::cout << "[" << std::put_time(std::localtime(&time), "%H:%M:%S") << "] [" << level << "] " << msg << std::endl;
    if (logFile.is_open()) {
        logFile << "[" << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") << "] [" << level << "] " << msg << std::endl;
        logFile.flush();
    }
}