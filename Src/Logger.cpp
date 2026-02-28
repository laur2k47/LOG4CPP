#include "Logger.hpp"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <vector>
#include <mutex>

// ========== Logger::Impl Definition ==========

class Logger::Impl
{
public:
    std::string componentName;
    LogLevel currentLevel;
    std::vector<OutputHandler> handlers;
    mutable std::mutex handlersMutex;

    Impl(const std::string &name, LogLevel level)
        : componentName(name), currentLevel(level)
    {
        // Register default console handler
    }

    static constexpr const char *levelToString(LogLevel level)
    {
        switch (level)
        {
        case LogLevel::TRACE:
            return "TRACE";
        case LogLevel::DEBUG3:
            return "DEBUG3";
        case LogLevel::DEBUG2:
            return "DEBUG2";
        case LogLevel::DEBUG1:
            return "DEBUG1";
        case LogLevel::INFO:
            return "INFO";
        case LogLevel::WARN:
            return "WARN";
        case LogLevel::ERROR:
            return "ERROR";
        default:
            return "UNKNOWN";
        }
    }

    static std::string getCurrentTimestamp()
    {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        auto us_total = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()) % 1000000;

        auto ms = us_total.count() / 1000;
        auto us = us_total.count() % 1000;

        std::ostringstream oss;
        oss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        oss << '.' << ms << std::setfill('0') << std::setw(3) << us;
        return oss.str();
    }

    void writeLog(LogLevel level, const std::string &function, int lineNumber, const std::string &message)
    {
        if (level < currentLevel)
        {
            return; // Don't log if below current level
        }

        LogEntry entry{
            getCurrentTimestamp(),
            levelToString(level),
            componentName,
            function,
            lineNumber,
            message};

        // Call all registered handlers (thread-safe)
        {
            std::lock_guard<std::mutex> lock(handlersMutex);
            for (const auto &handler : handlers)
            {
                handler(entry);
            }
        }
    }
};

// ========== Logger Static Members ==========

Logger *Logger::instance = nullptr;

// ========== Logger Implementation ==========

Logger::Logger(const std::string &name, LogLevel level)
    : impl(std::make_unique<Impl>(name, level))
{
    // Register default console handler
    registerHandler(defaultConsoleHandler);
}

Logger::~Logger() = default;

Logger *Logger::getInstance()
{
    static std::mutex initMutex;
    if (instance == nullptr)
    {
        std::lock_guard<std::mutex> lock(initMutex);
        if (instance == nullptr)
        {
            instance = new Logger("Logger", LogLevel::INFO);
        }
    }
    return instance;
}

void Logger::initialize(const std::string &name, LogLevel level)
{
    static std::mutex initMutex;
    std::lock_guard<std::mutex> lock(initMutex);
    if (instance == nullptr)
    {
        instance = new Logger(name, level);
    }
}

void Logger::defaultConsoleHandler(const LogEntry &entry)
{
    std::ostringstream funcInfo;
    funcInfo << entry.function << ":" << entry.lineNumber;

    std::ostringstream logStream;
    logStream << "[" << entry.timestamp << "]"
              << "[" << std::left << std::setw(6) << entry.level << "]"
              << "[" << entry.component << "]"
              << "[" << std::left << std::setw(20) << funcInfo.str() << "] "
              << entry.message;
    std::cout << logStream.str() << std::endl;
}

void Logger::registerHandler(OutputHandler handler)
{
    std::lock_guard<std::mutex> lock(impl->handlersMutex);
    impl->handlers.push_back(handler);
}

void Logger::clearHandlers()
{
    std::lock_guard<std::mutex> lock(impl->handlersMutex);
    impl->handlers.clear();
}

void Logger::setHandler(OutputHandler handler)
{
    std::lock_guard<std::mutex> lock(impl->handlersMutex);
    impl->handlers.clear();
    impl->handlers.push_back(handler);
}

void Logger::setLogLevel(LogLevel level)
{
    impl->currentLevel = level;
}

LogLevel Logger::getLogLevel() const
{
    return impl->currentLevel;
}

void Logger::writeLog(LogLevel level, const std::string &function, int lineNumber, const std::string &message)
{
    impl->writeLog(level, function, lineNumber, message);
}
