#pragma once

#include <string>
#include <memory>
#include <functional>
#include <sstream>

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

/**
 * Logger - Thread-safe singleton logging system with customizable handlers
 *
 * Public interface only; implementation details hidden via Pimpl pattern.
 */
class Logger
{
public:
    // Default console output handler
    static void defaultConsoleHandler(const LogEntry &entry);

    // Singleton getter (thread-safe with double-checked locking)
    static Logger *getInstance();

    // Initialize singleton with component name and log level (thread-safe)
    static void initialize(const std::string &name, LogLevel level = LogLevel::INFO);

    // Register a custom output handler (thread-safe)
    void registerHandler(OutputHandler handler);

    // Clear all handlers (thread-safe)
    void clearHandlers();

    // Replace all handlers with a new one (thread-safe)
    void setHandler(OutputHandler handler);

    // Set log level threshold
    void setLogLevel(LogLevel level);

    // Get current log level
    LogLevel getLogLevel() const;

    // Template logging methods
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

    // Destructor
    ~Logger();

private:
    // Pimpl: pointer to implementation
    class Impl;
    std::unique_ptr<Impl> impl;

    // Private constructor
    Logger(const std::string &name, LogLevel level);

    // Helper for variadic templates - forward to impl
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

    // Write log entry - forwards to impl
    void writeLog(LogLevel level, const std::string &function, int lineNumber, const std::string &message);

    // Singleton instance
    static Logger *instance;
};

// Convenience macros for automatic function name and line number
#define LOG_CPP_TRACE(...) Logger::getInstance()->trace(__FUNCTION__, __LINE__, __VA_ARGS__)
#define LOG_CPP_DEBUG3(...) Logger::getInstance()->debug3(__FUNCTION__, __LINE__, __VA_ARGS__)
#define LOG_CPP_DEBUG2(...) Logger::getInstance()->debug2(__FUNCTION__, __LINE__, __VA_ARGS__)
#define LOG_CPP_DEBUG1(...) Logger::getInstance()->debug1(__FUNCTION__, __LINE__, __VA_ARGS__)
#define LOG_CPP_INFO(...) Logger::getInstance()->info(__FUNCTION__, __LINE__, __VA_ARGS__)
#define LOG_CPP_WARN(...) Logger::getInstance()->warn(__FUNCTION__, __LINE__, __VA_ARGS__)
#define LOG_CPP_ERROR(...) Logger::getInstance()->error(__FUNCTION__, __LINE__, __VA_ARGS__)
