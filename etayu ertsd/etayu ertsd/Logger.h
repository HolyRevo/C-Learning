#pragma once
#include <string>

class Logger {
public:
    static void Init();
    static void Log(const std::string& message);
    static void Error(const std::string& message);
private:
    static void WriteToFile(const std::string& level, const std::string& msg);
};