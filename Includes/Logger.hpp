#pragma once

#include <iostream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <string>
#include <memory>
#include <vector>
#include <functional>
#include <mutex>

enum class LogLevel
{
    TRACE,
    DEBUG3,
    DEBUG2,
    DEBUG1,
    INFO,
    WARN,
    ERROR
};

// Structure to hold individual log fields
struct LogEntry
{
    std::string timestamp;
    std::string level;
    std::string component;
    std::string function;
    int lineNumber;
    std::string message;
};

// Output handler interface
using OutputHandler = std::function<void(const LogEntry &)>;

class Logger
{
private:
    std::string componentName;
    LogLevel currentLevel;
    std::vector<OutputHandler> handlers;
    mutable std::mutex handlersMutex; // Protects handlers vector access

    // Singleton instance and its mutex
    static Logger *instance;
    static std::mutex instanceMutex; // Protects singleton initialization

    // Private constructor
    Logger(const std::string &name, LogLevel level = LogLevel::INFO)
        : componentName(name), currentLevel(level)
    {
        // Register default console handler
        registerHandler(defaultConsoleHandler);
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

    template <typename T>
    static void formatArg(std::ostringstream &oss, const T &arg)
    {
        oss << arg;
    }

    template <typename T, typename... Args>
    static void formatArgs(std::ostringstream &oss, const T &arg, const Args &...args)
    {
        oss << arg;
        formatArgs(oss, args...);
    }

    static void formatArgs(std::ostringstream &)
    {
        // Base case: no more arguments
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

public:
    // Default console output handler
    static void defaultConsoleHandler(const LogEntry &entry)
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

    // Singleton getter (thread-safe with double-checked locking)
    static Logger *getInstance()
    {
        if (instance == nullptr)
        {
            std::lock_guard<std::mutex> lock(instanceMutex);
            if (instance == nullptr)
            {
                instance = new Logger("Logger");
            }
        }
        return instance;
    }

    // Initialize singleton with component name and log level (thread-safe)
    static void initialize(const std::string &name, LogLevel level = LogLevel::INFO)
    {
        std::lock_guard<std::mutex> lock(instanceMutex);
        if (instance == nullptr)
        {
            instance = new Logger(name, level);
        }
    }

    // Register a custom output handler (thread-safe)
    void registerHandler(OutputHandler handler)
    {
        std::lock_guard<std::mutex> lock(handlersMutex);
        handlers.push_back(handler);
    }

    // Clear all handlers (thread-safe)
    void clearHandlers()
    {
        std::lock_guard<std::mutex> lock(handlersMutex);
        handlers.clear();
    }

    // Replace all handlers with a new one (thread-safe)
    void setHandler(OutputHandler handler)
    {
        std::lock_guard<std::mutex> lock(handlersMutex);
        handlers.clear();
        handlers.push_back(handler);
    }

    void setLogLevel(LogLevel level)
    {
        currentLevel = level;
    }

    LogLevel getLogLevel() const
    {
        return currentLevel;
    }

    template <typename... Args>
    void trace(const std::string &function, int lineNumber, const Args &...args)
    {
        std::ostringstream oss;
        formatArgs(oss, args...);
        writeLog(LogLevel::TRACE, function, lineNumber, oss.str());
    }

    template <typename... Args>
    void debug3(const std::string &function, int lineNumber, const Args &...args)
    {
        std::ostringstream oss;
        formatArgs(oss, args...);
        writeLog(LogLevel::DEBUG3, function, lineNumber, oss.str());
    }

    template <typename... Args>
    void debug2(const std::string &function, int lineNumber, const Args &...args)
    {
        std::ostringstream oss;
        formatArgs(oss, args...);
        writeLog(LogLevel::DEBUG2, function, lineNumber, oss.str());
    }

    template <typename... Args>
    void debug1(const std::string &function, int lineNumber, const Args &...args)
    {
        std::ostringstream oss;
        formatArgs(oss, args...);
        writeLog(LogLevel::DEBUG1, function, lineNumber, oss.str());
    }

    template <typename... Args>
    void info(const std::string &function, int lineNumber, const Args &...args)
    {
        std::ostringstream oss;
        formatArgs(oss, args...);
        writeLog(LogLevel::INFO, function, lineNumber, oss.str());
    }

    template <typename... Args>
    void warn(const std::string &function, int lineNumber, const Args &...args)
    {
        std::ostringstream oss;
        formatArgs(oss, args...);
        writeLog(LogLevel::WARN, function, lineNumber, oss.str());
    }

    template <typename... Args>
    void error(const std::string &function, int lineNumber, const Args &...args)
    {
        std::ostringstream oss;
        formatArgs(oss, args...);
        writeLog(LogLevel::ERROR, function, lineNumber, oss.str());
    }
};

// Convenience macros for automatic function name and line number
#define LOG_CPP_TRACE(...) Logger::getInstance()->trace(__FUNCTION__, __LINE__, __VA_ARGS__)
#define LOG_CPP_DEBUG3(...) Logger::getInstance()->debug3(__FUNCTION__, __LINE__, __VA_ARGS__)
#define LOG_CPP_DEBUG2(...) Logger::getInstance()->debug2(__FUNCTION__, __LINE__, __VA_ARGS__)
#define LOG_CPP_DEBUG1(...) Logger::getInstance()->debug1(__FUNCTION__, __LINE__, __VA_ARGS__)
#define LOG_CPP_INFO(...) Logger::getInstance()->info(__FUNCTION__, __LINE__, __VA_ARGS__)
#define LOG_CPP_WARN(...) Logger::getInstance()->warn(__FUNCTION__, __LINE__, __VA_ARGS__)
#define LOG_CPP_ERROR(...) Logger::getInstance()->error(__FUNCTION__, __LINE__, __VA_ARGS__)
