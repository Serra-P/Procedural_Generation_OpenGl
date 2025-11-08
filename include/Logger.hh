#pragma once
#include <chrono>
#include <fstream>
#include <mutex>
#include <string>

class Logger
{
public:
    enum class Level
    {
        INFO,
        WARNING,
        ERROR,
        DEBUG
    };

    static Logger& Get()
    {
        static Logger instance;
        return instance;
    }

    void Log(const std::string& msg, Level level = Level::INFO);

    void SetLogFile(const std::string& filename);

private:
    Logger();
    ~Logger();

    std::string GetTimestamp();
    std::string LevelToString(Level level);
    std::string LevelToColor(Level level);

    std::ofstream logfile_;
    std::mutex mutex_;
};
