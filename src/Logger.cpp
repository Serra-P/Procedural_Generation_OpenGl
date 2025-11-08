#include "Logger.hh"

#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>

// Je sais pas pourquoi j'ai fait un logger xd mais c'est beau non ?

Logger::Logger()
{
    std::filesystem::create_directory("logs");
    logfile_.open("logs/app.log", std::ios::app);
}

Logger::~Logger()
{
    if (logfile_.is_open())
        logfile_.close();
}

void Logger::SetLogFile(const std::string& filename)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (logfile_.is_open())
        logfile_.close();
    logfile_.open(filename, std::ios::app);
}

void Logger::Log(const std::string& msg, Level level)
{
    std::string time = GetTimestamp();
    std::string levelStr = LevelToString(level);
    std::string color = LevelToColor(level);

    std::ostringstream formatted;
    formatted << "[" << time << "] [" << levelStr << "] " << msg;

    std::cout << color << formatted.str() << "\033[0m" << std::endl;

    if (logfile_.is_open())
    {
        logfile_ << formatted.str() << std::endl;
    }
}

std::string Logger::GetTimestamp()
{
    using namespace std::chrono;
    auto now = system_clock::now();
    std::time_t now_t = system_clock::to_time_t(now);
    std::tm tm;
#ifdef _WIN32
    localtime_s(&tm, &now_t);
#else
    localtime_r(&now_t, &tm);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string Logger::LevelToString(Level level)
{
    switch (level)
    {
    case Level::INFO:
        return "INFO";
    case Level::WARNING:
        return "WARN";
    case Level::ERROR:
        return "ERROR";
    case Level::DEBUG:
        return "DEBUG";
    default:
        return "UNKNOWN";
    }
}

std::string Logger::LevelToColor(Level level)
{
    switch (level)
    {
    case Level::INFO:
        return "\033[36m";
    case Level::WARNING:
        return "\033[33m";
    case Level::ERROR:
        return "\033[31m";
    case Level::DEBUG:
        return "\033[35m";
    default:
        return "\033[0m";
    }
}
